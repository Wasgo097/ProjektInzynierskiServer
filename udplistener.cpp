#include "udplistener.h"
#include "mainwindow.h"
#include "measurements.h"
#include "Global.h"
UdpListener::UdpListener(MainWindow * Parent):QThread{Parent}{
    if(!_socket.bind(PORT)){
        qDebug()<<_socket.errorString();
        qDebug()<<"Listening unsuccessfully, end thread";
        this->exit(-1);
        this->deleteLater();
    }
    else{
        qDebug()<<"Started udp on "<<_socket.localAddress()<<" : "<<_socket.localPort();
        connect(&_socket,&QUdpSocket::readyRead,this,&UdpListener::ReadyRead);
        _measurements=Measurements::GetInstance();
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
        QString line=datagram.data();
        auto list=line.split('|');
        if(list.size()==2){
            int Id=-1,Data=-1;
            try {
                Id=list[0].toInt();
                Data=list[1].toUInt();
                if(Id!=-1&&Data!=-1){
                    Measurement * slavemeasurement=new MeasurementSlave(Id,Data);
                    std::shared_ptr<Measurement> ptr(slavemeasurement);
                    _measurements->Push(ptr);
                }
                else
                    throw;
            }  catch (...) {
                qDebug()<<"Error with convert qstring to int(slave)";
            }
        }
        else if(list.size()==3){
            int Id=-1,Temperature=-1,Humidity=-1;
            try {
                Id=list[0].toInt();
                Temperature=list[1].toInt();
                Humidity=list[2].toInt();
                Measurement * mastermeasurement=new MeasurementMaster(Id,Condition(Temperature,Humidity));
                std::shared_ptr<Measurement> ptr(mastermeasurement);
                _measurements->Push(ptr);
            }  catch (...) {
                qDebug()<<"Error with convert qstring to int(master)";
            }
        }
        else{
            qDebug()<<"Server read invalid data";
        }
    }
}
