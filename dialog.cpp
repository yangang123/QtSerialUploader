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
   connect(_serialLink, &SerialLink::bytesReceived, _rtkConfig,   &RtkConfig::receiveBytes);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_open_button_clicked()
{
     if (_serialLink == 0) {
         _serialLink = new SerialLink;
     }
     _serialLink->portName = ui->serial_box->currentText();
     if (!_serialLink->connectLink()) {
        qDebug() <<  "link open error";
        return;
     }
     _rtkConfig->_link  = _serialLink;
     ui->status_display->setText("open ok");
}

void Dialog::on_read_version_button_clicked()
{
    qDebug() << "readVersion";
    _rtkConfig->readVersion();
}

void Dialog::on_update_firmware_button_clicked()
{
//    if(!mIsOpen) {
//        return;
//    }

//    sendReset();
//    QThread::msleep(500);

//    quint8 buffer[20];
//    quint16 tx_len =0;

//    pakect_send(FW_UPDATE_ERASE, (quint8*)0, 0, (quint8*)buffer, &tx_len);
//    QByteArray array;
//    if (tx_len) {
//        array = QByteArray((char*)buffer, tx_len);
//    }
//    mSerialPort->write(array);
////    mThread.set_cmd("sleep");
//    if (mSerialPort->waitForReadyRead(3000)) {
//        QByteArray responseData = mSerialPort->readAll();

//        while (mSerialPort->waitForReadyRead(100))
//            responseData += mSerialPort->readAll();

//        qDebug() << "datasize" << responseData.size();
//        char *buf = responseData.data();
//        for (quint8 i = 0; i <responseData.size(); i++ ) {
//             if(packet_parse_data_callback(buf[i], &mPacket)) {
//                 quint8 cmd = mPacket.data[0];
//                 switch(cmd) {
//                 case FW_UPDATE_OK:
//                      qDebug() << "ack";
//                      ui->status_display->setText("ack");
//                      send_file();
//                      update_req = true;
//                      update_i = 0;
//                      break;
//                 }
//             }
//        }
//    } else {
//         qDebug() << "timeout";
//         ui->status_display->setText("timeout");
//    }
}



//int Dialog::send_file(void)
//{
////    QFileDialog *fileDialog = new QFileDialog(this);
////    if(fileDialog->exec() == QDialog::Accepted) {
////            QString path = fileDialog->selectedFiles()[0];
////            qDebug() << "path" << path;
////            QFile file(path);

////            if (file.open(QIODevice::ReadOnly) == true)
////            {
////                qDebug() << "file open ok";
////            }

////            buffer_read = file.readAll();
////            file.close();

////         qint64 filesize = file.size();
////        _firmware_data.total_block = (filesize+IAP_FW_DATA_LEN)/IAP_FW_DATA_LEN;
////        _firmware_data.block_len = IAP_FW_DATA_LEN;
////        _firmware_data.cur_block = 1;
////         last_packet = filesize % IAP_FW_DATA_LEN ;

////        qDebug() << "firmware total_block:" <<_firmware_data.total_block << "len" << _firmware_data.block_len;
////        qDebug() << "filesize:" << filesize  << "last_packet" << last_packet;
////    }

//    return 0;
//}

void Dialog::statusStrShow(QString &status)
{
     ui->status_display->setText(status);
}



