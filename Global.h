#ifndef GLOBAL_H
#define GLOBAL_H

#define GLOBAL_DEBUG
#define ADV_LISTENER
//#define ADV_MANAGER
//#define MEAS_DEBUG
//#define MANA_DEBUG

#include <mutex>
#include <memory>
#include <QString>
#include <QDateTime>
template <class T>
struct ThreadingResources{
    std::shared_ptr<T> Resource;
    std::mutex Resource_mtx;
};
template <class T>
struct ThreadingResourcesLight{
    T Resource;
    std::mutex Resourc_mtx;
};
struct Condition{
    int Temperature=-1;
    int Humidity=-1;
    QString ToQStr()const{
        return QString::number(Temperature)+"|"+QString::number(Humidity);
    }
    Condition(int Temp,int Hum):Temperature{Temp},Humidity{Hum}{}
    Condition()=default;
};
class Measurement{
protected:
    Measurement(int Id,QDateTime Time):_deviceID{Id},_time{Time}{}
protected:
    int _deviceID;
    QDateTime _time;
public:
    virtual QString GetMeasurement()const=0;
};
class MeasurementSlave:public Measurement{
protected:
    int _data;
public:
    MeasurementSlave(int Id,QDateTime Time,int Data):Measurement{Id,Time},_data{Data}{}
    virtual QString GetMeasurement()const override{
        return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+QString::number(_data);
    }
};
class MeasurementMaster:public Measurement{
protected:
    Condition _condition;
public:
    MeasurementMaster(int Id,QDateTime Time,Condition Condition):Measurement{Id,Time},_condition{Condition}{}
    virtual QString GetMeasurement()const override{
        return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+_condition.ToQStr();
    }
};
#endif // GLOBAL_H
