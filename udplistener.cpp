#include "udplistener.h"
#include "mainwindow.h"
UdpListener::UdpListener(MainWindow * Parent):QThread{Parent}
{
    if(!Socket.bind(port)){
        qDebug()<<Socket.errorString();
        return;
    }
    else{
        qDebug()<<"Started udp on "<<Socket.localAddress()<<" : "<<Socket.localPort();
        connect(&Socket,&QUdpSocket::readyRead,this,&UdpListener::readyread);
    }
}

UdpListener::~UdpListener()
{
    qDebug()<<"!UdpListener";
    disconnect(&Socket,&QUdpSocket::readyRead,this,&UdpListener::readyread);
}

void UdpListener::quit()
{
    qDebug()<<"See ya";
    Socket.close();
    QThread::quit();
}

void UdpListener::readyread()
{
    QString date=QDateTime::currentDateTime().toString();
    if(Socket.hasPendingDatagrams()){
        QNetworkDatagram datagram=Socket.receiveDatagram();
        qDebug()<<"Read: "<<datagram.data()<<" from "<<datagram.senderAddress()<< " port "<<datagram.senderPort()<<" date "<<date;
    }
}

void UdpListener::run()
{
    qDebug()<<"Run";
}
