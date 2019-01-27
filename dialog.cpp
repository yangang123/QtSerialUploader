#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPortInfo>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    mSerialPort(new QSerialPort)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->serial_box->addItem(info.portName());
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_open_button_clicked()
{
    portName =  ui->serial_box->currentText();

    mSerialPort->setPortName(portName);
    mSerialPort->setBaudRate(QSerialPort::Baud115200);
    mSerialPort->setParity(QSerialPort::NoParity);
    mSerialPort->setStopBits(QSerialPort::OneStop);

    mSerialPort->open(QSerialPort::ReadWrite);
    mIsOpen = mSerialPort->isOpen();
    if (mIsOpen) {
        qDebug ("portName open\r\n");
        mSerialPort->write("helloworld");
    } else {
        qDebug ("portName open\r\n");
    }
}

void Dialog::on_read_version_button_clicked()
{
    if(!mIsOpen) {
        return;
    }

    mSerialPort->write("on_read_version_button_clicked");

}
