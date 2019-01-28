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
         QThread::sleep(1);
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

//    QString qStr = "abc";
//    QChar *str = qStr.data();
//    qDebug()<< qStr;
//    qDebug()<< str;

        quint8 buffer[3];
        buffer[0] = 0x03;
        buffer[1] = 0x00;
        buffer[2] = 0x03;

        QString output1 = QString("%1.").arg((int)buffer[0]);
        QString output2 = QString("%1.").arg((int)buffer[1]);
        QString output3 = QString("%1").arg((int)buffer[2]);
        output1 +=  output2;
        output1 +=  output3;

        qDebug()<<output1;

//    QByteArray a((char*)buffer, 1);
//    QString ver1;
//    ver1 = a.toInt(0,16);
//    qDebug() << "ver1:" << ver1 ;

//    qDebug() << "ver1 close" << ver2 ;
//    qDebug() << "ver1 close" << ver3 ;

//    3.0.3
//    quint8 tx_len =0;
//    pakect_send(FW_UPDATE_DATA, (quint8*)&_firmware_data,
//                (quint16)(6+_firmware_data.block_len), (quint8*)buffer, &tx_len);
//    QByteArray array;
//    if (tx_len) {
//        array = QByteArray((char*)buffer, tx_len);
//    }


    memset((void*)&mPacket, 0x00, (size_t)sizeof(packet_desc_t));
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
    quint8 tx_len =0;
    pakect_send(FW_UPDATE_VERREQ, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    QByteArray array;
    if (tx_len) {
        array = QByteArray((char*)buffer, tx_len);
    }
    mSerialPort->write(array);
    mThread.set_cmd("sleep");
    if (mSerialPort->waitForReadyRead(10000)) {
        QByteArray responseData = mSerialPort->readAll();
        while (mSerialPort->waitForReadyRead(10))
            responseData += mSerialPort->readAll();
        qDebug() << "datasize" << responseData.size();
        qDebug() << responseData;
        char *buf = responseData.data();
        for (quint8 i = 0; i <responseData.size(); i++ ) {

             if(packet_parse_data_callback(buf[i], &mPacket)) {
                 quint8 cmd = mPacket.data[0];
                 switch(cmd) {
                 case FW_UPDATE_REQ:
                     qDebug() << "req";
                     break;

                 case FW_UPDATE_ACK:
                     qDebug() << "ack";
                      break;

                 case FW_UPDATE_VERREPLY:
                     QByteArray ba;
                     ba = QByteArray((char*)&mPacket.data[1], 3);
                     qDebug() << "ver:" << ba.toHex();
                     ui->version_display->setText(ba.toHex());
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
     QFileDialog *fileDialog = new QFileDialog(this);
     if(fileDialog->exec() == QDialog::Accepted) {
          QString path = fileDialog->selectedFiles()[0];
          qDebug() << "path" << path;

//          char *path1 = "C:/Users/yangang/Desktop/qxwz_rtk_track.bin";
           char *path1 = "C:/Users/Administrator/Desktop/qxwz_rtk_track.bin";
          send_file(path1);
     }

//     if(!mIsOpen) {
//         return;
//     }

//     quint8 buffer[20];
//     quint8 tx_len =0;
//     pakect_send(FW_UPDATE_REQ, (quint8*)0, 0, (quint8*)buffer, &tx_len);
//     QByteArray array;
//     if (tx_len) {
//         array = QByteArray((char*)buffer, tx_len);
//     }
//     mSerialPort->write(array);
//     if (mSerialPort->waitForReadyRead(10000)) {
//         QByteArray responseData = mSerialPort->readAll();
//         while (mSerialPort->waitForReadyRead(10))
//             responseData += mSerialPort->readAll();
//         qDebug() << "datasize" << responseData.size();
//         qDebug() << responseData;
//         char *buf = responseData.data();
//         for (quint8 i = 0; i <responseData.size(); i++ ) {

//              if(packet_parse_data_callback(buf[i], &mPacket)) {
//                  quint8 cmd = mPacket.data[0];
//                  switch(cmd) {
//                  case FW_UPDATE_ACK:
//                       qDebug() << "ack";
//                       break;
//                  }
//              }
//         }
//     } else {
//          qDebug() << "timeout";
//     }



}

int Dialog::send_file(char *name)
{
    qDebug("File name :%s \n", name);

    FILE *fp = NULL;
    char tmp[128];
    fp = fopen(name, "r");
    if (NULL == fp)
    {
        qDebug("File:%s open fail!\n", name);
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    qint64 filesize = ftell(fp);

    _firmware_data.total_block = (filesize+IAP_FW_DATA_LEN)/IAP_FW_DATA_LEN;
    _firmware_data.block_len = IAP_FW_DATA_LEN;
    qDebug() << "firmware total_block:" <<_firmware_data.total_block << "len" << _firmware_data.block_len;
    qDebug() << "filesize:" <<filesize ;
    fseek(fp, 0L, SEEK_SET);

//    int time = 0;
    bool finish = false;
    _firmware_data.cur_block = 1;
    do {
        int cnt = fread(tmp, 1, 128, fp);
        if (cnt < 0) {
            qDebug("read finish: size: %dbytes, read cnt:%d, cnt:\r\n", (_firmware_data.cur_block-1)*128+cnt, _firmware_data.cur_block, cnt);
            finish = true;
        } else {
            qDebug("file read len: %dbytes, read cnt:%d cnt:%d\r\n", _firmware_data.cur_block*128, _firmware_data.cur_block, cnt);
        }

        //create packet
        memcpy(_firmware_data.data, tmp, cnt);
        _firmware_data.block_len = cnt;
        qDebug("total: %d, cur:%d, len:%d\r\n", _firmware_data.total_block, _firmware_data.cur_block, _firmware_data.block_len);
//        pakect_send(0, FW_UPDATE_DATA, &_firmware_data, 6+_firmware_data.block_len);

        quint8 buffer[256];
        quint8 tx_len =0;
        pakect_send(FW_UPDATE_DATA, (quint8*)&_firmware_data,
                    (quint16)(6+_firmware_data.block_len), (quint8*)buffer, &tx_len);
        QByteArray array;
        if (tx_len) {
            array = QByteArray((char*)buffer, tx_len);
        }
        mSerialPort->write(array);

        _firmware_data.cur_block++;

        if (finish) {
            break;
        }
//        usleep(200*1000);
        mThread.set_cmd("sleep");
    } while(1);
    fclose(fp);
    fp = NULL;

    return 0;
}
