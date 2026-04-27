#ifndef MONITORENGINE_H
#define MONITORENGINE_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include "Service.h"
#include "servicerepository.h"
#include "checkers/IChecker.h"
#include <QVariant>
#include "monitoring/latencystore.h"
#include "models/logmodel.h"
#include "monitoring/uptimetracker.h"
#include "storage/databasemanager.h"

class MonitorEngine : public QObject
{
    Q_OBJECT

public:
    explicit MonitorEngine(ServiceRepository *repo,
                           QObject *parent = nullptr);

    void startMonitoring(const QString &serviceId);
    void stopMonitoring(const QString &serviceId);
    void checkNow(const QString &serviceId);
    const LatencyStore& latencyStore() const { return m_latencyStore; }
    double uptimePercent(const QString &serviceId) const {
        return m_uptimeTracker.uptimePercent(serviceId);
    }
signals:
    void serviceStatusChanged(const Service &service);
    void logEntry(const LogEntry &entry);
private slots:
    void onTimerFired();
    void onCheckFinished(const CheckResult &result);

private:
    struct ServiceMonitor {
        QTimer   *timer;
        IChecker *checker;
    };

    void runCheck(const Service &service);

    ServiceRepository              *m_repo;
    QMap<QString, ServiceMonitor>   m_monitors;
    LatencyStore                    m_latencyStore;
    UptimeTracker m_uptimeTracker;
    DatabaseManager m_db;
};

#endif // MONITORENGINE_H
