#ifndef TCPCHECKER_H
#define TCPCHECKER_H

#include "core/checkers/IChecker.h"
#include <QTcpSocket>
#include <QTimer>
#include <QElapsedTimer>
#include <QMap>

class TcpChecker : public IChecker
{
    Q_OBJECT

public:
    explicit TcpChecker(QObject *parent = nullptr);

    void check(const Service &service) override;

private slots:
    void onConnected();
    void onError(QAbstractSocket::SocketError error);
    void onTimeout();

private:
    struct PendingCheck {
        QString       serviceId;
        int           degradedThresholdMs;
        QElapsedTimer timer;
        QTimer        *timeoutTimer;
    };

    QMap<QTcpSocket*, PendingCheck*> m_pending;

    void cleanup(QTcpSocket *socket);
};

#endif // TCPCHECKER_H
