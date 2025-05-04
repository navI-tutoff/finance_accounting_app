#ifndef MAIN_STACKED_WIDGET_H
#define MAIN_STACKED_WIDGET_H

#include <QWidget>

namespace Ui {
class MainStackedWidget;
}

class MainStackedWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainStackedWidget(QWidget *parent = nullptr);
    ~MainStackedWidget();

private:
    Ui::MainStackedWidget *ui;
};

#endif // MAIN_STACKED_WIDGET_H
