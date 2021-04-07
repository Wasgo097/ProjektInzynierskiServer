#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QThread>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
class MainWindow;
class DBManager : public QThread
{
    Q_OBJECT
public:
    explicit DBManager(MainWindow * Parent=nullptr);
    virtual ~DBManager();
public:
private:
private:
protected:
    void run()override;
    void ConnectDB();
protected:
};

#endif // DBMANAGER_H
