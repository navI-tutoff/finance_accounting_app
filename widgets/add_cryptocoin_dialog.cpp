#include "add_cryptocoin_dialog.h"
#include "ui_add_cryptocoin_dialog.h"

#include <QMessageBox>

AddCryptocoinDialog::AddCryptocoinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCryptocoinDialog) {
    ui->setupUi(this);
}

AddCryptocoinDialog::~AddCryptocoinDialog() {
    delete ui;
}

void AddCryptocoinDialog::on_okButton_clicked() {
    /// TODO сделать обработку
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

void AddCryptocoinDialog::on_cancelButton_clicked() { reject(); }

const QString& AddCryptocoinDialog::getCoinName() const {
    return coinName;
}

const double& AddCryptocoinDialog::getVolume() const {
    return volume;
}

const double& AddCryptocoinDialog::getAvgBuyPrice() const {
    return avgBuyPrice;
}
