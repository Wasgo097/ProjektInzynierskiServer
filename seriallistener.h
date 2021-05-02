#ifndef SERIALLISTENER_H
#define SERIALLISTENER_H
#include <memory>
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
    Measurements * _measurements=nullptr;
};

#endif // SERIALLISTENER_H
