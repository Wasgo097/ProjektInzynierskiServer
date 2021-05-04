#include "seriallistener.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
#include <QSerialPortInfo>
SerialListener::SerialListener(MainWindow *Parent):QThread{Parent}{
    _measurements=Measurements::GetInstance();
//    _serial.setPortName("com9");
//    _serial.setBaudRate(QSerialPort::Baud9600);
//    _serial.setDataBits(QSerialPort::Data8);
//    _serial.setParity(QSerialPort::NoParity);
//    _serial.setStopBits(QSerialPort::OneStop);
//    _serial.setFlowControl(QSerialPort::NoFlowControl);
//    if(_serial.open(QIODevice::ReadWrite)){
//        qDebug()<<"Serial Listener opening successfully";
//        _measurements=Measurements::GetInstance();
//        connect(&_serial,&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
//    }
//    else{
//        qDebug()<<"Serial Listener opening unsuccessfully, end thread";
//        this->exit(-1);
//        this->deleteLater();
//    }
}
SerialListener::~SerialListener(){
    qDebug()<<"~SerialListener";
    _serial->close();
    // disconnect(_serial.get(),&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
}
void SerialListener::Quit(){
#ifdef GLOBAL_DEBUG
        qDebug()<<"SerialListener quit";
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
                qDebug()<<"Serial Listener opening successfully";
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
                qDebug()<<"Serial Listener closed";
            }
        }
    }
}
void SerialListener::SetSerialParams(){
    _serial->setPortName("com9");
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
        qDebug()<<"SERIAL LISTENER READ: "<<line<<" date "<<date.toString(Qt::DateFormat::ISODate);
#endif
#ifdef ADV_SERIAL_LISTENER
        auto list=line.split('|');
        if(list.size()==2){
            int Id=-1,Data=-1;
            try {
                Id=list[0].toInt();
                Data=list[1].toUInt();
                if(Id!=-1&&Data!=-1){
                    Measurement * slavemeasurement=new MeasurementSlave(Id,date, Data);
                    std::shared_ptr<Measurement> ptr(slavemeasurement);
                    _measurements->Push(ptr);
                }
                else
                    throw;
            }  catch (...) {
                qDebug()<<"Error with convert qstring to int(slave)";
            }
        }
        else if(list.size()==3){
            int Id=-1,Temperature=-1,Humidity=-1;
            try {
                Id=list[0].toInt();
                Temperature=list[1].toInt();
                Humidity=list[2].toInt();
                if(Id!=-1&&Temperature!=-1&&Humidity!=-1){
                    Condition temp(Temperature,Humidity);
                    ServerInstance::GetInstance()->SetConditions(temp);
                    Measurement * mastermeasurement=new MeasurementMaster(Id,date,temp);
                    std::shared_ptr<Measurement> ptr(mastermeasurement);
                    _measurements->Push(ptr);
                }
                else
                    throw;
            }  catch (...) {
                qDebug()<<"Error with convert qstring to int(master)";
            }
        }
        else{
            qDebug()<<"Server read invalid data from serial";
        }
#endif
    }
}
