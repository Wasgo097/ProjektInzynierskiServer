#include "dbmanager.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include <algorithm>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include "logcontainer.h"
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
        LogContainer::GetInstance()->AddDBManagerLogs("Manager quit");
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
        LogContainer::GetInstance()->AddDBManagerLogs("DB Manager connecting successfully");
        _measurements=Measurements::GetInstance();
    }
    else{
        qDebug()<<"DB Manager connecting unsuccessfully, end thread";
        LogContainer::GetInstance()->AddDBManagerLogs("DB Manager connecting unsuccessfully, end thread");
        this->terminate();
    }
    ServerInstance * server=ServerInstance::GetInstance();
    bool valid_sensor_id=false;
    if(_db.isOpen()){
        QSqlQuery query("Select DISTINCT Id from Sensors;");
        if(query.exec()){
            valid_sensor_id=true;
            while(query.next()){
                server->AddSensorId(query.value(0).toInt());
            }
        }
    }
    if(!valid_sensor_id){
        qDebug()<<"ERROR: Can't get sensor id's from db.";
        LogContainer::GetInstance()->AddDBManagerLogs("ERROR: Can't get sensor id's from db.");
    }
    while(_db.isOpen()){
        QString cmd;
        ////params descriptions in doc
        QSqlQuery query;
        auto measurement=_measurements->Pop();
        auto measurementstring=measurement->GetMeasurement();
        auto measurementparams=measurementstring.split('|');
        if(measurement->GetMeasurementType()==MeasuremntType::Slave){
            Condition conditions=server->GetConditions();
            if(conditions!=Condition::DefaultCondition()){
                auto conditionparams=conditions.ToQStr().split('|');
                bool bid;
                int id=measurementparams[0].toInt(&bid);
                if(bid&&server->CheckSensorId(id))
                    cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+measurementparams[0]+",'"+measurementparams[1]+"',"+measurementparams[2]+","+conditionparams[0]+","+conditionparams[1]+");";
            }
            else{
                //sent measurement to buffer
                _measurement_buffer.push_front(measurement);
            }
        }
        //buffer service
        else{
            for(auto & current_measurement:_measurement_buffer){
                auto current_measurement_str=current_measurement->GetMeasurement();
                auto current_measurement_params=current_measurement_str.split('|');
                bool bid;
                int id=current_measurement_params[0].toInt(&bid);
                if(bid&&server->CheckSensorId(id)){
                    cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+current_measurement_params[0]+",'"+current_measurement_params[1]+"',"+current_measurement_params[2]+","+measurementparams[2]+","+measurementparams[3]+");";
                    QSqlQuery temp_query;
                    if(!temp_query.exec(cmd)){
                        qDebug()<<"Error with buffer "<<temp_query.lastError().text()<<"\t"<<cmd;
                        LogContainer::GetInstance()->AddDBManagerLogs("Error with buffer "+temp_query.lastError().text()+"\t"+cmd);
                    }
                    else{
                        qDebug()<<"Added data from buffer";
                        LogContainer::GetInstance()->AddDBManagerLogs("Added data from buffer");
                    }
                }
            }
            _measurement_buffer.clear();
            bool bid;
            int id=measurementparams[0].toInt(&bid);
            if(bid&&server->CheckSensorId(id)){
                cmd="INSERT INTO Measurements(SensorId,Date,Mea_Temp,Mea_Hum)VALUES("+measurementparams[0]+",'"+measurementparams[1]+"',"+measurementparams[2]+","+measurementparams[3]+");";
            }
        }
        if(cmd!=""){
#ifdef ADV_MANAGER
            if(query.exec(cmd)){
    #ifdef MANA_DEBUG
                qDebug()<<"Data added to db";
                LogContainer::GetInstance()->AddDBManagerLogs("Data added to db");
    #endif
            }
            else{
                qDebug()<<"ERROR WITH QUERY";
                qDebug()<<_db.lastError().text()<<"\t"<<query.lastError().text();
                LogContainer::GetInstance()->AddDBManagerLogs("ERROR WITH QUERY\n"+_db.lastError().text()+"\t"+query.lastError().text());
            }
#endif
        }
#ifdef GLOBAL_DEBUG
        else{
            qDebug()<<"CMD is null";
            LogContainer::GetInstance()->AddDBManagerLogs("CMD is null");
        }
#endif
    }
#ifdef MANA_DEBUG
    qDebug()<<"DB closed";
    LogContainer::GetInstance()->AddDBManagerLogs("DB closed");
#endif
}
