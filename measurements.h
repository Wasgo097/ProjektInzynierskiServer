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
public:
    std::shared_ptr<Measurement> Get();
    void Pop();
    void Push(std::shared_ptr<Measurement> Item);
    static int GetBufferSize();
protected:
    Measurements(ServerInstance * ServInst);
    Measurements()=default;
    static Measurements * Instance;
    static const int BUFFER_SIZE=100;
private:
    ServerInstance * _server_instance;
    std::queue<std::shared_ptr<Measurement>> _queue;
    std::mutex _mtx;
    std::condition_variable _cv;
};
#endif // MEASUREMENTS_H
