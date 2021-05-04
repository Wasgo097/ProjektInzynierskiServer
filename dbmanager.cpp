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
        QString cmd;
        auto measurement=_measurements->Pop();
        auto measurementstring=measurement->GetMeasurement();
        auto measurementparams=measurementstring.split('|');
        if(measurement->GetMeasurementType()==MeasuremntType::Slave){
            Condition conditions=server->GetConditions();
            if(conditions!=Condition::DefaultCondition()){
                auto conditionparams=conditions.ToQStr().split('|');
                //params descriptions in doc
                cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+measurementparams[0]+",'"+measurementparams[1]+"',"+measurementparams[2]+","+conditionparams[0]+","+conditionparams[1]+");";
            }
            else{
                //sent measurement to buffer
                _measurement_buffer.push_front(measurement);
            }
        }
        //buffer service
        else{
            for(auto & measurement:_measurement_buffer){
                auto temp_str=measurement->GetMeasurement();
                auto temp_params=temp_str.split('|');
                cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+temp_params[0]+",'"+temp_params[1]+"',"+temp_params[2]+","+measurementparams[0]+","+measurementparams[1]+");";
                QSqlQuery temp_query;
                if(!temp_query.exec(cmd))
                    qDebug()<<"Error with buffer "<<temp_query.lastError();
            }
            cmd="INSERT INTO Measurements(SensorId,Date,Mea_Temp,Mea_Hum)VALUES("+measurementparams[0]+",'"+measurementparams[1]+"',"+measurementparams[2]+","+measurementparams[3]+");";
        }
        if(cmd!=""){
#ifdef GLOBAL_DEBUG
            qDebug()<<"DB Manager query: "<<cmd;
#endif
#ifdef ADV_MANAGER
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
    }
#ifdef MANA_DEBUG
    qDebug()<<"DB closed";
#endif
}
