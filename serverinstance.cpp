#include "serverinstance.h"
#include "mainwindow.h"
#include "udpmanager.h"
#include "seriallistener.h"
#include "dbmanager.h"
#include <algorithm>
#include "logcontainer.h"
#include "sensor.h"
ServerInstance * ServerInstance::Instance=nullptr;
ServerInstance::ServerInstance(MainWindow *Window,QString serialport):_window{Window}{
//    _valid_sensor_id.Resource=std::shared_ptr<std::list<int>>(new std::list<int>);
//    _udplistener=std::shared_ptr<UdpManager>(new UdpManager(this->_window));
//    _seriallistener=std::shared_ptr<SerialListener>(new SerialListener(this->_window,serialport));
//    _dbManager=std::shared_ptr<DBManager>(new DBManager(this->_window));
    _valid_sensor_id.Resource=std::make_shared<std::list<int>>();
    _udplistener=std::make_shared<UdpManager>(Window);
    _seriallistener=std::make_shared<SerialListener>(Window,serialport);
    _dbManager=std::make_shared<DBManager>(Window);
    _sensors.Resource=std::make_shared<std::list<Sensor>>();
}
ServerInstance *ServerInstance::GetInstance(MainWindow *Window,QString serialport){
    if(Instance==nullptr){
        Instance=new ServerInstance(Window,serialport);
    }
    return Instance;
}
ServerInstance *ServerInstance::GetInstance(){
    if(Instance!=nullptr)
        return Instance;
    else
        return nullptr;
}
void ServerInstance::ClearInstance(){
    if(Instance!=nullptr){
        delete Instance;
        Instance=nullptr;
    }
}
void ServerInstance::StartListeners(QString serialport){
    if(_udplistener){
#ifdef SI_DEBUG
        QString log="Start existing udplistener";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _udplistener->start();
    }
    else{
#ifdef SI_DEBUG
        QString log="Start new udplistener";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _udplistener=std::shared_ptr<UdpManager>(new UdpManager(this->_window));
        _udplistener->start();
    }
    if(_seriallistener){
#ifdef SI_DEBUG
        QString log="Start existing seriallistener";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _seriallistener->start();
    }
    else{
#ifdef SI_DEBUG
        QString log="Start new seriallistener";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _seriallistener=std::shared_ptr<SerialListener>(new SerialListener(this->_window,serialport));
        _seriallistener->start();
    }
}
void ServerInstance::StopListeners(){
    if(_udplistener){
#ifdef SI_DEBUG
        QString log="Stop existing udplistener";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _udplistener->quit();
        _udplistener.reset();
    }
    if(_seriallistener){
#ifdef SI_DEBUG
        QString log="Stop existing seriallistener";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _seriallistener->Quit();
        _seriallistener.reset();
    }
}
void ServerInstance::StartDatabase(){
    if(_dbManager){
#ifdef SI_DEBUG
        QString log="Start existing db manager";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _dbManager->start();
    }
    else{
    #ifdef SI_DEBUG
        QString log="Start new db manager";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
    #endif
        _dbManager=std::shared_ptr<DBManager>(new DBManager(this->_window));
        _dbManager->start();
    }
}
void ServerInstance::StopDatabase(){
    if(_dbManager){
#ifdef SI_DEBUG
        QString log="Stop existing db manager";
        qDebug()<<log;
        LogContainer::GetInstance()->AddServerLogs(log);
        _window->AddLogToServer(log);
#endif
        _dbManager->Quit();
        _dbManager.reset();
    }
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
void ServerInstance::AddSensor(Sensor sensor){
    _sensors.Resource_mtx.lock();
    _sensors.Resource->push_back(sensor);
    _sensors.Resource_mtx.unlock();
}
bool ServerInstance::CheckSensorId(int Id){
    bool result;
    _valid_sensor_id.Resource_mtx.lock();
    result=(std::find(_valid_sensor_id.Resource->begin(), _valid_sensor_id.Resource->end(), Id) != _valid_sensor_id.Resource->end());
    _valid_sensor_id.Resource_mtx.unlock();
    return result;
}
int ServerInstance::IptoId(std::string ip){
    QString tempip=QString::fromStdString(ip);
    QString tempmac;
    QProcess process;
    process.start(QString("arp -a %1").arg(tempip));
    if(process.waitForFinished()){
        QString result=process.readAll();
        qDebug()<<result;
        LogContainer::GetInstance()->AddServerLogs(result);
        _window->AddLogToServer(result);
        QStringList list = result.split(QRegularExpression("\\s+"));
        if(list.contains(tempip)){
            tempmac = list.at(list.indexOf(tempip) + 1);
            std::string mac= tempmac.toStdString();
            _sensors.Resource_mtx.lock();
            for(auto&sensor:*_sensors.Resource){
                if(sensor.Mac()==mac)
                    return sensor.Id();
            }
            _sensors.Resource_mtx.unlock();
            //todo add sensor to db
            return 0;
        }
        else
            return -1;
    }
    return -1;
}
MainWindow * ServerInstance::GLOBAL_GET_WINDOW(){
    return _window;
}
