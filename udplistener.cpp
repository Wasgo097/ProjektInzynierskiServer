#include "udplistener.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include "Global.h"
#include <QDateTime>
#include "logcontainer.h"
UdpListener::UdpListener(MainWindow * Parent):QThread{Parent},_window{Parent}{
    if(!_socket.bind(PORT)){
        QString log=_socket.errorString()+"\n"+"Listening unsuccessfully, end thread";
        qDebug()<<log;
        LogContainer::GetInstance()->AddUdpLogs(log);
        this->terminate();
        //this->deleteLater();
    }
    else{
        QString log="Started udp on "+_socket.localAddress().toString()+" : "+_socket.localPort();
        qDebug()<<log;
        LogContainer::GetInstance()->AddUdpLogs(log);
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
            QString log="UDP LISTENER READ: "+line+" from "+datagram.senderAddress().toString()+" port "+datagram.senderPort()+" date "+date.toString(Qt::DateFormat::ISODate);
            qDebug()<<log;
            LogContainer::GetInstance()->AddUdpLogs(log);
    #endif
    #ifdef ADV_UDP_LISTENER
            //QString line=datagram.data();
            auto list=line.split('|');
            if(list.size()==2){
                int Id,Data;
                bool bId,bData;
                Id=list[0].toInt(&bId);
                Data=list[1].toUInt(&bData);
                if(bId&&bData){
                    if(ServerInstance::GetInstance()->CheckSensorId(Id)){
                        Measurement * slavemeasurement=new MeasurementSlave(Id,date, Data);
                        std::shared_ptr<Measurement> ptr(slavemeasurement);
                        _measurements->Push(ptr);
                    }
#ifdef GLOBAL_DEBUG
                    else{
                        QString log="Sensor Id is invalid";
                        qDebug()<<log;
                        LogContainer::GetInstance()->AddUdpLogs("Sensor Id is invalid");
                    }
#endif
                }
                else{
                    QString log="Error with convert qstring to int(slave) udplistener";
                    qDebug()<<log;
                    LogContainer::GetInstance()->AddUdpLogs(log);
                }
            }
            else if(list.size()==3){
                int Id,Temperature,Humidity;
                bool bId,bTemperature,bHumidity;
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
                    else{
                        QString log="Sensor Id is invalid";
                        qDebug()<<log;
                        LogContainer::GetInstance()->AddUdpLogs(log);
                    }
#endif
                }
                else {
                    QString log="Error with convert qstring to int(master) udplistener";
                    qDebug()<<log;
                    LogContainer::GetInstance()->AddUdpLogs(log);
                }
            }
            else{
                QString log="Server read invalid data from udp";
                qDebug()<<log;
                LogContainer::GetInstance()->AddUdpLogs(log);
            }
    #endif
        }
    }
}
