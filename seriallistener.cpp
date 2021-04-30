#include "seriallistener.h"
#include "mainwindow.h"
#include "measurements.h"
#include "serverinstance.h"
SerialListener::SerialListener(MainWindow *Parent):QThread{Parent}{
    _serial.setPortName("com9");
    _serial.setBaudRate(QSerialPort::Baud9600);
    _serial.setDataBits(QSerialPort::Data8);
    _serial.setParity(QSerialPort::NoParity);
    _serial.setStopBits(QSerialPort::OneStop);
    _serial.setFlowControl(QSerialPort::NoFlowControl);
    if(_serial.open(QIODevice::ReadWrite)){
        qDebug()<<"Serial Listener opening successfully";
        _measurements=Measurements::GetInstance();
        connect(&_serial,&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
    }
    else{
        qDebug()<<"Serial Listener opening unsuccessfully, end thread";
        this->exit(-1);
        this->deleteLater();
    }
}
SerialListener::~SerialListener(){
    qDebug()<<"~SerialListener";
    _serial.close();
    disconnect(&_serial,&QSerialPort::readyRead,this,&SerialListener::SerialReceived);
}
void SerialListener::SerialReceived(){
    QDateTime date=QDateTime::currentDateTime();
    if(_serial.canReadLine()){
        QString line=_serial.readAll();
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
                    Measurement * mastermeasurement=new MeasurementMaster(Id,date,temp);
                    std::shared_ptr<Measurement> ptr(mastermeasurement);
                    _measurements->Push(ptr);
                    ServerInstance::GetInstance()->SetConditions(temp);
                }
                else
                    throw;
            }  catch (...) {
                qDebug()<<"Error with convert qstring to int(master)";
            }
        }
        else{
            qDebug()<<"Server read invalid data";
        }
#endif
    }
}
