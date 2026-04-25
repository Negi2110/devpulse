#ifndef ADDSERVICEDIALOG_H
#define ADDSERVICEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include "core/Service.h"

class AddServiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddServiceDialog(QWidget *parent = nullptr);

    // Call this after exec() returns Accepted
    Service service() const;

private slots:
    void onAccepted();

private:
    void setupUi();
    void setupValidation();

    QLineEdit       *m_nameEdit;
    QLineEdit       *m_urlEdit;
    QSpinBox        *m_intervalSpin;
    QSpinBox        *m_thresholdSpin;
    QDialogButtonBox *m_buttonBox;

    Service m_service;
};

#endif // ADDSERVICEDIALOG_H
