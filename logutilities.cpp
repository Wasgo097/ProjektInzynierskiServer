#include "logutilities.h"

void Logs::AddUdpLogs(QString log){
    QString * tempptr=new QString(log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    UDPListenerLogs.push_back(tempsharptr);
}
void Logs::AddSerialLogs(QString log){
    QString * tempptr=new QString(log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    SerialListenerLogs.push_back(tempsharptr);
}
void Logs::AddDBManagerLogs(QString log){
    QString * tempptr=new QString(log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    DBManagerLogs.push_back(tempsharptr);
}
void Logs::AddServerLogs(QString log){
    QString * tempptr=new QString(log);
    std::shared_ptr<QString> tempsharptr(tempptr);
    AllLogs.push_back(tempsharptr);
    ServerLogs.push_back(tempsharptr);
}
