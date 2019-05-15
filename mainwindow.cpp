
#include <QSerialPortInfo>
#include <QDebug>
#include <QDate>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_LOGGING_CATEGORY(MainWindowLog, "MainWindowLog")

MainWindow * MainWindow::_instance = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _rtkConfig(new RtkConfig)
{
    ui->setupUi(this);

    _update = new QTimer(this);

    connect(_rtkConfig, &RtkConfig::sendStatusStr, this, &MainWindow::showStatus);
    connect(_rtkConfig, &RtkConfig::sendDeviceIdStr, this, &MainWindow::showDeviceID);
    connect(_rtkConfig, &RtkConfig::sendAcountStr, this, &MainWindow::showAcount);
    connect(_rtkConfig, &RtkConfig::sendProgressValue, this, &MainWindow::showProgress);
    connect(_rtkConfig, &RtkConfig::sendConfigDeviceID, this, &MainWindow::showConfigDeviceID);

    connect(_update, SIGNAL(timeout()), this, SLOT(autoUpdateLink()));

    autoUpdateLink();

    _update->start(1000);

    //定义log
    QLoggingCategory::setFilterRules(QStringLiteral("MainWindowLog.debug=false"));

}

void MainWindow::autoUpdateLink()
{
    QList<QSerialPortInfo>      serialPortNameList1;
    bool sync = false;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString str = info.portName() + " (" + info.description() + ")";
        if (ui->comboBox_serialPort->findText(str) == -1) {
           sync = true;
        }
        serialPortNameList1.append(info);
    }

    if (sync || ui->comboBox_serialPort->count() != serialPortNameList1.size()){
        QString text = ui->comboBox_serialPort->currentText();
        bool edited = ui->comboBox_serialPort->findText(text) == -1;
        ui->comboBox_serialPort->clear();
        foreach(const QSerialPortInfo &name1, serialPortNameList1) {
            QString name = name1.portName() + " (" + name1.description() + ")";
            ui->comboBox_serialPort->addItem(name);
        }

        if (!text.isEmpty() && (ui->comboBox_serialPort->findText(text) != -1 || edited)) {
            ui->comboBox_serialPort->setCurrentText(text);
        } else {
            ui->comboBox_serialPort->setCurrentIndex(0);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showProgress(int value)
{
    ui->progressBar_upload->setValue(value);
}

MainWindow* MainWindow::getInstance()
{
    if (_instance == NULL)  {
        _instance = new MainWindow();
    }
    return _instance;
}

void MainWindow::getSerialName(QString &name)
{
    QString portName = ui->comboBox_serialPort->currentText();
    name = portName.section(' ', 0, 0);
}

void MainWindow::showStatus(QString &status)
{
     ui->label_displayVersion->setText(status);
}

void MainWindow::showDeviceID(QString &id)
{
     ui->lineEdit_deviceID->setText(id);
}

void MainWindow::showAcount(QList<QString> &acount)
{
    if (acount.size()>=3 ) {
        ui->lineEdit_acountKey->setText(acount.at(0));
        ui->lineEdit_acountSecret->setText(acount.at(1));
        ui->lineEdit_acountType->setText(acount.at(2));
    }
}

void MainWindow::showConfigDeviceID()
{
    QMessageBox::information(this, tr("提示"), tr("成功"));
}

void MainWindow::on_pushButton_readVersion_clicked()
{
    _rtkConfig->readVersion();
}

void MainWindow::on_pushButton_uploadFirmware_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    if(fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        qCDebug(MainWindowLog) << "path" << path;
        _rtkConfig->updateFile(path);
    }
}

void MainWindow::on_pushButton_readDeviceID_clicked()
{
    _rtkConfig->readDeviceId();
}

void MainWindow::on_pushButton_configDeviceID_clicked()
{
    QString id = ui->lineEdit_deviceID->displayText();
    _rtkConfig->setDeviceID(id);
}

void MainWindow::on_pushButton_readAcount_clicked()
{
    _rtkConfig->readAcount();
}

void MainWindow::on_pushButton_configAcount_clicked()
{
    QString acount;
    acount= ui->lineEdit_acountKey->displayText() + ','
            + ui->lineEdit_acountSecret->displayText() + ',' + ','
            + ui->lineEdit_acountType->displayText();
   _rtkConfig->setAcount(acount);
}


void MainWindow::on_comboBox_serialPort_currentIndexChanged(int index)
{
    qCDebug(MainWindowLog) << "index" << index;
    QString name = ui->comboBox_serialPort->currentText();
    _rtkConfig->linkUpdate(name);
}
