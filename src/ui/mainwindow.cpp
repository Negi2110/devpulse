#include "ui/mainwindow.h"
#include "core/servicerepository.h"
#include "models/servicetablemodel.h"
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QStatusBar>
#include <QMenuBar>
#include "ui/dialogs/addservicedialog.h"
#include "core/monitorengine.h"
#include "ui/latencygraphwidget.h"
#include <QSplitter>
#include "ui/traymanager.h"
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QClipboard>
#include <QStyle>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include "core/prometheusserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_repo(new ServiceRepository(this))
    , m_tableModel(new ServiceTableModel(this))
    , m_engine(new MonitorEngine(m_repo, this))
    , m_trayManager(new TrayManager(this, this))
    , m_logModel(new LogModel(this))
    , m_prometheusServer(new PrometheusServer(m_repo, this))

{
    setupUi();
    connectSignals();
    autoLoadLastProfile();

}

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    setWindowTitle("DevPulse v0.1");
    setMinimumSize(1100, 650);

    // Table view
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_tableModel);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->setShowGrid(false);
    m_tableView->verticalHeader()->setDefaultSectionSize(36);
    m_tableView->setWordWrap(false);

    m_tableView->horizontalHeader()->setSectionResizeMode(
        ServiceTableModel::ColName, QHeaderView::Fixed);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        ServiceTableModel::ColUrl, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        ServiceTableModel::ColStatus, QHeaderView::Fixed);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        ServiceTableModel::ColLatency, QHeaderView::Fixed);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        ServiceTableModel::ColInterval, QHeaderView::Fixed);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        ServiceTableModel::ColUptime, QHeaderView::Fixed);

    m_tableView->horizontalHeader()->resizeSection(
        ServiceTableModel::ColName, 150);
    m_tableView->horizontalHeader()->resizeSection(
        ServiceTableModel::ColStatus, 100);
    m_tableView->horizontalHeader()->resizeSection(
        ServiceTableModel::ColLatency, 100);
    m_tableView->horizontalHeader()->resizeSection(
        ServiceTableModel::ColInterval, 80);
    m_tableView->horizontalHeader()->resizeSection(
        ServiceTableModel::ColUptime, 100);

    // Create graph widget
    m_graphWidget = new LatencyGraphWidget(this);
    m_graphWidget->setMinimumHeight(250);

    // Splitter — table on top, graph on bottom
    // Splitter — table | graph | logs
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_tableView);
    splitter->addWidget(m_graphWidget);
    m_logPanel = new LogPanelWidget(m_logModel, this);
    m_logPanel->setMinimumHeight(150);
    splitter->addWidget(m_logPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);

    setCentralWidget(splitter);
    // Status bar
    m_statusLabel = new QLabel("No services added", this);
    statusBar()->addWidget(m_statusLabel);

    // Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *addAction = new QAction("&Add Service", this);
    addAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddService);
    fileMenu->addAction(addAction);

    fileMenu->addSeparator();

    fileMenu->addSeparator();

    QAction *saveAction = new QAction("&Save Profile", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered,
            this, &MainWindow::onSaveProfile);
    fileMenu->addAction(saveAction);

    QAction *loadAction = new QAction("&Load Profile", this);
    saveAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered,
            this, &MainWindow::onLoadProfile);
    fileMenu->addAction(loadAction);

    fileMenu->addSeparator();
    QAction *exportAction = new QAction("&Export Logs", this);
    connect(exportAction, &QAction::triggered,
            this, &MainWindow::onExportLogs);
    fileMenu->addAction(exportAction);

    fileMenu->addSeparator();

    QAction *quitAction = new QAction("&Quit", this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &MainWindow::onFileQuit);

    fileMenu->addAction(quitAction);
    // Start Prometheus endpoint
    if (m_prometheusServer->start(9898)) {
        statusBar()->showMessage(
            "Metrics at http://localhost:9898/metrics", 5000);
    }

}

void MainWindow::connectSignals()
{
    connect(m_repo, &ServiceRepository::serviceAdded,
            m_tableModel, &ServiceTableModel::addService);

    connect(m_repo, &ServiceRepository::serviceRemoved,
            m_tableModel, &ServiceTableModel::removeService);

    connect(m_repo, &ServiceRepository::serviceUpdated,
            m_tableModel, &ServiceTableModel::updateService);

    connect(m_engine, &MonitorEngine::serviceStatusChanged,
            this, [this](const Service &service){
               m_graphWidget->updateForService(
                  service.id,
                m_engine->latencyStore()
            );
    });

    connect(m_tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex &current, const QModelIndex&) {
                if (!current.isValid()) return;
                QString id = m_tableModel->serviceIdAt(current.row());
                m_graphWidget->updateForService(id, m_engine->latencyStore());
                m_graphWidget->setTitle(
                    m_tableModel->data(
                                    m_tableModel->index(current.row(), 0)
                                    ).toString() + " — Latency"
                    );
            });

    connect(m_engine, &MonitorEngine::serviceStatusChanged,
            m_trayManager, &TrayManager::onServiceStatusChanged);

    connect(m_trayManager, &TrayManager::showMainWindowRequested,
            this, &QWidget::show);

    connect(m_trayManager, &TrayManager::quitRequested,
            this, &MainWindow::onFileQuit);
    connect(m_engine, &MonitorEngine::logEntry,
            m_logModel, &LogModel::addEntry);
    connect(m_tableView, &QTableView::customContextMenuRequested,
            this, &MainWindow::onTableContextMenu);
}

