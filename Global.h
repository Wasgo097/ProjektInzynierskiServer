#ifndef GLOBAL_H
#define GLOBAL_H
//todo remove
//#define GLOBAL_DEBUG
#define DBManagerDebug
#define ServerDebug
#define UDPDebug
#define SerialDebug
#define LogDebug
//use test database
#define DBTEST
//autostart server
#define AUTO_START

#include <mutex>
#include <memory>
template <class T>
struct ThreadingResources{
    std::shared_ptr<T> Resource;
    std::mutex Resource_mtx;
};
template <class T>
struct ThreadingResourcesLight{
    T Resource;
    std::mutex Resourc_mtx;
};
#endif // GLOBAL_H
