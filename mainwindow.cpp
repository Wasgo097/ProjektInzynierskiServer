#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "measurements.h"
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
        ui->plot->clearGraphs();
#ifdef TEST_PLOT
//        // generate some data:
//        QVector<double> x(101), y(101); // initialize with entries 0..100
//        for (int i=0; i<101; ++i)
//        {
//          x[i] = i/50.0 - 1; // x goes from -1 to 1
//          y[i] = x[i]*x[i]; // let's plot a quadratic function
//        }
//        // create graph and assign data to it:
//        ui->plot->addGraph();
//        ui->plot->graph(0)->setData(x, y);
//        // give the axes some labels:
//        ui->plot->xAxis->setLabel("x");
//        ui->plot->yAxis->setLabel("y");
//        // set axes ranges, so we see all data:
//        ui->plot->xAxis->setRange(-1, 1);
//        ui->plot->yAxis->setRange(0, 1);
//        ui->plot->replot();

        // set locale to english, so we get english month names:
        ui->plot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
        // seconds of current time, we'll use it as starting point in time for data:
        double now = QDateTime::currentDateTime().toTime_t();
        srand(8); // set the random seed, so we always get the same random data
        // create multiple graphs:
        ui->plot->addGraph();
        QColor color(20+200/4.0,70*(1.6/4.0), 150, 150);
        ui->plot->graph()->setLineStyle(QCPGraph::lsLine);
        ui->plot->graph()->setPen(QPen(color.lighter(200)));
        ui->plot->graph()->setBrush(QBrush(color));
        // generate random walk data:
        QVector<QCPGraphData> timeData(250);
        for (int i=0; i<250; ++i){
            timeData[i].key = now + 24*3600*i;
            if (i == 0)
                timeData[i].value = (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
            else
                timeData[i].value = qFabs(timeData[i-1].value)*(1+0.02/4.0*(4)) + (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
        }
        ui->plot->graph()->data()->set(timeData);
        // configure bottom axis to show date instead of number:
        QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
        dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
        ui->plot->xAxis->setTicker(dateTicker);
        // configure left axis text labels:
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTick(10, "a bit\nlow");
        textTicker->addTick(50, "quite\nhigh");
        ui->plot->yAxis->setTicker(textTicker);
        // set a more compact font size for bottom and left axis tick labels:
        ui->plot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
        ui->plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
        // set axis labels:
        ui->plot->xAxis->setLabel("Date");
        ui->plot->yAxis->setLabel("Random wobbly lines value");
        // make top and right axes visible but without ticks and labels:
        ui->plot->xAxis2->setVisible(true);
        ui->plot->yAxis2->setVisible(true);
        ui->plot->xAxis2->setTicks(false);
        ui->plot->yAxis2->setTicks(false);
        ui->plot->xAxis2->setTickLabels(false);
        ui->plot->yAxis2->setTickLabels(false);
        // set axis ranges to show all data:
        ui->plot->xAxis->setRange(now, now+24*3600*249);
        ui->plot->yAxis->setRange(0, 60);
        // show legend with slightly transparent background brush:
        ui->plot->legend->setVisible(true);
        ui->plot->legend->setBrush(QColor(255, 255, 255, 150));
        ui->plot->replot();
#else
        bool bid;
        int id=ui->device_id->toPlainText().toInt(&bid);
        bool bcount;
        int count=ui->point_count->toPlainText().toInt(&bcount);
        //valid device id and point count to graph
        if(bid==true&&bcount==true){
            //get Measurements as std list
            auto tempmeasurements=Measurements::GetInstance()->GetMeasurements(id,count);
            //convert list to vector for [] operator
            std::vector<std::shared_ptr<MeasurementFull>> measurements;
            for(const auto & tempmeasur:tempmeasurements)
                measurements.push_back(tempmeasur);
            if(!measurements.empty()){
                QMessageBox msg(this);
                msg.setIcon(QMessageBox::Information);
                msg.setText("Dane odczytane z czujników mogą się różnić w zależności od lokalizacji. Dodatkowo w tych samych warunkach czujniki zwracają inne wartości. Wyniki to z wbudowanych wad czujników");
                msg.setWindowTitle("Informacja o jakości pomiaru");
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
                for(int i=0;i<measurements.size();i++){
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
                textTicker->addTick(150, "green zone 150");
                textTicker->addTick(250, "yellow zone 250");
                textTicker->addTick(350, "red zone! 350");
                textTicker->addTick(700, "700");
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
#endif
    }
}
