#ifndef LOGCONTAINER_H
#define LOGCONTAINER_H
#include "logutilities.h"
#include "Global.h"
class LogContainer{
public:
    LogContainer();
    LogContainer(const LogContainer & Other)=delete;
    void operator =(const LogContainer & Other)=delete;
    void AddUdpLogs(QString log);
    void AddSerialLogs(QString log);
    void AddDBManagerLogs(QString log);
    void AddServerLogs(QString log);
    void SaveLog(LogType Type);
private:
    ThreadingResources<Logs> _logs;
};
#endif // LOGCONTAINER_H
