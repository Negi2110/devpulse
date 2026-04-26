#ifndef LATENCYGRAPHWIDGET_H
#define LATENCYGRAPHWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include "core/monitoring/latencystore.h"

QT_CHARTS_USE_NAMESPACE

    class LatencyGraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LatencyGraphWidget(QWidget *parent = nullptr);

    // Call this when a new check result arrives
    void updateForService(const QString &serviceId,
                          const LatencyStore &store);

    // Reset graph when user selects different service
    void clearGraph();

    void setTitle(const QString &title);

private:
    void setupChart();

    QChartView  *m_chartView;
    QChart      *m_chart;
    QLineSeries *m_series;
    QValueAxis  *m_axisX;
    QValueAxis  *m_axisY;

    static constexpr int MAX_POINTS = 60;

};

#endif // LATENCYGRAPHWIDGET_H
