#include "crypto_stacked_widget.h"
#include "ui_crypto_stacked_widget.h"

#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QDialog>
#include <QMessageBox>

#include "interaction_with_cryptocoin_dialog.h"

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

CryptoStackedWidget::CryptoStackedWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CryptoStackedWidget)
    , model(new QStandardItemModel(this))
    , networkManager(new QNetworkAccessManager(this))
    , updatePriceTimer(new QTimer(this))
{
    ui->setupUi(this);

    model->setHorizontalHeaderLabels({"Монета", "Объём", "Средняя цена покупки", "Цена", "Количество", "Стоимость", "Прибыль", "Прибыль %"});
    ui->tableView->setModel(model);

    loadDataFromDB();

    updatePriceTimer->setInterval(60000);
    connect(updatePriceTimer, &QTimer::timeout, this, &CryptoStackedWidget::fetchPriceForAllCoins);
    fetchPriceForAllCoins();
    updatePriceTimer->start();

    // interface design
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    QString API_URL_STR{"https://www.binance.com/api/v3/ticker/price?symbol="};
    API_URL_STR += coin + "USDT";
    const QUrl API_URL(API_URL_STR);
    QNetworkRequest request;
    request.setUrl(API_URL);

    QNetworkReply *networkReply = networkManager->get(request);
    QByteArray *dataBuffer = new QByteArray;

    connect(networkReply, &QIODevice::readyRead, this, [this, networkReply, dataBuffer](){
        dataBuffer->append(networkReply->readAll());
    });

    connect(networkReply, &QNetworkReply::finished, this, [this, coinRow, networkReply, dataBuffer, numberOfCoins](){
        if (networkReply->error()) {
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
        fetchPriceForCoin(model->item(row, Columns::Coin)->text().toUpper(), row, numberOfCoins);
    }
}

// ================ ↓ interaction with cryptocoin (add, edit, delete) ↓ ================

void CryptoStackedWidget::on_addCoinButton_clicked() {
    InteractionCryptocoinDialog *addCryptocoinDialog = new InteractionCryptocoinDialog(this);
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
    });

    connect(addCryptocoinDialog, &InteractionCryptocoinDialog::rejected, this, [=](){
        addCryptocoinDialog->close();
    });
}

void CryptoStackedWidget::on_editCoinButton_clicked() {
    InteractionCryptocoinDialog *editCryptocoinDialog = new InteractionCryptocoinDialog(this);
    editCryptocoinDialog->setTextLabel("Редактирование статистики");

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
    });

    connect(editCryptocoinDialog, &InteractionCryptocoinDialog::rejected, this, [=](){
        editCryptocoinDialog->close();
    });
}

void CryptoStackedWidget::on_deleteCoinButton_clicked() {
    InteractionCryptocoinDialog *deleteCryptocoinDialog = new InteractionCryptocoinDialog(this);
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
    });

    connect(deleteCryptocoinDialog, &InteractionCryptocoinDialog::rejected, this, [=](){
        deleteCryptocoinDialog->close();
    });
}

// ================ ↑ interaction with cryptocoin (buttons add, edit, delete) ↑ ================



CryptoStackedWidget::~CryptoStackedWidget() {
    delete ui;
}
