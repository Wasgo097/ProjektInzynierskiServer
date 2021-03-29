#ifndef UDPLISTENER_H
#define UDPLISTENER_H

#include <QThread>

class UdpListener : public QThread
{
    Q_OBJECT
public:
    UdpListener();
};

#endif // UDPLISTENER_H
