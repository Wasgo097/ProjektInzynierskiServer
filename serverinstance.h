#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H
#include <QString>
#include <memory>
#include <list>
#include "Global.h"
#include "measurement.h"
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
    virtual~ServerInstance();
    void StartListeners();
    void StopListeners();
    void StartDatabase();
    void StopDatabase();
    void SetConditions(Condition src);
    Condition GetConditions();
    void AddSensorId(int Id);
    bool CheckSensorId(int Id);
    void SaveLog(LogType type);
    void SetOnWindowUIMeasurementsCount(int count);
    void AddServerLog(const QString &log);
    std::list<std::shared_ptr<MeasurementFull>> GetMeasurementsForPlotting(int deviceid,int count);
    void AddSensor(Sensor sensor);
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
    QString _serialport;
    quint16 _port;
};
#endif // SERVERINSTANCE_H
