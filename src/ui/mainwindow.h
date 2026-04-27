#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableView>
#include "core/storage/jsonprofilemanager.h"
#include <QSettings>
#include "models/logmodel.h"
#include "ui/logpanelwidget.h"

class ServiceRepository;
class ServiceTableModel;
class MonitorEngine;
class LatencyGraphWidget;
class TrayManager;
class PrometheusServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFileQuit();
    void onAddService();
    void onSaveProfile();
    void onLoadProfile();
    void onExportLogs();
    void onTableContextMenu(const QPoint &pos);

private:
    void setupUi();
    void connectSignals();
    void closeEvent(QCloseEvent *event) override;
    void saveLastProfile(const QString &path);
    void autoLoadLastProfile();

    QLabel             *m_statusLabel;
    QTableView         *m_tableView;
    ServiceRepository  *m_repo;
    ServiceTableModel  *m_tableModel;
    MonitorEngine      *m_engine;
    LatencyGraphWidget *m_graphWidget;
    TrayManager        *m_trayManager;
    JsonProfileManager  m_profileManager;
    LogModel           *m_logModel;
    LogPanelWidget     *m_logPanel;
    PrometheusServer *m_prometheusServer;
};

#endif // MAINWINDOW_H
