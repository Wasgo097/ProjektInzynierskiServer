#include "measurementscontainer.h"
#include "serverinstance.h"
#include "mainwindow.h"
#include <QDebug>
std::shared_ptr<Measurement> MeasurementsContainer::Pop(){
    std::unique_lock<std::mutex> mlock(_mtx);
    while (_buffer.empty())
         _cv.wait(mlock);
    auto Item=_buffer.front();
    _buffer.pop();
    mlock.unlock();
    _cv.notify_all();
    return Item;
}
void MeasurementsContainer::Push(std::shared_ptr<Measurement> Item){
    std::unique_lock<std::mutex> mlock(_mtx);
    while (_buffer.size()>=_BUFFER_SIZE)
         _cv.wait(mlock);
    _buffer.push(Item);
    mlock.unlock();
    _cv.notify_all();
}
const int &MeasurementsContainer::GetBufferSize(){
    return _BUFFER_SIZE;
}
void MeasurementsContainer::SetBufferSize(int value){
    _BUFFER_SIZE=value;
}
std::list<std::shared_ptr<MeasurementFull>> MeasurementsContainer::GetMeasurements(int deviceid,int count){
    std::list<std::shared_ptr<MeasurementFull>> temp;
    _current_measurements.Resource_mtx.lock();
    int tempcount=0;
    //add element from end to begin
    for (auto it=_current_measurements.Resource->rbegin(); it!=_current_measurements.Resource->rend(); ++it){
        if((*it)->_deviceID==deviceid){
            temp.push_back(*it);
            tempcount++;
            if(tempcount==count)
                break;
        }
    }
    _current_measurements.Resource_mtx.unlock();
    return temp;
}
void MeasurementsContainer::AddValidMeasurment(std::shared_ptr<MeasurementFull> Measurement){
//    int count=0;
//    _current_measurements.Resource_mtx.lock();
//    _current_measurements.Resource->push_back(Measurement);
//    count=_current_measurements.Resource->size();
//    _current_measurements.Resource_mtx.unlock();
//    if(_server_instance!=nullptr){
//        _server_instance->GLOBAL_GET_WINDOW()->SetMeasurementsCount(count);
//    }
//    else if((_server_instance=ServerInstance::GetInstance())!=nullptr){
//        _server_instance->GLOBAL_GET_WINDOW()->SetMeasurementsCount(count);
//    }
}
MeasurementsContainer::MeasurementsContainer(ServerInstance & ServInst,const int BUFFER_SIZE):_server_instance{ServInst},_BUFFER_SIZE{BUFFER_SIZE}{
    _current_measurements.Resource=std::make_shared<std::list<std::shared_ptr<MeasurementFull>>>();
}
