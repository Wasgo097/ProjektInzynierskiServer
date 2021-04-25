#ifndef GLOBAL_H
#define GLOBAL_H

#define GLOBAL_DEBUG

#include <mutex>
#include <memory>
#include <QString>
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
    Measurement(int Id):_deviceID{Id}{}
protected:
    int _deviceID;
public:
    virtual QString GetMeasurement()const=0;
};
class MeasurementSlave:public Measurement{
protected:
    int _data;
public:
    MeasurementSlave(int Id,int Data):Measurement{Id},_data{Data}{}
    virtual QString GetMeasurement()const override{
        return QString::number(_deviceID)+"|"+QString::number(_data);
    }
};
class MeasurementMaster:public Measurement{
protected:
    Condition _condition;
public:
    MeasurementMaster(int Id,Condition Condition):Measurement{Id},_condition{Condition}{}
    virtual QString GetMeasurement()const override{
        return QString::number(_deviceID)+"|"+_condition.ToQStr();
    }
};
#endif // GLOBAL_H
