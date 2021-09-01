#include "seriallistener.h"
#include "mainwindow.h"
#include "measurementscontainer.h"
#include "serverinstance.h"
#include <QSerialPortInfo>
#include "logcontainer.h"
SerialListener::SerialListener(MainWindow &window,ServerInstance& server,MeasurementsContainer& measurements_container, LogContainer & log,const QString & serialport):QThread{&window},_window{window},
    _server{server},_measurements{measurements_container},_log{log},_serial_port{serialport}{
}
SerialListener::~SerialListener(){
    qDebug()<<"~SerialListener";
    _serial->close();
    // disconnect(_serial.get(),&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
}
void SerialListener::Quit(){
    QString log="SerialListener quit";
    SerialDebug(log)
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
                SerialDebug(log)
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
                SerialDebug(log)
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
        QString log="SERIAL LISTENER READ: "+line+" date "+date.toString(Qt::DateFormat::ISODate);
        SerialDebug(log)
        auto list=line.split('|');
        if(list.size()==2){
            int Id,Data;
            bool bId,bData;
            Id=list[0].toInt(&bId);
            Data=list[1].toUInt(&bData);
            if(bId&&bData){
                if(_server.CheckSensorId(Id)){
                    Measurement * slavemeasurement=new MeasurementSlave(Id,date, Data);
                    std::shared_ptr<Measurement> ptr(slavemeasurement);
                    _measurements.Push(ptr);
                }
                else{
                    QString log="Sensor Id is invalid";
                    SerialDebug(log)
                }
            }
            else{
                QString log="Error with convert qstring to int(slave) serial";
                SerialDebug(log)
            }

        }
        else if(list.size()==3){
            int Id,Temperature,Humidity;
            bool bId,bTemperature,bHumidity;
            Id=list[0].toInt(&bId);
            Temperature=list[1].toInt(&bTemperature);
            Humidity=list[2].toInt(&bHumidity);
            if(bId&&bTemperature&&bHumidity){
                if(_server.CheckSensorId(Id)){
                    Condition temp(Temperature,Humidity);
                    _server.SetConditions(temp);
                    Measurement * mastermeasurement=new MeasurementMaster(Id,date,temp);
                    std::shared_ptr<Measurement> ptr(mastermeasurement);
                    _measurements.Push(ptr);
                }
                else{
                    QString log="Sensor Id is invalid";
                    SerialDebug(log)
                }
            }
            else{
                QString log="Error with convert qstring to int(master) serial";
                SerialDebug(log)
            }
        }
        else{
            QString log="Server read invalid data from serial";
            SerialDebug(log)
        }
    }
}
