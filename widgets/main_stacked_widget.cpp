#include "main_stacked_widget.h"
#include "ui_main_stacked_widget.h"

MainStackedWidget::MainStackedWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainStackedWidget) {
    ui->setupUi(this);

}

MainStackedWidget::~MainStackedWidget() {
    delete ui;
}
