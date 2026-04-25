#ifndef HTTPCCHECKER_H
#define HTTPCCHECKER_H

#include "core/checkers/IChecker.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QElapsedTimer>
#include <QMap>

class HttpChecker : public IChecker
{
    Q_OBJECT

public:
    explicit HttpChecker(QObject *parent = nullptr);

    void check(const Service &service) override;

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;

    // Track start time per reply so we can measure latency
    QMap<QNetworkReply*, QElapsedTimer*> m_timers;

    // Track which reply belongs to which service
    QMap<QNetworkReply*, QString> m_serviceIds;

    // Track degraded threshold per reply
    QMap<QNetworkReply*, int> m_thresholds;
};

#endif // HTTPCCHECKER_H
