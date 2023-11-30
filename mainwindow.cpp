#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QSerialPortInfo>
#include <QMessageBox>

#define version "SerialToUDP 0.2"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(version);
    mUdp=new UDPData();
    mSensor=new SensorDialog;

    QSettings settings;
    QObject::connect(ui->btn_Start,&QPushButton::clicked,this,&MainWindow::clickOnStart);
    //QObject::connect(mUdpSBE,&UDPData::dataReceived,this,&MainWindow::readData);
    QObject::connect(mUdp,&UDPData::errorString,this,&MainWindow::errorMsg);
    QObject::connect(ui->btn_Refresh,&QPushButton::clicked,this,&MainWindow::majInfo);
    QObject::connect(ui->btn_Connect,&QPushButton::clicked,this,&MainWindow::clickOnConnect);

    QObject::connect(mSensor,&SensorDialog::dataReceived,this,&MainWindow::readData);
    QObject::connect(mSensor,&SensorDialog::errorString,this,&MainWindow::errorMsg);
    QObject::connect(ui->actionQuitter,&QAction::triggered,this,&MainWindow::close);
    QObject::connect(ui->actionApropos,&QAction::triggered,this,&MainWindow::aPropos);

    mPortName=settings.value("PortName","").toString();
    mBaudrate=settings.value("BaudRate","9600").toString();
    mParity=settings.value("Parity","Aucune").toString();
    mDatabits=settings.value("Databits","8").toString();
    mStopbits=settings.value("Stopbits","1").toString();

    mTypeConnec=SensorDialog::Serie;

    majInfo();
    ui->sp_PortOut->setValue(settings.value("PortOut",50000).toInt());
    ui->le_IPDiff->setText(settings.value("IpDiff","172.16.190.255").toString());
    ui->btn_Start->setEnabled(false);
    ui->cb_Baudrate->setCurrentIndex(ui->cb_Baudrate->findText(mBaudrate));
    ui->cb_Parity->setCurrentIndex(ui->cb_Parity->findText(mParity));
    ui->cb_Databits->setCurrentIndex(ui->cb_Databits->findText(mDatabits));
    ui->cb_Stopbits->setCurrentIndex(ui->cb_Stopbits->findText(mStopbits));
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clickOnStart()
{
    QSettings settings;
    if(ui->btn_Start->isChecked())
    {
        ui->btn_Start->setText("Arrêter");
        ui->le_IPDiff->setEnabled(false);
        ui->sp_PortOut->setEnabled(false);
        mDiffStatus=true;

    }
    else
    {
        ui->btn_Start->setText("Diffuser");
        ui->le_IPDiff->setEnabled(true);
        ui->sp_PortOut->setEnabled(true);
        mDiffStatus=false;

    }

}

void MainWindow::clickOnConnect()
{
    //affConnec(ui->btn_Connect->isChecked());
    if(ui->btn_Connect->isChecked())
    {
        initCOM();
    }
    else
    {
        mSensor->setDisconnected();
        affConnec(true);
        ui->statusbar->showMessage(QString("%1 déconnecté").arg(mPortName));
    }

}


void MainWindow::errorMsg(QString sMsg)
{
    ui->statusbar->showMessage(sMsg,30000);
}

void MainWindow::readData(QString sTrame)
{
    ui->l_TrameIn->setText("Trame reçue: "+sTrame);
    if(mDiffStatus)
        diffData(sTrame);
}

void MainWindow::initCOM()
{
    mPortName=ui->cb_Serial->currentText();
    mBaudrate=ui->cb_Baudrate->currentText();
    mParity=ui->cb_Parity->currentText();
    mDatabits=ui->cb_Databits->currentText();
    mStopbits=ui->cb_Stopbits->currentText();

    mSensor->initCOM(mPortName,mBaudrate,mParity,mDatabits,mStopbits,mTypeConnec);

    if(mSensor->setConnected())
    {
        ui->statusbar->showMessage(QString("%1 est connecté").arg(mPortName));
        affConnec(false);
        QSettings settings;

        settings.setValue("PortName",mPortName);
        settings.setValue("BaudRate",mBaudrate);
        settings.setValue("Parity",mParity);
        settings.setValue("Databits",mDatabits);
        settings.setValue("Stopbits",mStopbits);

    }
    else
    {
        ui->statusbar->showMessage(QString("%1 déconnecté").arg(mPortName));
        affConnec(true);
        majInfo();

    }
}

void MainWindow::affConnec(bool bStatus)
{
    if(bStatus)
        ui->btn_Connect->setText("Connecter");
    else
        ui->btn_Connect->setText("Déconnecter");

    ui->cb_Serial->setEnabled(bStatus);
    ui->cb_Baudrate->setEnabled(bStatus);
    ui->cb_Parity->setEnabled(bStatus);
    ui->cb_Databits->setEnabled(bStatus);
    ui->cb_Stopbits->setEnabled(bStatus);
    ui->btn_Refresh->setEnabled(bStatus);
    ui->btn_Start->setEnabled(!bStatus);
}

void MainWindow::diffData(QString sTrame)
{
    int nPortOut=ui->sp_PortOut->value();
    QString sIp=ui->le_IPDiff->text();
    if(mUdp->writeData(sIp,nPortOut,sTrame))
    {
        QSettings settings;
        settings.setValue("IpDiff",sIp);
        settings.setValue("PortOut",nPortOut);
        ui->l_TrameOut->setText(QString("Trame émise: "+sTrame));

    }
}

void MainWindow::majInfo()
{
    QStringList portList;
    ui->cb_Serial->clear();

    const auto infos = QSerialPortInfo::availablePorts();

        for (const QSerialPortInfo &info : infos)
        {
            portList.append(info.portName());
            this->ui->cb_Serial->addItem(info.portName());
        }

        QStringListIterator it (portList);
        bool bFound=it.findNext(mPortName);
        if(bFound)
        {
            ui->cb_Serial->setCurrentIndex(ui->cb_Serial->findText(mPortName));
        }

}

void MainWindow::aPropos()
{
    QString sText=QString("%1\nUtilisaire de diffusion de trames séries vers UDP\n\nSources: https://github.com/MaximeGeay/SerialToUDP\n"
                          "Développé avec Qt 5.14.1\nMaxime Geay\nNovembre 2023").arg(version);
    QMessageBox::information(this,"Informations",sText);
}

