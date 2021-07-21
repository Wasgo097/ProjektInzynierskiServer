#include "seriallistener.h"
#include "mainwindow.h"
#include "measurementscontainer.h"
#include "serverinstance.h"
#include <QSerialPortInfo>
#include "logcontainer.h"
SerialListener::SerialListener(MainWindow *Parent,QString serialport):QThread{Parent},_serial_port{serialport},_window{Parent}{
    _measurements=MeasurementsContainer::GetInstance();
}
SerialListener::~SerialListener(){
    qDebug()<<"~SerialListener";
    _serial->close();
    // disconnect(_serial.get(),&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
}
void SerialListener::Quit(){
#ifdef GLOBAL_DEBUG
    QString log="SerialListener quit";
    qDebug()<<log;
    LogContainer::GetInstance()->AddSerialLogs(log);
    _window->AddLogToSerial(log);
#endif
    terminate();
    quit();
}
void SerialListener::run(){
    while(true){
        _serial.reset(new QSerialPort);
        SetSerialParams();
        while(!_serial->isOpen()){
            if(_serial->open(QIODevice::ReadWrite)){
                QString log="Serial Listener opening successfully";
                qDebug()<<log;
                LogContainer::GetInstance()->AddSerialLogs(log);
                _window->AddLogToSerial(log);
                //connect(_serial.get(),&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
            }
        }
        while(_serial->isOpen()){
            if(_serial->waitForReadyRead(3000)){
                SerialReceived();
            }
            if(!IsValid()){
               // disconnect(_serial.get(),&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
                _serial->close();
                QString log="Serial Listener closed";
                qDebug()<<log;
                LogContainer::GetInstance()->AddSerialLogs(log);
                _window->AddLogToSerial(log);
            }
        }
    }
}
void SerialListener::SetSerialParams(){
    _serial->setPortName(_serial_port);
    _serial->setBaudRate(QSerialPort::Baud9600);
    _serial->setDataBits(QSerialPort::Data8);
    _serial->setParity(QSerialPort::NoParity);
    _serial->setStopBits(QSerialPort::OneStop);
    _serial->setFlowControl(QSerialPort::NoFlowControl);
}
bool SerialListener::IsValid(){
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;
    auto list=QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &serialPortInfo,list){
            if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
                if((serialPortInfo.productIdentifier() == arduino_uno_product_id)&&
                        (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)){
                    return true;
                }
            }
    }
    return false;
}
void SerialListener::SerialReceived(){
    QDateTime date=QDateTime::currentDateTime();
    _serial_buffer+=_serial->readAll();
    if(_serial_buffer.contains(';')){
        auto templist=_serial_buffer.split(';');
        QString line="";
        if(templist.size()>2){
            line=templist[templist.size()-2];
            _serial_buffer="";
        }
        else{
            line=templist[0];
            _serial_buffer="";
        }
#ifdef GLOBAL_DEBUG
        QString log="SERIAL LISTENER READ: "+line+" date "+date.toString(Qt::DateFormat::ISODate);
        qDebug()<<log;
        LogContainer::GetInstance()->AddSerialLogs(log);
        _window->AddLogToSerial(log);
#endif
        auto list=line.split('|');
        if(list.size()==2){
            int Id,Data;
            bool bId,bData;
            Id=list[0].toInt(&bId);
            Data=list[1].toUInt(&bData);
            if(bId&&bData){
                if(ServerInstance::GetInstance()->CheckSensorId(Id)){
                    Measurement * slavemeasurement=new MeasurementSlave(Id,date, Data);
                    std::shared_ptr<Measurement> ptr(slavemeasurement);
                    _measurements->Push(ptr);
                }
#ifdef GLOBAL_DEBUG
                else{
                    QString log="Sensor Id is invalid";
                    qDebug()<<log;
                    LogContainer::GetInstance()->AddSerialLogs(log);
                    _window->AddLogToSerial(log);
                }
#endif
            }
            else{
                QString log="Error with convert qstring to int(slave) serial";
                qDebug()<<log;
                LogContainer::GetInstance()->AddSerialLogs(log);
                _window->AddLogToSerial(log);
            }

        }
        else if(list.size()==3){
            int Id,Temperature,Humidity;
            bool bId,bTemperature,bHumidity;
            Id=list[0].toInt(&bId);
            Temperature=list[1].toInt(&bTemperature);
            Humidity=list[2].toInt(&bHumidity);
            if(bId&&bTemperature&&bHumidity){
                if(ServerInstance::GetInstance()->CheckSensorId(Id)){
                    Condition temp(Temperature,Humidity);
                    ServerInstance::GetInstance()->SetConditions(temp);
                    Measurement * mastermeasurement=new MeasurementMaster(Id,date,temp);
                    std::shared_ptr<Measurement> ptr(mastermeasurement);
                    _measurements->Push(ptr);
                }
#ifdef GLOBAL_DEBUG
                else{
                    QString log="Sensor Id is invalid";
                    qDebug()<<log;
                    LogContainer::GetInstance()->AddSerialLogs(log);
                    _window->AddLogToSerial(log);
                }
#endif
            }
            else{
                QString log="Error with convert qstring to int(master) serial";
                qDebug()<<log;
                LogContainer::GetInstance()->AddSerialLogs(log);
                _window->AddLogToSerial(log);
            }
        }
        else{
            QString log="Server read invalid data from serial";
            qDebug()<<log;
            LogContainer::GetInstance()->AddSerialLogs(log);
            _window->AddLogToSerial(log);
        }
    }
}
