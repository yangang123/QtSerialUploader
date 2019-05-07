#ifndef LINKINTERFACE_H
#define LINKINTERFACE_H

#include <QObject>
#include <QThread>

class LinkInterface : public QThread
{
    Q_OBJECT
public:
    LinkInterface():
    QThread(0)
    {
    }
};

#endif // LINKINTERFACE_H
