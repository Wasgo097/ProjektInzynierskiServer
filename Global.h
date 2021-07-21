#ifndef GLOBAL_H
#define GLOBAL_H

#define GLOBAL_DEBUG
#define MANA_DEBUG
#define SI_DEBUG
//#define DBTEST
////autostart server
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
