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

class SerialLink;

class RtkConfig : public QObject
{
    Q_OBJECT
public:
    explicit RtkConfig(QObject *parent = 0);
    void readVersion();
    void updateFile(QString &path);
    void sendErase();
    void sendReset();
    void open_link(QString &name);

signals:
    void sendStatusStr(QString &status);
    void bytesReceived(QByteArray data);

public slots:
    void update();
    void receiveBytes(QByteArray b);

private:
    int  send_file(void);
    void send_firmwre_file_one_packet(char*p, qint16 len);
    void send_firmwre_file_packet();
    void send_firmwre_file_last_packet();
    void send_firmwre_file();

    bool sendResetCmdFormBootloader();

    SerialLink *_link;

    packet_desc_t mPacket;
    QByteArray buffer_read;
    qint16 last_packet;
    QTimer *mTimer;
    bool update_req;
    qint16 update_i;
    fw_packet_t _firmware_data;
    bool packetReplyOk;
};

#endif // RTKCONFIG_H
