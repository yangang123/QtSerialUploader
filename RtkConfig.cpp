#include "RtkConfig.h"

RtkConfig::RtkConfig(QObject *parent) :
    QObject(parent),
    mTimer (new QTimer()),
    update_req(false),
    update_i(0)
{
    _link = new SerialLink();
    memset((void*)&mPacket, 0x00, (size_t)sizeof(packet_desc_t));
    memset((void*)&_firmware_data, 0x00, (size_t)sizeof(fw_packet_t));

    QObject::connect(_link, SIGNAL(bytesReceived(QByteArray)), this, SLOT(receiveBytes(QByteArray)));
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

void RtkConfig::readVersion()
{
    qDebug() << "readVersion";
    quint8 buffer[20];
    quint16 tx_len =0;
    pakect_send(FW_UPDATE_VERREQ, (quint8*)0, 0, (quint8*)buffer, &tx_len);
    _link->writeBytes((const char*)buffer, tx_len);
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

    update_req = true;
    update_i = 0;
}

void RtkConfig::open_link(QString &name)
{
    if (!_link->connectLink(name)) {
       qDebug() <<  "link open error";
    }
}

void RtkConfig::receiveBytes(QByteArray b)
{
       qDebug() << "size" << b.size();

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
                     QString output1;
                     output1.append(mPacket.data[13]);
                     output1.append('.');
                     output1.append(mPacket.data[15]);
                     output1.append('.');
                     output1.append(mPacket.data[16]);
                     qDebug() << output1;
                     emit sendStatusStr(output1);
                     break;
                  }
             }
        }
}
