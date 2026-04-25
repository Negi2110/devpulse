#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QUuid>

enum class ServiceStatus {
    Unknown,
    Up,
    Degraded,
    Down
};

enum class ServiceType {
    Http,
    Tcp
};

struct Service
{
    Q_GADGET
    Q_ENUM(ServiceStatus)
    Q_ENUM(ServiceType)

public:
    QString id;
    QString name;
    QString url;
    ServiceType type            = ServiceType::Http;
    int intervalSecs            = 30;
    int timeoutMs               = 5000;
    int degradedThresholdMs     = 2000;
    ServiceStatus status        = ServiceStatus::Unknown;
    int latencyMs               = -1;
    double uptimePercent        = 100.0;
    QDateTime lastChecked;
    QDateTime addedAt;
    QString notes;

    static Service create(const QString &name, const QString &url)
    {
        Service s;
        s.id      = QUuid::createUuid().toString(QUuid::WithoutBraces);
        s.name    = name;
        s.url     = url;
        s.addedAt = QDateTime::currentDateTimeUtc();
        if (url.startsWith("http://") || url.startsWith("https://"))
            s.type = ServiceType::Http;
        else
            s.type = ServiceType::Tcp;
        return s;
    }

    QString statusString() const
    {
        switch (status) {
        case ServiceStatus::Up:       return "Up";
        case ServiceStatus::Down:     return "Down";
        case ServiceStatus::Degraded: return "Degraded";
        default:                      return "Unknown";
        }
    }
};

#endif // SERVICE_H
