#include "RtkConfig.h"

RtkConfig::RtkConfig(QObject *parent) :
    QObject(parent),
    mTimer (new QTimer()),
    update_req(false),
    update_i(0)
{
    memset((void*)&mPacket, 0x00, (size_t)sizeof(packet_desc_t));
    memset((void*)&_firmware_data, 0x00, (size_t)sizeof(fw_packet_t));

    connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
    mTimer->start(200);
}

void RtkConfig::update()
{
    send_firmwre_file();
}

void RtkConfig::send_firmwre_file_one_packet(char*p, qint16 len)
{
    quint16 tx_len = 0;
    char buffer_ret[IAP_CONFIG_PACKET_BUFSIZE];
    memcpy(_firmware_data.data, p, len);

    pakect_send(FW_UPDATE_DATA, (uint8_t*)&_firmware_data,
                (quint16)(6+len), (quint8*)buffer_ret, &tx_len);

    QByteArray buf = QByteArray(buffer_ret, tx_len);
    qDebug() << "buf size" << buf.size();
    _link->mSerialPort->write(buf);
    _link->mSerialPort->flush();
    buf.clear();
}


void RtkConfig::send_firmwre_file_packet()
{
    char *p = buffer_read.data();
    char *p1 = p + (_firmware_data.cur_block-1) * 512;
    send_firmwre_file_one_packet(p1, 512);
    qDebug("total: %d, cur:%d, len:%d\r\n", _firmware_data.total_block, _firmware_data.cur_block,_firmware_data.block_len);
    _firmware_data.cur_block++;
}

void RtkConfig::send_firmwre_file_last_packet()
{
    _firmware_data.cur_block = _firmware_data.total_block;
    char *p = buffer_read.data();
    char *p1 = p + (_firmware_data.total_block-1) * 512;
    send_firmwre_file_one_packet(p1, last_packet);
}

void RtkConfig::send_firmwre_file()
{
    if (!update_req){
        return;
    }

    if (update_i < (_firmware_data.total_block -1)) {
        send_firmwre_file_packet();
        QString output1 = QString("total_block:%1.").arg((int)_firmware_data.total_block);
        QString output2 = QString("cur_block:%1.").arg((int)_firmware_data.cur_block);
        QString output3 = QString("block_len:%1").arg((int)_firmware_data.block_len);
        output1 +=  output2;
        output1 +=  output3;
        //ui->status_display->setText(output1);
        int value = ((float)_firmware_data.cur_block/_firmware_data.total_block) *100;
        //ui->progressBar->setValue(value);

    } else if(update_i == _firmware_data.total_block){
        send_firmwre_file_last_packet();
        update_req = false;
        update_i = 0;

        if (_link->mSerialPort->waitForReadyRead(3000)) {
            QByteArray responseData = _link->mSerialPort->readAll();

            while (_link->mSerialPort->waitForReadyRead(100))
                responseData += _link->mSerialPort->readAll();

            qDebug() << "datasize" << responseData.size();
            char *buf = responseData.data();
            for (quint8 i = 0; i <responseData.size(); i++ ) {
                 if(packet_parse_data_callback(buf[i], &mPacket)) {
                     quint8 cmd = mPacket.data[0];
                     switch(cmd) {
                     case FW_UPDATE_OK:

                          qDebug() << "download file ok";
                          //ui->status_display->setText("download file ok");

                          sendResetCmdFormBootloader();
                          break;
                     }
                 }
            }
        } else {
             qDebug() << "timeout";
             //ui->status_display->setText("timeout");
        }

        return;
    }

    update_i++;
}

bool RtkConfig::sendResetCmdFormBootloader()
{

    quint8 buffer[20];
    quint16 tx_len =0;

    pakect_send(FW_UPDATE_RESET, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    QByteArray array;
    if (tx_len) {
        array = QByteArray((char*)buffer, tx_len);
    }
    _link->mSerialPort->write(array);
    if (_link->mSerialPort->waitForReadyRead(3000)) {
        QByteArray responseData = _link->mSerialPort->readAll();

        while (_link->mSerialPort->waitForReadyRead(100))
            responseData += _link->mSerialPort->readAll();

        qDebug() << "datasize" << responseData.size();
        char *buf = responseData.data();
        for (quint8 i = 0; i <responseData.size(); i++ ) {
             if(packet_parse_data_callback(buf[i], &mPacket)) {
                 quint8 cmd = mPacket.data[0];
                 switch(cmd) {
                 case FW_UPDATE_OK:

                      QString str = QString("reset ok");
                      qDebug() << str;
                      emit sendStatusStr(str);
                      return true;
                 }
             }
        }
    }

    qDebug() << "timeout";
    //ui->status_display->setText("timeout");
    return false;
}

void RtkConfig::sendReset()
{

   QString command= QString("reboot\r\n");
   QByteArray array;
   array.append(command);
   _link->mSerialPort->write(array);
   _link->mSerialPort->flush();
}

void RtkConfig::readVersion()
{
    qDebug() << "readVersion";
    quint8 buffer[20];
    quint16 tx_len =0;
    pakect_send(FW_UPDATE_VERREQ, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    QByteArray array;
    if (tx_len) {
        array = QByteArray((char*)buffer, tx_len);
    }
    _link->mSerialPort->write(array);
    _link->mSerialPort->flush();
    if (_link->mSerialPort->waitForReadyRead(2000)) {
        QByteArray responseData = _link->mSerialPort->readAll();
        while (_link->mSerialPort->waitForReadyRead(10))
            responseData += _link->mSerialPort->readAll();
        qDebug() << "datasize" << responseData.size();
        char *buf = responseData.data();

        for (int i = 0; i < responseData.size(); i++ ) {

             if(packet_parse_data_callback(buf[i], &mPacket)) {
                 quint8 cmd = mPacket.data[0];

                 switch(cmd) {
                 case FW_UPDATE_ACK:
                      qDebug() << "ack";
                      break;

                 case FW_UPDATE_VERREPLY:
                    QString output1;
                    output1.append(mPacket.data[13]);
                    output1.append('.');
                    output1.append(mPacket.data[15]);
                    output1.append('.');
                    output1.append(mPacket.data[16]);
                    //ui->version_display->setText(output1);
                    break;
                 }
             }
        }
    } else {
         qDebug() << "timeout";
    }
}

void RtkConfig::receiveBytes(LinkInterface* link, QByteArray b)
{
     qDebug() << "size" << b.size();
}
