#include "logutilities.h"
#include <QDateTime>
void Logs::AddUdpLogs(QString log){
    QString DateTime=QDateTime::currentDateTime().toString();
    QString * tempptr=new QString(DateTime+" "+log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    UDPListenerLogs.push_back(tempsharptr);
    while(AllLogs.size()>50)
        AllLogs.pop_front();
    while(UDPListenerLogs.size()>50)
        UDPListenerLogs.pop_front();
}
void Logs::AddSerialLogs(QString log){
    QString DateTime=QDateTime::currentDateTime().toString();
    QString * tempptr=new QString(DateTime+" "+log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    SerialListenerLogs.push_back(tempsharptr);
    while(AllLogs.size()>50)
        AllLogs.pop_front();
    while(SerialListenerLogs.size()>50)
        SerialListenerLogs.pop_front();
}
void Logs::AddDBManagerLogs(QString log){
    QString DateTime=QDateTime::currentDateTime().toString();
    QString * tempptr=new QString(DateTime+" "+log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    DBManagerLogs.push_back(tempsharptr);
    while(AllLogs.size()>50)
        AllLogs.pop_front();
    while(DBManagerLogs.size()>50)
        DBManagerLogs.pop_front();
}
void Logs::AddServerLogs(QString log){
    QString DateTime=QDateTime::currentDateTime().toString();
    QString * tempptr=new QString(DateTime+" "+log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    ServerLogs.push_back(tempsharptr);
    while(AllLogs.size()>50)
        AllLogs.pop_front();
    while(ServerLogs.size()>50)
        ServerLogs.pop_front();
}
