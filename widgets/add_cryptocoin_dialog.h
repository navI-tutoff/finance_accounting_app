#ifndef ADD_CRYPTOCOIN_DIALOG_H
#define ADD_CRYPTOCOIN_DIALOG_H

#include <QDialog>

namespace Ui {
class AddCryptocoinDialog;
}

class AddCryptocoinDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddCryptocoinDialog(QWidget *parent = nullptr);
    ~AddCryptocoinDialog();

    const QString& getCoinName() const;
    const double& getVolume() const;
    const double& getAvgBuyPrice() const;

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AddCryptocoinDialog *ui;

    QString coinName{};
    double volume{};
    double avgBuyPrice{};
};

#endif // ADD_CRYPTOCOIN_DIALOG_H
