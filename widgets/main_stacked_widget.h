#ifndef MAIN_STACKED_WIDGET_H
#define MAIN_STACKED_WIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QBarSeries>
#include <QValueAxis>

#include <QStandardItemModel>
#include <QNetworkAccessManager>

#include "../styles/style_defines.h"
#include "../data_structures/user_session.h"

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
    void updateGrowthLeader(const QStandardItemModel* cryptoModel);

private:
    Ui::MainStackedWidget *ui;

    QNetworkAccessManager *networkManager;

    QStandardItemModel *popular24hStatModel;

    QChart *pieChart;

    QChart *growthLeaderChart;
    QValueAxis *growthChartAxisY;

    // QMap<QString, double> totalCryptoStatistic{}; // 1. "portfolioCost"; 2. "profit"; 3. "startCost"
    // const QStandardItemModel* cryptoModel{};
};

#endif // MAIN_STACKED_WIDGET_H
