#include "logpanelwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

LogPanelWidget::LogPanelWidget(LogModel *model, QWidget *parent)
    : QWidget(parent)
{
    setupUi(model);
}

void LogPanelWidget::setupUi(LogModel *model)
{
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_listView = new QListView(this);
    m_listView->setModel(m_proxyModel);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setUniformItemSizes(true);
    m_listView->setStyleSheet(
        "QListView { background-color: #1e1e1e; color: #d4d4d4; "
        "font-family: monospace; font-size: 12px; }"
        );

    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText("Filter logs...");

    connect(m_filterEdit, &QLineEdit::textChanged,
            m_proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    // Auto-scroll when new entries arrive
    connect(model, &LogModel::rowsInserted,
            this, &LogPanelWidget::scrollToBottom);

    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Filter:", this));
    filterLayout->addWidget(m_filterEdit);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(filterLayout);
    layout->addWidget(m_listView);
}

void LogPanelWidget::scrollToBottom()
{
    m_listView->scrollToBottom();
}
