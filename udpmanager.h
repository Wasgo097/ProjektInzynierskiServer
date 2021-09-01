#ifndef UDPMANAGER_H
#define UDPMANAGER_H
#include <QThread>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDebug>
class MainWindow;
class ServerInstance;
class MeasurementsContainer;
class LogContainer;
class UdpManager : public QThread{
    Q_OBJECT
public:
    UdpManager(MainWindow &window,ServerInstance& server,MeasurementsContainer& measurements_container, LogContainer & log,const quint16& port);
    virtual ~UdpManager();
protected slots:
    void ReadyRead();
protected:
    void run()override{}
private:
    MainWindow & _window;
    ServerInstance& _server;
    MeasurementsContainer & _measurements;
    LogContainer & _log;
    QUdpSocket _socket;
    QByteArray _udp_buffer;
    const quint16 _PORT=7654;
    QString _localIp;
};
#endif // UDPMANAGER_H
