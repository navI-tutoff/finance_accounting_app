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

#include <QStyledItemDelegate>

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

class CoinDelegate : public QStyledItemDelegate {
public:
    CoinDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {
        // Предзагрузка иконок (предположим, что они лежат в ресурсах)
        // icons["BTCUSDT"] = QPixmap(":/icons/btc.png");
        // icons["ETHUSDT"] = QPixmap(":/icons/eth.png");
        // icons["SOLUSDT"] = QPixmap(":/icons/sol.png");
        // icons["MEMEUSDT"] = QPixmap(":/icons/meme.png");
        // icons["ONDOUSDT"] = QPixmap(":/icons/ondo.png");
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        if (index.column() != 0) return;

        painter->save();
        QRect rect = option.rect;

        QString symbol = index.data(Qt::DisplayRole).toString();
        QString price = index.siblingAtColumn(1).data().toString();
        QString percent = index.siblingAtColumn(2).data().toString();

        // QPixmap icon = icons.value(symbol, QPixmap());

        QColor percentColor = percent.contains('-') ? QColor("#FF4C4C") : QColor("#00D26A");

        if (option.state & QStyle::State_MouseOver)
            painter->fillRect(rect, QColor("#2E2E2E"));

        // QRect iconRect(rect.left() + 6, rect.top() + 4, 32, 32);
        // if (!icon.isNull())
            // painter->drawPixmap(iconRect, icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        painter->setPen(Qt::white);
        painter->setFont(QFont("Segoe UI", 10, QFont::Bold));
        painter->drawText(rect.left() + 44, rect.top() + 18, symbol);

        painter->setFont(QFont("Segoe UI", 9));
        painter->drawText(rect.right() - 140, rect.top() + 18, price);

        painter->setPen(percentColor);
        painter->drawText(rect.right() - 60, rect.top() + 18, percent);

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override {
        return QSize(300, 40);
    }

private:
    QMap<QString, QPixmap> icons;
};

class CoinTableDelegate : public QStyledItemDelegate {
public:
    CoinTableDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const override {
        painter->save();

        /// TODO тут остановился. Играюсь с размерами фона у процентов
        if (index.column() == 2) { // percentage change of coin
            QRect rect = option.rect.adjusted(option.rect.width() * 0.25,
                                              option.rect.height() * 0.1,
                                              -option.rect.width() * 0.25,
                                              -option.rect.height() * 0.1);

            double percentageChange = index.data().toDouble();
            QColor color = percentageChange >= 0 ? QColor("#29dd87") : QColor("#ef454a");
            QString sign = percentageChange >= 0 ? QString{"+"} : QString{""};

            painter->setRenderHint(QPainter::Antialiasing);
            painter->setBrush(color);
            painter->setPen(Qt::NoPen);
            painter->drawRoundedRect(rect, 3, 3);

            painter->setPen(Qt::white);
            painter->drawText(rect, Qt::AlignCenter, sign + QString::number(percentageChange, 'f', 2) + "%");
        }

        painter->restore();
    }
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
    ui->popular24hStatTableView->setItemDelegateForColumn(2, new CoinTableDelegate(this));
    // ui->popular24hStatTableView->setStyleSheet("QTableView { background-color: #1E1E1E; color: white; }");
    // ui->popular24hStatTableView->horizontalHeader()->setVisible(false);

    ui->popular24hStatTableView->setShowGrid(false);
    ui->popular24hStatTableView->setFrameShape(QFrame::NoFrame);
    ui->popular24hStatTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->popular24hStatTableView->verticalHeader()->setVisible(false);
    ui->popular24hStatTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->popular24hStatTableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->popular24hStatTableView->resizeRowsToContents();
    ui->popular24hStatTableView->resizeColumnsToContents();
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
                row << new QStandardItem(map["priceChangePercent"].toString());

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
