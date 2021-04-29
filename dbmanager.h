#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QThread>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
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
private:
    Measurements * _measurements=nullptr;
};
#endif // DBMANAGER_H
