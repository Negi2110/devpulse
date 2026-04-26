#include "latencygraphwidget.h"
#include <QVBoxLayout>
QT_CHARTS_USE_NAMESPACE

LatencyGraphWidget::LatencyGraphWidget(QWidget *parent)
    : QWidget(parent)
{
    setupChart();
}

void LatencyGraphWidget::setupChart()
{
    m_series = new QLineSeries();
    m_series->setName("Latency");
    m_series->setColor(QColor(0, 200, 100));

    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setTitle("Latency (ms)");
    m_chart->setAnimationOptions(QChart::NoAnimation);
    m_chart->legend()->hide();
    m_chart->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_chart->setTitleBrush(QBrush(Qt::white));

    // X axis — sample index
    m_axisX = new QValueAxis();
    m_axisX->setRange(0, MAX_POINTS);
    m_axisX->setLabelFormat("%d");
    m_axisX->setTitleText("Samples");
    m_axisX->setLabelsColor(Qt::white);
    m_axisX->setTitleBrush(QBrush(Qt::white));
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);

    // Y axis — latency ms
    m_axisY = new QValueAxis();
    m_axisY->setRange(0, 1000);
    m_axisY->setLabelFormat("%d ms");
    m_axisY->setLabelsColor(Qt::white);
    m_axisY->setTitleBrush(QBrush(Qt::white));
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_chartView);
}

void LatencyGraphWidget::updateForService(const QString &serviceId,
                                          const LatencyStore &store)
{
    QVector<int> data = store.readings(serviceId);
    if (data.isEmpty()) return;

    m_series->clear();

    int maxLatency = 100;
    int xIndex = 0;                    // ← add separate x counter
    for (int i = 0; i < data.size(); ++i) {
        int val = data[i];
        if (val < 0) continue;
        m_series->append(xIndex, val); // ← use xIndex not i
        xIndex++;
        if (val > maxLatency)
            maxLatency = val;
    }

    m_axisY->setRange(0, maxLatency * 1.2);
    m_axisX->setRange(0, qMax(xIndex, MAX_POINTS));
}

void LatencyGraphWidget::clearGraph()
{
    m_series->clear();
    m_axisY->setRange(0, 1000);
}

void LatencyGraphWidget::setTitle(const QString &title)
{
    m_chart->setTitle(title);
}
