#include "logcontainer.h"
#include <QDateTime>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
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
    QString DateTime=QDateTime::currentDateTime().toString(Qt::ISODate);
    QFile file(DateTime);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        if(Type==LogType::All){
            for(auto & log:_logs.Resource->AllLogs){
                out<<*log<<"\n";
            }
        }
        else if (Type==LogType::Serial) {
            for(auto & log:_logs.Resource->SerialListenerLogs){
                out<<*log<<"\n";
            }
        }
        else if (Type==LogType::Server) {
            for(auto & log:_logs.Resource->ServerLogs){
                out<<*log<<"\n";
            }
        }
        else if(Type==LogType::Udp){
            for(auto & log:_logs.Resource->UDPListenerLogs){
                out<<*log<<"\n";
            }
        }
        else{
            for(auto & log:_logs.Resource->DBManagerLogs){
                out<<*log<<"\n";
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
