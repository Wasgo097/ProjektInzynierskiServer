#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H
#include <QString>
#include <memory>
#include <list>
#include "Global.h"
#include "measurements.h"
#include "udpmanager.h"
#include "seriallistener.h"
#include "dbmanager.h"
#include "measurementscontainer.h"
#include "logcontainer.h"
class MainWindow;
class Sensor;
class ServerInstance{
public:
    ServerInstance(const ServerInstance & Other)=delete;
    void operator =(const ServerInstance & Other)=delete;
    ServerInstance(MainWindow &window,const QString & serialport,const quint16& port);
    void StartListeners(const QString &serialport);
    void StopListeners();
    void StartDatabase();
    void StopDatabase();
    void SetConditions(Condition src);
    Condition GetConditions()const;
    void AddSensorId(int Id);
    bool CheckSensorId(int Id);
    //void AddSensor(Sensor sensor);
    //int IptoId(std::string ip);
private:
    MainWindow & _window;
    std::unique_ptr<UdpManager> _udpmanager;
    std::unique_ptr<SerialListener> _seriallistener;
    std::unique_ptr<DBManager> _dbManager;
    std::unique_ptr<MeasurementsContainer> _measurements;
    std::unique_ptr<LogContainer> _log;
    ThreadingResourcesLight<Condition> _current_conditions;
    ThreadingResources<std::list<int>> _valid_sensor_id;
    ThreadingResources<std::list<Sensor>> _sensors;
};
#endif // SERVERINSTANCE_H
