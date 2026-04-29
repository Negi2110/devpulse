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
    // Add mode
    explicit AddServiceDialog(QWidget *parent = nullptr);

    // Edit mode — pre-fills fields with existing service
    explicit AddServiceDialog(const Service &service,
                              QWidget *parent = nullptr);

    Service service() const;

private slots:
    void onAccepted();

private:
    void setupUi();
    void setupValidation();
    void populateFields(const Service &service);

    QLineEdit        *m_nameEdit;
    QLineEdit        *m_urlEdit;
    QSpinBox         *m_intervalSpin;
    QSpinBox         *m_thresholdSpin;
    QDialogButtonBox *m_buttonBox;

    Service m_service;
    bool    m_editMode = false;
};

#endif // ADDSERVICEDIALOG_H
