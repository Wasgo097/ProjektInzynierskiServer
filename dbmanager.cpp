#include "dbmanager.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include "Global.h"
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
    while(_can_run){
        auto measurement=_measurements->Pop();
        auto string=measurement->GetMeasurement();
#ifdef GLOBAL_DEBUG
        qDebug()<<string;
#endif
#ifdef GLOBAL_DEBUG
        qDebug()<<"Manager runing loop";
#endif
    }
}
bool DBManager::ConnectDB(){
    _db=QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("Database.db");
    return _db.open();
}
