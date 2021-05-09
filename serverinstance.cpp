#include "serverinstance.h"
#include "mainwindow.h"
#include "udplistener.h"
#include "seriallistener.h"
#include "dbmanager.h"
#include <algorithm>
ServerInstance * ServerInstance::Instance=nullptr;
ServerInstance::ServerInstance(MainWindow *Window):_window{Window}{
    _valid_sensor_id.Resource=std::shared_ptr<std::list<int>>(new std::list<int>);
    _udplistener=std::shared_ptr<UdpListener>(new UdpListener(this->_window));
    _seriallistener=std::shared_ptr<SerialListener>(new SerialListener(this->_window));
    _dbManager=std::shared_ptr<DBManager>(new DBManager(this->_window));
}
ServerInstance *ServerInstance::GetInstance(MainWindow *Window){
    if(Instance==nullptr){
        Instance=new ServerInstance(Window);
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
void ServerInstance::StartListeners(){
    if(_udplistener){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Start existing udplistener";
#endif
        _udplistener->start();
    }
    else{
#ifdef GLOBAL_DEBUG
        qDebug()<<"Start new udplistener";
#endif
        _udplistener=std::shared_ptr<UdpListener>(new UdpListener(this->_window));
        _udplistener->start();
    }
    if(_seriallistener){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Start existing seriallistener";
#endif
        _seriallistener->start();
    }
    else{
#ifdef GLOBAL_DEBUG
        qDebug()<<"Start new seriallistener";
#endif
        _seriallistener=std::shared_ptr<SerialListener>(new SerialListener(this->_window));
        _seriallistener->start();
    }
}
void ServerInstance::StopListeners(){
    if(_udplistener){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Stop existing udplistener";
#endif
        _udplistener->quit();
        _udplistener.reset();
    }
    if(_seriallistener){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Stop existing seriallistener";
#endif
        _seriallistener->Quit();
        _seriallistener.reset();
    }
}
void ServerInstance::StartDatabase(){
    if(_dbManager){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Start existing db manager";
#endif
        _dbManager->start();
    }
    else{
    #ifdef GLOBAL_DEBUG
            qDebug()<<"Start new db manager";
    #endif
        _dbManager=std::shared_ptr<DBManager>(new DBManager(this->_window));
        _dbManager->start();
    }
}
void ServerInstance::StopDatabase(){
    if(_dbManager){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Stop existing db manager";
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
