#ifndef SERIALLINK_H
#define SERIALLINK_H

#include <QObject>
#include <QDialog>
#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QDate>

#include "LinkInterface.h"
#include "packet.h"

class SerialLink : public LinkInterface
{
    Q_OBJECT
public:
    explicit SerialLink();
    bool connectLink(QString &name);
    bool disconnectLink();
    bool isConnect();

signals:
      void bytesReceived(LinkInterface *link, QByteArray data);

public slots:
      void _readBytes();
      void writeBytes(const char* data, qint64 length);

private:
      QSerialPort *_port;
      bool _isOpen;
};

#endif // SERIALLINK_H
