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

class RtkConfig : public QObject
{
    Q_OBJECT
public:
    explicit RtkConfig(QObject *parent = 0);
    SerialLink *_link;
    void readVersion();

signals:
    void sendStatusStr(QString &status);


public slots:
    void update();
    void receiveBytes(LinkInterface* link, QByteArray b);

private:
    int  send_file(void);
    void send_firmwre_file_one_packet(char*p, qint16 len);
    void send_firmwre_file_packet();
    void send_firmwre_file_last_packet();
    void send_firmwre_file();
    void sendReset();
    bool sendResetCmdFormBootloader();

    packet_desc_t mPacket;
    QByteArray buffer_read;
    qint16 last_packet;
    QTimer *mTimer;
    bool update_req;
    qint16 update_i;
    fw_packet_t _firmware_data;

    QString portName;
    bool mIsOpen;
    bool mFirstOpen;


};

#endif // RTKCONFIG_H
