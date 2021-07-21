#include "udpmanager.h"
#include "mainwindow.h"
#include "measurementscontainer.h"
#include "serverinstance.h"
#include "Global.h"
#include <QDateTime>
#include <QNetworkInterface>
#include "logcontainer.h"
UdpManager::UdpManager(MainWindow * Parent):QThread{Parent},_window{Parent}{
    if(!_socket.bind(PORT)){
        QString log=_socket.errorString()+"\n"+"Listening unsuccessfully, end thread";
        qDebug()<<log;
        LogContainer::GetInstance()->AddUdpLogs(log);
        _window->AddLogToUdp(log);
        this->terminate();
        //this->deleteLater();
    }
    else{
        QString log="Started udp on localhost : "+QString::number(PORT) ;
        qDebug()<<log;
        LogContainer::GetInstance()->AddUdpLogs(log);
        _window->AddLogToUdp(log);
        _measurements=MeasurementsContainer::GetInstance();
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost){
                 _localIp= address.toString();
                 qDebug()<<"Local ip: "<<_localIp;
                 break;
            }
        }
        connect(&_socket,&QUdpSocket::readyRead,this,&UdpManager::ReadyRead);
    }
}
UdpManager::~UdpManager(){
    qDebug()<<"~UdpListener";
    _socket.close();
    disconnect(&_socket,&QUdpSocket::readyRead,this,&UdpManager::ReadyRead);
}
void UdpManager::ReadyRead(){
    if(_socket.hasPendingDatagrams()){
        QNetworkDatagram datagram=_socket.receiveDatagram();
        QString line=datagram.data();
        if(line=="GETPARAMS"){
            QByteArray response="PARAMS|";
            response.append(_localIp.toLatin1());
            _socket.writeDatagram(response,datagram.senderAddress(),7654);
            _socket.waitForBytesWritten();
        }
        else{
            QDateTime date=QDateTime::currentDateTime();
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
                _window->AddLogToUdp(log);
        #endif
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
                            _window->AddLogToUdp(log);
                        }
        #endif
                    }
                    else{
                        QString log="Error with convert qstring to int(slave) udplistener";
                        qDebug()<<log;
                        LogContainer::GetInstance()->AddUdpLogs(log);
                        _window->AddLogToUdp(log);
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
                            _window->AddLogToUdp(log);
                        }
        #endif
                    }
                    else {
                        QString log="Error with convert qstring to int(master) udplistener";
                        qDebug()<<log;
                        LogContainer::GetInstance()->AddUdpLogs(log);
                        _window->AddLogToUdp(log);
                    }
                }
                else{
                    QString log="Server read invalid data from udp";
                    qDebug()<<log;
                    LogContainer::GetInstance()->AddUdpLogs(log);
                    _window->AddLogToUdp(log);
                }
            }
        }
    }
}
