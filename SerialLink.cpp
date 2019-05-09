#include "SerialLink.h"

SerialLink::SerialLink():
    mSerialPort(new QSerialPort)
{
     QObject::connect(mSerialPort, &QIODevice::readyRead, this, &SerialLink::_readBytes);
}

bool SerialLink::connectLink(QString &name)
{
    if (mIsOpen) {
         qDebug() << "portName close";
         mSerialPort->close();
    }
    mSerialPort->setPortName(name);
    mSerialPort->setBaudRate(QSerialPort::Baud115200);
    mSerialPort->setParity(QSerialPort::NoParity);
    mSerialPort->setStopBits(QSerialPort::OneStop);

    mSerialPort->open(QSerialPort::ReadWrite);
    mIsOpen = mSerialPort->isOpen();
    if (mIsOpen) {
        qDebug() << "portName open" << "openDate:" << QDate::currentDate();
        return true;
    }
    qDebug() << "portName close";
    return false ;
}

void SerialLink::_readBytes(void)
{
    if (mSerialPort && mSerialPort->isOpen()) {
        qint64 byteCount = mSerialPort->bytesAvailable();
        if (byteCount) {
            QByteArray buffer;
            buffer.resize(byteCount);
            mSerialPort->read(buffer.data(), buffer.size());
            qDebug() << "count" << byteCount;
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


