#include "checkerfactory.h"
#include "core/checkers/httpchecker.h"
#include "core/checkers/tcpchecker.h"

IChecker* CheckerFactory::create(ServiceType type, QObject *parent)
{
    switch (type) {
    case ServiceType::Http:
        return new HttpChecker(parent);
    case ServiceType::Tcp:
        return new TcpChecker(parent);
    default:
        return new HttpChecker(parent);
    }
}

IChecker* CheckerFactory::create(const Service &service, QObject *parent)
{
    return create(service.type, parent);
}
