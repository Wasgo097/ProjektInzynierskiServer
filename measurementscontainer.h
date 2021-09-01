#ifndef MEASUREMENTSCONTAINER_H
#define MEASUREMENTSCONTAINER_H
#include <queue>
#include <list>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "measurement.h"
#include "Global.h"
class ServerInstance;
class MeasurementsContainer{
public:
    MeasurementsContainer(const MeasurementsContainer & Other)=delete;
    void operator =(const MeasurementsContainer & Other)=delete;
    MeasurementsContainer(ServerInstance & ServInst,const int BUFFER_SIZE=100);
    std::shared_ptr<Measurement> Pop();
    void Push(std::shared_ptr<Measurement> Item);
    const int & GetBufferSize();
    void SetBufferSize(int value);
    std::list<std::shared_ptr<MeasurementFull>> GetMeasurementsForPlotting(int deviceid,int count);
    void AddValidMeasurment(std::shared_ptr<MeasurementFull> Measurement);
    virtual ~MeasurementsContainer()=default;
private:
    ServerInstance & _server;
    int _BUFFER_SIZE=100;
    std::queue<std::shared_ptr<Measurement>> _buffer;
    ThreadingResources<std::list<std::shared_ptr<MeasurementFull>>> _current_measurements;
    std::mutex _mtx;
    std::condition_variable _cv;
};
#endif // MEASUREMENTSCONTAINER_H
