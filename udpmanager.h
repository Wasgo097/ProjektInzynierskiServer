#ifndef UDPMANAGER_H
#define UDPMANAGER_H
#include <QThread>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDebug>
class MainWindow;
class MeasurementsContainer;
class UdpManager : public QThread{
    Q_OBJECT
public:
    explicit UdpManager(MainWindow * Parent=nullptr);
    virtual ~UdpManager();
protected slots:
    void ReadyRead();
protected:
    void run()override{}
private:
    MainWindow * _window=nullptr;
    QUdpSocket _socket;
    QByteArray _udp_buffer;
    const quint16 PORT=7654;
    MeasurementsContainer * _measurements=nullptr;
    QString _localIp;
};
#endif // UDPMANAGER_H
