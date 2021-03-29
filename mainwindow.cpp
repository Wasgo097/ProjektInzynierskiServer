#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    connect(&Timer,&QTimer::timeout,this,&MainWindow::timeout);
    connect(&Socket,&QUdpSocket::readyRead,this,&MainWindow::readyread);
    Timer.setInterval(1000);
    start();
}
MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::start()
{
    if(!Socket.bind(port)){
        qInfo()<<Socket.errorString();
        return;
    }
    qInfo()<<"Started udp on "<<Socket.localAddress()<<" : "<<Socket.localPort();
    broadcast();
}

void MainWindow::stop()
{
    Timer.stop();
    Socket.close();
    qInfo()<<"stopped";
}

void MainWindow::timeout()
{
    QString date=QDateTime::currentDateTime().toString();
    QByteArray data=date.toLatin1();
    QNetworkDatagram datagram(data,QHostAddress::Broadcast,port);
    qInfo()<<"Send: "<<data<<" my address "<<datagram.senderAddress();
    Socket.writeDatagram(datagram);
}

void MainWindow::readyread()
{
    while(Socket.hasPendingDatagrams()){
        QNetworkDatagram datagram=Socket.receiveDatagram();
        qInfo()<<"Read: "<<datagram.data()<<" from "<<datagram.senderAddress()<< " port "<<datagram.senderPort();
    }
}

void MainWindow::broadcast()
{
    qInfo()<<"Broadcasting";
    Timer.start();
}
