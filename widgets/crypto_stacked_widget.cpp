#include "crypto_stacked_widget.h"
#include "ui_crypto_stacked_widget.h"

#include <QSqlError>

CryptoStackedWidget::CryptoStackedWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CryptoStackedWidget) {
    ui->setupUi(this);

    model = new QSqlQueryModel(this);

    QSqlQuery selectCryptoInfoQuery;
    selectCryptoInfoQuery.prepare("SELECT * FROM crypto WHERE user_id = :id");
    selectCryptoInfoQuery.bindValue(":id", UserSession::instance().id());
    if(!selectCryptoInfoQuery.exec()) {
        return;
    }

    model->setQuery(std::move(selectCryptoInfoQuery));
    ui->tableView->setModel(model);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

CryptoStackedWidget::~CryptoStackedWidget() {
    delete ui;
}
