#ifndef SERIALLINK_H
#define SERIALLINK_H

#include <QObject>


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
    explicit SerialLink(SerialConfiguration *config);

signals:

public slots:

};

#endif // SERIALLINK_H
