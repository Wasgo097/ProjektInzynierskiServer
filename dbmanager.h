#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <memory>
#include <list>
#include <QThread>
#include <QtSql>
#include <QSqlDatabase>
#include "measurement.h"
class MainWindow;
class ServerInstance;
class MeasurementsContainer;
class Measurement;
class LogContainer;
class DBManager : public QThread{
    Q_OBJECT
public:
    DBManager(MainWindow &window,ServerInstance& server,MeasurementsContainer& measurements_container, LogContainer & log);
    virtual ~DBManager();
    void Quit();
protected:
    void run()override;
protected:
    QSqlDatabase _db;
    std::list<std::shared_ptr<Measurement>> _measurement_buffer;
private:
    MainWindow & _window;
    ServerInstance & _server;
    MeasurementsContainer & _measurements;
    LogContainer & _log;
};
#endif // DBMANAGER_H
