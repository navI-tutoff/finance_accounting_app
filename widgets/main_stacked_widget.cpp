#include "main_stacked_widget.h"
#include "ui_main_stacked_widget.h"

#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
    , popular24hStatModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

    popular24hStatModel->setHorizontalHeaderLabels({"Монета", "Цена", "Изменение, 24ч"});
    ui->popular24hStatTableView->setModel(popular24hStatModel);

    ui->totalBalanceLabel->setText("Стоимость портфеля: Загрузка...");

    updatePopular24hStatistics();
    updateGrowthLeader();

    // interface design
    ui->popular24hStatTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->popular24hStatTableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->popular24hStatTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainStackedWidget::updateTotalCryptoStatistics(const QMap<QString, double> &totalCryptoStatMap,
                                                    const QStandardItemModel* cryptoModel) {
    this->totalCryptoStatistic = totalCryptoStatMap;

    double portfolioCost{totalCryptoStatMap.value("portfolioCost")};
    this->ui->totalBalanceLabel->setText("Стоимость портфеля: " + QString::number(portfolioCost) + " $");

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
        slice->setLabel(slice->label() + " " + QString::number(coinPercentage * 100, 'a', 1) + "%");
    }

    QChart *chart = new QChart();
    chart->addSeries(pieCryptoSeries);
    // chart->legend()->setVisible(false);

    ui->pieChartView->setChart(chart);
}

void MainStackedWidget::updatePopular24hStatistics() {
    QString API_URL_STR{"https://www.binance.com/api/v3/ticker/24hr?symbols=[\"BTCUSDT\","
                                                                            "\"ETHUSDT\","
                                                                            "\"SOLUSDT\","
                                                                            "\"MEMEUSDT\","
                                                                            "\"ONDOUSDT\"]"};
    const QUrl API_URL{API_URL_STR};
    QNetworkRequest request;
    request.setUrl(API_URL);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QNetworkReply *networkReply = networkManager->get(request);
    QByteArray *dataBuffer = new QByteArray;

    connect(networkReply, &QIODevice::readyRead, this, [this, networkReply, dataBuffer](){
        dataBuffer->append(networkReply->readAll());
    });

    connect(networkReply, &QNetworkReply::finished, this, [this, networkReply, dataBuffer](){
        if (networkReply->error()) {
            qDebug() << "[ERROR] " << networkReply->errorString();
        } else {
            popular24hStatModel->removeRows(0, popular24hStatModel->rowCount());

            // turn the data into a json document
            QJsonDocument doc = QJsonDocument::fromJson(*dataBuffer);
            QJsonArray jsonArray = doc.array();
            auto jsonArraySize = jsonArray.size();

            for (size_t i{}; i < jsonArraySize; i++) {
                QJsonObject objectDoc = jsonArray.at(i).toObject();
                QVariantMap map = objectDoc.toVariantMap();

                QList<QStandardItem*> row;
                row << new QStandardItem(map["symbol"].toString());
                row << new QStandardItem(QString::number(map["lastPrice"].toDouble(), 'a', 2) + " $");
                row << new QStandardItem(map["priceChangePercent"].toString()  + " %");

                popular24hStatModel->appendRow(row);
            }
        }

        delete dataBuffer;
        networkReply->deleteLater();
    });
}

void MainStackedWidget::updateGrowthLeader() {
    QBarSet *set1 = new QBarSet("Coin1");
    QBarSet *set2 = new QBarSet("Coin2");
    QBarSet *set3 = new QBarSet("Coin3");
    QBarSet *set4 = new QBarSet("Coin4");
    /// TODO вопрос в memory leak. Изучить контроль памяти в плане Charts

    *set1 << 15;
    *set2 << 11;
    *set3 << 10;
    *set4 << 02;

    QBarSeries *series = new QBarSeries();
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);

    // QBarCategoryAxis *axisX = new QBarCategoryAxis();
    // axisX->append("Лидеры роста");

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 20);

    QChart *chart = new QChart();
    chart->setTitle("Лидеры роста");

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->addSeries(series);

    // chart->addAxis(axisX, Qt::AlignBottom);
    // series->attachAxis(axisY);

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->growLeaderChartView->setChart(chart);
}

MainStackedWidget::~MainStackedWidget() {
    delete ui;
}
