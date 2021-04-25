#include "udplistener.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include "Global.h"
#include <QDateTime>
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
void UdpListener::ReadyRead(){
    QDateTime date=QDateTime::currentDateTime();
    if(_socket.hasPendingDatagrams()){
        QNetworkDatagram datagram=_socket.receiveDatagram();
#ifdef GLOBAL_DEBUG
        qDebug()<<"Read: "<<datagram.data()<<" from "<<datagram.senderAddress()<< " port "<<datagram.senderPort()<<" date "<<date.toString(Qt::DateFormat::ISODate);
#endif
#ifdef ADV_LISTENER
        QString line=datagram.data();
        auto list=line.split('|');
        if(list.size()==2){
            int Id=-1,Data=-1;
            try {
                Id=list[0].toInt();
                Data=list[1].toUInt();
                if(Id!=-1&&Data!=-1){
                    Measurement * slavemeasurement=new MeasurementSlave(Id,date, Data);
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
                if(Id!=-1&&Temperature!=-1&&Humidity!=-1){
                    Condition temp(Temperature,Humidity);
                    Measurement * mastermeasurement=new MeasurementMaster(Id,date,temp);
                    std::shared_ptr<Measurement> ptr(mastermeasurement);
                    _measurements->Push(ptr);
                    ServerInstance::GetInstance()->SetConditions(temp);
                }
                else
                    throw;
            }  catch (...) {
                qDebug()<<"Error with convert qstring to int(master)";
            }
        }
        else{
            qDebug()<<"Server read invalid data";
        }
#endif
    }
}
