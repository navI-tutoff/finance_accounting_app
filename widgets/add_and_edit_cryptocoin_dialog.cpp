#include "add_and_edit_cryptocoin_dialog.h"
#include "ui_add_and_edit_cryptocoin_dialog.h"

#include <QMessageBox>

AddAndEditCryptocoinDialog::AddAndEditCryptocoinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCryptocoinDialog) {
    ui->setupUi(this);
}

AddAndEditCryptocoinDialog::~AddAndEditCryptocoinDialog() {
    delete ui;
}

void AddAndEditCryptocoinDialog::on_okButton_clicked() {
    bool ok = false;
    this->coinName = ui->coinNameLineEdit->text();
    this->volume = ui->volumeLineEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка", "Введите корректное число. Например: 104.2");
        return;
    }
    this->avgBuyPrice = ui->avgBuyPriceLineEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка", "Введите корректное число. Например: 104.2");
        return;
    }

    accept();
}

void AddAndEditCryptocoinDialog::on_cancelButton_clicked() { reject(); }

const QString& AddAndEditCryptocoinDialog::getCoinName() const {
    return coinName;
}

const double& AddAndEditCryptocoinDialog::getVolume() const {
    return volume;
}

const double& AddAndEditCryptocoinDialog::getAvgBuyPrice() const {
    return avgBuyPrice;
}

void AddAndEditCryptocoinDialog::setCoinName(const QString& coin) {
    this->coinName = coin;
}

void AddAndEditCryptocoinDialog::setVolume(const double& v) {
    this->volume = v;
}

void AddAndEditCryptocoinDialog::setAvgBuyPrice(const double& avgBuyPr) {
    this->avgBuyPrice = avgBuyPr;
}

void AddAndEditCryptocoinDialog::setTextLabel(const QString &text) {
    this->ui->textLabel->setText(text);
}

void AddAndEditCryptocoinDialog::setCoinNameLineEditText(const QString &coinNameLineEditText) {
    this->ui->coinNameLineEdit->setText(coinNameLineEditText);
}

void AddAndEditCryptocoinDialog::setVolumeLineEditText(const QString &volumeLineEditText) {
    this->ui->volumeLineEdit->setText(volumeLineEditText);
}

void AddAndEditCryptocoinDialog::setAvgBuyPriceLineEditText(const QString &avgBuyPriceLineEditText) {
    this->ui->avgBuyPriceLineEdit->setText(avgBuyPriceLineEditText);
}
