#include "measurements.h"


QString Condition::ToQStr() const{
    return QString::number(Temperature)+"|"+QString::number(Humidity);
}

bool Condition::Valid(){
    return Temperature!=-1&&Humidity!=-1;
}

bool Condition::operator ==(const Condition &arg) const{
    return this->Temperature==arg.Temperature&&this->Humidity==arg.Humidity;
}

bool Condition::operator !=(const Condition &arg) const{
    return !(*this==arg);
}

Condition Condition::DefaultCondition(){
    return Condition();
}

Measurement::Measurement(int Id, QDateTime Time):_deviceID{Id},_time{Time}{

}

QDateTime Measurement::GetTime() const{
    return _time;
}

MeasurementSlave::MeasurementSlave(int Id, QDateTime Time, int Data):Measurement{Id,Time},_data{Data}{

}

QString MeasurementSlave::GetMeasurement() const{
    return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+QString::number(_data);
}

MeasuremntType MeasurementSlave::GetMeasurementType() const{
    return MeasuremntType::Slave;
}

MeasurementMaster::MeasurementMaster(int Id, QDateTime Time, Condition Condition):Measurement{Id,Time},_condition{Condition}{

}

QString MeasurementMaster::GetMeasurement() const{
    return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+_condition.ToQStr();
}

MeasuremntType MeasurementMaster::GetMeasurementType() const{
    return MeasuremntType::Master;
}

MeasurementFull::MeasurementFull(int Id, QDateTime Time, int Data, Condition Condition):Measurement(Id,Time),MeasurementSlave(Id,Time,Data),MeasurementMaster(Id,Time,Condition){

}

QString MeasurementFull::GetMeasurement() const{
    return QString::number(_deviceID)+"|"+_time.toString(Qt::DateFormat::ISODate)+"|"+QString::number(_data)+"|"+_condition.ToQStr();
}

MeasuremntType MeasurementFull::GetMeasurementType() const{
    return MeasuremntType::Full;
}
