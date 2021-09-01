#include "serverinstance.h"
#include "mainwindow.h"
#include "udpmanager.h"
#include "seriallistener.h"
#include "dbmanager.h"
#include "logcontainer.h"
#include "sensor.h"
#include <algorithm>
ServerInstance::ServerInstance(MainWindow &window,const QString & serialport,const quint16& port):_window(window){
    _measurements=std::make_unique<MeasurementsContainer>(*this,100);
    _log=std::make_unique<LogContainer>();
    _udpmanager=std::make_unique<UdpManager>(window,*this,*_measurements,*_log,port);
    _seriallistener=std::make_unique<SerialListener>(window,*this,*_measurements,*_log,serialport);
    _dbManager=std::make_unique<DBManager>(window,*this,*_measurements,*_log);
    _valid_sensor_id.Resource=std::make_shared<std::list<int>>();
    _sensors.Resource=std::make_unique<std::list<Sensor>>();
}
void ServerInstance::StartListeners(const QString &serialport){
    if(_udpmanager){
        QString log="Start existing udplistener";
        ServerDebug(log)
        _udpmanager->start();
    }
//    else{
//        //todo
//#ifdef ServerDebug
//        QString log="Start new udplistener";
//        qDebug()<<log;
//        _log->AddServerLogs(log);
//        _window.AddLogToServer(log);
//#endif
//        _udpmanager=std::make_unique<UdpManager>(this->_window);
//        _udpmanager->start();
//    }
    if(_seriallistener){
        QString log="Start existing seriallistener";
        ServerDebug(log)
        _seriallistener->start();
    }
//    else{
//        //todo
//#ifdef ServerDebug
//        QString log="Start new seriallistener";
//        qDebug()<<log;
//        _log->AddServerLogs(log);
//        _window.AddLogToServer(log);
//#endif
//        _seriallistener=std::make_unique<SerialListener>(this->_window,serialport);
//        _seriallistener->start();
//    }
}
void ServerInstance::StopListeners(){
//    if(_udpmanager){
//#ifdef ServerDebug
//        QString log="Stop existing udplistener";
//        qDebug()<<log;
//        _log->AddServerLogs(log);
//        _window.AddLogToServer(log);
//#endif
//        _udpmanager->quit();
//        _udpmanager.reset();
//    }
//    if(_seriallistener){
//#ifdef ServerDebug
//        QString log="Stop existing seriallistener";
//        qDebug()<<log;
//        _log->AddServerLogs(log);
//        _window.AddLogToServer(log);
//#endif
//        _seriallistener->Quit();
//        _seriallistener.reset();
//    }
}
void ServerInstance::StartDatabase(){
    if(_dbManager){
        QString log="Start existing db manager";
        ServerDebug(log)
        _dbManager->start();
    }
//    else{
//    #ifdef ServerDebug
//        QString log="Start new db manager";
//        qDebug()<<log;
//        _log->AddServerLogs(log);
//        _window.AddLogToServer(log);
//    #endif
//        _dbManager=std::make_unique<DBManager>(new DBManager(this->_window));
//        _dbManager->start();
//    }
}
void ServerInstance::StopDatabase(){
//    if(_dbManager){
//#ifdef ServerDebug
//        QString log="Stop existing db manager";
//        qDebug()<<log;
//        _log->AddServerLogs(log);
//        _window.AddLogToServer(log);
//#endif
//        _dbManager->Quit();
//        _dbManager.reset();
//    }
}
void ServerInstance::SetConditions(Condition src){
    _current_conditions.Resourc_mtx.lock();
    _current_conditions.Resource=src;
    _current_conditions.Resourc_mtx.unlock();
}
Condition ServerInstance::GetConditions() const{
    return _current_conditions.Resource;
}
void ServerInstance::AddSensorId(int Id){
    _valid_sensor_id.Resource_mtx.lock();
    _valid_sensor_id.Resource->push_back(Id);
    _valid_sensor_id.Resource_mtx.unlock();
}
bool ServerInstance::CheckSensorId(int Id){
    bool result;
    _valid_sensor_id.Resource_mtx.lock();
    result=(std::find(_valid_sensor_id.Resource->begin(), _valid_sensor_id.Resource->end(), Id) != _valid_sensor_id.Resource->end());
    _valid_sensor_id.Resource_mtx.unlock();
    return result;
}
//void ServerInstance::AddSensor(Sensor sensor){
//    _sensors.Resource_mtx.lock();
//    _sensors.Resource->push_back(sensor);
//    _sensors.Resource_mtx.unlock();
//}
//int ServerInstance::IptoId(std::string ip){
//    QString tempip=QString::fromStdString(ip);
//    QString tempmac;
//    QProcess process;
//    process.start(QString("arp -a %1").arg(tempip));
//    if(process.waitForFinished()){
//        QString result=process.readAll();
//        qDebug()<<result;
//        LogContainer::GetInstance()->AddServerLogs(result);
//        _window->AddLogToServer(result);
//        QStringList list = result.split(QRegularExpression("\\s+"));
//        if(list.contains(tempip)){
//            tempmac = list.at(list.indexOf(tempip) + 1);
//            std::string mac= tempmac.toStdString();
//            _sensors.Resource_mtx.lock();
//            for(auto&sensor:*_sensors.Resource){
//                if(sensor.Mac()==mac)
//                    return sensor.Id();
//            }
//            _sensors.Resource_mtx.unlock();
//            //todo add sensor to db
//            return 0;
//        }
//        else
//            return -1;
//    }
//    return -1;
//}
