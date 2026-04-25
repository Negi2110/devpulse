#include "tcpchecker.h"

TcpChecker::TcpChecker(QObject *parent)
    : IChecker(parent)
{}

void TcpChecker::check(const Service &service)
{
    // Parse host and port from "localhost:5432"
    QString address = service.url;
    // Remove tcp:// prefix if present
    if (address.startsWith("tcp://"))
        address = address.mid(6);

    int colonPos = address.lastIndexOf(':');
    if (colonPos < 0) {
        CheckResult result;
        result.serviceId    = service.id;
        result.status       = ServiceStatus::Down;
        result.latencyMs    = -1;
        result.errorMessage = "Invalid address — expected host:port";
        emit checkFinished(result);
        return;
    }

    QString host = address.left(colonPos);
    quint16 port = address.mid(colonPos + 1).toUShort();

    QTcpSocket *socket = new QTcpSocket(this);

    PendingCheck *pending   = new PendingCheck();
    pending->serviceId      = service.id;
    pending->degradedThresholdMs = service.degradedThresholdMs;
    pending->timeoutTimer   = new QTimer(this);
    pending->timer.start();

    m_pending[socket] = pending;

    // Timeout timer
    pending->timeoutTimer->setSingleShot(true);
    pending->timeoutTimer->setInterval(service.timeoutMs);

    connect(socket, &QTcpSocket::connected,
            this, &TcpChecker::onConnected);
    connect(socket, &QAbstractSocket::errorOccurred,
            this, &TcpChecker::onError);
    connect(pending->timeoutTimer, &QTimer::timeout,
            this, &TcpChecker::onTimeout);

    pending->timeoutTimer->start();
    socket->connectToHost(host, port);
}

void TcpChecker::onConnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket || !m_pending.contains(socket)) return;

    PendingCheck *pending = m_pending[socket];
    int latencyMs = static_cast<int>(pending->timer.elapsed());

    CheckResult result;
    result.serviceId = pending->serviceId;
    result.latencyMs = latencyMs;
    result.status    = (latencyMs > pending->degradedThresholdMs)
                        ? ServiceStatus::Degraded
                        : ServiceStatus::Up;

    cleanup(socket);
    emit checkFinished(result);
}

void TcpChecker::onError(QAbstractSocket::SocketError)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket || !m_pending.contains(socket)) return;

    PendingCheck *pending = m_pending[socket];

    CheckResult result;
    result.serviceId    = pending->serviceId;
    result.latencyMs    = -1;
    result.status       = ServiceStatus::Down;
    result.errorMessage = socket->errorString();

    cleanup(socket);
    emit checkFinished(result);
}

void TcpChecker::onTimeout()
{
    QTimer *timeoutTimer = qobject_cast<QTimer*>(sender());

    // Find which socket this timeout belongs to
    QTcpSocket *targetSocket = nullptr;
    for (auto it = m_pending.begin(); it != m_pending.end(); ++it) {
        if (it.value()->timeoutTimer == timeoutTimer) {
            targetSocket = it.key();
            break;
        }
    }

    if (!targetSocket) return;

    PendingCheck *pending = m_pending[targetSocket];

    CheckResult result;
    result.serviceId    = pending->serviceId;
    result.latencyMs    = -1;
    result.status       = ServiceStatus::Down;
    result.errorMessage = "Connection timed out";

    cleanup(targetSocket);
    emit checkFinished(result);
}

void TcpChecker::cleanup(QTcpSocket *socket)
{
    if (!m_pending.contains(socket)) return;

    PendingCheck *pending = m_pending.take(socket);
    pending->timeoutTimer->stop();
    pending->timeoutTimer->deleteLater();
    delete pending;

    socket->abort();
    socket->deleteLater();
}
