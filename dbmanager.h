#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QThread>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
class MainWindow;
class DBManager : public QThread{
    Q_OBJECT
public:
    explicit DBManager(MainWindow * Parent=nullptr);
    virtual ~DBManager();
protected:
    void run()override{}
    bool ConnectDB();
private:
    QSqlDatabase _db;
};
#endif // DBMANAGER_H
