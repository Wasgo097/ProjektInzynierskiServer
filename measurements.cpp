#include "measurements.h"
#include "serverinstance.h"
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
    //_current_measurements.push_back(Item);
    mlock.unlock();
    _cv.notify_all();
}
int Measurements::GetBufferSize(){
    return BUFFER_SIZE;
}
std::list<std::shared_ptr<MeasurementFull>> Measurements::GetMeasurements(){
    std::list<std::shared_ptr<MeasurementFull>> temp;
    _quality_measurements.Resource_mtx.lock();
    temp=*_quality_measurements.Resource;
    _quality_measurements.Resource_mtx.unlock();
    return temp;
}
void Measurements::AddValidSlaveMeasurment(std::shared_ptr<MeasurementFull> Measurement){
    _quality_measurements.Resource_mtx.lock();
    _quality_measurements.Resource->push_back(Measurement);
    _quality_measurements.Resource_mtx.unlock();
}
Measurements::Measurements(ServerInstance *ServInst):_server_instance{ServInst}{
}
