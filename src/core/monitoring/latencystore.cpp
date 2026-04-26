#include "latencystore.h"
#include <algorithm>

LatencyStore::LatencyStore(int capacity)
    : m_capacity(capacity)
{}

void LatencyStore::addReading(const QString &serviceId, int latencyMs)
{
    if (!m_buffers.contains(serviceId)) {
        RingBuffer buf;
        buf.data.resize(m_capacity);
        buf.data.fill(-1);
        buf.head  = 0;
        buf.count = 0;
        m_buffers[serviceId] = buf;
    }

    RingBuffer &buf = m_buffers[serviceId];
    buf.data[buf.head] = latencyMs;
    buf.head = (buf.head + 1) % m_capacity;
    if (buf.count < m_capacity)
        buf.count++;
}

QVector<int> LatencyStore::readings(const QString &serviceId) const
{
    const RingBuffer *buf = bufferFor(serviceId);
    if (!buf) return {};

    QVector<int> result;
    result.reserve(buf->count);

    // Read from oldest to newest
    int start = (buf->count < m_capacity)
                    ? 0
                    : buf->head;

    for (int i = 0; i < buf->count; ++i) {
        int idx = (start + i) % m_capacity;
        result.append(buf->data[idx]);
    }

    return result;
}

int LatencyStore::latest(const QString &serviceId) const
{
    const RingBuffer *buf = bufferFor(serviceId);
    if (!buf || buf->count == 0) return -1;
    int lastIdx = (buf->head - 1 + m_capacity) % m_capacity;
    return buf->data[lastIdx];
}

int LatencyStore::minimum(const QString &serviceId) const
{
    QVector<int> r = readings(serviceId);
    if (r.isEmpty()) return -1;
    return *std::min_element(r.begin(), r.end());
}

int LatencyStore::maximum(const QString &serviceId) const
{
    QVector<int> r = readings(serviceId);
    if (r.isEmpty()) return -1;
    return *std::max_element(r.begin(), r.end());
}

double LatencyStore::average(const QString &serviceId) const
{
    QVector<int> r = readings(serviceId);
    if (r.isEmpty()) return -1.0;
    double sum = 0;
    for (int v : r) sum += v;
    return sum / r.size();
}

void LatencyStore::removeService(const QString &serviceId)
{
    m_buffers.remove(serviceId);
}

const LatencyStore::RingBuffer*
LatencyStore::bufferFor(const QString &serviceId) const
{
    auto it = m_buffers.find(serviceId);
    if (it == m_buffers.end()) return nullptr;
    return &it.value();
}
