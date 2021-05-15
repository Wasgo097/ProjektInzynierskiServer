#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDebug>
#include <memory>
#include <mutex>
class ServerInstance;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void AddLogToServer(QString log);
    void AddLogToUdp(QString log);
    void AddLogToSerial(QString log);
    void AddLogToDBManager(QString log);
    void SetMeasurementsCount(int count);
private slots:
    void on_StartBtn_clicked();
    void on_SaveLogBtn_clicked();
    void on_StopBtn_clicked();
private:
    Ui::MainWindow *ui=nullptr;
    ServerInstance *_server_instance=nullptr;
    std::mutex _ui_mtx;
private:
    void StartListeners(QString serialport);
    void StopListeners();
    void StarManager();
    void StopManager();
};
#endif // MAINWINDOW_H
