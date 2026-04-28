#ifndef WEBHOOKALERTER_H
#define WEBHOOKALERTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include "Service.h"

class WebhookAlerter : public QObject
{
    Q_OBJECT

public:
    explicit WebhookAlerter(QObject *parent = nullptr);

    void setWebhookUrl(const QString &url);
    QString webhookUrl() const { return m_webhookUrl; }
    bool isConfigured() const { return !m_webhookUrl.isEmpty(); }

public slots:
    void onServiceStatusChanged(const Service &service);

private:
    void postAlert(const QString &message);

    QNetworkAccessManager *m_manager;
    QString                m_webhookUrl;

    // Track last status to detect transitions
    QMap<QString, ServiceStatus> m_lastStatus;
};

#endif // WEBHOOKALERTER_H
