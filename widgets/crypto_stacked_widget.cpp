#include "crypto_stacked_widget.h"
#include "ui_crypto_stacked_widget.h"

#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QDialog>
#include <QMessageBox>

#include <QStyledItemDelegate>
#include <QPainter>

#include "interaction_with_cryptocoin_dialog.h"

#include "../styles/style_defines.cpp"

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

namespace DelegateTools {
const QColor stateSelectedRowColor{58, 58, 58};

void setupMonoFont(QPainter *painter) {
    QFont monoFont("DejaVu Sans Mono");
    // monoFont.setPixelSize(13);
    painter->setFont(monoFont);
}

void setupStateSelectedStyle(QPainter *painter, const QStyleOptionViewItem &option) {
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, stateSelectedRowColor);
    }
}

class CoinDelegate : public QStyledItemDelegate {
public:
    CoinDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        painter->save();

        setupStateSelectedStyle(painter, option);

        setupMonoFont(painter);

        QFont curFont = painter->font();
        curFont.setBold(true);
        curFont.setPixelSize(13);
        painter->setPen(QColor(51, 210, 110));
        painter->setFont(curFont);
        painter->drawText(option.rect, Qt::AlignCenter, index.data().toString());

        painter->restore();
    }
};

class VolumeDelegate : public QStyledItemDelegate {
public:
    VolumeDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        auto value = index.data().toDouble();

        painter->save();

        setupStateSelectedStyle(painter, option);

        setupMonoFont(painter);

        QString formattedString = QString("%1 $").arg(value, 6, 'f', 2); // 2 - precision

        painter->drawText(option.rect, Qt::AlignCenter, formattedString);
        painter->restore();
    }
};

class AvgBuyPriceDelegate : public QStyledItemDelegate {
public:
    AvgBuyPriceDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        auto value = index.data().toDouble();

        painter->save();

        setupStateSelectedStyle(painter, option);

        setupMonoFont(painter);

        QString formattedString;
        if (value < 0.01) { // if value too small, add precision
            formattedString = QString("%1 $").arg(value, 10, 'f', 6);
        } else {
            formattedString = QString("%1 $").arg(value, 6, 'f', 2);
        }

        painter->drawText(option.rect, Qt::AlignCenter, formattedString);
        painter->restore();
    }
};

class AmountCurrentDelegate : public QStyledItemDelegate {
public:
    AmountCurrentDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        auto value = index.data().toDouble();

        painter->save();

        setupStateSelectedStyle(painter, option);

        setupMonoFont(painter);

        QString formattedString;
        if (value < 0.01) { // if value too small, add precision
            formattedString = QString("%1").arg(value, 10, 'f', 6);
        } else {
            formattedString = QString("%1").arg(value, 6, 'f', 2);
        }

        painter->drawText(option.rect, Qt::AlignCenter, formattedString);
        painter->restore();
    }
};

class CurrentCostDelegate : public QStyledItemDelegate {
public:
    CurrentCostDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        auto value = index.data().toDouble();

        painter->save();

        setupStateSelectedStyle(painter, option);

        setupMonoFont(painter);

        QString formattedString{QString("%1 $").arg(value, 6, 'f', 2)};

        painter->drawText(option.rect, Qt::AlignCenter, formattedString);
        painter->restore();
    }
};

class profitDelegate : public QStyledItemDelegate {
public:
    profitDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        auto value = index.data().toDouble();

        painter->save();

        QColor color = value >= 0 ? QColor(46, 89, 46) : QColor(89, 46, 46);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->fillRect(option.rect, color);
        painter->restore(); // after rect

        painter->save();

        setupStateSelectedStyle(painter, option);
        setupMonoFont(painter);

        QString formattedString{QString("%1 $").arg(value, 6, 'f', 2)};

        painter->drawText(option.rect, Qt::AlignCenter, formattedString);
        painter->restore();
    }
};

class profitPercentageDelegate : public QStyledItemDelegate {
public:
    profitPercentageDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        auto value = index.data().toDouble();

