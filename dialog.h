#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "packet.h"
#include <QThread>

namespace Ui {
class Dialog;
}

class MasterThread : public QThread
{
    Q_OBJECT

public:
    MasterThread(QObject *parent = 0);
    ~MasterThread();

//    void transaction(const QString &portName, int waitTimeout, const QString &request);
    void run();
    void set_cmd(const QString &s);

signals:
//    void response(const QString &s);
//    void error(const QString &s);
//    void timeout(const QString &s);

private:
//    QString portName;
//    QString request;
//    int waitTimeout;
//    QMutex mutex;
//    QWaitCondition cond;
    bool quit;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    int send_file(char *name);

private slots:
    void on_open_button_clicked();

    void on_read_version_button_clicked();

    void on_update_firmware_button_clicked();

private:
    Ui::Dialog *ui;
    QSerialPort *mSerialPort;
    QString portName;
    bool mIsOpen;
    bool mFirstOpen;
    packet_desc_t mPacket;
    MasterThread mThread;

//    int _fd = -1;
//    uint8_t buf[128];
//    bool req = false;
//    bool ack = false;
//    packet_desc_t packet;
    fw_packet_t _firmware_data;

};

#endif // DIALOG_H
