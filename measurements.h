#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H
#include <queue>
#include <list>
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
    std::shared_ptr<Measurement> Pop();
    void Push(std::shared_ptr<Measurement> Item);
    static int GetBufferSize();
    std::list<std::shared_ptr<MeasurementFull>> GetMeasurements();
    void AddValidMeasurment(std::shared_ptr<MeasurementFull> Measurement);
protected:
    Measurements(ServerInstance * ServInst);
    Measurements();
    static Measurements * Instance;
    static const int BUFFER_SIZE=10;
private:
    ServerInstance * _server_instance=nullptr;
    std::queue<std::shared_ptr<Measurement>> _buffer;
    ThreadingResources<std::list<std::shared_ptr<MeasurementFull>>> _current_measurements;
    std::mutex _mtx;
    std::condition_variable _cv;
};
#endif // MEASUREMENTS_H
