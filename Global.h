#ifndef GLOBAL_H
#define GLOBAL_H
//todo remove
//#define GLOBAL_DEBUG
#define DBManagerDebug(log) qDebug()<<log;_log.AddDBManagerLogs(log);_window.AddLogToDBManager(log);
#define ServerDebug(log) qDebug()<<log;_log->AddServerLogs(log);_window.AddLogToServer(log);
#define UDPDebug(log) qDebug()<<log;_log.AddUdpLogs(log);_window.AddLogToUdp(log);
#define SerialDebug(log) qDebug()<<log;_log.AddSerialLogs(log);_window.AddLogToSerial(log);
//use test database
#define DBTEST
//autostart server
#define AUTO_START

#include <mutex>
#include <memory>
template <class T>
struct ThreadingResources{
    std::shared_ptr<T> Resource;
    std::mutex Resource_mtx;
};
template <class T>
struct ThreadingResourcesLight{
    T Resource;
    std::mutex Resourc_mtx;
};
#endif // GLOBAL_H
