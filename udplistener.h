#ifndef UDPLISTENER_H
#define UDPLISTENER_H
#include <QThread>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDebug>
class MainWindow;
class UdpListener : public QThread
{
    Q_OBJECT
public:
    explicit UdpListener(MainWindow * Parent=nullptr);
    virtual ~UdpListener();
signals:
public slots:
    void quit();
    void readyread();
protected:
    void run()override;
private:
    MainWindow * ParentWindow;
    QUdpSocket Socket;
    const quint16 port=7654;
};

#endif // UDPLISTENER_H
