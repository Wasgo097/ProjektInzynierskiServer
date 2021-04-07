#include "serverinstance.h"
#include "mainwindow.h"
#include "udplistener.h"
#include "dbmanager.h"
ServerInstance::ServerInstance(MainWindow *Window):Window{Window}{
    Listener=std::shared_ptr<UdpListener>(new UdpListener(this->Window));
    DbManager=std::shared_ptr<DBManager>(new DBManager(this->Window));
}
ServerInstance *ServerInstance::GetInstance(MainWindow *Window){
    if(Instance){
        Instance=new ServerInstance(Window);
        return Instance;
    }
    else
        return Instance;
}
//ServerInstance *ServerInstance::GetInstance(){
//    if(Instance!=nullptr)
//        return Instance;
//    else
//        return nullptr;
//}
void ServerInstance::ClearInstance(){
    if(Instance!=nullptr)
        delete Instance;
}
void ServerInstance::StartListening(){
    if(Listener)
        Listener->start();
}
void ServerInstance::StopListening(){
    Listener->quit();
    Listener.reset();
}
