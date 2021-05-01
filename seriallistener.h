#ifndef SERIALLISTENER_H
#define SERIALLISTENER_H
#include <QThread>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>
class MainWindow;
class Measurements;
class SerialListener : public QThread{
    Q_OBJECT
public:
    SerialListener(MainWindow * Parent=nullptr);
    virtual ~SerialListener();
protected:
    void run()override{}
protected slots:
    void SerialReceived();
private:
    QSerialPort _serial;    
    QByteArray _serialbuffer;
    Measurements * _measurements=nullptr;
};

#endif // SERIALLISTENER_H
