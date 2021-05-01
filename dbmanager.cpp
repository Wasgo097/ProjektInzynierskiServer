#include "dbmanager.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include "Global.h"
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
DBManager::DBManager(MainWindow * Parent):QThread{Parent}{
}
DBManager::~DBManager(){
    qDebug()<<"~DBManager";
    if(_db.isOpen()){
        _db.close();
    }
}
void DBManager::Quit(){
    //_can_run=false;
#ifdef GLOBAL_DEBUG
        qDebug()<<"Manager quit";
#endif
    terminate();
    quit();
}
void DBManager::run(){
    _db=QSqlDatabase::addDatabase("QSQLITE");
#ifdef DBTEST
    _db.setDatabaseName("DatabaseCopy.db");
#else
    _db.setDatabaseName("Database.db");
#endif

    if(_db.open()){
        qDebug()<<"DB Manager connecting successfully";
        _measurements=Measurements::GetInstance();
    }
    else{
        qDebug()<<"DB Manager connecting unsuccessfully, end thread";
        this->exit(-1);
        this->deleteLater();
    }
    ServerInstance * server=ServerInstance::GetInstance();
    while(_db.isOpen()){
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
        qDebug()<<"DB Manager get "<<string;
#endif
#ifdef ADV_MANAGER
        QString cmd;
        cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+list[0]+",'"+list[1]+"',"+list[2]+","+list[3]+","+list[4]+");";
        QSqlQuery query;
        if(query.exec(cmd)){
#ifdef MANA_DEBUG
            qDebug()<<"Data added to db";
#endif
        }
        else{
            qDebug()<<"ERROR WITH QUERY";
            qDebug()<<_db.lastError().text()<<"\t"<<query.lastError().text();
        }
#endif
    }
#ifdef MANA_DEBUG
    qDebug()<<"DB closed";
#endif
}
