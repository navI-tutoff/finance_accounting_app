#include "main_stacked_widget.h"
#include "ui_main_stacked_widget.h"

#include <QChart>
#include <QChartView>
#include <QPieSeries>

enum Columns  {
    Coin,
    Volume,
    AvgBuyPrice,
    CurrentPrice,
    Amount,
    TotalCost, // current cost of the coin in the portfolio
    Profit,
    ProfitPercent
};

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

    QVector<QPair<QString, double>> coinAndPriceVector{};

    // QPieSeries *pieCryptoSeries = new QPieSeries();
    // pieCryptoSeries->append("ETH", .50);
    // pieCryptoSeries->append("BTC", .25);
    // pieCryptoSeries->append("LTC", .25);



    // QChart *chart = new QChart();
    // chart->addSeries(pieCryptoSeries);
    // chart->legend()->setVisible(false);

    // ui->pieChartView->setChart(chart);
}

void MainStackedWidget::updateTotalCryptoStatistic(const QMap<QString, double> &totalCryptoStatMap,
                                                   const QStandardItemModel* cryptoModel) {
    qDebug() << totalCryptoStatMap;
    this->totalCryptoStatistic = totalCryptoStatMap;

    double portfolioCost{totalCryptoStatMap.value("portfolioCost")};
    this->ui->totalBalanceLabel->setText("Стоимость портфеля: " + QString::number(portfolioCost) + " $");

    // QVector<QPair<QString, double>> coinAndPriceVector{};
    QPieSeries *pieCryptoSeries = new QPieSeries();
    auto modelRowsCount{cryptoModel->rowCount()};
    for (size_t row{}; row < modelRowsCount; row++) {
        QString coinName{cryptoModel->data(QModelIndex(
                                               cryptoModel->index(row, Columns::Coin))).toString()};

        qreal coinPrice{cryptoModel->data(QModelIndex(
                                              cryptoModel->index(row, Columns::TotalCost))).toReal()};
        // coin price / total portfolio cost
        qreal coinPercentage{coinPrice / portfolioCost};

        pieCryptoSeries->append(coinName, coinPercentage);

        QPieSlice *slice = pieCryptoSeries->slices().at(row);
        slice->setLabelVisible();
    }

    QChart *chart = new QChart();
    chart->addSeries(pieCryptoSeries);
    // chart->legend()->setVisible(false);

    ui->pieChartView->setChart(chart);
}

MainStackedWidget::~MainStackedWidget() {
    delete ui;
}
