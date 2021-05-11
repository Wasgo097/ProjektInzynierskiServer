#include "serverinstance.h"
#include "mainwindow.h"
#include "udplistener.h"
#include "seriallistener.h"
#include "dbmanager.h"
#include <algorithm>
#include "logcontainer.h"
ServerInstance * ServerInstance::Instance=nullptr;
ServerInstance::ServerInstance(MainWindow *Window,QString serialport):_window{Window}{
    _valid_sensor_id.Resource=std::shared_ptr<std::list<int>>(new std::list<int>);
    _udplistener=std::shared_ptr<UdpListener>(new UdpListener(this->_window));
    _seriallistener=std::shared_ptr<SerialListener>(new SerialListener(this->_window,serialport));
    _dbManager=std::shared_ptr<DBManager>(new DBManager(this->_window));
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
        qDebug()<<"Start existing udplistener";
        LogContainer::GetInstance()->AddServerLogs("Start existing udplistener");
#endif
        _udplistener->start();
    }
    else{
#ifdef SI_DEBUG
        qDebug()<<"Start new udplistener";
        LogContainer::GetInstance()->AddServerLogs("Start new udplistener");
#endif
        _udplistener=std::shared_ptr<UdpListener>(new UdpListener(this->_window));
        _udplistener->start();
    }
    if(_seriallistener){
#ifdef SI_DEBUG
        qDebug()<<"Start existing seriallistener";
        LogContainer::GetInstance()->AddServerLogs("Start existing seriallistener");
#endif
        _seriallistener->start();
    }
    else{
#ifdef SI_DEBUG
        qDebug()<<"Start new seriallistener";
        LogContainer::GetInstance()->AddServerLogs("Start new seriallistener");
#endif
        _seriallistener=std::shared_ptr<SerialListener>(new SerialListener(this->_window,serialport));
        _seriallistener->start();
    }
}
void ServerInstance::StopListeners(){
    if(_udplistener){
#ifdef SI_DEBUG
        qDebug()<<"Stop existing udplistener";
        LogContainer::GetInstance()->AddServerLogs("Stop existing udplistener");
#endif
        _udplistener->quit();
        _udplistener.reset();
    }
    if(_seriallistener){
#ifdef SI_DEBUG
        qDebug()<<"Stop existing seriallistener";
        LogContainer::GetInstance()->AddServerLogs("Stop existing seriallistener");
#endif
        _seriallistener->Quit();
        _seriallistener.reset();
    }
}
void ServerInstance::StartDatabase(){
    if(_dbManager){
#ifdef SI_DEBUG
        qDebug()<<"Start existing db manager";
        LogContainer::GetInstance()->AddServerLogs("Start existing db manager");
#endif
        _dbManager->start();
    }
    else{
    #ifdef SI_DEBUG
            qDebug()<<"Start new db manager";
            LogContainer::GetInstance()->AddServerLogs("Start new db manager");
    #endif
        _dbManager=std::shared_ptr<DBManager>(new DBManager(this->_window));
        _dbManager->start();
    }
}
void ServerInstance::StopDatabase(){
    if(_dbManager){
#ifdef SI_DEBUG
        qDebug()<<"Stop existing db manager";
        LogContainer::GetInstance()->AddServerLogs("Stop existing db manager");
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
bool ServerInstance::CheckSensorId(int Id){
    bool result;
    _valid_sensor_id.Resource_mtx.lock();
    result=(std::find(_valid_sensor_id.Resource->begin(), _valid_sensor_id.Resource->end(), Id) != _valid_sensor_id.Resource->end());
    _valid_sensor_id.Resource_mtx.unlock();
    return result;
}
