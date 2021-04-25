#include "measurements.h"
#include "serverinstance.h"
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
std::shared_ptr<Measurement> Measurements::Get(){
    std::unique_lock<std::mutex> mlock(_mtx);
    while (_queue.empty()) {
         _cv.wait(mlock);
    }
    auto Item=_queue.front();
    mlock.unlock();
    _cv.notify_one();
    return Item;
}
void Measurements::Pop(){
    std::unique_lock<std::mutex> mlock(_mtx);
    while (_queue.empty()) {
         _cv.wait(mlock);
    }
    _queue.pop();
    mlock.unlock();
    _cv.notify_one();
}
void Measurements::Push(std::shared_ptr<Measurement> Item){
    std::unique_lock<std::mutex> mlock(_mtx);
    while (_queue.size()>=100) {
         _cv.wait(mlock);
    }
    _queue.push(Item);
    mlock.unlock();
    _cv.notify_one();
}
int Measurements::GetBufferSize(){
    return BUFFER_SIZE;
}
Measurements::Measurements(ServerInstance *ServInst):_server_instance{ServInst}{
}
