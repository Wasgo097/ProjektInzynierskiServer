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
    //static ServerInstance *GetInstance();
    static void ClearInstance();
    void StartListening();
    void StopListening();
//    void ConnectDB();
protected:
    ServerInstance(MainWindow * Window);
    static ServerInstance * Instance;
private:
    std::shared_ptr<UdpListener> Listener;
    std::shared_ptr<DBManager> DbManager;
    MainWindow * Window;
};
#endif // SERVERINSTANCE_H
