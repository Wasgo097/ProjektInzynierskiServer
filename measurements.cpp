#include "measurements.h"
#include "serverinstance.h"
#include "mainwindow.h"
#include <QDebug>
Measurements * Measurements::Instance=nullptr;
Measurements *Measurements::GetInstance(ServerInstance* ServInst){
    if(Instance==nullptr){
        Instance=new Measurements(ServInst);
    }
    return Instance;
}
Measurements *Measurements::GetInstance(){
    if(Instance!=nullptr){
        return Instance;
    }
    else{
        Instance=new Measurements();
        return Instance;
    }
}
void Measurements::ClearInstance(){
    if(Instance!=nullptr){
        delete Instance;
        Instance=nullptr;
    }
}
std::shared_ptr<Measurement> Measurements::Pop(){
    std::unique_lock<std::mutex> mlock(_mtx);
    while (_buffer.empty()) {
#ifdef MEAS_DEBUG
        qDebug()<<"pop wait";
#endif
         _cv.wait(mlock);
    }
#ifdef MEAS_DEBUG
    qDebug()<<"get go!";
#endif
    auto Item=_buffer.front();
    _buffer.pop();
    mlock.unlock();
    _cv.notify_all();
    return Item;
}
void Measurements::Push(std::shared_ptr<Measurement> Item){
    std::unique_lock<std::mutex> mlock(_mtx);
    while (_buffer.size()>=100) {
#ifdef MEAS_DEBUG
        qDebug()<<"push wait";
#endif
         _cv.wait(mlock);
    }
#ifdef MEAS_DEBUG
    qDebug()<<"push go";
#endif
    _buffer.push(Item);
    mlock.unlock();
    _cv.notify_all();
}
int Measurements::GetBufferSize(){
    return BUFFER_SIZE;
}
std::list<std::shared_ptr<MeasurementFull>> Measurements::GetMeasurements(){
    std::list<std::shared_ptr<MeasurementFull>> temp;
    _current_measurements.Resource_mtx.lock();
    temp=*_current_measurements.Resource;
    _current_measurements.Resource_mtx.unlock();
    return temp;
}
void Measurements::AddValidMeasurment(std::shared_ptr<MeasurementFull> Measurement){
    int count=0;
    _current_measurements.Resource_mtx.lock();
    _current_measurements.Resource->push_back(Measurement);
    count=_current_measurements.Resource->size();
    _current_measurements.Resource_mtx.unlock();
    if(_server_instance!=nullptr){
        _server_instance->GLOBAL_GET_WINDOW()->SetMeasurementsCount(count);
    }
    else if((_server_instance=ServerInstance::GetInstance())!=nullptr){
        _server_instance->GLOBAL_GET_WINDOW()->SetMeasurementsCount(count);
    }
}
Measurements::Measurements(ServerInstance *ServInst):_server_instance{ServInst}{
    _current_measurements.Resource=std::shared_ptr<std::list<std::shared_ptr<MeasurementFull>>>(new std::list<std::shared_ptr<MeasurementFull>>);
}
Measurements::Measurements(){
    _current_measurements.Resource=std::shared_ptr<std::list<std::shared_ptr<MeasurementFull>>>(new std::list<std::shared_ptr<MeasurementFull>>);
}
