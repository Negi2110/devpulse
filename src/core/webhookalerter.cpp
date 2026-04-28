#include "webhookalerter.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>


WebhookAlerter::WebhookAlerter(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{}

void WebhookAlerter::setWebhookUrl(const QString &url)
{
    m_webhookUrl = url;
}

void WebhookAlerter::onServiceStatusChanged(const Service &service)
{
    if (!isConfigured()) return;

    ServiceStatus previous = m_lastStatus.value(
        service.id, ServiceStatus::Unknown);

    // DOWN transition
    if (previous != ServiceStatus::Down &&
        service.status == ServiceStatus::Down) {
        postAlert(QString("🔴 *Service Down*: %1 is not responding | %2")
                      .arg(service.name)
                      .arg(service.url));
    }

    // RECOVERY transition
    if (previous == ServiceStatus::Down &&
        service.status != ServiceStatus::Down) {
        postAlert(QString("✅ *Service Recovered*: %1 is back online | Latency: %2ms")
                      .arg(service.name)
                      .arg(service.latencyMs));
    }

    // DEGRADED transition
    if (previous != ServiceStatus::Degraded &&
        service.status == ServiceStatus::Degraded) {
        postAlert(QString("🟡 *Service Degraded*: %1 is responding slowly | Latency: %2ms")
                      .arg(service.name)
                      .arg(service.latencyMs));
    }

    m_lastStatus[service.id] = service.status;
}

void WebhookAlerter::postAlert(const QString &message)
{
    if (m_webhookUrl.isEmpty()) return;

    QJsonObject payload;
    payload["text"] = message;

    QUrl url(m_webhookUrl);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/json");

    QByteArray body = QJsonDocument(payload).toJson();
    QNetworkReply *reply = m_manager->post(req, body);

    connect(reply, &QNetworkReply::finished,
            reply, &QNetworkReply::deleteLater);
}
