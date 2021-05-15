#include "dbmanager.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include <algorithm>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include "logcontainer.h"
DBManager::DBManager(MainWindow * Parent):QThread{Parent},_window{Parent}{
}
DBManager::~DBManager(){
    qDebug()<<"~DBManager";
    if(_db.isOpen()){
        _db.close();
    }
}
void DBManager::Quit(){
#ifdef GLOBAL_DEBUG
    QString log="Manager quit";
    qDebug()<<log;
    LogContainer::GetInstance()->AddDBManagerLogs(log);
    _window->AddLogToDBManager(log);
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
        QString log="DB Manager connecting successfully";
        qDebug()<<log;
        LogContainer::GetInstance()->AddDBManagerLogs(log);
        _window->AddLogToDBManager(log);
        _measurements=Measurements::GetInstance();
    }
    else{
        QString log="DB Manager connecting unsuccessfully, end thread";
        qDebug()<<log;
        LogContainer::GetInstance()->AddDBManagerLogs(log);
        _window->AddLogToDBManager(log);
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
        QString log="ERROR: Can't get sensor id's from db";
        qDebug()<<log;
        LogContainer::GetInstance()->AddDBManagerLogs(log);
        _window->AddLogToDBManager(log);
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
                if(bid&&server->CheckSensorId(id)){
                    cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+measurementparams[0]+",'"+measurementparams[1]+"',"+measurementparams[2]+","+conditionparams[0]+","+conditionparams[1]+");";
                    ///todo add valid measurement
                    std::shared_ptr<MeasurementFull> tempptr= std::make_shared<MeasurementFull>(id,measurement->GetTime(),measurementparams[2].toInt(),conditions);
                    _measurements->AddValidMeasurment(tempptr);
                }
            }
            else{
                //sent measurement to buffer
                _measurement_buffer.push_front(measurement);
            }
        }
        //master sensor
        else{
            //buffer service
            for(auto & current_measurement:_measurement_buffer){
                auto current_measurement_str=current_measurement->GetMeasurement();
                auto current_measurement_params=current_measurement_str.split('|');
                bool bid;
                int id=current_measurement_params[0].toInt(&bid);
                if(bid&&server->CheckSensorId(id)){
                    cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+current_measurement_params[0]+",'"+current_measurement_params[1]+"',"+current_measurement_params[2]+","+measurementparams[2]+","+measurementparams[3]+");";
                    QSqlQuery temp_query;
                    if(!temp_query.exec(cmd)){
                        QString log="Error with buffer "+temp_query.lastError().text()+"\t"+cmd;
                        qDebug()<<log;
                        LogContainer::GetInstance()->AddDBManagerLogs(log);
                        _window->AddLogToDBManager(log);
                    }
                    else{
                        ///todo add valid measurement
                        Condition condition=ServerInstance::GetInstance()->GetConditions();
                        MeasurementFull * meas=new MeasurementFull(id,measurement->GetTime(),measurementparams[2].toInt(),condition);
                        std::shared_ptr<MeasurementFull> tempptr(meas);
                        _measurements->AddValidMeasurment(tempptr);
#ifdef MANA_DEBUG
                        QString log="Added data from buffer";
                        qDebug()<<log;
                        LogContainer::GetInstance()->AddDBManagerLogs(log);
                        _window->AddLogToDBManager(log);
#endif
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
                QString log="Data added to db";
                qDebug()<<log;
                LogContainer::GetInstance()->AddDBManagerLogs(log);
                _window->AddLogToDBManager(log);
    #endif
            }
            else{
                QString log="ERROR WITH QUERY\n"+_db.lastError().text()+"\t"+query.lastError().text();
                qDebug()<<log;
                LogContainer::GetInstance()->AddDBManagerLogs(log);
                _window->AddLogToDBManager(log);
            }
#endif
        }
#ifdef GLOBAL_DEBUG
        else{
            QString log="CMD is null";
            qDebug()<<log;
            LogContainer::GetInstance()->AddDBManagerLogs(log);
            _window->AddLogToDBManager(log);
        }
#endif
    }
#ifdef MANA_DEBUG
    QString log="DB closed";
    qDebug()<<log;
    LogContainer::GetInstance()->AddDBManagerLogs(log);
    _window->AddLogToDBManager(log);
#endif
}
