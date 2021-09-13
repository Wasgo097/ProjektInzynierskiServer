#include "logcontainer.h"
#include "serverinstance.h"
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <string>
LogContainer::LogContainer(ServerInstance &server):_server(server){
    _logs.Resource=std::make_shared<Logs>();
}
void LogContainer::AddUdpLogs(QString log){
    std::lock_guard lock(_logs.Resource_mtx);
    _logs.Resource->AddUdpLogs(log);
}
void LogContainer::AddSerialLogs(QString log){
    std::lock_guard lock(_logs.Resource_mtx);
    _logs.Resource->AddSerialLogs(log);
}
void LogContainer::AddDBManagerLogs(QString log){
    std::lock_guard lock(_logs.Resource_mtx);
    _logs.Resource->AddDBManagerLogs(log);
}
void LogContainer::AddServerLogs(QString log){
    std::lock_guard lock(_logs.Resource_mtx);
    _logs.Resource->AddServerLogs(log);
}
void LogContainer::SaveLog(LogType Type){
    std::lock_guard lock(_logs.Resource_mtx);
    QString DateTime=QDateTime::currentDateTime().toString("dd MM yyyy hh mm ss");
    std::string file_name=DateTime.toStdString()+"_log.txt";
    if(Type==LogType::All)
        file_name="All_"+file_name;
    else if (Type==LogType::Serial)
        file_name="Serial_"+file_name;
    else if (Type==LogType::Server)
        file_name="Server_"+file_name;
    else if(Type==LogType::Udp)
        file_name="Udp"+file_name;
    else
        file_name="DBManager_"+file_name;
    file_name="logs\\"+file_name;
    std::fstream file;
    file.open(file_name.c_str(),std::fstream::out);
    if(!file.is_open()){
        _server.AddServerLog("Error: Cant create log file!!");
        return;
    }
    if(Type==LogType::All){
        for(auto & log:_logs.Resource->AllLogs){
            file<<log->toStdString()<<std::endl;
        }
    }
    else if (Type==LogType::Serial) {
        for(auto & log:_logs.Resource->SerialListenerLogs){
            file<<log->toStdString()<<std::endl;
        }
    }
    else if (Type==LogType::Server) {
        for(auto & log:_logs.Resource->ServerLogs){
            file<<log->toStdString()<<std::endl;
        }
    }
    else if(Type==LogType::Udp){
        for(auto & log:_logs.Resource->UDPListenerLogs){
            file<<log->toStdString()<<std::endl;
        }
    }
    else{
        for(auto & log:_logs.Resource->DBManagerLogs){
            file<<log->toStdString()<<std::endl;
        }
    }
    _server.AddServerLog("Create new log file "+QString::fromStdString(file_name));
    file.close();
}
