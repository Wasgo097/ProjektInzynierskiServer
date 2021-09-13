#ifndef MEASUREMENT_H
#define MEASUREMENT_H
#include <QString>
#include <QDateTime>
struct Condition{
    int Temperature=-1;
    int Humidity=-1;
    QString ToQStr()const;
    Condition(int Temp,int Hum):Temperature{Temp},Humidity{Hum}{}
    Condition()=default;
    bool Valid();
    bool operator ==(const Condition & arg)const;
    bool operator !=(const Condition & arg)const;
    static Condition DefaultCondition();
};

enum class MeasuremntType{
    Slave,
    Master,
    Full
};

class Measurement{
    friend class MeasurementsContainer;
protected:
    Measurement(int Id,QDateTime Time);
protected:
    int _deviceID;
    QDateTime _time;
public:
    virtual QString GetMeasurement()const=0;
    virtual MeasuremntType GetMeasurementType()const=0;
    virtual ~Measurement()=default;
    QDateTime GetTime()const;
};

class MeasurementSlave:virtual public Measurement{
    friend class MeasurementsContainer;
protected:
    int _data;
public:
    MeasurementSlave(int Id,QDateTime Time,int Data);
    virtual QString GetMeasurement()const override;
    virtual MeasuremntType GetMeasurementType() const override;
    virtual ~MeasurementSlave()=default;
    int GetData()const;
};

class MeasurementMaster:virtual public Measurement{
    friend class MeasurementsContainer;
protected:
    Condition _condition;
public:
    MeasurementMaster(int Id,QDateTime Time,Condition Condition);
    virtual QString GetMeasurement()const override;
    virtual MeasuremntType GetMeasurementType() const override;
    virtual ~MeasurementMaster()=default;
    Condition GetCondition()const{return _condition;}
};

class MeasurementFull: public MeasurementSlave,public MeasurementMaster{
    friend class MeasurementsContainer;
public:
    MeasurementFull(int Id,QDateTime Time,int Data,Condition Condition);
    virtual QString GetMeasurement()const override;
    virtual MeasuremntType GetMeasurementType() const override;
    virtual ~MeasurementFull()=default;
};

#endif // MEASUREMENT_H
