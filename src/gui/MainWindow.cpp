/**
 * @file MainWindow.cpp
 * @brief Реализация главного окна приложения
 */

#include "MainWindow.hpp"
#include "../core/DomainService.hpp"
#include "../utils/Database.hpp"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>

namespace TariffSystem {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , centralTabWidget_(new QTabWidget(this))
{
    setWindowTitle(tr("Система управления тарифами"));
    resize(1200, 800);
    
    setCentralWidget(centralTabWidget_);
    
    createMenus();
    createToolBars();
    createStatusBar();
    
    statusBar()->showMessage(tr("Готов к работе. Подключитесь к БД для начала работы"), 5000);
}

MainWindow::~MainWindow() = default;

void MainWindow::createMenus() {
    // Меню "Файл"
    QMenu* fileMenu = menuBar()->addMenu(tr("&Файл"));
    
    QAction* connectAction = fileMenu->addAction(tr("&Подключиться к БД..."));
    connect(connectAction, &QAction::triggered, this, &MainWindow::connectToDatabase);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction(tr("&Выход"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // Меню "Справочники"
    QMenu* catalogsMenu = menuBar()->addMenu(tr("&Справочники"));
    
    QAction* servicesAction = catalogsMenu->addAction(tr("&Услуги"));
    connect(servicesAction, &QAction::triggered, this, &MainWindow::openServiceCatalog);
    
    QAction* tariffsAction = catalogsMenu->addAction(tr("&Тарифы"));
    connect(tariffsAction, &QAction::triggered, this, &MainWindow::openTariffCatalog);
    
    // Меню "Заказы"
    QMenu* ordersMenu = menuBar()->addMenu(tr("&Заказы"));
    
    QAction* newOrderAction = ordersMenu->addAction(tr("&Новый заказ"));
    newOrderAction->setShortcut(QKeySequence::New);
    connect(newOrderAction, &QAction::triggered, this, &MainWindow::createOrder);
    
    QAction* searchAction = ordersMenu->addAction(tr("&Поиск оптимального тарифа"));
    connect(searchAction, &QAction::triggered, this, &MainWindow::openOptimalSearch);
    
    // Меню "Помощь"
    QMenu* helpMenu = menuBar()->addMenu(tr("&Помощь"));
    
    QAction* aboutAction = helpMenu->addAction(tr("&О программе"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::createToolBars() {
    QToolBar* mainToolBar = addToolBar(tr("Основная панель"));
    mainToolBar->setMovable(false);
    
    QAction* connectAction = mainToolBar->addAction(tr("Подключиться"));
    connect(connectAction, &QAction::triggered, this, &MainWindow::connectToDatabase);
    
    mainToolBar->addSeparator();
    
    QAction* servicesAction = mainToolBar->addAction(tr("Услуги"));
    connect(servicesAction, &QAction::triggered, this, &MainWindow::openServiceCatalog);
    
    QAction* tariffsAction = mainToolBar->addAction(tr("Тарифы"));
    connect(tariffsAction, &QAction::triggered, this, &MainWindow::openTariffCatalog);
    
    mainToolBar->addSeparator();
    
    QAction* newOrderAction = mainToolBar->addAction(tr("Новый заказ"));
    connect(newOrderAction, &QAction::triggered, this, &MainWindow::createOrder);
    
    QAction* searchAction = mainToolBar->addAction(tr("Поиск"));
    connect(searchAction, &QAction::triggered, this, &MainWindow::openOptimalSearch);
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Готов"));
}

void MainWindow::openServiceCatalog() {
    QMessageBox::information(this, tr("Справочник услуг"),
        tr("Справочник услуг будет реализован в следующей версии.\n\n"
           "Функциональность:\n"
           "• Просмотр иерархии классов услуг\n"
           "• Редактирование параметров услуг\n"
           "• Добавление новых услуг"));
}

void MainWindow::openTariffCatalog() {
    QMessageBox::information(this, tr("Справочник тарифов"),
        tr("Справочник тарифов будет реализован в следующей версии.\n\n"
           "Функциональность:\n"
           "• Просмотр списка тарифов\n"
           "• Редактирование правил тарифа\n"
           "• Визуализация дерева правил\n"
           "• Конструктор тарифов"));
}

void MainWindow::createOrder() {
    QMessageBox::information(this, tr("Новый заказ"),
        tr("Создание заказа будет реализовано в следующей версии.\n\n"
           "Функциональность:\n"
           "• Выбор услуги\n"
           "• Ввод параметров заказа\n"
           "• Расчет стоимости по тарифу\n"
           "• Сохранение заказа в БД"));
}

void MainWindow::openOptimalSearch() {
    QMessageBox::information(this, tr("Поиск оптимального тарифа"),
        tr("Поиск оптимального тарифа будет реализован в следующей версии.\n\n"
           "Функциональность:\n"
           "• Задание параметров поиска\n"
           "• Сравнение всех доступных тарифов\n"
           "• Ранжирование по стоимости\n"
           "• Анализ экономии"));
}

void MainWindow::showAbout() {
    QMessageBox::about(this, tr("О программе"),
        tr("<h2>Система управления тарифами</h2>"
           "<p>Версия 1.0.0</p>"
           "<p>Курсовая работа по проектированию информационной системы "
           "для работы с тарифами на услуги.</p>"
           "<p><b>Функциональность:</b></p>"
           "<ul>"
           "<li>Ведение справочника услуг</li>"
           "<li>Ведение справочника тарифов</li>"
           "<li>Формирование заказов на услуги</li>"
           "<li>Расчет стоимости заказа по тарифу</li>"
           "<li>Поиск оптимального исполнителя</li>"
           "</ul>"
           "<p><b>Технологии:</b> C++20, Qt 6, PostgreSQL</p>"));
}

void MainWindow::connectToDatabase() {
    bool ok;
    QString host = QInputDialog::getText(this, tr("Подключение к БД"),
        tr("Адрес сервера:"), QLineEdit::Normal, "localhost", &ok);
    
    if (!ok || host.isEmpty()) {
        return;
    }
    
    QString dbName = QInputDialog::getText(this, tr("Подключение к БД"),
        tr("Имя базы данных:"), QLineEdit::Normal, "tariff_system", &ok);
    
    if (!ok || dbName.isEmpty()) {
        return;
    }
    
    QString user = QInputDialog::getText(this, tr("Подключение к БД"),
        tr("Пользователь:"), QLineEdit::Normal, "postgres", &ok);
    
    if (!ok || user.isEmpty()) {
        return;
    }
    
    QString password = QInputDialog::getText(this, tr("Подключение к БД"),
        tr("Пароль:"), QLineEdit::Password, "", &ok);
    
    if (!ok) {
        return;
    }
    
    try {
        auto db = std::make_shared<TariffSystem::Database::DatabaseManager>();
        TariffSystem::Database::DatabaseManager::ConnectionParams params;
        params.host = host.toStdString();
        params.database = dbName.toStdString();
        params.user = user.toStdString();
        params.password = password.toStdString();
        //params.port = 5432;
        
        db->connect(params);
        
        // Создание DomainService для управления всеми операциями
        domainService_ = std::make_shared<TariffSystem::Core::DomainService>(db);
        
        statusBar()->showMessage(tr("Подключено к БД: %1@%2 | DomainService инициализирован").arg(dbName, host), 5000);
        
        QMessageBox::information(this, tr("Подключение"),
            tr("Успешно подключено к базе данных!\n\n"
               "DomainService готов к работе:\n"
               "• Оркестрация бизнес-логики\n"
               "• Персистентность данных\n"
               "• Транзакционность операций"));
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Ошибка подключения"),
            tr("Не удалось подключиться к базе данных:\n%1")
                .arg(QString::fromStdString(e.what())));
    }
}

} // namespace TariffSystem
