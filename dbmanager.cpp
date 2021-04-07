#include "dbmanager.h"
#include "mainwindow.h"
DBManager::DBManager(MainWindow * Parent):QThread{Parent}{
    ConnectDB();
}
DBManager::~DBManager(){

}
void DBManager::run(){

}

void DBManager::ConnectDB()
{
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
}
