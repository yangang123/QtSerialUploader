#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QDate>
#include "packet.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    mSerialPort(new QSerialPort)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString serialPortItemName = info.manufacturer();
        serialPortItemName.append('(');
        serialPortItemName += info.portName();
        serialPortItemName.append(')');
        ui->serial_box->addItem(serialPortItemName);
    }

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

//    ui->open_button->setEnabled(false);
//    ui->open_button->setText("关闭");

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
        if (mIsOpen) {
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
//            mSerialPort->write("helloworld");
        } else {
            qDebug() << "portName close";
        }
    }
}

void Dialog::on_read_version_button_clicked()
{
    if(!mIsOpen) {
        return;
    }
    quint8 buffer[20];
    quint8 tx_len =0;

    pakect_send(FW_UPDATE_REQ, (quint8*)0, 0, (quint8*)buffer, &tx_len);

    QByteArray array;
    if (tx_len) {
        array = QByteArray((char*)buffer, tx_len);
    }
    mSerialPort->write(array);

    if (mSerialPort->waitForReadyRead(10000)) {
        QByteArray responseData = mSerialPort->readAll();
        while (mSerialPort->waitForReadyRead(10))
            responseData += mSerialPort->readAll();
        qDebug() << "datasize" << responseData.size();

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
                 }
             }
        }

//        QString response(responseData);
    } else {
         qDebug() << "timeout";
    }
}
