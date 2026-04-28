#ifndef PINGCHECKERPLUGIN_H
#define PINGCHECKERPLUGIN_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QElapsedTimer>
#include "core/plugins/ICheckerPlugin.h"
#include "core/checkers/IChecker.h"

// Standalone checker — no dependency on HttpChecker
class PingChecker : public IChecker
{
    Q_OBJECT
public:
    explicit PingChecker(QObject *parent = nullptr)
        : IChecker(parent)
        , m_manager(new QNetworkAccessManager(this))
    {
        connect(m_manager, &QNetworkAccessManager::finished,
                this, &PingChecker::onReply);
    }

    void check(const Service &service) override
    {
        QString fixed = service.url;
        fixed.replace("ping://", "https://");

        m_pendingId = service.id;
        m_threshold = service.degradedThresholdMs;
        m_timer.start();

        QUrl dest(fixed);
        QNetworkRequest req;
        req.setUrl(dest);
        req.setTransferTimeout(service.timeoutMs);
        m_manager->get(req);
    }

private slots:
    void onReply(QNetworkReply *reply)
    {
        int latency = static_cast<int>(m_timer.elapsed());
        CheckResult result;
        result.serviceId = m_pendingId;
        result.latencyMs = latency;

        if (reply->error() == QNetworkReply::NoError) {
            result.status = (latency > m_threshold)
            ? ServiceStatus::Degraded
            : ServiceStatus::Up;
        } else {
            result.status       = ServiceStatus::Down;
            result.errorMessage = reply->errorString();
        }

        reply->deleteLater();
        emit checkFinished(result);
    }

private:
    QNetworkAccessManager *m_manager;
    QElapsedTimer          m_timer;
    QString                m_pendingId;
    int                    m_threshold = 2000;
};

class PingCheckerPlugin : public QObject, public ICheckerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ICheckerPlugin_IID)
    Q_INTERFACES(ICheckerPlugin)

public:
    QString name() const override { return "PingChecker"; }
    QString version() const override { return "1.0.0"; }
    QString description() const override {
        return "Standalone HTTP ping checker plugin";
    }
    QStringList supportedSchemes() const override {
        return {"ping"};
    }
    IChecker* createChecker(QObject *parent = nullptr) override {
        return new PingChecker(parent);
    }
};

#endif // PINGCHECKERPLUGIN_H
