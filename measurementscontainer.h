#ifndef MEASUREMENTSCONTAINER_H
#define MEASUREMENTSCONTAINER_H
#include <queue>
#include <list>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "measurements.h"
#include "Global.h"
class ServerInstance;
class MeasurementsContainer{
public:
    MeasurementsContainer(const MeasurementsContainer & Other)=delete;
    void operator =(const MeasurementsContainer & Other)=delete;
    static MeasurementsContainer * GetInstance(ServerInstance *ServInst);
    static MeasurementsContainer * GetInstance();
    static void ClearInstance();
public:
    std::shared_ptr<Measurement> Pop();
    void Push(std::shared_ptr<Measurement> Item);
    static int GetBufferSize();
    std::list<std::shared_ptr<MeasurementFull>> GetMeasurements(int deviceid,int count);
    void AddValidMeasurment(std::shared_ptr<MeasurementFull> Measurement);
protected:
    MeasurementsContainer(ServerInstance * ServInst);
    MeasurementsContainer();
    static MeasurementsContainer * Instance;
    static const int BUFFER_SIZE=100;
private:
    ServerInstance * _server_instance=nullptr;
    std::queue<std::shared_ptr<Measurement>> _buffer;
    ThreadingResources<std::list<std::shared_ptr<MeasurementFull>>> _current_measurements;
    std::mutex _mtx;
    std::condition_variable _cv;
};
#endif // MEASUREMENTSCONTAINER_H
