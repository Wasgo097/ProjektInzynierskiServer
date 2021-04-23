#include "serverinstance.h"
#include "mainwindow.h"
#include "udplistener.h"
#include "dbmanager.h"
#include "Global.h"
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
        return Instance;
}
void ServerInstance::ClearInstance(){
    if(Instance!=nullptr){
        delete Instance;
        Instance=nullptr;
    }
}
void ServerInstance::StartListening(){
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
void ServerInstance::StopListening(){
    if(_listener){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Stop existing listener";
#endif
        _listener->quit();
        _listener.reset();
    }
}
