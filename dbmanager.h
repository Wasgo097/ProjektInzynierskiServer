#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QThread>

class DBManager : public QThread
{
    Q_OBJECT
public:
    DBManager();
};

#endif // DBMANAGER_H
