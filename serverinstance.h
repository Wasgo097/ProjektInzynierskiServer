#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H
#include <memory>
#include <list>
#include "Global.h"
class UdpListener;
class DBManager;
class SerialListener;
class MainWindow;
class ServerInstance{
public:
    ServerInstance(const ServerInstance & Other)=delete;
    void operator =(const ServerInstance & Other)=delete;
    static ServerInstance *GetInstance(MainWindow * Window);
    static ServerInstance *GetInstance();
    static void ClearInstance();
    void StartListeners();
    void StopListeners();
    void StartDatabase();
    void StopDatabase();
    void SetConditions(Condition src);
    Condition GetConditions()const;
    void AddSensorId(int Id);
    bool CheckSensorId(int Id);
protected:
    ServerInstance(MainWindow * Window);
    static ServerInstance * Instance;
private:
    MainWindow * _window;
    std::shared_ptr<UdpListener> _udplistener;
    std::shared_ptr<SerialListener> _seriallistener;
    std::shared_ptr<DBManager> _dbManager;
    ThreadingResourcesLight<Condition> _current_conditions;
    ThreadingResources<std::list<int>> _valid_sensor_id;
};
#endif // SERVERINSTANCE_H
