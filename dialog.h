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


private slots:
    void on_open_button_clicked();
    void on_read_version_button_clicked();
    void on_update_firmware_button_clicked();

    void statusStrShow(QString &status);

private:
    Ui::Dialog *ui;
    RtkConfig *_rtkConfig;
    SerialLink *_serialLink;
};



#endif // DIALOG_H
