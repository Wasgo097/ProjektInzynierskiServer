#include "measurementscontainer.h"
#include "serverinstance.h"
#include "mainwindow.h"
#include <QDebug>
MeasurementsContainer * MeasurementsContainer::Instance=nullptr;
MeasurementsContainer *MeasurementsContainer::GetInstance(ServerInstance* ServInst){
    if(Instance==nullptr){
        Instance=new MeasurementsContainer(ServInst);
    }
    return Instance;
}
MeasurementsContainer *MeasurementsContainer::GetInstance(){
    if(Instance!=nullptr){
        return Instance;
    }
    else{
        Instance=new MeasurementsContainer();
        return Instance;
    }
}
void MeasurementsContainer::ClearInstance(){
    if(Instance!=nullptr){
        delete Instance;
        Instance=nullptr;
    }
}
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
    while (_buffer.size()>=BUFFER_SIZE)
         _cv.wait(mlock);
    _buffer.push(Item);
    mlock.unlock();
    _cv.notify_all();
}
int MeasurementsContainer::GetBufferSize(){
    return BUFFER_SIZE;
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
MeasurementsContainer::MeasurementsContainer(ServerInstance *ServInst):_server_instance{ServInst}{
    //_current_measurements.Resource=std::shared_ptr<std::list<std::shared_ptr<MeasurementFull>>>(new std::list<std::shared_ptr<MeasurementFull>>);
    _current_measurements.Resource=std::make_shared<std::list<std::shared_ptr<MeasurementFull>>>();
}
MeasurementsContainer::MeasurementsContainer(){
    //_current_measurements.Resource=std::shared_ptr<std::list<std::shared_ptr<MeasurementFull>>>(new std::list<std::shared_ptr<MeasurementFull>>);
    _current_measurements.Resource=std::make_shared<std::list<std::shared_ptr<MeasurementFull>>>();
}
