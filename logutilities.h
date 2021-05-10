#ifndef LOGUTILITIES_H
#define LOGUTILITIES_H
#include <list>
#include <memory>
#include <QString>
enum class LogType{
    All=0,
    Udp=1,
    Serial=2,
    Server=3,
    DBManager=4
};
class Logs{
    friend class LogContainer;
    std::list<std::shared_ptr<QString>> AllLogs;
    std::list<std::shared_ptr<QString>> UDPListenerLogs;
    std::list<std::shared_ptr<QString>> SerialListenerLogs;
    std::list<std::shared_ptr<QString>> DBManagerLogs;
    std::list<std::shared_ptr<QString>> ServerLogs;
public:
    void AddUdpLogs(QString log);
    void AddSerialLogs(QString log);
    void AddDBManagerLogs(QString log);
    void AddServerLogs(QString log);
};
#endif // LOGUTILITIES_H
