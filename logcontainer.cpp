#include "logcontainer.h"
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <string>
LogContainer::LogContainer(){
    _logs.Resource=std::make_shared<Logs>();
}
void LogContainer::AddUdpLogs(QString log){
    _logs.Resource_mtx.lock();
    _logs.Resource->AddUdpLogs(log);
    _logs.Resource_mtx.unlock();
}
void LogContainer::AddSerialLogs(QString log){
    _logs.Resource_mtx.lock();
    _logs.Resource->AddSerialLogs(log);
    _logs.Resource_mtx.unlock();
}
void LogContainer::AddDBManagerLogs(QString log){
    _logs.Resource_mtx.lock();
    _logs.Resource->AddDBManagerLogs(log);
    _logs.Resource_mtx.unlock();
}
void LogContainer::AddServerLogs(QString log){
    _logs.Resource_mtx.lock();
    _logs.Resource->AddServerLogs(log);
    _logs.Resource_mtx.unlock();
}
void LogContainer::SaveLog(LogType Type){
    _logs.Resource_mtx.lock();
    QString DateTime=QDateTime::currentDateTime().toString("dd MM yyyy hh mm ss");
    std::string file_name=DateTime.toStdString()+"_log.txt";
    if(Type==LogType::All){
        file_name="All_"+file_name;
    }
    else if (Type==LogType::Serial) {
        file_name="Serial_"+file_name;
    }
    else if (Type==LogType::Server) {
        file_name="Server_"+file_name;
    }
    else if(Type==LogType::Udp){
        file_name="Udp"+file_name;
    }
    else{
        file_name="DBManager_"+file_name;
    }
    file_name="logs\\"+file_name;
    std::fstream file;
    file.open(file_name.c_str(),std::fstream::out);
    if(file.is_open()==true){
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
        file.close();
    }
#ifdef LogDebug
    else{
        qDebug()<<"Error: Cant create log file";
    }
#endif
    _logs.Resource_mtx.unlock();
}
