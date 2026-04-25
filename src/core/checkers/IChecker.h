#ifndef ICHECKER_H
#define ICHECKER_H

#include <QObject>
#include "core/Service.h"


// CheckResult — what a single check returns
struct CheckResult {
    QString       serviceId;
    ServiceStatus status;
    int           latencyMs;
    QString       errorMessage;  // empty if ok
};

// Pure abstract interface — all checkers must implement check()
class IChecker : public QObject
{
    Q_OBJECT

public:
    explicit IChecker(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IChecker() = default;

    // Trigger a check — result comes back via checkFinished signal
    virtual void check(const Service &service) = 0;

signals:
    void checkFinished(const CheckResult &result);
};

#endif // ICHECKER_H
