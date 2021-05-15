#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "Global.h"
#include "logcontainer.h"
#include "logutilities.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    ui->LogType->addItem("All Logs");
    ui->LogType->addItem("UdpListener Logs");
    ui->LogType->addItem("SerialListener Logs");
    ui->LogType->addItem("Server Logs");
    ui->LogType->addItem("DB Manager Logs");
#ifdef AUTO_START
    on_StartBtn_clicked();
#endif
}
MainWindow::~MainWindow(){
    StopManager();
    StopListeners();
    delete ui;
}
void MainWindow::AddLogToServer(QString log){
    //error with QTextCursor
    _ui_mtx.lock();
    ui->serverlogs->append(log);
    _ui_mtx.unlock();
}
void MainWindow::AddLogToUdp(QString log){
    //error with QTextCursor
    _ui_mtx.lock();
    ui->udplogs->append(log);
    _ui_mtx.unlock();
}
void MainWindow::AddLogToSerial(QString log){
    //error with QTextCursor
    _ui_mtx.lock();
    ui->seriallogs->append(log);
    _ui_mtx.unlock();
}
void MainWindow::AddLogToDBManager(QString log){
    //error with QTextCursor
    _ui_mtx.lock();
    ui->dbmanagerlogs->append(log);
    _ui_mtx.unlock();
}
void MainWindow::SetMeasurementsCount(int count){
    //error with QTextCursor
    _ui_mtx.lock();
    ui->count_of_measurements->setText(QString::number(count));
    _ui_mtx.unlock();
}
void MainWindow::StartListeners(QString serialport){
    if(_server_instance!=nullptr){
        _server_instance->StartListeners(serialport);
    }
}
void MainWindow::StopListeners(){
    if(_server_instance!=nullptr){
        _server_instance->StopListeners();
    }
}
void MainWindow::StarManager(){
    if(_server_instance!=nullptr){
        _server_instance->StartDatabase();
    }
}
void MainWindow::StopManager(){
    if(_server_instance!=nullptr){
        _server_instance->StopDatabase();
    }
}
void MainWindow::on_StartBtn_clicked(){
    QString serialport=ui->txt_serialport->toPlainText();
    _server_instance=ServerInstance::GetInstance(this,serialport);
    StartListeners(serialport);
    StarManager();
}
void MainWindow::on_SaveLogBtn_clicked(){
    int index=ui->LogType->currentIndex();
    if(index>=0){
        LogType type=LogType(index);
        LogContainer::GetInstance()->SaveLog(type);
    }
}
void MainWindow::on_StopBtn_clicked(){
//    StopManager();
//    StopListeners();
}
void MainWindow::on_tabWidget_currentChanged(int index){
    //plotting
    if(index==2){
#ifdef TEST_PLOT
        // generate some data:
        QVector<double> x(101), y(101); // initialize with entries 0..100
        for (int i=0; i<101; ++i)
        {
          x[i] = i/50.0 - 1; // x goes from -1 to 1
          y[i] = x[i]*x[i]; // let's plot a quadratic function
        }
        // create graph and assign data to it:
        ui->plot->addGraph();
        ui->plot->graph(0)->setData(x, y);
        // give the axes some labels:
        ui->plot->xAxis->setLabel("x");
        ui->plot->yAxis->setLabel("y");
        // set axes ranges, so we see all data:
        ui->plot->xAxis->setRange(-1, 1);
        ui->plot->yAxis->setRange(0, 1);
        ui->plot->replot();
#else
        qDebug()<<"Draw plot";
#endif
    }
}
