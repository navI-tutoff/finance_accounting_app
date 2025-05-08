#ifndef INTERACTION_WITH_CRYPTOCOIN_DIALOG_H
#define INTERACTION_WITH_CRYPTOCOIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class AddCryptocoinDialog;
}

// dialog class for add / edit / delete cryptocoin on crypto_stacked_widget tableView
class InteractionCryptocoinDialog : public QDialog {
    Q_OBJECT

public:
    explicit InteractionCryptocoinDialog(QWidget *parent = nullptr);
    ~InteractionCryptocoinDialog();

    const QString& getCoinName() const;
    const double& getVolume() const;
    const double& getAvgBuyPrice() const;

    void setCoinName(const QString& coin);
    void setVolume(const double& v);
    void setAvgBuyPrice(const double& avgBuyPr);

    void setTextLabel(const QString &text);

    QLineEdit* coinNameLineEdit();
    QLineEdit* volumeLineEdit();
    QLineEdit* avgBuyPriceLineEdit();

    QPushButton* okButton();
    QPushButton* cancelButton();

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AddCryptocoinDialog *ui;

    QString coinName{};
    double volume{};
    double avgBuyPrice{};
};

#endif // INTERACTION_WITH_CRYPTOCOIN_DIALOG_H