void MainWindow::onFileQuit()
{
    QApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_trayManager->isAvailable()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}
void MainWindow::onSaveProfile()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        "Save Profile",
        QDir::homePath(),
        "DevPulse Profiles (*.json)"
        );

    if (path.isEmpty()) return;

    if (!path.endsWith(".json"))
        path += ".json";

    if (m_profileManager.save(m_repo->allServices(), path)) {
        statusBar()->showMessage("Profile saved: " + path, 3000);
        saveLastProfile(path);

    } else {
        QMessageBox::warning(this, "Save Failed",
                             m_profileManager.lastError());
    }
}

void MainWindow::onLoadProfile()
{
    QString path = QFileDialog::getOpenFileName(
        this,
        "Load Profile",
        QDir::homePath(),
        "DevPulse Profiles (*.json)"
        );

    if (path.isEmpty()) return;

    QVector<Service> services = m_profileManager.load(path);

    if (services.isEmpty() && !m_profileManager.lastError().isEmpty()) {
        QMessageBox::warning(this, "Load Failed",
                             m_profileManager.lastError());
        return;
    }

    for (const Service &s : services)
        m_repo->addService(s);

    saveLastProfile(path);


    m_statusLabel->setText(
        QString("%1 service(s) monitored").arg(m_repo->count())
        );
    statusBar()->showMessage("Profile loaded: " + path, 3000);
}
void MainWindow::onAddService()
{
    AddServiceDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_repo->addService(dlg.service());
        m_statusLabel->setText(
            QString("%1 service(s) monitored").arg(m_repo->count())
            );
    }
}
void MainWindow::onTableContextMenu(const QPoint &pos)
{
    QModelIndex index = m_tableView->indexAt(pos);
    if (!index.isValid()) return;

    QString serviceId = m_tableModel->serviceIdAt(index.row());
    Service service   = m_repo->findById(serviceId);
    if (service.id.isEmpty()) return;

    QMenu menu(this);

    // Check Now
    QAction *checkNow = menu.addAction(
        QString("Check Now — %1").arg(service.name));
    connect(checkNow, &QAction::triggered, this, [this, serviceId]() {
        m_engine->checkNow(serviceId);
        statusBar()->showMessage("Manual check triggered", 2000);
    });

    menu.addSeparator();

    // Copy URL
    QAction *copyUrl = menu.addAction("Copy URL");
    connect(copyUrl, &QAction::triggered, this, [service]() {
        QApplication::clipboard()->setText(service.url);
    });

    menu.addSeparator();

    // Remove Service
    QAction *remove = menu.addAction(
        QString("Remove %1").arg(service.name));
    remove->setIcon(QApplication::style()->standardIcon(
        QStyle::SP_TrashIcon));
    connect(remove, &QAction::triggered, this, [this, serviceId]() {
        m_repo->removeService(serviceId);
        m_statusLabel->setText(
            QString("%1 service(s) monitored").arg(m_repo->count())
            );
    });

    menu.exec(m_tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::onExportLogs()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        "Export Logs",
        QDir::homePath() + "/devpulse-logs.txt",
        "Text Files (*.txt);;All Files (*)"
        );

    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Export Failed",
                             "Cannot write to: " + path);
        return;
    }

    QTextStream out(&file);
    out << "DevPulse Log Export\n";
    out << "Generated: "
        << QDateTime::currentDateTime().toString(Qt::ISODate)
        << "\n";
    out << QString("=").repeated(60) << "\n\n";

    for (int i = 0; i < m_logModel->rowCount(); ++i) {
        QModelIndex idx = m_logModel->index(i, 0);
        out << m_logModel->data(idx, Qt::DisplayRole).toString() << "\n";
    }

    file.close();
    statusBar()->showMessage("Logs exported to: " + path, 3000);
}

void MainWindow::saveLastProfile(const QString &path)
{
    QSettings settings("devpulse", "devpulse");
    settings.setValue("lastProfile", path);
}

void MainWindow::autoLoadLastProfile()
{
    QSettings settings("devpulse", "devpulse");
    QString path = settings.value("lastProfile").toString();

    if (path.isEmpty()) return;

    QFileInfo info(path);
    if (!info.exists()) return;

    QVector<Service> services = m_profileManager.load(path);
    if (services.isEmpty()) return;

    for (const Service &s : services)
        m_repo->addService(s);

    m_statusLabel->setText(
        QString("%1 service(s) monitored").arg(m_repo->count())
        );
    statusBar()->showMessage("Auto-loaded: " + path, 3000);
}
