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
    _rtkConfig(new RtkConfig),
    _serialLink(0)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString serialPortItemName = info.manufacturer();
        serialPortItemName = info.portName();
        ui->serial_box->addItem(serialPortItemName);
    }

   connect(_rtkConfig, &RtkConfig::sendStatusStr, this, &Dialog::statusStrShow);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_open_button_clicked()
{
     if (_serialLink == 0) {
         _serialLink = new SerialLink();
     }
     _serialLink->portName = ui->serial_box->currentText();
     if (!_serialLink->connectLink()) {
        qDebug() <<  "link open error";
        return;
     }
     _rtkConfig->_link  = _serialLink;
     _serialLink->setRkConfig(_rtkConfig);
     ui->status_display->setText("open ok");
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
     ui->version_display->setText(status);
}

void Dialog::on_pushButton_erase_clicked()
{
     _rtkConfig->sendErase();
}

void Dialog::on_pushButton_reset_clicked()
{
    _rtkConfig->sendReset();
}
