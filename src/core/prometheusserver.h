#ifndef PROMETHEUSSERVER_H
#define PROMETHEUSSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "servicerepository.h"

class PrometheusServer : public QObject
{
    Q_OBJECT

public:
    explicit PrometheusServer(ServiceRepository *repo,
                              QObject *parent = nullptr);

    bool start(quint16 port = 9898);
    void stop();
    bool isRunning() const;
    quint16 port() const;

private slots:
    void onNewConnection();
    void onClientReadable();

private:
    QString buildMetrics() const;
    void sendResponse(QTcpSocket *socket, const QString &body);

    QTcpServer       *m_server;
    ServiceRepository *m_repo;
};

#endif // PROMETHEUSSERVER_H
