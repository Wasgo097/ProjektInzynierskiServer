#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H
#include <memory>
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
protected:
    ServerInstance(MainWindow * Window);
    static ServerInstance * Instance;
private:
    std::shared_ptr<UdpListener> _listener;
    std::shared_ptr<DBManager> _dbManager;
    MainWindow * _window;
};
#endif // SERVERINSTANCE_H
