#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMap>
#include "core/Service.h"

class TrayManager : public QObject
{
    Q_OBJECT

public:
    explicit TrayManager(QWidget *mainWindow,
                         QObject *parent = nullptr);

    // Call this whenever a service status changes
    void onServiceStatusChanged(const Service &service);

    bool isAvailable() const;

signals:
    void showMainWindowRequested();
    void quitRequested();

private:
    void setupTray(QWidget *mainWindow);
    void updateTrayIcon();
    void showNotification(const QString &title,
                          const QString &message,
                          QSystemTrayIcon::MessageIcon icon);

    QSystemTrayIcon *m_trayIcon;
    QMenu           *m_trayMenu;

    // Track last known status per service to detect changes
    QMap<QString, ServiceStatus> m_lastStatus;

    // Count of services in each state
    int m_downCount     = 0;
    int m_degradedCount = 0;
    int m_totalCount    = 0;
};

#endif // TRAYMANAGER_H
