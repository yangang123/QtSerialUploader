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
    void getSerialName(QString &name);
    static Dialog* getInstance();

private slots:
    void showStatus(QString &status);
    void showDeviceID(QString &id);
    void showAcount(QList<QString> &acount);




    void on_pushButton_readVersion_clicked();

    void on_pushButton_uploadFirmware_clicked();

    void on_pushButton_readDeviceID_clicked();

    void on_pushButton_configDeviceID_clicked();

    void on_pushButton_readAcount_clicked();

    void on_pushButton_configAcount_clicked();

private:
    void on_pushButton_erase_clicked();

    void on_pushButton_reset_clicked();
    Ui::Dialog *ui;
    RtkConfig *_rtkConfig;
    static Dialog * _instance;
};

extern Dialog* uploadApp(void);


#endif // DIALOG_H
