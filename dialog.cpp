#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QDate>
#include <QFileDialog>
#include "packet.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    _rtkConfig(new RtkConfig)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString serialPortItemName = info.manufacturer();
        serialPortItemName = info.portName();
        ui->comboBox_serialPort->addItem(serialPortItemName);
    }

   connect(_rtkConfig, &RtkConfig::sendStatusStr, this, &Dialog::statusStrShow);

   QObject::connect(_rtkConfig->_link,&SerialLink::bytesReceived, _rtkConfig, &RtkConfig::receiveBytes);
}

Dialog::~Dialog()
{
    delete ui;
}
void Dialog::on_open_button_clicked()
{
   QString name =ui->comboBox_serialPort->currentText();
   qDebug() << "name" << name;
   _rtkConfig->open_link(name);
}

void Dialog::on_read_version_button_clicked()
{
    qDebug() << "readVersion";
    _rtkConfig->readVersion();
}

void Dialog::on_update_firmware_button_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    if(fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        qDebug() << "path" << path;
        _rtkConfig->updateFile(path);
     }
}

void Dialog::statusStrShow(QString &status)
{
     ui->label_displayVersion->setText(status);
}

void Dialog::on_pushButton_erase_clicked()
{
     _rtkConfig->sendErase();
}

void Dialog::on_pushButton_reset_clicked()
{
    _rtkConfig->sendReset();
}

void Dialog::on_pushButton_readVersion_clicked()
{

}

void Dialog::on_pushButton_uploadFirmware_clicked()
{

}

void Dialog::on_pushButton_readDeviceID_clicked()
{

}

void Dialog::on_pushButton_configDeviceID_clicked()
{

}

void Dialog::on_pushButton_readAcount_clicked()
{

}

void Dialog::on_pushButton_configAcount_clicked()
{

}
