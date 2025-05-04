#include "crypto_stacked_widget.h"
#include "widgets/ui_crypto_stacked_widget.h"

CryptoStackedWidget::CryptoStackedWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CryptoStackedWidget) {
    ui->setupUi(this);
}

CryptoStackedWidget::~CryptoStackedWidget() {
    delete ui;
}
