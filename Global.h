#ifndef GLOBAL_H
#define GLOBAL_H

#define ADV_UDP_LISTENER
#define ADV_SERIAL_LISTENER
#define ADV_MANAGER
#define GLOBAL_DEBUG
//#define MEAS_DEBUG
#define MANA_DEBUG
#define SI_DEBUG
#define DBTEST
//autostart server
#define AUTO_START
//#define TEST_PLOT

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
    bool Valid(){
        return Temperature!=-1&&Humidity!=-1;
    }
    bool operator ==(const Condition & arg)const{
        return this->Temperature==arg.Temperature&&this->Humidity==arg.Humidity;
    }
    bool operator !=(const Condition & arg)const{
        return !(*this==arg);
    }
    static Condition DefaultCondition(){
        return Condition();
    }
};
enum class MeasuremntType{
    Slave,
    Master,
    Full
};
class Measurement{
    friend class Measurements;
protected:
    Measurement(int Id,QDateTime Time):_deviceID{Id},_time{Time}{}
protected:
    int _deviceID;
    QDateTime _time;
public:
    virtual QString GetMeasurement()const=0;
    virtual MeasuremntType GetMeasurementType()const=0;
    virtual ~Measurement(){}
    QDateTime GetTime()const{return _time;}
};
class MeasurementSlave:virtual public Measurement{
    friend class Measurements;
protected:
    int _data;
public:
    MeasurementSlave(int Id,QDateTime Time,int Data):Measurement{Id,Time},_data{Data}{}
    virtual QString GetMeasurement()const override{
        return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+QString::number(_data);
    }
    virtual MeasuremntType GetMeasurementType() const override{
        return MeasuremntType::Slave;
    }
    virtual ~MeasurementSlave(){}
};
class MeasurementMaster:virtual public Measurement{
    friend class Measurements;
protected:
    Condition _condition;
public:
    MeasurementMaster(int Id,QDateTime Time,Condition Condition):Measurement{Id,Time},_condition{Condition}{}
    virtual QString GetMeasurement()const override{
        return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+_condition.ToQStr();
    }
    virtual MeasuremntType GetMeasurementType() const override{
        return MeasuremntType::Master;
    }
    virtual ~MeasurementMaster(){}
};
class MeasurementFull: public MeasurementSlave,public MeasurementMaster{
    friend class Measurements;
public:
    MeasurementFull(int Id,QDateTime Time,int Data,Condition Condition):Measurement(Id,Time),MeasurementSlave(Id,Time,Data),MeasurementMaster(Id,Time,Condition){
    }
    virtual QString GetMeasurement()const override{
        return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+QString::number(_data)+"|"+_condition.ToQStr();
    }
    virtual MeasuremntType GetMeasurementType() const override{
        return MeasuremntType::Full;
    }
    virtual ~MeasurementFull(){}
};
#endif // GLOBAL_H