        painter->save();

        QColor color = value >= 0 ? QColor(46, 89, 46) : QColor(89, 46, 46);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->fillRect(option.rect, color);
        painter->restore(); // after rect

        painter->save();

        setupStateSelectedStyle(painter, option);
        setupMonoFont(painter);

        // QString sign = value >= 0 ? QString{"+"} : QString{""};
        QString formattedString{QString("%1 %").arg(value, 6, 'f', 2)};

        painter->drawText(option.rect, Qt::AlignCenter, formattedString);
        painter->restore();
    }
};
}
using namespace DelegateTools;

CryptoStackedWidget::CryptoStackedWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CryptoStackedWidget)
    , model(new QStandardItemModel(this))
    , networkManager(new QNetworkAccessManager(this))
    , updatePriceTimer(new QTimer(this))
{
    ui->setupUi(this);

    model->setHorizontalHeaderLabels({"Монета", "Объём", "Ср. цена покупки", "Цена", "Количество", "Стоимость", "Прибыль", "Прибыль, %"});
    ui->tableView->setModel(model);

    // updatePriceTimer->setInterval(60000);
    updatePriceTimer->setInterval(5000);
    connect(updatePriceTimer, &QTimer::timeout, this, &CryptoStackedWidget::fetchPriceForAllCoins);
    updatePriceTimer->start();

    // *************** interface design ***************

// ============================= ↓ Crypto Table Settings ↓ =============================
    ui->tableView->setItemDelegateForColumn(0, new CoinDelegate(this));
    ui->tableView->setItemDelegateForColumn(1, new VolumeDelegate(this));
    ui->tableView->setItemDelegateForColumn(2, new AvgBuyPriceDelegate(this));
    ui->tableView->setItemDelegateForColumn(3, new AvgBuyPriceDelegate(this)); // same delegate
    ui->tableView->setItemDelegateForColumn(4, new AmountCurrentDelegate(this));
    ui->tableView->setItemDelegateForColumn(5, new CurrentCostDelegate(this));
    ui->tableView->setItemDelegateForColumn(6, new profitDelegate(this));
    ui->tableView->setItemDelegateForColumn(7, new profitPercentageDelegate(this));

    // ui->tableView->setShowGrid(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setFrameShape(QFrame::Box);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    // ui->tableView->setSortingEnabled(true); // not working properly
    ui->tableView->setStyleSheet(R"(
        QHeaderView::section {
            font-size: 15px;
            font-weight: bold;
        }
    )");

// ============================= ↑ Crypto Table Settings ↑ =============================

    // buttons' icons
    QSize iconSize{24, 24};
    QIcon addIcon{":/assets/add_icon.png"};
    ui->addCoinButton->setText("");
    ui->addCoinButton->setIcon(addIcon);
    ui->addCoinButton->setIconSize(iconSize);
    ui->addCoinButton->setCursor(Qt::PointingHandCursor);
    ui->addCoinButton->setStyleSheet(R"(
        background: transparent;
        border: none;
    )");

    QIcon editIcon{":/assets/edit_icon.png"};
    ui->editCoinButton->setText("");
    ui->editCoinButton->setIcon(editIcon);
    ui->editCoinButton->setIconSize(iconSize);
    ui->editCoinButton->setCursor(Qt::PointingHandCursor);
    ui->editCoinButton->setStyleSheet(R"(
        background: transparent;
        border: none;
    )");

    QIcon deleteIcon{":/assets/delete_icon.png"};
    ui->deleteCoinButton->setText("");
    ui->deleteCoinButton->setIcon(deleteIcon);
    ui->deleteCoinButton->setIconSize(iconSize);
    ui->deleteCoinButton->setCursor(Qt::PointingHandCursor);
    ui->deleteCoinButton->setStyleSheet(R"(
        background: transparent;
        border: none;
    )");

    ui->innerButtonsFrame->setStyleSheet(R"(
        background-color: rgb(58, 58, 58); /* = stateSelectedRowColor */
        border: none;
        border-radius: 12px;
    )");
}

