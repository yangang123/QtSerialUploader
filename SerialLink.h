#ifndef SERIALLINK_H
#define SERIALLINK_H

#include <QObject>
#include <QDialog>
#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QDate>
#include <QLoggingCategory>

#include "LinkInterface.h"
#include "packet.h"

Q_DECLARE_LOGGING_CATEGORY(SerialLinkLog)

class SerialLink : public LinkInterface
{
    Q_OBJECT
public:
    explicit SerialLink();
    bool connectLink(QString &name);
    bool disconnectLink();
    bool isConnect();
    void writeBytes(const char* data, qint64 length);
    void linkUpdate(QString &name);

signals:
    void bytesReceived(LinkInterface *link, QByteArray data);

private slots:
    void _readBytes();


private:
    QSerialPort     *_port;
    bool            _isOpen;
};

#endif // SERIALLINK_H
