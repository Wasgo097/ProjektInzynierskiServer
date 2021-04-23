#include "udplistener.h"
#include "mainwindow.h"
#include "Global.h"
UdpListener::UdpListener(MainWindow * Parent):QThread{Parent}{
    if(!_socket.bind(PORT)){
        qDebug()<<_socket.errorString();
#ifdef GLOBAL_DEBUG
        qDebug()<<"Listening unsuccessfully";
#endif
        this->exit(-1);
        this->deleteLater();
    }
    else{
        qDebug()<<"Started udp on "<<_socket.localAddress()<<" : "<<_socket.localPort();
        connect(&_socket,&QUdpSocket::readyRead,this,&UdpListener::ReadyRead);
    }
}
UdpListener::~UdpListener(){
    qDebug()<<"~UdpListener";
    _socket.close();
    disconnect(&_socket,&QUdpSocket::readyRead,this,&UdpListener::ReadyRead);
}
//void UdpListener::Quit(){
//#ifdef GLOBAL_DEBUG
//    qDebug()<<"Close socket";
//#endif
//    _socket.close();
//    QThread::quit();
//}
void UdpListener::ReadyRead(){
    QString date=QDateTime::currentDateTime().toString();
    if(_socket.hasPendingDatagrams()){
        QNetworkDatagram datagram=_socket.receiveDatagram();
        qDebug()<<"Read: "<<datagram.data()<<" from "<<datagram.senderAddress()<< " port "<<datagram.senderPort()<<" date "<<date;
    }
}
