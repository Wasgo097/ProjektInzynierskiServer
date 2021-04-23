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
    int Humidity=-1;
    int Temperature=-1;
    QString ToQStr()const{
        return QString::number(Temperature)+"|"+QString::number(Humidity);
    }
};
class Measurement{
protected:
    Measurement(QString Id):_deviceID{Id}{}
protected:
    QString _deviceID;
public:
    virtual QString GetMeasurement()const=0;
};
class MeasurementSlave:public Measurement{
protected:
    QString _data;
public:
    MeasurementSlave(QString Id,QString Data):Measurement{Id},_data{Data}{}
    virtual QString GetMeasurement()const override{
        return _deviceID+"|"+_data;
    }
};
class MeasurementMaster:public Measurement{
protected:
    Condition _condition;
public:
    MeasurementMaster(QString Id,Condition Condition):Measurement{Id},_condition{Condition}{}
    virtual QString GetMeasurement()const override{
        return _deviceID+"|"+_condition.ToQStr();
    }
};

#endif // GLOBAL_H
