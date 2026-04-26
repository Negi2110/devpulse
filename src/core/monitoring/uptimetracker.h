#ifndef UPTIMETRACKER_H
#define UPTIMETRACKER_H

#include <QMap>
#include <QString>
#include <QVector>
#include "core/Service.h"

class UptimeTracker
{
public:
    explicit UptimeTracker(int windowSize = 100);

    // Record a check result
    void record(const QString &serviceId, bool wasUp);

    // Get uptime percentage 0.0 - 100.0
    double uptimePercent(const QString &serviceId) const;

    // Total checks recorded
    int totalChecks(const QString &serviceId) const;

    void removeService(const QString &serviceId);

private:
    struct Window {
        QVector<bool> data;
        int head  = 0;
        int count = 0;
    };

    int m_windowSize;
    QMap<QString, Window> m_windows;

    const Window* windowFor(const QString &serviceId) const;
};

#endif // UPTIMETRACKER_H
