#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <memory>
#include <QThread>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include "Global.h"
class MainWindow;
class Measurements;
class DBManager : public QThread{
    Q_OBJECT
public:
    explicit DBManager(MainWindow * Parent=nullptr);
    virtual ~DBManager();
    void Quit();
protected:
    void run()override;
protected:
    QSqlDatabase _db;
    std::list<std::shared_ptr<Measurement>> _measurement_buffer;
private:
    Measurements * _measurements=nullptr;
};
#endif // DBMANAGER_H
