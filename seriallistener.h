#ifndef SERIALLISTENER_H
#define SERIALLISTENER_H
#include <memory>
#include <QThread>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>
class MainWindow;
class ServerInstance;
class MeasurementsContainer;
class LogContainer;
class SerialListener : public QThread{
    Q_OBJECT
public:
    SerialListener(MainWindow &window,ServerInstance& server,MeasurementsContainer& measurements_container, LogContainer & log,const QString & serialport);
    virtual ~SerialListener();
    void Quit();
protected:
    void run()override;
    void SetSerialParams();
    bool IsValid();
protected slots:
    void SerialReceived();
private:
    MainWindow & _window;
    ServerInstance& _server;
    MeasurementsContainer & _measurements;
    LogContainer & _log;
    std::unique_ptr<QSerialPort> _serial;
    QByteArray _serial_buffer;
    const QString _serial_port;
};

#endif // SERIALLISTENER_H
