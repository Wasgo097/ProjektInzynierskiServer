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
        return nullptr;
    }
}
void Measurements::ClearInstance(){
    if(Instance!=nullptr){
        delete Instance;
        Instance=nullptr;
    }
}
Measurements::Measurements(ServerInstance *ServInst):_server_instance{ServInst}{

}
