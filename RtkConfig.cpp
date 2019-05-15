#include "RtkConfig.h"
#include "crc16.h"

Q_LOGGING_CATEGORY(RtkConfigLog, "RtkConfigLog")

RtkConfig::RtkConfig(QObject *parent) :
    QObject(parent),
    _timer (new QTimer()),
    _linkTimer(new QTimer()),
    _updateReq(false),
    _updateIndex(0)
{
    _link = new SerialLink();
    memset((void*)&_packet, 0x00, (size_t)sizeof(packet_desc_t));
    memset((void*)&_firmwareData, 0x00, (size_t)sizeof(fw_packet_t));
    _configDeviceIdStatusEnable = false;

    QObject::connect(_link,&SerialLink::bytesReceived, this, &RtkConfig::receiveBytes);
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));

    QLoggingCategory::setFilterRules(QStringLiteral("RtkConfigLog.debug=false"));
}

void RtkConfig::linkUpdate(QString &name)
{
    _link->linkUpdate(name);
}

void RtkConfig::update()
{
    _sendFirmwareFile();
}


void RtkConfig::_sendFirmwareFileOnePacket(char*p, qint16 len)
{
    quint16 tx_len = 0;
    char buffer_ret[IAP_CONFIG_PACKET_BUFSIZE];
    memcpy(_firmwareData.data, p, len);

    pakect_send(FW_UPDATE_DATA, (uint8_t*)&_firmwareData,
                (quint16)(6+len), (quint8*)buffer_ret, &tx_len);
    _link->writeBytes((const char*)buffer_ret, tx_len);
}

void RtkConfig::_sendFirmwareFilePacket()
{
    char *p = _bufferRead.data();
    char *p1 = p + (_firmwareData.cur_block-1) * 512;
    _sendFirmwareFileOnePacket(p1, 512);
    qCDebug(RtkConfigLog) << "total:" <<_firmwareData.total_block
                          << "cur:" <<_firmwareData.cur_block  <<  "len" << _firmwareData.block_len;
    _firmwareData.cur_block++;
}

void RtkConfig::_sendFirmwareFileLastPacket()
{
    _firmwareData.cur_block = _firmwareData.total_block;
    char *p = _bufferRead.data();
    char *p1 = p + (_firmwareData.total_block-1) * 512;
    _sendFirmwareFileOnePacket(p1, _lastPacket);
}

void RtkConfig::_sendFirmwareFile()
{
    if (_packetReplyOk) {
        if (_updateReq ==1) {
            _updateReq = 2;
            qCDebug(RtkConfigLog) << "replay";
            _updateIndex = 0;
        }
        _packetReplyOk = false;
    }

    if (!(_updateReq==2)){
        return;
    }

    if (_updateIndex < (_firmwareData.total_block -1)) {
        _sendFirmwareFilePacket();
        QString output1 = QString("total_block:%1.").arg((int)_firmwareData.total_block);
        QString output2 = QString("cur_block:%1.").arg((int)_firmwareData.cur_block);
        QString output3 = QString("block_len:%1").arg((int)_firmwareData.block_len);
        output1 +=  output2;
        output1 +=  output3;
        int value = ((float)_firmwareData.cur_block/_firmwareData.total_block) *100;
        emit sendProgressValue(value);
    } else if(_updateIndex == _firmwareData.total_block){
        _sendFirmwareFileLastPacket();
        _updateReq = false;
        _updateIndex = 0;
        _sendResetCmdFormBootloader();
        return;
    }

    _updateIndex++;
}

void RtkConfig::sendReset()
{
    if(!_link->isConnect()) {
       QString name;
       MainWindow::getInstance()->getSerialName(name);
       if (!_link->connectLink(name)) {
            return;
       }
    }

    const char* command = "reboot\r\n";
    _link->writeBytes((const char*)command, 8);
}

