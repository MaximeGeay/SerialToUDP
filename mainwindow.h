#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "udpdata.h"
#include "sensordialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
     void readData(QString sTrame);
     void clickOnStart();
     void clickOnConnect();
     void errorMsg(QString sMsg);
     void majInfo();
     void aPropos();

private:
    Ui::MainWindow *ui;
    UDPData *mUdp;
    void initCOM();
    void affConnec(bool bStatus);
    void diffData(QString sTrame);

    QString mPortName;
    QString mBaudrate;
    QString mParity;
    QString mDatabits;
    QString mStopbits;
    SensorDialog* mSensor;
    SensorDialog::ConnexionType mTypeConnec;
    int mPortIn;
    bool mDiffStatus=false;

};
#endif // MAINWINDOW_H