void CryptoStackedWidget::loadDataFromDB() {
    QSqlQuery selectQueryFromCryptoTable;
    selectQueryFromCryptoTable.prepare("SELECT coin, volume, avg_buy_price FROM crypto WHERE user_id = :id;");
    selectQueryFromCryptoTable.bindValue(":id", UserSession::instance().id());

    if (!selectQueryFromCryptoTable.exec()) {
        qDebug() << "[DATABASE ERROR] " << selectQueryFromCryptoTable.lastError().text();
        return;
    }

    model->removeRows(0, model->rowCount());

    while (selectQueryFromCryptoTable.next()) {
        QList<QStandardItem*> row;
        row << new QStandardItem(selectQueryFromCryptoTable.value("coin").toString());
        row << new QStandardItem(selectQueryFromCryptoTable.value("volume").toString());
        row << new QStandardItem(selectQueryFromCryptoTable.value("avg_buy_price").toString());
        row << new QStandardItem(QString{"Загрузка"}); // current price (need to fetch from API)
        // counting amount of coin
        row << new QStandardItem(QString::number(
            selectQueryFromCryptoTable.value("volume").toDouble() /
            selectQueryFromCryptoTable.value("avg_buy_price").toDouble()));
        row << new QStandardItem(QString{"Загрузка"}); // current cost
        row << new QStandardItem(QString{"Загрузка"}); // profit
        row << new QStandardItem(QString{"Загрузка"}); // percentage profit

        model->appendRow(row);
    }
}

const QStandardItemModel* CryptoStackedWidget::getModel() const {
    return this->model;
};

// calculate all information about portfolio price and profit. Put info in labels
// returns Map {start cost, portfolio cost, profit}
void CryptoStackedWidget::calculateTotalStatistic() {
    double startCost{};
    double portfolioCost{};
    double profit{};

    auto modelRowsCount{ui->tableView->model()->rowCount()};
    for (size_t row{}; row < modelRowsCount; row++) {
        profit += ui->tableView->model()->data(QModelIndex(
                  ui->tableView->model()->index(row, Columns::Profit))).toDouble();

        portfolioCost += ui->tableView->model()->data(QModelIndex(
                         ui->tableView->model()->index(row, Columns::TotalCost))).toDouble();

        startCost += ui->tableView->model()->data(QModelIndex(
                     ui->tableView->model()->index(row, Columns::Volume))).toDouble();
    }

    ui->startPriceLabel->setText("Начальная цена портфеля: " + QString::number(startCost) + " $");
    ui->currentPriceLabel->setText("Текущая цена портфеля: " + QString::number(portfolioCost) + " $");
    ui->profitLabel->setText("Прибыль по портфелю: " + QString::number(profit) + " $");

    this->totalCryptoStatMap = {{"startCost", startCost},
                                {"portfolioCost", portfolioCost},
                                {"profit", profit}};
}

const QMap<QString, double>& CryptoStackedWidget::getTotalCryptoStatMap() const {
    return this->totalCryptoStatMap;
}

