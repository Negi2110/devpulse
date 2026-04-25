#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QUuid>
// WHY enum class: scoped, no name pollution, can't accidentally
// compare with integers. Always use enum class in modern C++.

enum class ServiceStatus {
    Unknown,    // never been checked yet
    Up,         // responding fine
    Degraded,   // responding but too slow
    Down        // not responding
};

enum class ServiceType {
    Http,       // we'll use QNetworkAccessManager
    Tcp         // we'll use QTcpSocket
};

struct Service
{
    // Q_GADGET lets us use Q_ENUM without inheriting QObject
    // This means Qt knows the enum names as strings at runtime
    Q_GADGET
    Q_ENUM(ServiceStatus)
    Q_ENUM(ServiceType)

    // --- Identity ---
    QString id;             // UUID, generated at creation
    QString name;           // "Auth API", "Postgres" etc
    QString url;            // "http://localhost:3000" or "localhost:5432"

    // --- Config ---
    ServiceType type        = ServiceType::Http;
    int intervalSecs        = 30;       // check every 30 seconds
    int timeoutMs           = 5000;     // give up after 5 seconds
    int degradedThresholdMs = 2000;     // slower than 2s = Degraded

    // --- Live State ---
    ServiceStatus status    = ServiceStatus::Unknown;
    int latencyMs           = -1;       // -1 means not measured yet
    double uptimePercent    = 100.0;

    // --- Timestamps ---
    QDateTime lastChecked;  // when did we last ping it
    QDateTime addedAt;      // when did user add it

    // --- Optional ---
    QString notes;          // user can add a note

    // --- Factory method ---
    // Static because it creates a Service without needing an existing one
    static Service create(const QString &name, const QString &url)
    {
        Service s;
        s.id      = QUuid::createUuid().toString(QUuid::WithoutBraces);
        s.name    = name;
        s.url     = url;
        s.addedAt = QDateTime::currentDateTimeUtc();

        // Auto-detect type from URL
        if (url.startsWith("http://") || url.startsWith("https://")) {
            s.type = ServiceType::Http;
        } else {
            s.type = ServiceType::Tcp;
        }

        return s;
    }

    // --- Helper ---
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
