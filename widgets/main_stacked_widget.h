#ifndef MAIN_STACKED_WIDGET_H
#define MAIN_STACKED_WIDGET_H

#include <QWidget>

#include "../data_structures/user_session.h"

#include <QtCharts/QChartView>


namespace Ui {
class MainStackedWidget;
}

class MainStackedWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainStackedWidget(QWidget *parent = nullptr);
    ~MainStackedWidget();

    void saveTotalCryptoStatistic(QMap<QString, double> map);

private:
    Ui::MainStackedWidget *ui;

    QMap<QString, double> totalCryptoStatistic{};
};

#endif // MAIN_STACKED_WIDGET_H
