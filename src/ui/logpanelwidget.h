#ifndef LOGPANELWIDGET_H
#define LOGPANELWIDGET_H

#include <QWidget>
#include <QListView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include "models/logmodel.h"

class LogPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogPanelWidget(LogModel *model, QWidget *parent = nullptr);

    void scrollToBottom();

private:
    void setupUi(LogModel *model);

    QListView            *m_listView;
    QLineEdit            *m_filterEdit;
    QSortFilterProxyModel *m_proxyModel;
};

#endif // LOGPANELWIDGET_H


