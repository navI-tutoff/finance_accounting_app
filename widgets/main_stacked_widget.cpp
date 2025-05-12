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

class PercentageChangeCoinDelegate : public QStyledItemDelegate {
public:
    PercentageChangeCoinDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const override {
        painter->save();

        QRect fullRect = option.rect;
        int minWidth = 50;
        int maxWidth = 70;
        int adjustedWidth = fullRect.width() * 0.5;
        int finalWidth = qBound(minWidth, adjustedWidth, maxWidth); // value will be between -> [minWidth; maxWidth]
        int paddingY = fullRect.height() * 0.1;
        int finalHeight = fullRect.height() - 2 * paddingY;

        // setting up the rect for needed height and width and also centered
        QRect rect(
            fullRect.center().x() - finalWidth / 2,
            fullRect.y() + paddingY,
            finalWidth,
            finalHeight
        );

        // painter->setPen(QPen(QColor("#f4f4f4"), 1));
        // painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        // painter->drawLine(option.rect.x(), option.rect.bottom() + 1, option.rect.right(), option.rect.bottom() + 1);

        double percentageChange = index.data().toDouble();
        QColor color = percentageChange >= 0 ? QColor(39, 208, 127) : QColor(239, 69, 74);
        QString sign = percentageChange >= 0 ? QString{"+"} : QString{""};

        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(rect, 3, 3);

        painter->setPen(Qt::white);
        QFont curFont = painter->font();
        curFont.setBold(true);
        painter->setFont(curFont);
        painter->drawText(rect, Qt::AlignCenter, sign + QString::number(percentageChange, 'f', 2) + "%");

        painter->restore();
    }
};

class CoinNameDelegate : public QStyledItemDelegate {
public:
    CoinNameDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        painter->save();

        // painter->setPen(QPen(QColor("#f4f4f4"), 1));
        // painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        // painter->drawLine(option.rect.x(), option.rect.bottom() + 1, option.rect.right(), option.rect.bottom() + 1);

        painter->setPen(QColor("#f4f4f4"));
        QFont curFont = painter->font();
        curFont.setBold(true);
        curFont.setPixelSize(13);
        painter->setFont(curFont);
        painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, index.data().toString().remove("USDT"));

        painter->restore();
    }
};

class CoinPriceDelegate : public QStyledItemDelegate {
public:
    CoinPriceDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        painter->save();

        // painter->setPen(QPen(QColor("#f4f4f4"), 1));
        // painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        // painter->drawLine(option.rect.x(), option.rect.bottom() + 1, option.rect.right(), option.rect.bottom() + 1);

        painter->setPen(QColor("#f4f4f4"));
        QFont curFont = painter->font();
        curFont.setBold(true);
        painter->setFont(curFont);
        painter->drawText(option.rect, Qt::AlignRight | Qt::AlignVCenter, index.data().toString());

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

// ============================= ↓ Popular Statistic Table Settings ↓ =============================
    ui->popular24hStatTableView->setItemDelegateForColumn(0, new CoinNameDelegate(this));
    ui->popular24hStatTableView->setItemDelegateForColumn(1, new CoinPriceDelegate(this));
    ui->popular24hStatTableView->setItemDelegateForColumn(2, new PercentageChangeCoinDelegate(this));
    // ui->popular24hStatTableView->setStyleSheet("QTableView { background-color: #1E1E1E; color: white; }");
    // ui->popular24hStatTableView->horizontalHeader()->setVisible(false);

    ui->popular24hStatTableView->setShowGrid(false);
    // ui->popular24hStatTableView->setGridStyle(Qt::SolidLine);
    ui->popular24hStatTableView->setFrameShape(QFrame::NoFrame);
    ui->popular24hStatTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->popular24hStatTableView->verticalHeader()->setVisible(false);
    ui->popular24hStatTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->popular24hStatTableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->popular24hStatTableView->resizeRowsToContents();
    ui->popular24hStatTableView->resizeColumnsToContents();
    this->popular24hStatModel->setHeaderData(0, Qt::Horizontal, QVariant(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);  // align 'coin' column to the left
    this->popular24hStatModel->setHeaderData(1, Qt::Horizontal, QVariant(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole); // align 'price' column to the right
    // remove borders from horizontal header
    ui->popular24hStatTableView->horizontalHeader()->setStyleSheet(R"(
        QHeaderView::section {
            border: none;
        }
    )");
    // transparent table settings
    ui->popular24hStatTableView->setStyleSheet(R"(
        QTableView {
            background: transparent;
            border: none;
        }
        QTableView::item {
            background: transparent;
        }
        QHeaderView {
            background: transparent;
        }
        QHeaderView::section {
            background: transparent;
        }
        QTableCornerButton::section {
            background: transparent;
        }
    )");
// ============================= ↑ Popular Statistic Table Settings ↑ =============================
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
