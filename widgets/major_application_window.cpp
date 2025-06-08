#include "major_application_window.h"
#include "ui_major_application_window.h"

#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>

MajorApplicationWindow::MajorApplicationWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MajorApplicationWindow) {
    ui->setupUi(this);

    this->resize(1660, 820);
    this->setWindowTitle("Приложение для учёта финансов");
    this->setWindowIcon(QIcon{":/assets/crypto_icon.png"});

    QIcon menuIcon{":/assets/menu_icon.png"};
    ui->menuPushButton->setText("");
    ui->menuPushButton->setIcon(menuIcon);
    ui->menuPushButton->setIconSize(QSize(36, 36));
    ui->menuPushButton->setCursor(Qt::PointingHandCursor);
    ui->menuPushButton->setStyleSheet(R"(
        background: transparent;
        border: none;
    )");

    QIcon majorHouseIcon{":/assets/major_house_icon.png"};
    ui->mainWindowPushButton->setText("");
    ui->mainWindowPushButton->setIcon(majorHouseIcon);
    ui->mainWindowPushButton->setIconSize(QSize(44, 44));
    ui->mainWindowPushButton->setCursor(Qt::PointingHandCursor);
    ui->mainWindowPushButton->setStyleSheet(R"(
        background: transparent;
        border: none;
    )");

    QIcon cryptoIcon{":/assets/crypto_icon.png"};
    ui->cryptoWindowPushButton->setText("");
    ui->cryptoWindowPushButton->setIcon(cryptoIcon);
    ui->cryptoWindowPushButton->setIconSize(QSize(47, 47));
    ui->cryptoWindowPushButton->setCursor(Qt::PointingHandCursor);
    ui->cryptoWindowPushButton->setStyleSheet(R"(
        background: transparent;
        border: none;
    )");

    ui->menuFrame->setStyleSheet(R"(
        background-color: #1f1f1f;
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
