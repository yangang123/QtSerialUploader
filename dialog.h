#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "packet.h"
#include <QThread>
#include <QTimer>
#include "RtkConfig.h"
#include "SerialLink.h"

namespace Ui {
class Dialog;
}
class RtkConfig;
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
     RtkConfig *_rtkConfig;

private slots:
    void on_open_button_clicked();
    void on_read_version_button_clicked();
    void on_update_firmware_button_clicked();

    void statusStrShow(QString &status);


    void on_pushButton_erase_clicked();

    void on_pushButton_reset_clicked();

    void on_pushButton_readVersion_clicked();

    void on_pushButton_uploadFirmware_clicked();

    void on_pushButton_readDeviceID_clicked();

    void on_pushButton_configDeviceID_clicked();

    void on_pushButton_readAcount_clicked();

    void on_pushButton_configAcount_clicked();

private:
    Ui::Dialog *ui;
};



#endif // DIALOG_H
