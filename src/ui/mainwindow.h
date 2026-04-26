#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableView>

// Forward declare instead of including — breaks circular includes
class ServiceRepository;
class ServiceTableModel;
class MonitorEngine;
class LatencyGraphWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFileQuit();
    void onAddService();

private:
    void setupUi();
    void connectSignals();

    QLabel             *m_statusLabel;
    QTableView         *m_tableView;
    ServiceRepository  *m_repo;
    ServiceTableModel  *m_tableModel;
    MonitorEngine     *m_engine;
    LatencyGraphWidget *m_graphWidget;


};

#endif // MAINWINDOW_H
