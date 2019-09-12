#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDialog>
#include <QSerialPort>
#include <QThread>
#include <QTimer>

#include "packet.h"
#include "RtkConfig.h"

namespace Ui {
class MainWindow;
}

class RtkConfig;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void getSerialName(QString &name);
    static MainWindow* getInstance();

private slots:
    void on_pushButton_readVersion_clicked();

    void on_pushButton_uploadFirmware_clicked();

    void on_pushButton_readDeviceID_clicked();

    void on_pushButton_configDeviceID_clicked();

    void on_pushButton_readAcount_clicked();

    void on_pushButton_configAcount_clicked();

    void showStatus(QString &status);
    void showDeviceID(QString &id);
    void showAcount(QList<QString> &acount);
    void showProgress(int value);
    void showConfigDeviceID(void);
private:
    Ui::MainWindow *ui;
    RtkConfig *_rtkConfig;
    static MainWindow * _instance;
    QList<QString> serialPortNameList;
};

#endif // MAINWINDOW_H
