#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableView>
#include "core/storage/jsonprofilemanager.h"
#include <QSettings>
#include "models/logmodel.h"
#include "ui/logpanelwidget.h"

// Forward declare instead of including — breaks circular includes
class ServiceRepository;
class ServiceTableModel;
class MonitorEngine;
class LatencyGraphWidget;
class TrayManager;

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
    void closeEvent(QCloseEvent *event) override;
    void onSaveProfile();
    void onLoadProfile();
    void saveLastProfile(const QString &path);
    void autoLoadLastProfile();

    QLabel             *m_statusLabel;
    QTableView         *m_tableView;
    ServiceRepository  *m_repo;
    ServiceTableModel  *m_tableModel;
    MonitorEngine     *m_engine;
    LatencyGraphWidget *m_graphWidget;
    TrayManager *m_trayManager;
    JsonProfileManager m_profileManager;
    LogModel       *m_logModel;
    LogPanelWidget *m_logPanel;
};

#endif // MAINWINDOW_H
