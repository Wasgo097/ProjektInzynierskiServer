#include "dbmanager.h"
#include "mainwindow.h"
#include "measurementscontainer.h"
#include "serverinstance.h"
#include <algorithm>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include <QDebug>
#include "logcontainer.h"
#include "sensor.h"
DBManager::DBManager(MainWindow &window,ServerInstance& server,MeasurementsContainer& measurements_container,LogContainer & log):QThread{&window},_window{window},
    _server{server},_measurements{measurements_container},_log{log}{
}
DBManager::~DBManager(){
    qDebug()<<"~DBManager";
    if(_db.isOpen()){
        _db.close();
    }
}
void DBManager::Quit(){
    QString log="Manager quit";
    DBManagerDebug(log)
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
        DBManagerDebug(log)
    }
    else{
        QString log="DB Manager connecting unsuccessfully, end thread";
        DBManagerDebug(log)
        this->terminate();
        return;
    }
    QSqlQuery queryid("Select DISTINCT Id from Sensors;");
    //if(_db.isOpen()){
        if(queryid.exec()){
            while(queryid.next()){
                _server.AddSensorId(queryid.value(0).toInt());
            }
        }
        else{
            QString log="ERROR: Can't get sensor id's from db: "+_db.lastError().text()+"\t"+queryid.lastError().text();
            DBManagerDebug(log)
        }
   // }
    //QSqlQuery querysensors("SELECT Id,Type,Mac from Sensors;");
    if(queryid.exec()){
        while(queryid.next()){
            int id=queryid.value(0).toInt();
            std::string type=queryid.value(1).toString().toStdString();
            std::string mac=queryid.value(2).toString().toStdString();
            Sensor sensor(id,type,mac);
        }
    }
    else{
        QString log="ERROR: Can't get sensors from db: "+_db.lastError().text()+"\t"+queryid.lastError().text();
        DBManagerDebug(log)
    }
    while(_db.isOpen()){
        QString cmd;
        ////params descriptions in doc
        QSqlQuery query;
        auto measurement=_measurements.Pop();
        auto measurementstring=measurement->GetMeasurement();
        auto measurementparams=measurementstring.split('|');
        if(measurement->GetMeasurementType()==MeasuremntType::Slave){
            Condition conditions=_server.GetConditions();
            if(conditions!=Condition::DefaultCondition()){
                auto conditionparams=conditions.ToQStr().split('|');
                bool bid;
                int id=measurementparams[0].toInt(&bid);
                if(bid&&_server.CheckSensorId(id)){
                    cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+measurementparams[0]+",'"+measurementparams[1]+"',"+measurementparams[2]+","+conditionparams[0]+","+conditionparams[1]+");";
                    //add fullmeas  to chart buffer
                    std::shared_ptr<MeasurementFull> tempptr= std::make_shared<MeasurementFull>(id,measurement->GetTime(),measurementparams[2].toInt(),conditions);
                    _measurements.AddValidMeasurment(tempptr);
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
                if(bid&&_server.CheckSensorId(id)){
                    cmd="INSERT INTO Measurements(SensorId,Date,Mea_Data,Mea_Temp,Mea_Hum)VALUES("+current_measurement_params[0]+",'"+current_measurement_params[1]+"',"+current_measurement_params[2]+","+measurementparams[2]+","+measurementparams[3]+");";
                    QSqlQuery temp_query;
                    if(!temp_query.exec(cmd)){
                        QString log="Error with buffer "+temp_query.lastError().text()+"\t"+cmd;
                        DBManagerDebug(log)
                    }
                    else{
                        //add fullmeas  to chart buffer
                        Condition condition=_server.GetConditions();
                        MeasurementFull * meas=new MeasurementFull(id,measurement->GetTime(),measurementparams[2].toInt(),condition);
                        std::shared_ptr<MeasurementFull> tempptr(meas);
                        _measurements.AddValidMeasurment(tempptr);
                        QString log="Added data from buffer";
                        DBManagerDebug(log)
                    }
                }
            }
            _measurement_buffer.clear();
            bool bid;
            int id=measurementparams[0].toInt(&bid);
            if(bid&&_server.CheckSensorId(id)){
                cmd="INSERT INTO Measurements(SensorId,Date,Mea_Temp,Mea_Hum)VALUES("+measurementparams[0]+",'"+measurementparams[1]+"',"+measurementparams[2]+","+measurementparams[3]+");";
            }
        }
        if(cmd!=""){
            if(query.exec(cmd)){
                QString log="Data added to db";
                DBManagerDebug(log)
            }
            else{
                QString log="ERROR WITH QUERY\n"+_db.lastError().text()+"\t"+query.lastError().text();
                DBManagerDebug(log)
            }
        }
        else{
            QString log="CMD is null";
            DBManagerDebug(log)
        }
    }
    QString log="DB closed";
    DBManagerDebug(log)
}
