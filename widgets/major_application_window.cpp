#include "major_application_window.h"
#include "ui_major_application_window.h"

#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>

MajorApplicationWindow::MajorApplicationWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MajorApplicationWindow) {
    ui->setupUi(this);

    this->resize(1580, 820);

//     ui->cryptoWindowPushButton->setStyleSheet(R"(
//     QPushButton {
//         background-color: #2980b9;
//         color: white;
//         padding: 10px;
//         border-radius: 5px;
//     }
//     QPushButton:hover {
//         background-color: #3498db;
//     }
// )");

    ui->menuGroupBox->setStyleSheet(R"(
        QPushButton {
            background-color: #797979;
            color: white;
            border: none;
            padding: 12px;
            text-align: left;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #a0a0a0;
        }
        QPushButton:checked {
            background-color: #1abc9c;
        }
    )");

    mainStackedWidget = new MainStackedWidget(this);
    cryptoStackedWidget = new CryptoStackedWidget(this);

    ui->basicStackedWidget->addWidget(mainStackedWidget);
    ui->basicStackedWidget->addWidget(cryptoStackedWidget);

    connect(cryptoStackedWidget, &CryptoStackedWidget::allPricesFetched, this, [=](){
        mainStackedWidget->updateTotalCryptoStatistics(
            cryptoStackedWidget->getTotalCryptoStatMap(),
            cryptoStackedWidget->getModel()
        );

        mainStackedWidget->updateGrowthLeader(cryptoStackedWidget->getModel());
        mainStackedWidget->updatePopular24hStatistics();
    });

    cryptoStackedWidget->loadDataFromDB();
    if (cryptoStackedWidget->getModel()->rowCount() > 0) {
        cryptoStackedWidget->fetchPriceForAllCoins();
    } else {
        qDebug() << ">>> No coins loaded, skipping fetchPriceForAllCoins()";
    }

    connect(ui->mainWindowPushButton, &QPushButton::clicked, this, [=]() {
        ui->basicStackedWidget->setCurrentWidget(mainStackedWidget);
    });

    connect(ui->cryptoWindowPushButton, &QPushButton::clicked, this, [=]() {
        ui->basicStackedWidget->setCurrentWidget(cryptoStackedWidget);

        // cryptoStackedWidget->loadDataFromDB();
        // cryptoStackedWidget->fetchPriceForAllCoins();
        // mainStackedWidget->saveTotalCryptoStatistic(cryptoStackedWidget->calculateTotalStatistic());
    });
}

MajorApplicationWindow::~MajorApplicationWindow() {
    delete ui;
}
