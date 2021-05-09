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
        this->terminate();
        //this->deleteLater();
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
        _udp_buffer+=datagram.data();
        if(_udp_buffer.contains(';')){
            auto templist=_udp_buffer.split(';');
            QString line="";
            if(templist.size()>2){
                line=templist[templist.size()-2];
                _udp_buffer="";
            }
            else{
                line=templist[0];
                _udp_buffer="";
            }
    #ifdef GLOBAL_DEBUG
            qDebug()<<"UDP LISTENER READ: "<</*datagram.data()*/ line<<" from "<<datagram.senderAddress()<< " port "<<datagram.senderPort()<<" date "<<date.toString(Qt::DateFormat::ISODate);
    #endif
    #ifdef ADV_UDP_LISTENER
            //QString line=datagram.data();
            auto list=line.split('|');
            if(list.size()==2){
                int Id,Data;
                bool bId,bData;
                try {
                    Id=list[0].toInt(&bId);
                    Data=list[1].toUInt(&bData);
                    if(bId&&bData){
                        if(ServerInstance::GetInstance()->CheckSensorId(Id)){
                            Measurement * slavemeasurement=new MeasurementSlave(Id,date, Data);
                            std::shared_ptr<Measurement> ptr(slavemeasurement);
                            _measurements->Push(ptr);
                        }
#ifdef GLOBAL_DEBUG
                        else
                            qDebug()<<"Sensor Id is invalid";
#endif
                    }
                    else
                        throw;
                }  catch (...) {
                    qDebug()<<"Error with convert qstring to int(slave)";
                }
            }
            else if(list.size()==3){
                int Id,Temperature,Humidity;
                bool bId,bTemperature,bHumidity;
                try {
                    Id=list[0].toInt(&bId);
                    Temperature=list[1].toInt(&bTemperature);
                    Humidity=list[2].toInt(&bHumidity);
                    if(bId&&bTemperature&&bHumidity){
                        if(ServerInstance::GetInstance()->CheckSensorId(Id)){
                            Condition temp(Temperature,Humidity);
                            ServerInstance::GetInstance()->SetConditions(temp);
                            Measurement * mastermeasurement=new MeasurementMaster(Id,date,temp);
                            std::shared_ptr<Measurement> ptr(mastermeasurement);
                            _measurements->Push(ptr);
                        }
#ifdef GLOBAL_DEBUG
                        else
                            qDebug()<<"Sensor Id is invalid";
#endif
                    }
                    else
                        throw;
                }  catch (...) {
                    qDebug()<<"Error with convert qstring to int(master)";
                }
            }
            else{
                qDebug()<<"Server read invalid data from udp";
            }
    #endif
        }
    }
}
