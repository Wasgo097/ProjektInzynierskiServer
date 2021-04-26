#include "dbmanager.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include "Global.h"
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
DBManager::DBManager(MainWindow * Parent):QThread{Parent}{
    if(ConnectDB()){
        qDebug()<<"Connecting successfully";
        _measurements=Measurements::GetInstance();
    }
    else{
        qDebug()<<"Connecting unsuccessfully, end thread";
        this->exit(-1);
        this->deleteLater();
    }
}
DBManager::~DBManager(){
    qDebug()<<"~DBManager";
    if(_db.isOpen()){
        _db.close();
    }
}
void DBManager::Quit(){
    _can_run=false;
#ifdef GLOBAL_DEBUG
        qDebug()<<"Manager quit";
#endif
    terminate();
    quit();
}
void DBManager::run(){
    ServerInstance * server=ServerInstance::GetInstance();
    while(_can_run){
        auto measurement=_measurements->Pop();
        auto string=measurement->GetMeasurement();
        auto listfrommeasurement=string.split('|');
        auto listfromcondition=server->GetConditions().ToQStr().split('|');
        QStringList list;
        list.append(listfrommeasurement[0]);
        list.append(listfrommeasurement[1]);
        list.append(listfrommeasurement[2]);
        list.append(listfromcondition[0]);
        list.append(listfromcondition[1]);
#ifdef GLOBAL_DEBUG
        qDebug()<<string;
#endif
#ifdef GLOBAL_DEBUG
        qDebug()<<"Manager runing loop";
#endif
#ifdef ADV_MANAGER
        QSqlQuery query;
        QString cmd;
        cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+list[0]+","+list[1]+","+list[2]+","+list[3]+","+list[4]+");";
        if(_db.exec(cmd).exec()){
#ifdef MANA_DEBUG
            qDebug()<<"Data added to db";
#endif
        }
        else{
            qDebug()<<"Error in exec query";
            qDebug()<<_db.lastError().text()<<"\n"<<query.lastError().text();
        }
#endif
    }
}
bool DBManager::ConnectDB(){
    _db=QSqlDatabase::addDatabase("QSQLITE","DatabaseCopy.db");
    //_db.setDatabaseName("Database.db");
    return _db.open();
}
