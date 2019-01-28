#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QDate>
#include <QFileDialog>
#include "packet.h"

MasterThread::MasterThread(QObject *parent)
    : QThread(parent),quit(false)
{
}

//! [0]
MasterThread::~MasterThread()
{

}

void MasterThread::set_cmd(const QString &s)
{
    if (s == "sleep") {
         qDebug() << "sleep";
         QThread::msleep(200);
    }
}


void MasterThread::run()
{
    while (!quit) {
        QThread::sleep(1);
        qDebug() << "master thread";
    }
}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    mSerialPort(new QSerialPort)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString serialPortItemName = info.manufacturer();
        serialPortItemName = info.portName();
        ui->serial_box->addItem(serialPortItemName);
    }

    memset((void*)&mPacket, 0x00, (size_t)sizeof(packet_desc_t));
    memset((void*)&_firmware_data, 0x00, (size_t)sizeof(fw_packet_t));

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_open_button_clicked()
{
    QString currentPortName;
    bool currentPortNameChanged = false;

    currentPortName =  ui->serial_box->currentText();
    if (!mFirstOpen) {
        portName = currentPortName;
        mFirstOpen = true;
        currentPortNameChanged = true;
    } else {

        if (currentPortName != portName) {
            portName = currentPortName;
            qDebug() << "portname:" << portName;
            currentPortNameChanged = true;
        }
    }

    if (currentPortNameChanged) {
        qDebug() << "portName new" << portName;
        if (mIsOpen) {
             qDebug() << "portName close";
             mSerialPort->close();
        }
        mSerialPort->setPortName(portName);
        mSerialPort->setBaudRate(QSerialPort::Baud115200);
        mSerialPort->setParity(QSerialPort::NoParity);
        mSerialPort->setStopBits(QSerialPort::OneStop);

        mSerialPort->open(QSerialPort::ReadWrite);
        mIsOpen = mSerialPort->isOpen();
        if (mIsOpen) {
            qDebug() << "portName open" << "openDate:" << QDate::currentDate();
        } else {
            qDebug() << "portName close";
        }

        mThread.set_cmd("start");
    }
}

