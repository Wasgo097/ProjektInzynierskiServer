#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "measurementscontainer.h"
#include "Global.h"
#include "logcontainer.h"
#include "logutilities.h"
//for color
#include <QRandomGenerator>
#include <QMessageBox>
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
    ui->serverlogs->append(log);
}
void MainWindow::AddLogToUdp(QString log){
    //error with QTextCursor
    ui->udplogs->append(log);
}
void MainWindow::AddLogToSerial(QString log){
    //error with QTextCursor
    std::lock_guard lock(_ui_mtx);
    ui->seriallogs->append(log);
}
void MainWindow::AddLogToDBManager(QString log){
    //error with QTextCursor
    std::lock_guard lock(_ui_mtx);
    ui->dbmanagerlogs->append(log);
}
void MainWindow::SetMeasurementsCount(int count){
    //error with QTextCursor
    std::lock_guard lock(_ui_mtx);
    ui->count_of_measurements->setText(QString::number(count));
}
void MainWindow::StartListeners(){
    if(_server){
        _server->StartListeners();
    }
}
void MainWindow::StopListeners(){
    if(_server){
        _server->StopListeners();
    }
}
void MainWindow::StarManager(){
    if(_server){
        _server->StartDatabase();
    }
}
void MainWindow::StopManager(){
    if(_server){
        _server->StopDatabase();
    }
}
void MainWindow::on_StartBtn_clicked(){
    QString serialport=ui->txt_serialport->toPlainText();
    _server=std::make_unique<ServerInstance>(*this,serialport,7654);
    StartListeners();
    StarManager();
}
void MainWindow::on_StopBtn_clicked(){
    StopManager();
    StopListeners();
}
void MainWindow::on_SaveLogBtn_clicked(){
    int index=ui->LogType->currentIndex();
    if(index>=0){
        LogType type=LogType(index);
        _server->SaveLog(type);
    }
}
void MainWindow::on_tabWidget_currentChanged(int index){
    qDebug()<<"void MainWindow::on_tabWidget_currentChanged(int index)";
    //plotting
    if(index==2){
        ui->plot->clearGraphs();
        bool bid;
        int id=ui->device_id->toPlainText().toInt(&bid);
        bool bcount;
        int count=ui->point_count->toPlainText().toInt(&bcount);
        //valid device id and point count to graph
        if(bid==true&&bcount==true){
            //get Measurements as std list
            auto measurements_temp_list=_server->GetMeasurementsForPlotting(id,count);
            //convert list to vector for [] operator
            std::vector<std::shared_ptr<MeasurementFull>> measurements;
            measurements.reserve(measurements_temp_list.size());
            for(const auto & tempmeasur:measurements_temp_list)
                measurements.push_back(tempmeasur);
            if(!measurements.empty()){
                QMessageBox msg(this);
                msg.setIcon(QMessageBox::Information);
                msg.setText("Dane odczytane z czujników mogą się różnić w zależności od lokalizacji i warunków atmosferycznych.");
                msg.setWindowTitle("Informacja o jakości pomiaru");
                msg.setDetailedText("Odczyty pomiaró w z czujników w tych samych lokalizacjach mogą się różnić.");
                msg.setStandardButtons(QMessageBox::Ok);
                msg.exec();
                // set locale to english, so we get english month names:
                ui->plot->setLocale(QLocale(QLocale::Polish, QLocale::Poland));
                ui->plot->addGraph();
                QColor color(QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255));
                ui->plot->graph()->setLineStyle(QCPGraph::lsLine);
                ui->plot->graph()->setPen(QPen(color.lighter(200)));
                ui->plot->graph()->setBrush(QBrush(color));
                //data on graph
                QVector<QCPGraphData> timeData(measurements.size());
                for(size_t i=0;i<measurements.size();i++){
                    timeData[i].key=measurements[i]->GetTime().toTime_t();
                    timeData[i].value=measurements[i]->GetData();
                }
                ui->plot->graph()->data()->set(timeData);
                // configure bottom axis to show date instead of number:
                QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
                dateTicker->setDateTimeFormat("d. MMMM yyyy \n hh:mm:ss");
                ui->plot->xAxis->setTicker(dateTicker);
                // configure left axis text labels:
                QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
                textTicker->addTick(200, "green zone 200");
                textTicker->addTick(350, "yellow zone 350");
                textTicker->addTick(450, "red zone! 450");
                textTicker->addTick(700, "stalker zone!! 700");
                ui->plot->yAxis->setTicker(textTicker);
                // set a more compact font size for bottom and left axis tick labels:
                ui->plot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
                ui->plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
                // set axis labels:
                ui->plot->xAxis->setLabel("Date");
                ui->plot->yAxis->setLabel("Air sensor");
                // make top and right axes visible but without ticks and labels:
                ui->plot->xAxis2->setVisible(true);
                ui->plot->yAxis2->setVisible(true);
                ui->plot->xAxis2->setTicks(false);
                ui->plot->yAxis2->setTicks(false);
                ui->plot->xAxis2->setTickLabels(false);
                ui->plot->yAxis2->setTickLabels(false);
                // set axis ranges to show all data:
                ui->plot->xAxis->setRange(measurements.front()->GetTime().toTime_t() , measurements.back()->GetTime().toTime_t());
                ui->plot->yAxis->setRange(0, 700);
                ui->plot->replot();
            }
        }
    }
}
