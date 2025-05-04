#ifndef CRYPTO_STACKED_WIDGET_H
#define CRYPTO_STACKED_WIDGET_H

#include <QWidget>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlQuery>

#include "../data_structures/user_session.h"

namespace Ui {
class CryptoStackedWidget;
}

class CryptoStackedWidget : public QWidget {
    Q_OBJECT

public:
    explicit CryptoStackedWidget(QWidget *parent = nullptr);
    ~CryptoStackedWidget();

private:
    Ui::CryptoStackedWidget *ui;

    QTableView *tableView;
    QSqlQueryModel *model;
};

#endif // CRYPTO_STACKED_WIDGET_H
