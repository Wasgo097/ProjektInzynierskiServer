#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H
#include <memory>
#include "Global.h"
class UdpListener;
class DBManager;
class MainWindow;
class ServerInstance{
public:
    ServerInstance(const ServerInstance & Other)=delete;
    void operator =(const ServerInstance & Other)=delete;
    static ServerInstance *GetInstance(MainWindow * Window);
    static ServerInstance *GetInstance();
    static void ClearInstance();
    void StartListening();
    void StopListening();
    void SetConditions(Condition src);
    Condition GetConditions()const;
protected:
    ServerInstance(MainWindow * Window);
    static ServerInstance * Instance;
private:
    MainWindow * _window;
    std::shared_ptr<UdpListener> _listener;
    std::shared_ptr<DBManager> _dbManager;
    ThreadingResourcesLight<Condition> _current_conditions;
};
#endif // SERVERINSTANCE_H
