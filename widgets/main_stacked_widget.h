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

    void updateTotalCryptoStatistics(const QMap<QString, double> &totalCryptoStatMap,
                                     const QStandardItemModel* cryptoModel);
    // getting info about popular currencies and 24h change statistics
    void updatePopular24hStatistics();

    // calculating the leader of growth from the portfolio, put it into Bar Chart
    void updateGrowthLeader();

private:
    Ui::MainStackedWidget *ui;

    QStandardItemModel *popular24hStatModel;

    QMap<QString, double> totalCryptoStatistic{}; // 1. "portfolioCost"; 2. "profit"; 3. "startCost"
    // const QStandardItemModel* cryptoModel{};
};

#endif // MAIN_STACKED_WIDGET_H
