#include "dbmanager.h"
#include "mainwindow.h"
#include "measurements.h"
#include "Global.h"
DBManager::DBManager(MainWindow * Parent):QThread{Parent}{
    if(ConnectDB()){
#ifdef GLOBAL_DEBUG
        qDebug()<<"Connecting successfully";
#endif
        _measurements=Measurements::GetInstance();
    }
    else{
#ifdef GLOBAL_DEBUG
        qDebug()<<"Connecting unsuccessfully, end thread";
#endif
        this->exit(-1);
        this->deleteLater();
    }
}
DBManager::~DBManager(){
#ifdef GLOBAL_DEBUG
    qDebug()<<"~DBManager";
#endif
    if(_db.isOpen()){
        _db.close();
    }
}
bool DBManager::ConnectDB(){
    _db=QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("Database.db");
    return _db.open();
}
