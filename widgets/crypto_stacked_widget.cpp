#include "crypto_stacked_widget.h"
#include "ui_crypto_stacked_widget.h"

#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QDialog>
#include <QMessageBox>

#include "add_and_edit_cryptocoin_dialog.h"

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

void CryptoStackedWidget::fetchPriceForCoin(const QString &coin, const size_t &coin_row) {
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

    connect(networkReply, &QNetworkReply::finished, this, [this, coin_row, networkReply, dataBuffer](){
        if (networkReply->error()) {
            qDebug() << "[ERROR] " << networkReply->errorString();
        } else {
            // turn the data into a json document
            QJsonDocument doc = QJsonDocument::fromJson(*dataBuffer);
            QJsonObject objectDoc = doc.toVariant().toJsonObject();
            QVariantMap map = objectDoc.toVariantMap();
            QString currentPrice = map["price"].toString();
            model->item(coin_row, Columns::CurrentPrice)->setText(currentPrice); // set column value for current price

            // calculate our own total cost at the moment
            double totalCost = model->item(coin_row, Columns::Amount)->text().toDouble() *
                               model->item(coin_row, Columns::CurrentPrice)->text().toDouble();
            model->item(coin_row, Columns::TotalCost)->setText(QString::number(totalCost));

            // calculate profit
            double profit = totalCost - model->item(coin_row, Columns::Volume)->text().toDouble();
            model->item(coin_row, Columns::Profit)->setText(QString::number(profit));
            double volume = model->item(coin_row, Columns::Volume)->text().toDouble();
            double profitPercent = profit / volume * 100.0;
            model->item(coin_row, Columns::ProfitPercent)->setText(QString::number(profitPercent));
        }
    });
}

void CryptoStackedWidget::fetchPriceForAllCoins() {
    for (size_t row{}; row < model->rowCount(); row++) {
        fetchPriceForCoin(model->item(row, Columns::Coin)->text().toUpper(), row);
    }
}


CryptoStackedWidget::~CryptoStackedWidget() {
    delete ui;
}

void CryptoStackedWidget::on_addCoinButton_clicked() {
    AddAndEditCryptocoinDialog *addCryptocoinDialog = new AddAndEditCryptocoinDialog(this);
    addCryptocoinDialog->setTextLabel("Добавление новой монеты");

    addCryptocoinDialog->show();

    connect(addCryptocoinDialog, &AddAndEditCryptocoinDialog::accepted, this, [=](){
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

    connect(addCryptocoinDialog, &AddAndEditCryptocoinDialog::rejected, this, [=](){
        addCryptocoinDialog->close();
    });
}

void CryptoStackedWidget::on_editCoinButton_clicked() {
    AddAndEditCryptocoinDialog *editCryptocoinDialog = new AddAndEditCryptocoinDialog(this);
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
    editCryptocoinDialog->setCoinNameLineEditText(editCryptocoinDialog->getCoinName());
    editCryptocoinDialog->setVolumeLineEditText(QString::number(editCryptocoinDialog->getVolume()));
    editCryptocoinDialog->setAvgBuyPriceLineEditText(QString::number(editCryptocoinDialog->getAvgBuyPrice()));

    editCryptocoinDialog->show();

    connect(editCryptocoinDialog, &AddAndEditCryptocoinDialog::accepted, this, [=](){
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
            return;
        }

        loadDataFromDB();
        fetchPriceForAllCoins();
    });

    connect(editCryptocoinDialog, &AddAndEditCryptocoinDialog::rejected, this, [=](){
        editCryptocoinDialog->close();
    });
}

void CryptoStackedWidget::on_deleteCoinButton_clicked() {}
