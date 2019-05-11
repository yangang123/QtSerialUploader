#ifndef RTKCONFIG_H
#define RTKCONFIG_H

#include <QObject>
#include <QDialog>
#include <QSerialPort>
#include "packet.h"
#include <QThread>
#include <QTimer>
#include <QDebug>
#include "SerialLink.h"
#include <QFile>
#include "dialog.h"
#include <QVariant>

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
    void sendErase();
    void sendReset();
    void open_link(QString &name);
    void setDeviceID(QString &id);
    void setAcount(QString &acount);

signals:
    void sendStatusStr(QString &status);
    void sendDeviceIdStr(QString &id);
    void sendAcountStr( QList<QString> &acount);

public slots:
    void update();
    void receiveBytes(LinkInterface *link, QByteArray b);

private:
    int  send_file(void);
    void send_firmwre_file_one_packet(char*p, qint16 len);
    void send_firmwre_file_packet();
    void send_firmwre_file_last_packet();
    void send_firmwre_file();
    void sendOnePacket(qint8 cmd);
    void sendOnePacket(qint8 cmd, QByteArray &playload);
    bool sendResetCmdFormBootloader();
    void _packetSend(uint8_t cmd, QByteArray &playload, QByteArray &buf);

    SerialLink *_link;
    packet_desc_t mPacket;
    QByteArray buffer_read;
    qint16 last_packet;
    QTimer *_timer;
   qint8 update_req;
    qint16 update_i;
    fw_packet_t _firmware_data;
    bool packetReplyOk;
};

#endif // RTKCONFIG_H
