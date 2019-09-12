#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QDate>
#include <QFileDialog>
#include "packet.h"

Dialog * Dialog::_instance = 0;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    _rtkConfig(new RtkConfig)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString serialPortItemName = info.portName();
        ui->comboBox_serialPort->addItem(info.description()+ '(' + serialPortItemName + ')');
        serialPortNameList.append(serialPortItemName);
    }

    connect(_rtkConfig, &RtkConfig::sendStatusStr, this, &Dialog::showStatus);
    connect(_rtkConfig, &RtkConfig::sendDeviceIdStr, this, &Dialog::showDeviceID);
    connect(_rtkConfig, &RtkConfig::sendAcountStr, this, &Dialog::showAcount);
    connect(_rtkConfig, &RtkConfig::sendProgressValue, this, &Dialog::showProgress);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::showProgress(int value)
{
    ui->progressBar_upload->setValue(value);
}

Dialog* Dialog::getInstance()
{
    if (_instance == NULL)  {
        _instance = new Dialog();
    }
    return _instance;
}

void Dialog::getSerialName(QString &name)
{
    int i = ui->comboBox_serialPort->currentIndex();
    name = serialPortNameList.at(i);
    qDebug() << name;
}

void Dialog::showStatus(QString &status)
{
     ui->label_displayVersion->setText(status);
}

void Dialog::showDeviceID(QString &id)
{
     ui->lineEdit_deviceID->setText(id);
}

void Dialog::showAcount(QList<QString> &acount)
{
    if (acount.size()>=3 ) {
        ui->lineEdit_acountKey->setText(acount.at(0));
        ui->lineEdit_acountSecret->setText(acount.at(1));
        ui->lineEdit_acountType->setText(acount.at(2));
    }
}

void Dialog::on_pushButton_readVersion_clicked()
{
    _rtkConfig->readVersion();
}

void Dialog::on_pushButton_uploadFirmware_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    if(fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        qDebug() << "path" << path;
        _rtkConfig->updateFile(path);
    }
}

void Dialog::on_pushButton_readDeviceID_clicked()
{
    _rtkConfig->readDeviceId();
}

void Dialog::on_pushButton_configDeviceID_clicked()
{
    QString id = ui->lineEdit_deviceID->displayText();
    _rtkConfig->setDeviceID(id);
}

void Dialog::on_pushButton_readAcount_clicked()
{
     _rtkConfig->readAcount();
}

void Dialog::on_pushButton_configAcount_clicked()
{
    QString acount;
    acount= ui->lineEdit_acountKey->displayText() + ','
            + ui->lineEdit_acountSecret->displayText() + ',' + ','
            + ui->lineEdit_acountType->displayText();
   _rtkConfig->setAcount(acount);
}
