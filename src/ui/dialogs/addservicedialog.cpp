#include "addservicedialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

AddServiceDialog::AddServiceDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setupValidation();
}

void AddServiceDialog::setupUi()
{
    setWindowTitle("Add Service");
    setMinimumWidth(400);

    // Form fields
    m_nameEdit     = new QLineEdit(this);
    m_urlEdit      = new QLineEdit(this);
    m_intervalSpin = new QSpinBox(this);
    m_thresholdSpin = new QSpinBox(this);

    m_nameEdit->setPlaceholderText("e.g. Auth API");
    m_urlEdit->setPlaceholderText("e.g. http://localhost:3000 or localhost:5432");

    m_intervalSpin->setRange(5, 300);
    m_intervalSpin->setValue(30);
    m_intervalSpin->setSuffix(" sec");

    m_thresholdSpin->setRange(100, 10000);
    m_thresholdSpin->setValue(2000);
    m_thresholdSpin->setSuffix(" ms");

    // Form layout — label + field pairs
    QFormLayout *form = new QFormLayout;
    form->addRow("Service Name:", m_nameEdit);
    form->addRow("URL / Address:", m_urlEdit);
    form->addRow("Check Interval:", m_intervalSpin);
    form->addRow("Degraded Threshold:", m_thresholdSpin);

    // OK / Cancel buttons
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this
        );

    QVBoxLayout *main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addWidget(m_buttonBox);

    connect(m_buttonBox, &QDialogButtonBox::accepted,
            this, &AddServiceDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

void AddServiceDialog::setupValidation()
{
    // Disable OK until both name and URL are filled
    auto validate = [this]() {
        bool ok = !m_nameEdit->text().trimmed().isEmpty() &&
                  !m_urlEdit->text().trimmed().isEmpty();
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
    };

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(m_nameEdit, &QLineEdit::textChanged, this, validate);
    connect(m_urlEdit,  &QLineEdit::textChanged, this, validate);
}

void AddServiceDialog::onAccepted()
{
    m_service = Service::create(
        m_nameEdit->text().trimmed(),
        m_urlEdit->text().trimmed()
        );
    m_service.intervalSecs        = m_intervalSpin->value();
    m_service.degradedThresholdMs = m_thresholdSpin->value();

    accept();
}

Service AddServiceDialog::service() const
{
    return m_service;
}
