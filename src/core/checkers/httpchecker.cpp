#include "httpchecker.h"
#include <QNetworkRequest>
#include <QUrl>

HttpChecker::HttpChecker(QObject *parent)
    : IChecker(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &HttpChecker::onReplyFinished);
}

void HttpChecker::check(const Service &service)
{
    QNetworkRequest request(QUrl(service.url));
    request.setTransferTimeout(service.timeoutMs);

    QNetworkReply *reply = m_manager->get(request);

    // Store timer for this reply
    QElapsedTimer *timer = new QElapsedTimer();
    timer->start();

    m_timers[reply]     = timer;
    m_serviceIds[reply] = service.id;
    m_thresholds[reply] = service.degradedThresholdMs;
}

void HttpChecker::onReplyFinished(QNetworkReply *reply)
{
    QString serviceId = m_serviceIds.value(reply);
    QElapsedTimer *timer = m_timers.value(reply, nullptr);
    int threshold = m_thresholds.value(reply, 2000);

    int latencyMs = timer ? static_cast<int>(timer->elapsed()) : -1;

    CheckResult result;
    result.serviceId = serviceId;
    result.latencyMs = latencyMs;

    if (reply->error() == QNetworkReply::NoError) {
        int statusCode = reply->attribute(
                                  QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode >= 200 && statusCode < 400) {
            result.status = (latencyMs > threshold)
            ? ServiceStatus::Degraded
            : ServiceStatus::Up;
        } else {
            result.status       = ServiceStatus::Down;
            result.errorMessage = QString("HTTP %1").arg(statusCode);
        }
    } else {
        result.status       = ServiceStatus::Down;
        result.errorMessage = reply->errorString();
    }

    // Cleanup
    delete m_timers.take(reply);
    m_serviceIds.remove(reply);
    m_thresholds.remove(reply);
    reply->deleteLater();

    emit checkFinished(result);
}