void CryptoStackedWidget::fetchPriceForCoin(const QString &coin,
                                            const size_t &coinRow,
                                            const size_t &numberOfCoins) {
    QString API_URL_STR{"https://www.binance.com/api/v3/ticker/price?symbol=" + coin + "USDT"};
    const QUrl API_URL(API_URL_STR);
    QNetworkRequest request(API_URL);

    QNetworkReply *networkReply = networkManager->get(request);
    QByteArray *dataBuffer = new QByteArray;
    connect(networkReply, &QIODevice::readyRead, this, [this, networkReply, dataBuffer](){
        dataBuffer->append(networkReply->readAll());
    });

    connect(networkReply, &QNetworkReply::finished, this, [this, networkReply, coinRow, numberOfCoins, dataBuffer](){
        if (networkReply->error() != QNetworkReply::NoError) {
            qDebug() << "[ERROR] " << networkReply->errorString();
        } else {
            // turn the data into a json document
            QJsonDocument doc = QJsonDocument::fromJson(*dataBuffer);
            QJsonObject objectDoc = doc.toVariant().toJsonObject();
            QVariantMap map = objectDoc.toVariantMap();

            QString currentPrice = map["price"].toString();
            model->item(coinRow, Columns::CurrentPrice)->setText(currentPrice); // set column value for current price

            // calculate our own total cost at the moment
            double totalCost = model->item(coinRow, Columns::Amount)->text().toDouble() *
                               model->item(coinRow, Columns::CurrentPrice)->text().toDouble();
            model->item(coinRow, Columns::TotalCost)->setText(QString::number(totalCost));

            // calculate profit
            double profit = totalCost - model->item(coinRow, Columns::Volume)->text().toDouble();
            model->item(coinRow, Columns::Profit)->setText(QString::number(profit));
            double volume = model->item(coinRow, Columns::Volume)->text().toDouble();
            double profitPercent = profit / volume * 100.0;
            model->item(coinRow, Columns::ProfitPercent)->setText(QString::number(profitPercent));
        }

        this->completedRequests++;
        if (numberOfCoins == completedRequests) { // all coins fetched
            calculateTotalStatistic();

            completedRequests = 0; // reset
            emit CryptoStackedWidget::allPricesFetched();
        }

        delete dataBuffer;
        networkReply->deleteLater();
    });
}

void CryptoStackedWidget::fetchPriceForAllCoins() {    
    size_t numberOfCoins = model->rowCount();
    for (size_t row{}; row < numberOfCoins; row++) {
        QTimer::singleShot(50 * row, [=](){
            fetchPriceForCoin(model->item(row, Columns::Coin)->text().toUpper(), row, numberOfCoins);
        });
    }
}

// ================ ↓ interaction with cryptocoin (add, edit, delete) ↓ ================

void CryptoStackedWidget::on_addCoinButton_clicked() {
    InteractionCryptocoinDialog *addCryptocoinDialog = new InteractionCryptocoinDialog(this);
    addCryptocoinDialog->setWindowTitle("Добавление новой монеты");
    addCryptocoinDialog->setTextLabel("Добавление новой монеты");

    addCryptocoinDialog->show();

    connect(addCryptocoinDialog, &InteractionCryptocoinDialog::accepted, this, [=](){
        QSqlQuery addCoinQuery;
        addCoinQuery.prepare("INSERT INTO crypto (user_id, coin, volume, avg_buy_price) "
                             "VALUES (:user_id, :coin, :volume, :avg_buy_price);");
        addCoinQuery.bindValue(":user_id", UserSession::instance().id());
        addCoinQuery.bindValue(":coin", addCryptocoinDialog->getCoinName());
        addCoinQuery.bindValue(":volume", addCryptocoinDialog->getVolume());
        addCoinQuery.bindValue(":avg_buy_price", addCryptocoinDialog->getAvgBuyPrice());

        if (!addCoinQuery.exec()) {
            QMessageBox::warning(this, "Ошибка", "Что-то пошло не так");
            return;
        }

        loadDataFromDB();
        fetchPriceForAllCoins();

        delete addCryptocoinDialog;
    });

    connect(addCryptocoinDialog, &InteractionCryptocoinDialog::rejected, this, [=](){
        addCryptocoinDialog->close();
        delete addCryptocoinDialog;
    });
}

