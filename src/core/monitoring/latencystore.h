#ifndef LATENCYSTORE_H
#define LATENCYSTORE_H

#include <QVector>
#include <QString>
#include <QMap>

class LatencyStore
{
public:
    explicit LatencyStore(int capacity = 60);

    // Add a new reading for a service
    void addReading(const QString &serviceId, int latencyMs);

    // Get all readings in chronological order
    QVector<int> readings(const QString &serviceId) const;

    // Stats
    int latest(const QString &serviceId) const;
    int minimum(const QString &serviceId) const;
    int maximum(const QString &serviceId) const;
    double average(const QString &serviceId) const;

    // Remove when service is deleted
    void removeService(const QString &serviceId);

    int capacity() const { return m_capacity; }

private:
    struct RingBuffer {
        QVector<int> data;
        int head  = 0;    // where next write goes
        int count = 0;    // how many valid entries
    };

    int m_capacity;
    QMap<QString, RingBuffer> m_buffers;

    const RingBuffer* bufferFor(const QString &serviceId) const;
};

#endif // LATENCYSTORE_H
