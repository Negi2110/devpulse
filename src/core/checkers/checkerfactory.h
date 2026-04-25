#ifndef CHECKERFACTORY_H
#define CHECKERFACTORY_H

#include "core/checkers/IChecker.h"
#include "core/Service.h"

class CheckerFactory
{
public:
    // Returns the right IChecker subclass for this service type
    // Caller does NOT own the checker if parent is set
    static IChecker* create(ServiceType type, QObject *parent = nullptr);

    // Convenience overload — reads type from service directly
    static IChecker* create(const Service &service, QObject *parent = nullptr);

private:
    // Pure static class — no instances allowed
    CheckerFactory() = delete;
};

#endif // CHECKERFACTORY_H
