#ifndef RTKCONFIG_H
#define RTKCONFIG_H

#include <QObject>
#include <QDialog>
#include <QSerialPort>
#include "packet.h"
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QVariant>
#include <QLoggingCategory>

#include "mainwindow.h"
#include "SerialLink.h"

Q_DECLARE_LOGGING_CATEGORY(RtkConfigLog)

class SerialLink;
class RtkConfig : public QObject
{
    Q_OBJECT
public:
    explicit RtkConfig(QObject *parent = 0);
    void readVersion() {_sendOnePacket(FW_UPDATE_VERREQ);}
    void readDeviceId(){_sendOnePacket(FW_UPDATE_GET_DEVICE_ID);}
    void readAcount(){_sendOnePacket(FW_UPDATE_ACCOUNT_INFO_GET);}
    void updateFile(QString &path);
    void sendErase(){_sendOnePacket(FW_UPDATE_ERASE);}
    void setDeviceID(QString &id);
    void setAcount(QString &acount);
    void sendReset();
    void linkUpdate(QString &name);

signals:
    void sendStatusStr(QString &status);
    void sendDeviceIdStr(QString &id);
    void sendAcountStr( QList<QString> &acount);
    void sendProgressValue(int value);
    void sendConfigDeviceID();
    void sendSerialPortInfo(QString &portInfo);

public slots:
    void update();
    void receiveBytes(LinkInterface *link, QByteArray b);


private:
    void _sendFirmwareFileOnePacket(char*p, qint16 len);
    void _sendFirmwareFilePacket();
    void _sendFirmwareFileLastPacket();
    void _sendFirmwareFile();
    void _sendOnePacket(qint8 cmd);
    void _sendOnePacket(qint8 cmd, QByteArray &playload);
    void _sendResetCmdFormBootloader(){_configDeviceIdStatusEnable = true;_sendOnePacket(FW_UPDATE_RESET);}
    void _sendPacket(uint8_t cmd, QByteArray &playload, QByteArray &buf);

    SerialLink*         _link;
    packet_desc_t       _packet;
    QByteArray          _bufferRead;
    qint16              _lastPacket;
    QTimer*             _timer;
    QTimer*             _linkTimer;
    qint8               _updateReq;
    qint16              _updateIndex;
    fw_packet_t         _firmwareData;
    bool                _packetReplyOk;
    bool                _configDeviceIdStatusEnable;
};

#endif // RTKCONFIG_H
