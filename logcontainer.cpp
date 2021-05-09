#include "logcontainer.h"
LogContainer * LogContainer::Instance=nullptr;
LogContainer::LogContainer(){
    _logs.Resource=std::shared_ptr<Logs>(new Logs);
}
