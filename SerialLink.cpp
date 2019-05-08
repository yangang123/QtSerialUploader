#include "SerialLink.h"

SerialLink::SerialLink():
    mSerialPort(new QSerialPort)
{
     //QObject::connect(mSerialPort, &QIODevice::readyRead, this, &SerialLink::_readBytes);
}

bool SerialLink::connectLink()
{
    if (mIsOpen) {
         qDebug() << "portName close";
         mSerialPort->close();
    }
    mSerialPort->setPortName(portName);
    mSerialPort->setBaudRate(QSerialPort::Baud115200);
    mSerialPort->setParity(QSerialPort::NoParity);
    mSerialPort->setStopBits(QSerialPort::OneStop);

    mSerialPort->open(QSerialPort::ReadWrite);
    mIsOpen = mSerialPort->isOpen();
    if (mIsOpen) {
        qDebug() << "portName open" << "openDate:" << QDate::currentDate();
        start();
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
            //emit bytesReceived(this, buffer);
           // _config->receiveBytes(this, buffer);

            char *buf = buffer.data();
            for (int i = 0; i < buffer.size(); i++ ) {
                 if(packet_parse_data_callback(buf[i], &mPacket)) {
                     quint8 cmd = mPacket.data[0];

                     switch(cmd) {
                     case FW_UPDATE_OK:
                          qDebug() << "OK";
                          packetReplyOk = true;
                          break;

                     case FW_UPDATE_VERREPLY:
                         QString output1;
                         output1.append(mPacket.data[13]);
                         output1.append('.');
                         output1.append(mPacket.data[15]);
                         output1.append('.');
                         output1.append(mPacket.data[16]);
                         qDebug() << output1;
                         emit sendStatusStr(output1);
                         break;
                      }
                 }
            }
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

void SerialLink::run()
{
    while(1) {
        QThread::sleep(1);
        _readBytes();
        qDebug() << "run";
    }
}

