#include "measurementscontainer.h"
#include "serverinstance.h"
#include "mainwindow.h"
#include <QDebug>
MeasurementsContainer::MeasurementsContainer(ServerInstance & ServInst,const int BUFFER_SIZE):_server{ServInst},_BUFFER_SIZE{BUFFER_SIZE}{
    _current_measurements.Resource=std::make_shared<std::list<std::shared_ptr<MeasurementFull>>>();
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
    while (_buffer.size()>= static_cast<size_t>(_BUFFER_SIZE))
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
std::list<std::shared_ptr<MeasurementFull>> MeasurementsContainer::GetMeasurementsForPlotting(int deviceid,int count){
    std::list<std::shared_ptr<MeasurementFull>> temp;
    std::lock_guard(_current_measurements.Resource_mtx);
    int tempcount=0;
//    for (auto it=_current_measurements.Resource->begin(); it!=_current_measurements.Resource->end(); ++it){
//        if((*it)->_deviceID==deviceid){
//            temp.push_back(*it);
//            tempcount++;
//            if(tempcount==count)
//                break;
//        }
//    }
    for (auto &currentmeasurement:*_current_measurements.Resource){
        if(currentmeasurement->_deviceID==deviceid){
            temp.push_back(currentmeasurement);
            tempcount++;
            if(tempcount==count)
                break;
        }
    }
    return temp;
}
void MeasurementsContainer::AddValidMeasurment(std::shared_ptr<MeasurementFull> Measurement){
    int count=0;
    {
        std::lock_guard(_current_measurements.Resource_mtx);
        _current_measurements.Resource->push_back(Measurement);
        count=_current_measurements.Resource->size();
    }
    _server.SetOnWindowUIMeasurementsCount(count);
}