void Dialog::on_read_version_button_clicked()
{
    if(!mIsOpen) {
        return;
    }

    quint8 buffer[20];
    quint16 tx_len =0;
    pakect_send(FW_UPDATE_VERREQ, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    QByteArray array;
    if (tx_len) {
        array = QByteArray((char*)buffer, tx_len);
    }
    mSerialPort->write(array);
    mSerialPort->flush();
    mThread.set_cmd("sleep");
    if (mSerialPort->waitForReadyRead(2000)) {
        QByteArray responseData = mSerialPort->readAll();
        while (mSerialPort->waitForReadyRead(10))
            responseData += mSerialPort->readAll();
        qDebug() << "datasize" << responseData.size();
        char *buf = responseData.data();
        for (quint8 i = 0; i <responseData.size(); i++ ) {

             if(packet_parse_data_callback(buf[i], &mPacket)) {
                 quint8 cmd = mPacket.data[0];
                 switch(cmd) {
                 case FW_UPDATE_ACK:
                      qDebug() << "ack";
                      break;

                 case FW_UPDATE_VERREPLY:
                    QString output1 = QString("%1.").arg((int)mPacket.data[1]);
                    QString output2 = QString("%1.").arg((int)mPacket.data[2]);
                    QString output3 = QString("%1").arg((int)mPacket.data[3]);
                    output1 +=  output2;
                    output1 +=  output3;
                    ui->version_display->setText(output1);
                    break;
                 }
             }
        }
    } else {
         qDebug() << "timeout";
    }
}

void Dialog::on_update_firmware_button_clicked()
{
    if(!mIsOpen) {
        return;
    }

    quint8 buffer[20];
    quint16 tx_len =0;
    pakect_send(FW_UPDATE_REQ, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    QByteArray array;
    if (tx_len) {
        array = QByteArray((char*)buffer, tx_len);
    }
    mSerialPort->write(array);
    mThread.set_cmd("sleep");
    if (mSerialPort->waitForReadyRead(3000)) {
        QByteArray responseData = mSerialPort->readAll();
        while (mSerialPort->waitForReadyRead(10))
            responseData += mSerialPort->readAll();
        qDebug() << "datasize" << responseData.size();
        char *buf = responseData.data();
        for (quint8 i = 0; i <responseData.size(); i++ ) {
             if(packet_parse_data_callback(buf[i], &mPacket)) {
                 quint8 cmd = mPacket.data[0];
                 switch(cmd) {
                 case FW_UPDATE_ACK:
                      qDebug() << "ack";
                      send_file();
                      on_test_button_4_clicked();
                      break;
                 }
             }
        }
    } else {
         qDebug() << "timeout";
    }


}

int Dialog::send_file(void)
{
    QFileDialog *fileDialog = new QFileDialog(this);
    if(fileDialog->exec() == QDialog::Accepted) {
           QString path = fileDialog->selectedFiles()[0];
           qDebug() << "path" << path;
           QFile file(path);

           if (file.open(QIODevice::ReadOnly) == true)
           {
               qDebug() << "file open ok";
           }
         buffer_read = file.readAll();
         file.close();

         qint64 filesize = file.size();
        _firmware_data.total_block = (filesize+IAP_FW_DATA_LEN)/IAP_FW_DATA_LEN;
        _firmware_data.block_len = IAP_FW_DATA_LEN;
        _firmware_data.cur_block = 1;
         last_packet = filesize % IAP_FW_DATA_LEN ;

        qDebug() << "firmware total_block:" <<_firmware_data.total_block << "len" << _firmware_data.block_len;
        qDebug() << "filesize:" << filesize  << "last_packet" << last_packet;
    }

    return 0;
}

void Dialog::send_onepakcet(char*p, qint16 len)
{
    if(!mIsOpen) {
        qDebug() << "serial no open!";
        return;
    }

    quint16 tx_len = 0;
    char buffer_ret[IAP_CONFIG_PACKET_BUFSIZE];
    memcpy(_firmware_data.data, p, len);

    pakect_send(FW_UPDATE_DATA, (uint8_t*)&_firmware_data,
                (quint16)(6+len), (quint8*)buffer_ret, &tx_len);

    QByteArray buf = QByteArray(buffer_ret, tx_len);
    qDebug() << "buf size" << buf.size();
    mSerialPort->write(buf);
    mSerialPort->flush();
    buf.clear();
}

void Dialog::on_test_button_clicked()
{
    char *p = buffer_read.data();
    char *p1 = p + (_firmware_data.cur_block-1) * 512;
    send_onepakcet(p1, 512);
    qDebug("total: %d, cur:%d, len:%d\r\n", _firmware_data.total_block, _firmware_data.cur_block,_firmware_data.block_len);
    _firmware_data.cur_block++;
    mThread.set_cmd("sleep");
}

void Dialog::on_test_button_2_clicked()
{
    _firmware_data.cur_block = _firmware_data.total_block;

    char *p = buffer_read.data();
    char *p1 = p + (_firmware_data.total_block-1) * 512;
    send_onepakcet(p1, last_packet);
    mThread.set_cmd("sleep");
}

void Dialog::on_test_button_3_clicked()
{
    char *p = buffer_read.data();

    char *p1 = p + (_firmware_data.total_block-2) * 512;

    send_onepakcet(p1, 512);

    qDebug("total: %d, cur:%d, len:%d\r\n", _firmware_data.total_block, _firmware_data.cur_block,_firmware_data.block_len);
    _firmware_data.cur_block++;
    mThread.set_cmd("sleep");
}

void Dialog::on_test_button_4_clicked()
{
    for (qint16 i = 1; i < 165; i++) {
        on_test_button_clicked();
    }

    on_test_button_2_clicked();
}
