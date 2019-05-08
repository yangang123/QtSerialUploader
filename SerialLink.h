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
#include "RtkConfig.h"

class RtkConfig;
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


class SerialLink : public QThread
{
    Q_OBJECT
public:
    explicit SerialLink();
    explicit SerialLink(SerialConfiguration *config);
    bool connectLink();
    void run();

    QSerialPort *mSerialPort;
    QString portName;
    bool mIsOpen;
    bool mFirstOpen;
    RtkConfig *_config;
    void setRkConfig(RtkConfig *config)
    {
        _config = config;
    }

public slots:
      void _readBytes();
      void writeBytes(const char* data, qint64 length);

signals:
      void bytesReceived(LinkInterface* link, QByteArray data);

public slots:

private:

};

#endif // SERIALLINK_H
