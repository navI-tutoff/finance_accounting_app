#ifndef MAIN_STACKED_WIDGET_H
#define MAIN_STACKED_WIDGET_H

#include <QWidget>

#include "../data_structures/user_session.h"

#include <QtCharts/QChartView>

#include <QStandardItemModel>

namespace Ui {
class MainStackedWidget;
}

class MainStackedWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainStackedWidget(QWidget *parent = nullptr);
    ~MainStackedWidget();

    void updateTotalCryptoStatistic(const QMap<QString, double> &totalCryptoStatMap,
                                    const QStandardItemModel* cryptoModel);

private:
    Ui::MainStackedWidget *ui;

    QMap<QString, double> totalCryptoStatistic{}; // 1. "portfolioCost"; 2. "profit"; 3. "startCost"
    const QStandardItemModel* cryptoModel{};
};

#endif // MAIN_STACKED_WIDGET_H
