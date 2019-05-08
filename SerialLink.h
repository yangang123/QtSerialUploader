#ifndef SERIALLINK_H
#define SERIALLINK_H

#include <QObject>
#include <QDialog>
#include <QSerialPort>
#include "packet.h"
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QDate>

#include "LinkInterface.h"

class SerialConfiguration
{
public:
    SerialConfiguration(const QString& name);


private:
    int _baud;
    int _dataBits;
    int _flowControl;
    int _stopBits;
    int _parity;
    QString _portName;
    QString _name;
};


class SerialLink : public LinkInterface
{
    Q_OBJECT
public:
    explicit SerialLink();
    explicit SerialLink(SerialConfiguration *config);
    bool connectLink();

    QSerialPort *mSerialPort;
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

public slots:
      void _readBytes();

signals:
      void bytesReceived(LinkInterface* link, QByteArray data);

public slots:

private:

};

#endif // SERIALLINK_H
