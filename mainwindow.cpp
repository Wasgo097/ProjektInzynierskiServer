#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    SInstance=ServerInstance::GetInstance(this);
    StartListening();
}
MainWindow::~MainWindow(){
    StopListening();
    delete ui;
}

void MainWindow::StartListening()
{
    if(SInstance!=nullptr)
        SInstance->StartListening();
}

void MainWindow::StopListening()
{
    if(SInstance!=nullptr)
        SInstance->StopListening();
}
