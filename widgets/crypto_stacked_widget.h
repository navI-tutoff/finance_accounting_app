#ifndef CRYPTO_STACKED_WIDGET_H
#define CRYPTO_STACKED_WIDGET_H

#include <QWidget>

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
};

#endif // CRYPTO_STACKED_WIDGET_H
