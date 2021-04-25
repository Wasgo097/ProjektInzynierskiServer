#include "serverinstance.h"
#include "mainwindow.h"
#include "udplistener.h"
#include "dbmanager.h"
ServerInstance * ServerInstance::Instance=nullptr;
ServerInstance::ServerInstance(MainWindow *Window):_window{Window}{
    _listener=std::shared_ptr<UdpListener>(new UdpListener(this->_window));
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
void ServerInstance::StartListener(){
    if(_listener){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Start existing listener";
#endif
        _listener->start();
    }
    else{
#ifdef GLOBAL_DEBUG
        qDebug()<<"Start new listener";
#endif
        _listener=std::shared_ptr<UdpListener>(new UdpListener(this->_window));
        _listener->start();
    }
}
void ServerInstance::StopListener(){
    if(_listener){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Stop existing listener";
#endif
        _listener->quit();
        _listener.reset();
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
