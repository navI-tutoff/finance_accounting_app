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

    ui->totalBalanceLabel->setText("Стоимость портфеля: Загрузка...");

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

void MainStackedWidget::updateTotalCryptoStatistic(const QMap<QString, double> &totalCryptoStatMap) {
    qDebug() << totalCryptoStatMap;
    this->totalCryptoStatistic = totalCryptoStatMap;

    this->ui->totalBalanceLabel->setText("Стоимость портфеля: " + QString::number(totalCryptoStatMap.value("portfolioCost")) + " $");
}

MainStackedWidget::~MainStackedWidget() {
    delete ui;
}
