#include "serverinstance.h"
#include "mainwindow.h"
#include "udpmanager.h"
#include "seriallistener.h"
#include "dbmanager.h"
#include "logcontainer.h"
#include "sensor.h"
#include <algorithm>
#include <mutex>
ServerInstance::ServerInstance(MainWindow &window,const QString & serialport,const quint16& port):_window(window),_serialport(serialport),_port(port){
    _measurements=std::make_unique<MeasurementsContainer>(*this,100);
    _log=std::make_unique<LogContainer>(*this);
    _udpmanager=std::make_unique<UdpManager>(_window,*this,*_measurements,*_log,_port);
    _seriallistener=std::make_unique<SerialListener>(_window,*this,*_measurements,*_log,_serialport);
    _dbManager=std::make_unique<DBManager>(_window,*this,*_measurements,*_log);
    _valid_sensor_id.Resource=std::make_shared<std::list<int>>();
    _sensors.Resource=std::make_unique<std::list<Sensor>>();
}
ServerInstance::~ServerInstance(){
    StopListeners();
    StopDatabase();
}
void ServerInstance::StartListeners(){
    if(_udpmanager){
        QString log="Start existing udplistener";
        ServerDebug(log)
        _udpmanager->start();
    }
    else{
        QString log="Start new udplistener";
        ServerDebug(log)
        _udpmanager=std::make_unique<UdpManager>(_window,*this,*_measurements,*_log,_port);
        _udpmanager->start();
    }

    if(_seriallistener){
        QString log="Start existing seriallistener";
        ServerDebug(log)
        _seriallistener->start();
    }
    else{
        QString log="Start new seriallistener";
        ServerDebug(log)
        _seriallistener=std::make_unique<SerialListener>(_window,*this,*_measurements,*_log,_serialport);
        _seriallistener->start();
    }
}
void ServerInstance::StopListeners(){
    if(_udpmanager){
        QString log="Stop existing udplistener";
        ServerDebug(log)
        _udpmanager->quit();
        _udpmanager.reset();
    }

    if(_seriallistener){
        QString log="Stop existing seriallistener";
        ServerDebug(log)
        _seriallistener->Quit();
        _seriallistener.reset();
    }
}
void ServerInstance::StartDatabase(){
    if(_dbManager){
        QString log="Start existing db manager";
        ServerDebug(log)
        _dbManager->start();
    }
    else{
        QString log="Start new db manager";
        ServerDebug(log)
        _dbManager=std::make_unique<DBManager>(_window,*this,*_measurements,*_log);
        _dbManager->start();
    }
}
void ServerInstance::StopDatabase(){
    if(_dbManager){
        QString log="Stop existing db manager";
        ServerDebug(log)
        _dbManager->Quit();
        _dbManager.reset();
    }
}
void ServerInstance::SetConditions(Condition src){
    std::lock_guard(_current_conditions.Resourc_mtx);
    _current_conditions.Resource=src;
}
Condition ServerInstance::GetConditions(){
    std::lock_guard(_current_conditions.Resourc_mtx);
    return _current_conditions.Resource;
}
void ServerInstance::AddSensorId(int Id){
    std::lock_guard(_valid_sensor_id.Resource_mtx);
    _valid_sensor_id.Resource->push_back(Id);
}
bool ServerInstance::CheckSensorId(int Id){
    std::lock_guard(_valid_sensor_id.Resource_mtx);
    return(std::find(_valid_sensor_id.Resource->begin(), _valid_sensor_id.Resource->end(), Id) != _valid_sensor_id.Resource->end());
}
void ServerInstance::SaveLog(LogType type){
    _log->SaveLog(type);
}
void ServerInstance::SetOnWindowUIMeasurementsCount(int count){
    _window.SetMeasurementsCount(count);
}
void ServerInstance::AddServerLog(const QString &log){
    ServerDebug(log);
}
std::list<std::shared_ptr<MeasurementFull> > ServerInstance::GetMeasurementsForPlotting(int deviceid, int count){
    return _measurements->GetMeasurementsForPlotting(deviceid,count);
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