void CryptoStackedWidget::on_editCoinButton_clicked() {
    InteractionCryptocoinDialog *editCryptocoinDialog = new InteractionCryptocoinDialog(this);
    editCryptocoinDialog->setWindowTitle("Редактирование статистики");
    editCryptocoinDialog->setTextLabel("Редактирование статистики");

    /// TODO кажется, можно переписать функционал без лишнего Query. Брать все данные из model
    QSqlQuery neccessaryCoinSelectQuery;
    neccessaryCoinSelectQuery.prepare("SELECT * FROM crypto WHERE user_id = :id AND coin = :coin;");
    neccessaryCoinSelectQuery.bindValue(":id", UserSession::instance().id());

    QModelIndexList selectedRowsList = ui->tableView->selectionModel()->selectedRows();
    if (selectedRowsList.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не выбрана ни одна строчка");
        return;
    }

    auto selectedRow = selectedRowsList.first().row();
    neccessaryCoinSelectQuery.bindValue(":coin", ui->tableView->model()
                                                 ->data(QModelIndex(
                                                 ui->tableView->model()->index(selectedRow, Columns::Coin)))
                                                     .toString());
    if (!neccessaryCoinSelectQuery.exec()) {
        QMessageBox::warning(this, "Ошибка", "Что-то пошло не так");
        return;
    }

    neccessaryCoinSelectQuery.next();
    editCryptocoinDialog->setCoinName(neccessaryCoinSelectQuery.value("coin").toString());
    editCryptocoinDialog->setVolume(neccessaryCoinSelectQuery.value("volume").toDouble());
    editCryptocoinDialog->setAvgBuyPrice(neccessaryCoinSelectQuery.value("avg_buy_price").toDouble());

    QString oldCoinName = editCryptocoinDialog->getCoinName();
    editCryptocoinDialog->coinNameLineEdit()->setText(editCryptocoinDialog->getCoinName());
    editCryptocoinDialog->volumeLineEdit()->setText(QString::number(editCryptocoinDialog->getVolume()));
    editCryptocoinDialog->avgBuyPriceLineEdit()->setText(QString::number(editCryptocoinDialog->getAvgBuyPrice()));

    editCryptocoinDialog->show();

    connect(editCryptocoinDialog, &InteractionCryptocoinDialog::accepted, this, [=](){
        QSqlQuery updateCoinQuery;
        updateCoinQuery.prepare("UPDATE crypto "
                                "SET coin = :coin, volume = :volume, avg_buy_price = :avg_buy_price "
                                "WHERE user_id = :id AND coin = :old_coin_name;");
        updateCoinQuery.bindValue(":coin", editCryptocoinDialog->getCoinName());
        updateCoinQuery.bindValue(":volume", editCryptocoinDialog->getVolume());
        updateCoinQuery.bindValue(":avg_buy_price", editCryptocoinDialog->getAvgBuyPrice());
        updateCoinQuery.bindValue(":id", UserSession::instance().id());
        updateCoinQuery.bindValue(":old_coin_name", oldCoinName);

        if (!updateCoinQuery.exec()) {
            QMessageBox::warning(this, "Ошибка", "Что-то пошло не так");
            /// TODO ВАЖНО!!! Возможно, тут должно быть не this, а экземпляр AddAndEditCryptocoinDialog (пересмотреть всё остальное!!)
            return;
        }

        loadDataFromDB();
        fetchPriceForAllCoins();

        delete editCryptocoinDialog;
    });

    connect(editCryptocoinDialog, &InteractionCryptocoinDialog::rejected, this, [=](){
        editCryptocoinDialog->close();
        delete editCryptocoinDialog;
    });
}

