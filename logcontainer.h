#ifndef LOGCONTAINER_H
#define LOGCONTAINER_H
#include "logutilities.h"
#include "Global.h"
class LogContainer{
public:
    LogContainer(const LogContainer & Other)=delete;
    void operator =(const LogContainer & Other)=delete;
    static LogContainer * GetInstance();
    static void ClearInstance();
protected:
    LogContainer();
    static LogContainer * Instance;
private:
    ThreadingResources<Logs> _logs;
};
#endif // LOGCONTAINER_H
