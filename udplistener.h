#ifndef UDPLISTENER_H
#define UDPLISTENER_H
#include <QThread>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDebug>
class MainWindow;
class Measurements;
class UdpListener : public QThread{
    Q_OBJECT
public:
    explicit UdpListener(MainWindow * Parent=nullptr);
    virtual ~UdpListener();
public slots:
    //void Quit();
    void ReadyRead();
protected:
    void run()override{}
private:
    MainWindow * _window=nullptr;
    QUdpSocket _socket;
    const quint16 PORT=7654;
    Measurements * _measurements=nullptr;
};
#endif // UDPLISTENER_H
