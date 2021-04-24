#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "Global.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    _server_instance=ServerInstance::GetInstance(this);
    StartListening();
}
MainWindow::~MainWindow(){
    StopListening();
    delete ui;
}
void MainWindow::StartListening(){
    if(_server_instance!=nullptr)
        _server_instance->StartListening();
}
void MainWindow::StopListening(){
    if(_server_instance!=nullptr)
        _server_instance->StopListening();
}
