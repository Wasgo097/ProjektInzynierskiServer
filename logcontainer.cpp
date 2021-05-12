#include "logcontainer.h"
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <string>
LogContainer * LogContainer::Instance=nullptr;
LogContainer::LogContainer(){
    _logs.Resource=std::shared_ptr<Logs>(new Logs);
}
LogContainer *LogContainer::GetInstance(){
    if(Instance!=nullptr){
        return Instance;
    }
    else{
        Instance=new LogContainer;
        return Instance;
    }
}
void LogContainer::ClearInstance(){
    if(Instance!=nullptr){
        delete Instance;
        Instance=nullptr;
    }
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
    std::string file_name="logs\\"+DateTime.toStdString()+"_log.txt";
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
#ifdef GLOBAL_DEBUG
    else{
        qDebug()<<"Error: Cant create log file";
    }
#endif
    _logs.Resource_mtx.unlock();
}
