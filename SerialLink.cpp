#include "SerialLink.h"

Q_LOGGING_CATEGORY(SerialLinkLog, "SerialLinkLog")

SerialLink::SerialLink():
    _port(new QSerialPort),
    _isOpen(false)
{
     QObject::connect(_port, &QIODevice::readyRead, this, &SerialLink::_readBytes);

     QLoggingCategory::setFilterRules(QStringLiteral("SerialLinkLog.debug=false"));
     QLoggingCategory::setFilterRules(QStringLiteral("SerialLinkLog.info=false"));
     QLoggingCategory::setFilterRules(QStringLiteral("SerialLinkLog.warning=false"));
}

bool SerialLink::connectLink(QString &name)
{
    if (_isOpen) {
         qCWarning(SerialLinkLog) << "port already open";
         _port->close();
    }
    _port->setPortName(name);
    _port->setBaudRate(QSerialPort::Baud115200);
    _port->setParity(QSerialPort::NoParity);
    _port->setStopBits(QSerialPort::OneStop);

    _port->open(QSerialPort::ReadWrite);
    _isOpen = _port->isOpen();
    if (_isOpen) {
        qCInfo(SerialLinkLog) << "portName open" << "openDate:" << QDate::currentDate();
        return true;
    }
    qCWarning(SerialLinkLog) << "portName close";
    return false ;
}

bool SerialLink::disconnectLink()
{
    if (_isOpen) {
         qCInfo(SerialLinkLog) << "portName close";
         _port->close();
         _isOpen = false;
    }
    return true;
}

void SerialLink::linkUpdate(QString &name)
{
    //没有使用name,link更新,仅仅做了关闭链路的过程
    name = name;
    disconnectLink();
}

bool SerialLink::isConnect()
{
    return _isOpen;
}

void SerialLink::_readBytes(void)
{
    if (_port && _port->isOpen()) {
        qint64 byteCount = _port->bytesAvailable();
        if (byteCount) {
            QByteArray buffer;
            buffer.resize(byteCount);
            _port->read(buffer.data(), buffer.size());
            emit bytesReceived(this, buffer);
        }
    }
}

void SerialLink::writeBytes(const char* data, qint64 size)
{
    if(_port && _port->isOpen()) {
        _port->write(data, size);
        _port->flush();
    }
}


