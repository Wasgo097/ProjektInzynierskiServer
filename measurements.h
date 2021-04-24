#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "Global.h"
class ServerInstance;
class Measurements{
public:
    Measurements(const Measurements & Other)=delete;
    void operator =(const Measurements & Other)=delete;
    static Measurements * GetInstance(ServerInstance *ServInst);
    static Measurements * GetInstance();
    static void ClearInstance();
protected:
    Measurements(ServerInstance * ServInst);
    static Measurements * Instance;
private:
    ServerInstance * _server_instance;
    std::queue<std::shared_ptr<Measurement>> _queue;
    std::mutex _mtx;
    std::condition_variable _cv;
};
#endif // MEASUREMENTS_H
