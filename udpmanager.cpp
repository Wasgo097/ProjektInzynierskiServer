#include "udpmanager.h"
#include "mainwindow.h"
#include "measurementscontainer.h"
#include "serverinstance.h"
#include "Global.h"
#include <QDateTime>
#include <QNetworkInterface>
#include "logcontainer.h"
UdpManager::UdpManager(MainWindow &window,ServerInstance& server,MeasurementsContainer& measurements_container, LogContainer & log,const quint16& port):QThread{&window},_window{window},
    _server{server},_measurements{measurements_container},_log{log},_PORT{port}{
    if(!_socket.bind(_PORT)){
        QString log=_socket.errorString()+"\n"+"Listening unsuccessfully, end thread";
        qDebug()<<log;
        _log.AddUdpLogs(log);
        _window.AddLogToUdp(log);
        this->terminate();
        //this->deleteLater();
    }
    else{
        QString log="Started udp on localhost : "+QString::number(_PORT) ;
        qDebug()<<log;
        _log.AddUdpLogs(log);
        _window.AddLogToUdp(log);
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
        #ifdef UDPDebug
                QString log="UDP LISTENER READ: "+line+" from "+datagram.senderAddress().toString()+" port "+datagram.senderPort()+" date "+date.toString(Qt::DateFormat::ISODate);
                qDebug()<<log;
                _log.AddUdpLogs(log);
                _window.AddLogToUdp(log);
        #endif
                auto list=line.split('|');
                if(list.size()==2){
                    int Id,Data;
                    bool bId,bData;
                    Id=list[0].toInt(&bId);
                    Data=list[1].toUInt(&bData);
                    if(bId&&bData){
                        if(_server.CheckSensorId(Id)){
                            Measurement * slavemeasurement=new MeasurementSlave(Id,date, Data);
                            std::shared_ptr<Measurement> ptr(slavemeasurement);
                            _measurements.Push(ptr);
                        }
        #ifdef UDPDebug
                        else{
                            QString log="Sensor Id is invalid";
                            qDebug()<<log;
                            _log.AddUdpLogs(log);
                            _window.AddLogToUdp(log);
                        }
        #endif
                    }
                    else{
                        QString log="Error with convert qstring to int(slave) udplistener";
                        qDebug()<<log;
                        _log.AddUdpLogs(log);
                        _window.AddLogToUdp(log);
                    }
                }
                else if(list.size()==3){
                    int Id,Temperature,Humidity;
                    bool bId,bTemperature,bHumidity;
                    Id=list[0].toInt(&bId);
                    Temperature=list[1].toInt(&bTemperature);
                    Humidity=list[2].toInt(&bHumidity);
                    if(bId&&bTemperature&&bHumidity){
                        if(_server.CheckSensorId(Id)){
                            Condition temp(Temperature,Humidity);
                            _server.SetConditions(temp);
                            Measurement * mastermeasurement=new MeasurementMaster(Id,date,temp);
                            std::shared_ptr<Measurement> ptr(mastermeasurement);
                            _measurements.Push(ptr);
                        }
        #ifdef UDPDebug
                        else{
                            QString log="Sensor Id is invalid";
                            qDebug()<<log;
                            _log.AddUdpLogs(log);
                            _window.AddLogToUdp(log);
                        }
        #endif
                    }
                    else {
                        QString log="Error with convert qstring to int(master) udplistener";
                        qDebug()<<log;
                        _log.AddUdpLogs(log);
                        _window.AddLogToUdp(log);
                    }
                }
                else{
                    QString log="Server read invalid data from udp";
                    qDebug()<<log;
                    _log.AddUdpLogs(log);
                    _window.AddLogToUdp(log);
                }
            }
        }
    }
}
