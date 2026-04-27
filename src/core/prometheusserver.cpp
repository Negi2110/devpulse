#include "prometheusserver.h"
#include <QTcpSocket>
#include <QDateTime>

PrometheusServer::PrometheusServer(ServiceRepository *repo,
                                   QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_repo(repo)
{
    connect(m_server, &QTcpServer::newConnection,
            this, &PrometheusServer::onNewConnection);
}

bool PrometheusServer::start(quint16 port)
{
    if (m_server->isListening())
        return true;

    if (!m_server->listen(QHostAddress::LocalHost, port)) {
        return false;
    }
    return true;
}

void PrometheusServer::stop()
{
    m_server->close();
}

bool PrometheusServer::isRunning() const
{
    return m_server->isListening();
}

quint16 PrometheusServer::port() const
{
    return m_server->serverPort();
}

void PrometheusServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead,
                this, &PrometheusServer::onClientReadable);
        connect(socket, &QTcpSocket::disconnected,
                socket, &QTcpSocket::deleteLater);
    }
}

void PrometheusServer::onClientReadable()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString request = QString::fromUtf8(socket->readAll());

    // Only handle GET /metrics
    if (request.startsWith("GET /metrics")) {
        sendResponse(socket, buildMetrics());
    } else {
        // 404 for everything else
        QString response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
        socket->write(response.toUtf8());
    }

    socket->flush();
    socket->disconnectFromHost();
}

QString PrometheusServer::buildMetrics() const
{
    QString out;
    QVector<Service> services = m_repo->allServices();

    // devpulse_service_up
    out += "# HELP devpulse_service_up 1 if service is up, 0 if down\n";
    out += "# TYPE devpulse_service_up gauge\n";
    for (const Service &s : services) {
        int val = (s.status == ServiceStatus::Up ||
                   s.status == ServiceStatus::Degraded) ? 1 : 0;
        out += QString("devpulse_service_up{service=\"%1\"} %2\n")
                   .arg(s.name).arg(val);
    }

    out += "\n";

    // devpulse_latency_ms
    out += "# HELP devpulse_latency_ms Last measured latency in ms\n";
    out += "# TYPE devpulse_latency_ms gauge\n";
    for (const Service &s : services) {
        if (s.latencyMs >= 0) {
            out += QString("devpulse_latency_ms{service=\"%1\"} %2\n")
            .arg(s.name).arg(s.latencyMs);
        }
    }

    out += "\n";

    // devpulse_uptime_percent
    out += "# HELP devpulse_uptime_percent Rolling uptime percentage\n";
    out += "# TYPE devpulse_uptime_percent gauge\n";
    for (const Service &s : services) {
        out += QString("devpulse_uptime_percent{service=\"%1\"} %2\n")
        .arg(s.name).arg(s.uptimePercent, 0, 'f', 2);
    }

    return out;
}

void PrometheusServer::sendResponse(QTcpSocket *socket,
                                    const QString &body)
{
    QByteArray bodyBytes = body.toUtf8();
    QString response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; version=0.0.4\r\n"
        "Content-Length: " + QString::number(bodyBytes.size()) + "\r\n"
                                              "\r\n";
    socket->write(response.toUtf8());
    socket->write(bodyBytes);
}
