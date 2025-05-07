#ifndef ADD_AND_EDIT_CRYPTOCOIN_DIALOG_H
#define ADD_AND_EDIT_CRYPTOCOIN_DIALOG_H

#include <QDialog>

namespace Ui {
class AddCryptocoinDialog;
}

class AddAndEditCryptocoinDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddAndEditCryptocoinDialog(QWidget *parent = nullptr);
    ~AddAndEditCryptocoinDialog();

    const QString& getCoinName() const;
    const double& getVolume() const;
    const double& getAvgBuyPrice() const;

    void setCoinName(const QString& coin);
    void setVolume(const double& v);
    void setAvgBuyPrice(const double& avgBuyPr);

    void setTextLabel(const QString &text);

    void setCoinNameLineEditText(const QString &coinNameLineEditText);
    void setVolumeLineEditText(const QString &volumeLineEditText);
    void setAvgBuyPriceLineEditText(const QString &avgBuyPriceLineEditText);



private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AddCryptocoinDialog *ui;

    QString coinName{};
    double volume{};
    double avgBuyPrice{};
};

#endif // ADD_AND_EDIT_CRYPTOCOIN_DIALOG_H
