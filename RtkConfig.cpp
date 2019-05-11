#include "RtkConfig.h"
#include "crc16.h"
RtkConfig::RtkConfig(QObject *parent) :
    QObject(parent),
    _timer (new QTimer()),
    update_req(false),
    update_i(0)
{
    _link = new SerialLink();
    memset((void*)&mPacket, 0x00, (size_t)sizeof(packet_desc_t));
    memset((void*)&_firmware_data, 0x00, (size_t)sizeof(fw_packet_t));

    QObject::connect(_link,&SerialLink::bytesReceived, this, &RtkConfig::receiveBytes);
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
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
    _link->writeBytes((const char*)buffer_ret, tx_len);
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
    if (packetReplyOk) {
        if (update_req ==1) {
            update_req = 2;
            qDebug() << "replay";
            update_i = 0;
        }
        packetReplyOk = false;
    }

    if (!(update_req==2)){
        return;
    }

    if (update_i < (_firmware_data.total_block -1)) {
        send_firmwre_file_packet();
        QString output1 = QString("total_block:%1.").arg((int)_firmware_data.total_block);
        QString output2 = QString("cur_block:%1.").arg((int)_firmware_data.cur_block);
        QString output3 = QString("block_len:%1").arg((int)_firmware_data.block_len);
        output1 +=  output2;
        output1 +=  output3;
        int value = ((float)_firmware_data.cur_block/_firmware_data.total_block) *100;
        emit sendProgressValue(value);
    } else if(update_i == _firmware_data.total_block){
        send_firmwre_file_last_packet();
        update_req = false;
        update_i = 0;
        sendResetCmdFormBootloader();
        return;
    }

    update_i++;
}

bool RtkConfig::sendResetCmdFormBootloader()
{

    quint8 buffer[20];
    quint16 tx_len =0;

    pakect_send(FW_UPDATE_RESET, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    _link->writeBytes((const char*)buffer, tx_len);
    return true;
}

void RtkConfig::sendReset()
{
   char* command = "reboot\r\n";
   _link->writeBytes((const char*)command, 8);
}

void RtkConfig::sendErase()
{
    qDebug() << "erase";
    quint8 buffer[20];
    quint16 tx_len =0;
    pakect_send(FW_UPDATE_ERASE, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    _link->writeBytes((const char*)buffer, tx_len);
}

void RtkConfig::sendOnePacket(qint8 cmd)
{
    if(!_link->isConnect()) {
       QString name;
       Dialog::getInstance()->getSerialName(name);
       if (!_link->connectLink(name)) {
            return;
       }
    }

    quint8 buffer[20];
    quint16 tx_len =0;
    pakect_send(cmd, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    _link->writeBytes((const char*)buffer, tx_len);
}

void RtkConfig::_packetSend(uint8_t cmd, QByteArray &playload, QByteArray &buf)
{
    buf.resize(7);
    int length = playload.size();

    buf[FW_START1_POS] = FW_HEAD_BYTE_1;
    buf[FW_START2_POS] = FW_HEAD_BYTE_2;
    buf[FW_DST_POS]    = 0;
    buf[FW_SRC_POS]    = 0;

    buf[FW_LEN_L_POS]  = (length+1) & 0xff;
    buf[FW_LEN_H_POS]  = ((length+1) >> 8) & 0xff;
    buf[FW_CMD_POS]    = cmd;

    if (length) {
       buf += playload;
    }

    uint16_t cksum_pos = FW_PAKET_HEAD_LEN+length;
    uint16_t cksum;
    cksum = calc_crc((const uint8_t*)buf.data(), FW_PAKET_HEAD_LEN+length);
    buf[cksum_pos]   = cksum & 0x00ff;
    buf[cksum_pos+1] = (cksum >> 8) & 0x00ff;
    buf[cksum_pos+2] = FW_END_BYTE_1;
    buf[cksum_pos+3] = FW_END_BYTE_2;
}

void RtkConfig::sendOnePacket(qint8 cmd, QByteArray &playload)
{
    if(!_link->isConnect()) {
       QString name;
       Dialog::getInstance()->getSerialName(name);
       if (!_link->connectLink(name)) {
            return;
       }
    }
    QByteArray new_buffer ;
    _packetSend(cmd, playload, new_buffer);
    _link->writeBytes((const char*)new_buffer.data(), new_buffer.size());
}

void RtkConfig::setAcount(QString &acount)
{
    acount.prepend(acount.size());
    QByteArray a = acount.toLatin1();
    sendOnePacket(FW_UPDATE_ACCOUNT_INFO_SET, a);
}

void RtkConfig::updateFile(QString &path)
{
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

    sendReset();
    QThread::msleep(500);
    packetReplyOk = false;
    update_req = 1;
    sendErase();
     _timer->start(200);
}

void RtkConfig::setDeviceID(QString &id)
{
    qDebug() << id << "size" << id.size();
    int appendLen= 10 - id.size();
    qDebug() << id << id.size();

    QByteArray a;
    a = id.toLatin1();
    for (qint8 i =0; i < appendLen; i++) {
        a.append((char)0);
    }
    sendOnePacket(FW_UPDATE_SET_DEVICE_ID, a);
}

void RtkConfig::receiveBytes(LinkInterface *link, QByteArray b)
{
    qDebug()<< "size" << b.size();

    QString output1;
    QList<QString> acount;
    QString deviceID;
    char *buf = b.data();
    for (int i = 0; i < b.size(); i++ ) {
        if(packet_parse_data_callback(buf[i], &mPacket)) {
            quint8 cmd = mPacket.data[0];
            switch(cmd) {
                case FW_UPDATE_OK:
                qDebug() << "OK";
                packetReplyOk = true;
                break;

            case FW_UPDATE_VERREPLY:
                output1.append(mPacket.data[13]);
                output1.append('.');
                output1.append(mPacket.data[15]);
                output1.append('.');
                output1.append(mPacket.data[16]);
                qDebug() << output1;
                emit sendStatusStr(output1);
                break;

            case FW_UPDATE_REPLY_DEVICE_ID:
                for (qint8 i = 0; i < 10; i++ ){
                if (mPacket.data[1+i] != '\0') {
                    deviceID.append(mPacket.data[1+i]);
                    }
                }
                qDebug() << deviceID;
                emit sendDeviceIdStr(deviceID);
                break;

            case FW_UPDATE_ACCOUNT_INFO_REPLY:
                mPacket.data[2] = mPacket.data[2];
                for (int i = 0; i < mPacket.data[1]; i++) {
                    if (mPacket.data[2+i] != ',') {
                        deviceID.append(mPacket.data[2+i]);
                    } else if (mPacket.data[2+i] == ','){
                        if (deviceID.size() > 0) {
                            acount.append(deviceID);
                            deviceID.clear();
                        }
                    }
                }
                if (deviceID.size() > 0) {
                    acount.append(deviceID);
                    deviceID.clear();
                }
                emit sendAcountStr(acount);
//                for (qint8 i = 0; i< acount.size(); i++) {
//                    qDebug() << acount.at(i);
//                }
                break;
            }
       }
   }
}
