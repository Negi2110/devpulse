#include "traymanager.h"
#include <QApplication>
#include <QWidget>
#include <QStyle>

TrayManager::TrayManager(QWidget *mainWindow, QObject *parent)
    : QObject(parent)
{
    setupTray(mainWindow);
}

void TrayManager::setupTray(QWidget *mainWindow)
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayMenu = new QMenu(mainWindow);

    QAction *showAction = new QAction("Show DevPulse", this);
    connect(showAction, &QAction::triggered,
            this, &TrayManager::showMainWindowRequested);

    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered,
            this, &TrayManager::quitRequested);

    m_trayMenu->addAction(showAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);

    // Use a built-in Qt icon as placeholder
    m_trayIcon->setIcon(QApplication::style()->standardIcon(
        QStyle::SP_ComputerIcon));
    m_trayIcon->setToolTip("DevPulse — All systems operational");

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger)
                    emit showMainWindowRequested();
            });

    m_trayIcon->show();
}

void TrayManager::onServiceStatusChanged(const Service &service)
{
    ServiceStatus previous = m_lastStatus.value(
        service.id, ServiceStatus::Unknown);

    bool wasDown = (previous == ServiceStatus::Down);
    bool isDown  = (service.status == ServiceStatus::Down);
    bool wasDegraded = (previous == ServiceStatus::Degraded);
    bool isDegraded  = (service.status == ServiceStatus::Degraded);

    // Detect DOWN transition
    if (!wasDown && isDown) {
        m_downCount++;
        showNotification(
            "Service Down",
            service.name + " is not responding",
            QSystemTrayIcon::Critical
            );
    }

    // Detect RECOVERY
    if (wasDown && !isDown) {
        m_downCount = qMax(0, m_downCount - 1);
        showNotification(
            "Service Recovered",
            service.name + " is back online",
            QSystemTrayIcon::Information
            );
    }

    // Detect DEGRADED
    if (!wasDegraded && isDegraded && !isDown) {
        showNotification(
            "Service Degraded",
            service.name + " is responding slowly",
            QSystemTrayIcon::Warning
            );
    }

    m_lastStatus[service.id] = service.status;
    updateTrayIcon();
}

void TrayManager::updateTrayIcon()
{
    QStyle *style = QApplication::style();

    if (m_downCount > 0) {
        m_trayIcon->setIcon(style->standardIcon(
            QStyle::SP_MessageBoxCritical));
        m_trayIcon->setToolTip(
            QString("DevPulse — %1 service(s) DOWN").arg(m_downCount));
    } else if (m_degradedCount > 0) {
        m_trayIcon->setIcon(style->standardIcon(
            QStyle::SP_MessageBoxWarning));
        m_trayIcon->setToolTip("DevPulse — Some services degraded");
    } else {
        m_trayIcon->setIcon(style->standardIcon(
            QStyle::SP_ComputerIcon));
        m_trayIcon->setToolTip("DevPulse — All systems operational");
    }
}

void TrayManager::showNotification(const QString &title,
                                   const QString &message,
                                   QSystemTrayIcon::MessageIcon icon)
{
    if (!m_trayIcon->supportsMessages()) return;
    m_trayIcon->showMessage(title, message, icon, 4000);
}

bool TrayManager::isAvailable() const
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}
