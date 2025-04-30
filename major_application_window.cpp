#include "major_application_window.h"
#include "ui_major_application_window.h"

MajorApplicationWindow::MajorApplicationWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MajorApplicationWindow) {
    ui->setupUi(this);

    ui->qMain->setBaseSize(1900, 1000);

}

MajorApplicationWindow::~MajorApplicationWindow() {
    delete ui;
}

void MajorApplicationWindow::on_mainWindowPushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MajorApplicationWindow::on_cryptoWindowPushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}