void CryptoStackedWidget::on_deleteCoinButton_clicked() {
    InteractionCryptocoinDialog *deleteCryptocoinDialog = new InteractionCryptocoinDialog(this);
    deleteCryptocoinDialog->setWindowTitle("Удаление выбранной монеты");
    deleteCryptocoinDialog->setTextLabel("Удаление выбранной монеты");
    deleteCryptocoinDialog->okButton()->setText("Удалить");

    QSqlQuery neccessaryCoinSelectQuery;
    neccessaryCoinSelectQuery.prepare("SELECT * FROM crypto WHERE user_id = :id AND coin = :coin;");
    neccessaryCoinSelectQuery.bindValue(":id", UserSession::instance().id());

    QModelIndexList selectedRowsList = ui->tableView->selectionModel()->selectedRows();
    if (selectedRowsList.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не выбрана ни одна строчка");
        return;
    }

    auto selectedRow = selectedRowsList.first().row();
    neccessaryCoinSelectQuery.bindValue(":coin", ui->tableView->model()
                                                     ->data(QModelIndex(
                                                         ui->tableView->model()->index(selectedRow, Columns::Coin)))
                                                     .toString());
    if (!neccessaryCoinSelectQuery.exec()) {
        QMessageBox::warning(this, "Ошибка", "Что-то пошло не так");
        return;
    }

    neccessaryCoinSelectQuery.next();
    deleteCryptocoinDialog->setCoinName(neccessaryCoinSelectQuery.value("coin").toString());
    deleteCryptocoinDialog->setVolume(neccessaryCoinSelectQuery.value("volume").toDouble());
    deleteCryptocoinDialog->setAvgBuyPrice(neccessaryCoinSelectQuery.value("avg_buy_price").toDouble());

    deleteCryptocoinDialog->coinNameLineEdit()->setText(deleteCryptocoinDialog->getCoinName());
    deleteCryptocoinDialog->volumeLineEdit()->setText(QString::number(deleteCryptocoinDialog->getVolume()));
    deleteCryptocoinDialog->avgBuyPriceLineEdit()->setText(QString::number(deleteCryptocoinDialog->getAvgBuyPrice()));

    deleteCryptocoinDialog->coinNameLineEdit()->setReadOnly(true);
    deleteCryptocoinDialog->volumeLineEdit()->setReadOnly(true);
    deleteCryptocoinDialog->avgBuyPriceLineEdit()->setReadOnly(true);

    deleteCryptocoinDialog->show();

    connect(deleteCryptocoinDialog, &InteractionCryptocoinDialog::accepted, this, [=](){
        QSqlQuery deleteCoinQuery;
        deleteCoinQuery.prepare("DELETE FROM crypto WHERE user_id = :id AND coin = :coin;");
        deleteCoinQuery.bindValue(":id", UserSession::instance().id());
        deleteCoinQuery.bindValue(":coin", deleteCryptocoinDialog->getCoinName());

        if (!deleteCoinQuery.exec()) {
            QMessageBox::warning(this, "Ошибка", "Что-то пошло не так");
            return;
        }

        loadDataFromDB();
        fetchPriceForAllCoins();

        delete deleteCryptocoinDialog;
    });

    connect(deleteCryptocoinDialog, &InteractionCryptocoinDialog::rejected, this, [=](){
        delete deleteCryptocoinDialog;
    });
}

// ================ ↑ interaction with cryptocoin (buttons add, edit, delete) ↑ ================



CryptoStackedWidget::~CryptoStackedWidget() {
    delete ui;
}

// void CryptoStackedWidget::on_pushButton_clicked() {
//     QString API_URL_STR{"http://127.0.0.1:8001"};
//     const QUrl API_URL(API_URL_STR);
//     QNetworkRequest request(API_URL);

//     QNetworkReply *networkReply = networkManager->get(request);
//     QByteArray *dataBuffer = new QByteArray;
//     connect(networkReply, &QIODevice::readyRead, this, [this, networkReply, dataBuffer](){
//         dataBuffer->append(networkReply->readAll());
//     });

//     connect(networkReply, &QNetworkReply::finished, this, [this, networkReply, dataBuffer](){
//         if (networkReply->error() != QNetworkReply::NoError) {
//             qDebug() << "[ERROR] " << networkReply->errorString();
//         } else {
//             // turn the data into a json document
//             QJsonDocument doc = QJsonDocument::fromJson(*dataBuffer);
//             QJsonObject objectDoc = doc.toVariant().toJsonObject();
//             QVariantMap map = objectDoc.toVariantMap();

//             qDebug() << map;

//         }


//         networkReply->deleteLater();
//     });
// }
