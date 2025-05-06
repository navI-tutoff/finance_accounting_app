#ifndef CRYPTO_STACKED_WIDGET_H
#define CRYPTO_STACKED_WIDGET_H

#include <QWidget>
#include <QTableView>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

#include "../data_structures/user_session.h"

namespace Ui {
class CryptoStackedWidget;
}

class CryptoStackedWidget : public QWidget {
    Q_OBJECT

public:
    explicit CryptoStackedWidget(QWidget *parent = nullptr);
    ~CryptoStackedWidget();

private slots:
    void on_addCoinButton_clicked();

    void on_editCoinButton_clicked();

    void on_deleteCoinButton_clicked();

private:
    void loadDataFromDB();
    void fetchPriceForCoin(const QString &coin, const size_t &coin_row);
    void fetchPriceForAllCoins();

private:
    Ui::CryptoStackedWidget *ui;

    QStandardItemModel *model;

    QNetworkAccessManager *networkManager;

    QTimer *updatePriceTimer;
};

#endif // CRYPTO_STACKED_WIDGET_H
