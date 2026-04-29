#include "monitorengine.h"
#include "checkers/checkerfactory.h"
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include "checkers/checkerfactory.h"
MonitorEngine::MonitorEngine(ServiceRepository *repo, QObject *parent)
    : QObject(parent)
    , m_repo(repo)
{
    connect(repo, &ServiceRepository::serviceAdded,
            this, [this](const Service &s) {
                startMonitoring(s.id);
            });
    connect(repo, &ServiceRepository::serviceRemoved,
            this, [this](const QString &id) {
                stopMonitoring(id);
            });

    QString dbPath = QDir::homePath() + "/.devpulse.db";
    m_db.open(dbPath);
    QString pluginDir = QCoreApplication::applicationDirPath()
                        + "/../../plugins/bin";
    int loaded = m_pluginLoader.loadPlugins(pluginDir);
    if (loaded > 0)
        CheckerFactory::setPluginLoader(&m_pluginLoader);
}
void MonitorEngine::clearLatencyHistory(const QString &serviceId)
{
    m_latencyStore.removeService(serviceId);
    m_uptimeTracker.removeService(serviceId);
}
void MonitorEngine::startMonitoring(const QString &serviceId)
{
    if (m_monitors.contains(serviceId))
        return;

    Service service = m_repo->findById(serviceId);
    if (service.id.isEmpty())
        return;

    IChecker *checker = CheckerFactory::create(service, this);
    connect(checker, &IChecker::checkFinished,
            this, &MonitorEngine::onCheckFinished);

    QTimer *timer = new QTimer(this);
    timer->setInterval(service.intervalSecs * 1000);
    timer->setProperty("serviceId", serviceId);

    connect(timer, &QTimer::timeout,
            this, &MonitorEngine::onTimerFired);

    m_monitors[serviceId] = { timer, checker };
    timer->start();

    runCheck(service);
}

void MonitorEngine::stopMonitoring(const QString &serviceId)
{
    if (!m_monitors.contains(serviceId))
        return;

    ServiceMonitor mon = m_monitors.take(serviceId);
    mon.timer->stop();
    mon.timer->deleteLater();
    mon.checker->deleteLater();
}

void MonitorEngine::checkNow(const QString &serviceId)
{
    Service service = m_repo->findById(serviceId);
    if (!service.id.isEmpty())
        runCheck(service);
}

void MonitorEngine::onTimerFired()
{
    QTimer *timer = qobject_cast<QTimer*>(sender());
    if (!timer) return;

    QString serviceId = timer->property("serviceId").toString();
    Service service = m_repo->findById(serviceId);
    if (!service.id.isEmpty())
        runCheck(service);
}

void MonitorEngine::onCheckFinished(const CheckResult &result)
{
    Service service = m_repo->findById(result.serviceId);
    if (service.id.isEmpty())
        return;

    service.status     = result.status;
    service.latencyMs   = result.latencyMs;
    service.lastChecked = QDateTime::currentDateTimeUtc();
    service.uptimePercent = m_uptimeTracker.uptimePercent(result.serviceId);
    if (result.latencyMs > 0)
        m_latencyStore.addReading(result.serviceId,result.latencyMs);
    if (m_db.isOpen()) {
        m_db.insertCheckResult(
            result.serviceId,
            service.statusString(),
            result.latencyMs
            );
    }
    m_uptimeTracker.record(result.serviceId,
                           result.status != ServiceStatus::Down);
    LogEntry entry;
    entry.timestamp   = QDateTime::currentDateTimeUtc();
    entry.serviceId   = service.id;
    entry.serviceName = service.name;
    entry.level      = (result.status == ServiceStatus::Down)
                      ? LogLevel::Error
                      : (result.status == ServiceStatus::Degraded)
                            ? LogLevel::Warning
                            : LogLevel::Info;
    entry.message     = QString("Status: %1 | Latency: %2ms")
                        .arg(service.statusString())
                        .arg(result.latencyMs);

    emit logEntry(entry);

    m_repo->updateService(service);
    emit serviceStatusChanged(service);
}

void MonitorEngine::runCheck(const Service &service)
{
    if (!m_monitors.contains(service.id))
        return;

    m_monitors[service.id].checker->check(service);
}