void RtkConfig::_sendOnePacket(qint8 cmd)
{
    if(!_link->isConnect()) {
       QString name;
       MainWindow::getInstance()->getSerialName(name);
       if (!_link->connectLink(name)) {
            return;
       }
    }

    quint8 buffer[20];
    quint16 tx_len =0;
    pakect_send(cmd, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    _link->writeBytes((const char*)buffer, tx_len);
}

void RtkConfig::_sendPacket(uint8_t cmd, QByteArray &playload, QByteArray &buf)
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

void RtkConfig::_sendOnePacket(qint8 cmd, QByteArray &playload)
{
    if(!_link->isConnect()) {
       QString name;
       MainWindow::getInstance()->getSerialName(name);
       if (!_link->connectLink(name)) {
            return;
       }
    }
    QByteArray new_buffer ;
    _sendPacket(cmd, playload, new_buffer);
    _link->writeBytes((const char*)new_buffer.data(), new_buffer.size());
}

void RtkConfig::setAcount(QString &acount)
{
    acount.prepend(acount.size());
    QByteArray a = acount.toLatin1();
    _sendOnePacket(FW_UPDATE_ACCOUNT_INFO_SET, a);
}

void RtkConfig::updateFile(QString &path)
{
    QFile file(path);

    if (file.open(QIODevice::ReadOnly) == true)
    {
        qCDebug(RtkConfigLog) << "file open ok";
    }

    _bufferRead = file.readAll();
    file.close();

    qint64 filesize = file.size();
    _firmwareData.total_block = (filesize+IAP_FW_DATA_LEN)/IAP_FW_DATA_LEN;
    _firmwareData.block_len = IAP_FW_DATA_LEN;
    _firmwareData.cur_block = 1;
    _lastPacket = filesize % IAP_FW_DATA_LEN ;

    qCDebug(RtkConfigLog) << "firmware total_block:" <<_firmwareData.total_block << "len" << _firmwareData.block_len;
    qCDebug(RtkConfigLog) << "filesize:" << filesize  << "_lastPacket" << _lastPacket;

    sendReset();
    QThread::msleep(500);
    _packetReplyOk = false;
    _updateReq = 1;
    sendErase();
     _timer->start(200);
}

void RtkConfig::setDeviceID(QString &id)
{
    int appendLen= 10 - id.size();

    QByteArray a;
    a = id.toLatin1();
    for (qint8 i =0; i < appendLen; i++) {
        a.append((char)0);
    }
    _configDeviceIdStatusEnable = true;
    _sendOnePacket(FW_UPDATE_SET_DEVICE_ID, a);
}

void RtkConfig::receiveBytes(LinkInterface *link, QByteArray b)
{
    link = link;
    QString output1;
    QList<QString> acount;
    QString deviceID;
    char *buf = b.data();
    for (int i = 0; i < b.size(); i++ ) {
        if(packet_parse_data_callback(buf[i], &_packet)) {
            quint8 cmd = _packet.data[0];
            switch(cmd) {
                case FW_UPDATE_OK:
                qCDebug(RtkConfigLog) << "OK";
                _packetReplyOk = true;
                if (_configDeviceIdStatusEnable) {
                    emit sendConfigDeviceID();
                    _configDeviceIdStatusEnable = false;
                }
                break;

            case FW_UPDATE_VERREPLY:
                output1.append(_packet.data[13]);
                output1.append('.');
                output1.append(_packet.data[15]);
                output1.append('.');
                output1.append(_packet.data[16]);
                qCDebug(RtkConfigLog) << output1;
                emit sendStatusStr(output1);
                break;

            case FW_UPDATE_REPLY_DEVICE_ID:
                for (qint8 i = 0; i < 10; i++ ){
                if (_packet.data[1+i] != '\0') {
                    deviceID.append(_packet.data[1+i]);
                    }
                }
                qCDebug(RtkConfigLog) << deviceID;
                emit sendDeviceIdStr(deviceID);
                break;

            case FW_UPDATE_ACCOUNT_INFO_REPLY:
                _packet.data[2] = _packet.data[2];
                for (int i = 0; i < _packet.data[1]; i++) {
                    if (_packet.data[2+i] != ',') {
                        deviceID.append(_packet.data[2+i]);
                    } else if (_packet.data[2+i] == ','){
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
                break;
            }
       }
   }
}
