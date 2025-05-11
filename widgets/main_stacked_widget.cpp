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
    , networkManager(new QNetworkAccessManager(this))
    , popular24hStatModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

    popular24hStatModel->setHorizontalHeaderLabels({"Монета", "Цена", "Изменение, 24ч"});
    ui->popular24hStatTableView->setModel(popular24hStatModel);

    ui->totalBalanceLabel->setText("Стоимость портфеля: Загрузка...");

    updatePopular24hStatistics();

    // interface design
    ui->popular24hStatTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->popular24hStatTableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->popular24hStatTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainStackedWidget::updateTotalCryptoStatistics(const QMap<QString, double> &totalCryptoStatMap,
                                                    const QStandardItemModel* cryptoModel) {
    // this->totalCryptoStatistic = totalCryptoStatMap;

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

void MainStackedWidget::updateGrowthLeader(const QStandardItemModel* cryptoModel) {
    QString API_URL_STR{"https://www.binance.com/api/v3/ticker/24hr?symbols=["};

    auto cryptoModelSize = cryptoModel->rowCount();
    for (size_t row{}; row < cryptoModelSize; row++) {
        auto coin = cryptoModel->data(QModelIndex(cryptoModel->index(row, Columns::Coin))).toString();
        API_URL_STR += "\"" + coin + "USDT\"";

        if (row != cryptoModelSize - 1) { API_URL_STR += ","; }
    }
    API_URL_STR += "]";

    const QUrl API_URL{API_URL_STR};
    QNetworkRequest request(API_URL);

    QNetworkReply *networkReply = networkManager->get(request);
    QByteArray *dataBuffer = new QByteArray;

    connect(networkReply, &QIODevice::readyRead, this, [this, networkReply, dataBuffer](){
        dataBuffer->append(networkReply->readAll());
    });

    connect(networkReply, &QNetworkReply::finished, this, [this, networkReply, dataBuffer](){
        if (networkReply->error()) {
            qDebug() << "[ERROR] " << networkReply->errorString();
        } else {
            // turn the data into a json document
            QJsonDocument doc = QJsonDocument::fromJson(*dataBuffer);
            QJsonArray jsonArray = doc.array();
            auto jsonArraySize = jsonArray.size();

            QBarSeries *series = new QBarSeries();
            for (size_t i{}; i < jsonArraySize; i++) {
                QJsonObject objectDoc = jsonArray.at(i).toObject();
                QVariantMap map = objectDoc.toVariantMap();

                QBarSet *set = new QBarSet(map["symbol"].toString());
                *set << map["priceChangePercent"].toDouble();

                series->append(set);
            }

            QValueAxis *axisY = new QValueAxis();
            axisY->setRange(-5, 5);

            QChart *chart = new QChart();
            chart->setTitle("Лидеры роста");

            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignBottom);
            chart->addSeries(series);

            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);

            ui->growLeaderChartView->setChart(chart);
        }

        delete dataBuffer;
        networkReply->deleteLater();
    });
}

MainStackedWidget::~MainStackedWidget() {
    delete ui;
}
