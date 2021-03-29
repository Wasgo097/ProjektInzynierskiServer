#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    connect(&Socket,&QUdpSocket::readyRead,this,&MainWindow::readyread);
    StartListening();
}
MainWindow::~MainWindow(){
    StopListening();
    delete ui;
}

void MainWindow::StartListening()
{
    if(!Socket.bind(port)){
        qInfo()<<Socket.errorString();
        return;
    }
    qInfo()<<"Started udp on "<<Socket.localAddress()<<" : "<<Socket.localPort();
}

void MainWindow::StopListening()
{
    Socket.close();
}

void MainWindow::readyread()
{
    QString date=QDateTime::currentDateTime().toString();
    if(Socket.hasPendingDatagrams()){
        QNetworkDatagram datagram=Socket.receiveDatagram();
        qInfo()<<"Read: "<<datagram.data()<<" from "<<datagram.senderAddress()<< " port "<<datagram.senderPort()<<" date "<<date;
    }
}
