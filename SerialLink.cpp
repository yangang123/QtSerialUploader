#include "SerialLink.h"

SerialLink::SerialLink():
    mSerialPort(new QSerialPort)
{
     QObject::connect(mSerialPort, &QIODevice::readyRead, this, &SerialLink::_readBytes);
}

bool SerialLink::connectLink(QString &name)
{
    if (isOpen) {
         qDebug() << "port already open";
         mSerialPort->close();
    }
    mSerialPort->setPortName(name);
    mSerialPort->setBaudRate(QSerialPort::Baud115200);
    mSerialPort->setParity(QSerialPort::NoParity);
    mSerialPort->setStopBits(QSerialPort::OneStop);

    mSerialPort->open(QSerialPort::ReadWrite);
    isOpen = mSerialPort->isOpen();
    if (isOpen) {
        qDebug() << "portName open" << "openDate:" << QDate::currentDate();
        return true;
    }
    qDebug() << "portName close";
    return false ;
}

bool SerialLink::disconnect()
{
    if (isOpen) {
         qDebug() << "portName close";
         mSerialPort->close();
         isOpen = false;
    }
    return true;
}

bool SerialLink::isConnect()
{
    return isOpen;
}

void SerialLink::_readBytes(void)
{
    if (mSerialPort && mSerialPort->isOpen()) {
        qint64 byteCount = mSerialPort->bytesAvailable();
        if (byteCount) {
            QByteArray buffer;
            buffer.resize(byteCount);
            mSerialPort->read(buffer.data(), buffer.size());
            emit bytesReceived(this, buffer);
        }
    }
}

void SerialLink::writeBytes(const char* data, qint64 size)
{
    if(mSerialPort && mSerialPort->isOpen()) {
        mSerialPort->write(data, size);
        mSerialPort->flush();
    }
}


