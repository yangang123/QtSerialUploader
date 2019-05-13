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

#include "mainwindow.h"
#include "SerialLink.h"

class SerialLink;

class RtkConfig : public QObject
{
    Q_OBJECT
public:
    explicit RtkConfig(QObject *parent = 0);
    void readVersion() {sendOnePacket(FW_UPDATE_VERREQ);}
    void readDeviceId(){sendOnePacket(FW_UPDATE_GET_DEVICE_ID);}
    void readAcount(){sendOnePacket(FW_UPDATE_ACCOUNT_INFO_GET);}
    void updateFile(QString &path);
    void sendErase(){sendOnePacket(FW_UPDATE_ERASE);}
    void setDeviceID(QString &id);
    void setAcount(QString &acount);
    void sendReset();

signals:
    void sendStatusStr(QString &status);
    void sendDeviceIdStr(QString &id);
    void sendAcountStr( QList<QString> &acount);
    void sendProgressValue(int value);
    void sendConfigDeviceID();

public slots:
    void update();
    void receiveBytes(LinkInterface *link, QByteArray b);

private:
    void sendFirmwareFileOnePacket(char*p, qint16 len);
    void sendFirmwareFilePacket();
    void sendFirmwareFileLastPacket();
    void sendFirmwareFile();
    void sendOnePacket(qint8 cmd);
    void sendOnePacket(qint8 cmd, QByteArray &playload);
    void sendResetCmdFormBootloader(){configDeviceIdStatusEnable = true;sendOnePacket(FW_UPDATE_RESET);}
    void sendPacket(uint8_t cmd, QByteArray &playload, QByteArray &buf);

    SerialLink *_link;
    packet_desc_t mPacket;
    QByteArray buffer_read;
    qint16 last_packet;
    QTimer *_timer;
    qint8 update_req;
    qint16 update_i;
    fw_packet_t _firmware_data;
    bool packetReplyOk;
    bool configDeviceIdStatusEnable;
};

#endif // RTKCONFIG_H
