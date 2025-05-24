#include "interaction_with_cryptocoin_dialog.h"
#include "ui_interaction_with_cryptocoin_dialog.h"

#include <QMessageBox>

InteractionCryptocoinDialog::InteractionCryptocoinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCryptocoinDialog) {
    ui->setupUi(this);

    this->setWindowIcon(QIcon{":/assets/crypto_icon.png"});
}

InteractionCryptocoinDialog::~InteractionCryptocoinDialog() {
    delete ui;
}

void InteractionCryptocoinDialog::on_okButton_clicked() {
    bool ok = false;
    this->coinName = ui->coinNameLineEdit->text();
    this->volume = ui->volumeLineEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка", "Введите корректное число. Например: 104.2");
        reject();
        return;
    }
    this->avgBuyPrice = ui->avgBuyPriceLineEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка", "Введите корректное число. Например: 104.2");
        reject();
        return;
    }

    accept();
}

void InteractionCryptocoinDialog::on_cancelButton_clicked() { reject(); }

const QString& InteractionCryptocoinDialog::getCoinName() const {
    return coinName;
}

const double& InteractionCryptocoinDialog::getVolume() const {
    return volume;
}

const double& InteractionCryptocoinDialog::getAvgBuyPrice() const {
    return avgBuyPrice;
}

void InteractionCryptocoinDialog::setCoinName(const QString& coin) {
    this->coinName = coin;
}

void InteractionCryptocoinDialog::setVolume(const double& v) {
    this->volume = v;
}

void InteractionCryptocoinDialog::setAvgBuyPrice(const double& avgBuyPr) {
    this->avgBuyPrice = avgBuyPr;
}

void InteractionCryptocoinDialog::setTextLabel(const QString &text) {
    // this->ui->textLabel->setStyleSheet(R"(
    //     color: rgb(255, 246, 246); /* warm white */
    //     font-size: 14px;
    //     font-weight: bold;
    // )");
    this->ui->textLabel->setText(text);
}

QLineEdit* InteractionCryptocoinDialog::coinNameLineEdit() {
    return this->ui->coinNameLineEdit;
}

QLineEdit* InteractionCryptocoinDialog::volumeLineEdit() {
    return this->ui->volumeLineEdit;
}

QLineEdit* InteractionCryptocoinDialog::avgBuyPriceLineEdit() {
    return this->ui->avgBuyPriceLineEdit;
}

QPushButton* InteractionCryptocoinDialog::okButton() {
    return this->ui->okButton;
}

QPushButton* InteractionCryptocoinDialog::cancelButton() {
    return this->ui->cancelButton;
}
