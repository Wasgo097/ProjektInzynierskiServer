#ifndef SERIALLISTENER_H
#define SERIALLISTENER_H

#include <QThread>

class SerialListener : public QThread
{
    Q_OBJECT
public:
    SerialListener();
};

#endif // SERIALLISTENER_H
