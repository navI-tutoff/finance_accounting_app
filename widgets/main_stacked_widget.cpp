#include "main_stacked_widget.h"
#include "ui_main_stacked_widget.h"

#include <QChart>
#include <QChartView>
#include <QLineSeries>

MainStackedWidget::MainStackedWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainStackedWidget)
{
    ui->setupUi(this);

    ///
    /// Текущую стоимость монеты разделить на текущую цену портфеля
    /// Умножить на 100. Так получаем %-ное соотношение стоимости монеты к общей стоимости портфеля
    ///

    double portfolioCost = this->totalCryptoStatistic.value("portfolioCost");
    ui->totalBalanceLabel->setText("Общая стоимость портфеля: " + QString::number(portfolioCost) + " $");

    QLineSeries *lineSeries = new QLineSeries(this);
    lineSeries->append(0, 2);
    lineSeries->append(1, 3);
    lineSeries->append(2, 5);
    lineSeries->append(3, 4);
    lineSeries->append(4, 7);

    QChart *chart = new QChart();
    chart->addSeries(lineSeries);
    chart->legend()->setVisible(false);

    ui->pieChartView->setChart(chart);
}

void MainStackedWidget::saveTotalCryptoStatistic(QMap<QString, double> map) {
    qDebug() << map;
    this->totalCryptoStatistic = map;
}

MainStackedWidget::~MainStackedWidget() {
    delete ui;
}
