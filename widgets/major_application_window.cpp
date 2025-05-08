#include "major_application_window.h"
#include "ui_major_application_window.h"

#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>

MajorApplicationWindow::MajorApplicationWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MajorApplicationWindow) {
    ui->setupUi(this);

    // ui->qMain->setBaseSize(1900, 1000);
    // ui->qMain->setStyleSheet("background-color: yellow");

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

//     ui->menuGroupBox->setStyleSheet(R"(
//         QPushButton {
//             background-color: #2c3e50;
//             color: white;
//             border: none;
//             padding: 12px;
//             text-align: left;
//             font-size: 14px;
//         }
//         QPushButton:hover {
//             background-color: #34495e;
//         }
//         QPushButton:checked {
//             background-color: #1abc9c;
//         }
//     )");

    mainStackedWidget = new MainStackedWidget(this);
    cryptoStackedWidget = new CryptoStackedWidget(this);

    ui->basicStackedWidget->addWidget(mainStackedWidget);
    ui->basicStackedWidget->addWidget(cryptoStackedWidget);

    connect(ui->mainWindowPushButton, &QPushButton::clicked, this, [=]() {
        ui->basicStackedWidget->setCurrentWidget(mainStackedWidget);
    });

    connect(ui->cryptoWindowPushButton, &QPushButton::clicked, this, [=]() {
        ui->basicStackedWidget->setCurrentWidget(cryptoStackedWidget);
        cryptoStackedWidget->loadDataFromDB();
        cryptoStackedWidget->fetchPriceForAllCoins();
        cryptoStackedWidget->calculateTotalStatistic();
    });

    // QSqlQuery query(Database::instance().database());
    // query.prepare("INSERT INTO crypto (id, user_id, coin, volume, avg_buy_price, current_price, amount) VALUES (1, 15, :coin, 2000, 4000, 4600, 1.14);");
    // query.bindValue(":coin", "ETH");
    // query.exec();
}

MajorApplicationWindow::~MajorApplicationWindow() {
    delete ui;
}
