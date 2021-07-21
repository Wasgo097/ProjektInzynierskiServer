#ifndef SERIALLISTENER_H
#define SERIALLISTENER_H
#include <memory>
#include <QThread>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>
class MainWindow;
class MeasurementsContainer;
class SerialListener : public QThread{
    Q_OBJECT
public:
    SerialListener(MainWindow * Parent,QString serialport);
    virtual ~SerialListener();
    void Quit();
protected:
    void run()override;
    void SetSerialParams();
    bool IsValid();
protected slots:
    void SerialReceived();
private:
    std::shared_ptr<QSerialPort> _serial;
    QByteArray _serial_buffer;
    MeasurementsContainer * _measurements=nullptr;
    QString _serial_port;
    MainWindow * _window=nullptr;
};

#endif // SERIALLISTENER_H
