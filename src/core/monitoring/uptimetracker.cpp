#include "uptimetracker.h"

UptimeTracker::UptimeTracker(int windowSize)
    : m_windowSize(windowSize)
{}

void UptimeTracker::record(const QString &serviceId, bool wasUp)
{
    if (!m_windows.contains(serviceId)) {
        Window w;
        w.data.resize(m_windowSize);
        w.data.fill(false);
        w.head  = 0;
        w.count = 0;
        m_windows[serviceId] = w;
    }

    Window &w = m_windows[serviceId];
    w.data[w.head] = wasUp;
    w.head = (w.head + 1) % m_windowSize;
    if (w.count < m_windowSize)
        w.count++;
}

double UptimeTracker::uptimePercent(const QString &serviceId) const
{
    const Window *w = windowFor(serviceId);
    if (!w || w->count == 0) return 100.0;

    int upCount = 0;
    for (int i = 0; i < w->count; ++i)
        if (w->data[i]) upCount++;

    return (static_cast<double>(upCount) / w->count) * 100.0;
}

int UptimeTracker::totalChecks(const QString &serviceId) const
{
    const Window *w = windowFor(serviceId);
    return w ? w->count : 0;
}

void UptimeTracker::removeService(const QString &serviceId)
{
    m_windows.remove(serviceId);
}

const UptimeTracker::Window*
UptimeTracker::windowFor(const QString &serviceId) const
{
    auto it = m_windows.find(serviceId);
    if (it == m_windows.end()) return nullptr;
    return &it.value();
}
