#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "packet.h"
#include <QThread>
#include <QTimer>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    int send_file(void);
    void send_firmwre_file_one_packet(char*p, qint16 len);
    void send_firmwre_file_packet();
    void send_firmwre_file_last_packet();
    void send_firmwre_file();

private slots:
    void on_open_button_clicked();
    void on_read_version_button_clicked();
    void on_update_firmware_button_clicked();
    void update();

private:
    Ui::Dialog *ui;
    QSerialPort *mSerialPort;
    QString portName;
    bool mIsOpen;
    bool mFirstOpen;
    packet_desc_t mPacket;
    QByteArray buffer_read;
    qint16 last_packet;
    QTimer *mTimer;
    bool update_req;
    qint16 update_i;
    fw_packet_t _firmware_data;
};

#endif // DIALOG_H
