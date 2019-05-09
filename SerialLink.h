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

class SerialLink : public LinkInterface
{
    Q_OBJECT
public:
    explicit SerialLink();
    bool connectLink(QString &name);

signals:
      void bytesReceived(LinkInterface *link, QByteArray data);

public slots:
      void _readBytes();
      void writeBytes(const char* data, qint64 length);

private:
      QSerialPort *mSerialPort;
      QString portName;
      bool mIsOpen;
      bool mFirstOpen;
};

#endif // SERIALLINK_H
