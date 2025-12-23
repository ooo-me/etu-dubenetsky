#include "MainWindow.h"
#include "dialogs/ServiceTypeDialog.h"
#include "dialogs/ExecutorDialog.h"
#include "dialogs/TariffDialog.h"
#include "dialogs/OrderDialog.h"
#include "dialogs/ParameterDialog.h"
#include "dialogs/UnitDialog.h"
#include "dialogs/CoefficientDialog.h"
#include "dialogs/OptimalSearchDialog.h"

#include <QMenuBar>
#include <QHeaderView>
#include <QInputDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Система тарифов на услуги");
    resize(1200, 800);
    
    dbManager_ = std::make_shared<db::DatabaseManager>();
    
    setupUi();
    setupMenu();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    tabWidget_ = new QTabWidget(this);
    setCentralWidget(tabWidget_);
    
    createServiceTypesTab();
    createExecutorsTab();
    createTariffsTab();
    createOrdersTab();
    createParametersTab();
    createUnitsTab();
    createCoefficientsTab();
    
    statusBar()->showMessage("Не подключено к базе данных");
}

void MainWindow::setupMenu()
{
    auto fileMenu = menuBar()->addMenu("&Файл");
    
    auto connectAction = fileMenu->addAction("&Подключиться к БД...");
    connect(connectAction, &QAction::triggered, this, &MainWindow::onConnectDatabase);
    
    auto initAction = fileMenu->addAction("&Инициализировать БД");
    connect(initAction, &QAction::triggered, this, &MainWindow::onInitializeDatabase);
    
    fileMenu->addSeparator();
    
    auto exitAction = fileMenu->addAction("&Выход");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    auto toolsMenu = menuBar()->addMenu("&Инструменты");
    
    auto optimalAction = toolsMenu->addAction("Поиск оптимального исполнителя...");
    connect(optimalAction, &QAction::triggered, this, &MainWindow::onFindOptimalExecutor);
}

void MainWindow::createServiceTypesTab()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    
    serviceTypesTable_ = new QTableWidget();
    serviceTypesTable_->setColumnCount(5);
    serviceTypesTable_->setHorizontalHeaderLabels({"ID", "Код", "Наименование", "Класс", "Примечание"});
    serviceTypesTable_->horizontalHeader()->setStretchLastSection(true);
    serviceTypesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    serviceTypesTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(serviceTypesTable_);
    
    auto btnLayout = new QHBoxLayout();
    auto addBtn = new QPushButton("Добавить");
    auto editBtn = new QPushButton("Редактировать");
    auto deleteBtn = new QPushButton("Удалить");
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddServiceType);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditServiceType);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteServiceType);
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    tabWidget_->addTab(widget, "Типы услуг");
}

void MainWindow::createExecutorsTab()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    
    executorsTable_ = new QTableWidget();
    executorsTable_->setColumnCount(7);
    executorsTable_->setHorizontalHeaderLabels({"ID", "Код", "Наименование", "Адрес", "Телефон", "Email", "Активен"});
    executorsTable_->horizontalHeader()->setStretchLastSection(true);
    executorsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    executorsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(executorsTable_);
    
    auto btnLayout = new QHBoxLayout();
    auto addBtn = new QPushButton("Добавить");
    auto editBtn = new QPushButton("Редактировать");
    auto deleteBtn = new QPushButton("Удалить");
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddExecutor);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditExecutor);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteExecutor);
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    tabWidget_->addTab(widget, "Исполнители");
}

void MainWindow::createTariffsTab()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    
    tariffsTable_ = new QTableWidget();
    tariffsTable_->setColumnCount(9);
    tariffsTable_->setHorizontalHeaderLabels({"ID", "Код", "Наименование", "Тип услуги", "Исполнитель", 
                                               "Дата начала", "Дата окончания", "НДС", "Активен"});
    tariffsTable_->horizontalHeader()->setStretchLastSection(true);
    tariffsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tariffsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(tariffsTable_);
    
    auto btnLayout = new QHBoxLayout();
    auto addBtn = new QPushButton("Добавить");
    auto editBtn = new QPushButton("Редактировать");
    auto deleteBtn = new QPushButton("Удалить");
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddTariff);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditTariff);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteTariff);
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    tabWidget_->addTab(widget, "Тарифы");
}

void MainWindow::createOrdersTab()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    
    ordersTable_ = new QTableWidget();
    ordersTable_->setColumnCount(9);
    ordersTable_->setHorizontalHeaderLabels({"ID", "Код", "Тип услуги", "Дата создания", "Дата исполнения", 
                                              "Статус", "Исполнитель", "Тариф", "Стоимость"});
    ordersTable_->horizontalHeader()->setStretchLastSection(true);
    ordersTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(ordersTable_);
    
    auto btnLayout = new QHBoxLayout();
    auto addBtn = new QPushButton("Добавить");
    auto editBtn = new QPushButton("Редактировать");
    auto deleteBtn = new QPushButton("Удалить");
    auto calcBtn = new QPushButton("Рассчитать стоимость");
    auto validateBtn = new QPushButton("Проверить");
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddOrder);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditOrder);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteOrder);
    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::onCalculateOrderCost);
    connect(validateBtn, &QPushButton::clicked, this, &MainWindow::onValidateOrder);
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(calcBtn);
    btnLayout->addWidget(validateBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    tabWidget_->addTab(widget, "Заказы");
}

void MainWindow::createParametersTab()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    
    parametersTable_ = new QTableWidget();
    parametersTable_->setColumnCount(6);
    parametersTable_->setHorizontalHeaderLabels({"ID", "Код", "Наименование", "Тип", "Ед. изм.", "Примечание"});
    parametersTable_->horizontalHeader()->setStretchLastSection(true);
    parametersTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    parametersTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(parametersTable_);
    
    auto btnLayout = new QHBoxLayout();
    auto addBtn = new QPushButton("Добавить");
    auto editBtn = new QPushButton("Редактировать");
    auto deleteBtn = new QPushButton("Удалить");
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddParameter);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditParameter);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteParameter);
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    tabWidget_->addTab(widget, "Параметры");
}

void MainWindow::createUnitsTab()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    
    unitsTable_ = new QTableWidget();
    unitsTable_->setColumnCount(4);
    unitsTable_->setHorizontalHeaderLabels({"ID", "Код", "Наименование", "Примечание"});
    unitsTable_->horizontalHeader()->setStretchLastSection(true);
    unitsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    unitsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(unitsTable_);
    
    auto btnLayout = new QHBoxLayout();
    auto addBtn = new QPushButton("Добавить");
    auto editBtn = new QPushButton("Редактировать");
    auto deleteBtn = new QPushButton("Удалить");
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddUnit);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditUnit);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteUnit);
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    tabWidget_->addTab(widget, "Единицы измерения");
}

void MainWindow::createCoefficientsTab()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    
    coefficientsTable_ = new QTableWidget();
    coefficientsTable_->setColumnCount(6);
    coefficientsTable_->setHorizontalHeaderLabels({"ID", "Код", "Наименование", "Мин.", "Макс.", "По умолч."});
    coefficientsTable_->horizontalHeader()->setStretchLastSection(true);
    coefficientsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    coefficientsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(coefficientsTable_);
    
    auto btnLayout = new QHBoxLayout();
    auto addBtn = new QPushButton("Добавить");
    auto editBtn = new QPushButton("Редактировать");
    auto deleteBtn = new QPushButton("Удалить");
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddCoefficient);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditCoefficient);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteCoefficient);
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    tabWidget_->addTab(widget, "Коэффициенты");
}

bool MainWindow::ensureConnected()
{
    if (!isConnected_)
    {
        QMessageBox::warning(this, "Ошибка", "Необходимо подключиться к базе данных");
        return false;
    }
    return true;
}

void MainWindow::onConnectDatabase()
{
    bool ok;
    QString host = QInputDialog::getText(this, "Подключение к БД", "Хост:", QLineEdit::Normal, "localhost", &ok);
    if (!ok) return;
    
    QString port = QInputDialog::getText(this, "Подключение к БД", "Порт:", QLineEdit::Normal, "5433", &ok);
    if (!ok) return;
    
    QString database = QInputDialog::getText(this, "Подключение к БД", "База данных:", QLineEdit::Normal, "tariff_system", &ok);
    if (!ok) return;
    
    QString user = QInputDialog::getText(this, "Подключение к БД", "Пользователь:", QLineEdit::Normal, "postgres", &ok);
    if (!ok) return;
    
    QString password = QInputDialog::getText(this, "Подключение к БД", "Пароль:", QLineEdit::Password, "postgres", &ok);
    if (!ok) return;
    
    db::DatabaseManager::ConnectionParams params;
    params.host = host.toStdString();
    params.port = port.toStdString();
    params.database = database.toStdString();
    params.user = user.toStdString();
    params.password = password.toStdString();
    
    if (dbManager_->Connect(params))
    {
        dbApi_ = std::make_shared<db::DbApi>(dbManager_);
        service_ = std::make_shared<core::TariffService>(dbApi_);
        isConnected_ = true;
        statusBar()->showMessage("Подключено к " + QString::fromStdString(params.database));
        refreshAllTabs();
    }
    else
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных: " + 
                              QString::fromStdString(dbManager_->GetLastError()));
    }
}

void MainWindow::onInitializeDatabase()
{
    if (!ensureConnected()) return;
    
    auto result = QMessageBox::question(this, "Подтверждение", 
        "Инициализация базы данных создаст все необходимые таблицы и процедуры. Продолжить?");
    
    if (result == QMessageBox::Yes)
    {
        try
        {
            service_->InitializeDatabase();
            QMessageBox::information(this, "Успех", "База данных успешно инициализирована");
            refreshAllTabs();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::refreshAllTabs()
{
    if (!isConnected_) return;
    
    refreshServiceTypes();
    refreshExecutors();
    refreshTariffs();
    refreshOrders();
    refreshParameters();
    refreshUnits();
    refreshCoefficients();
}

// Типы услуг
void MainWindow::onAddServiceType()
{
    if (!ensureConnected()) return;
    // TODO: Реализовать диалог
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onEditServiceType()
{
    if (!ensureConnected()) return;
    // TODO: Реализовать диалог
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onDeleteServiceType()
{
    if (!ensureConnected()) return;
    auto row = serviceTypesTable_->currentRow();
    if (row < 0) return;
    
    int id = serviceTypesTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить тип услуги?") == QMessageBox::Yes)
    {
        try
        {
            service_->DeleteServiceType(id);
            refreshServiceTypes();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::refreshServiceTypes()
{
    try
    {
        auto types = service_->GetAllServiceTypes();
        serviceTypesTable_->setRowCount(static_cast<int>(types.size()));
        
        for (size_t i = 0; i < types.size(); ++i)
        {
            const auto& t = types[i];
            serviceTypesTable_->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
            serviceTypesTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t.code)));
            serviceTypesTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(t.name)));
            serviceTypesTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(t.className)));
            serviceTypesTable_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(t.note)));
        }
    }
    catch (const std::exception& e)
    {
        statusBar()->showMessage(QString::fromStdString(e.what()));
    }
}

// Исполнители
void MainWindow::onAddExecutor()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onEditExecutor()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onDeleteExecutor()
{
    if (!ensureConnected()) return;
    auto row = executorsTable_->currentRow();
    if (row < 0) return;
    
    int id = executorsTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить исполнителя?") == QMessageBox::Yes)
    {
        try
        {
            service_->DeleteExecutor(id);
            refreshExecutors();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::refreshExecutors()
{
    try
    {
        auto executors = service_->GetAllExecutors();
        executorsTable_->setRowCount(static_cast<int>(executors.size()));
        
        for (size_t i = 0; i < executors.size(); ++i)
        {
            const auto& e = executors[i];
            executorsTable_->setItem(i, 0, new QTableWidgetItem(QString::number(e.id)));
            executorsTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.code)));
            executorsTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(e.name)));
            executorsTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(e.address)));
            executorsTable_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(e.phone)));
            executorsTable_->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(e.email)));
            executorsTable_->setItem(i, 6, new QTableWidgetItem(e.isActive ? "Да" : "Нет"));
        }
    }
    catch (const std::exception& e)
    {
        statusBar()->showMessage(QString::fromStdString(e.what()));
    }
}

// Тарифы
void MainWindow::onAddTariff()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onEditTariff()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onDeleteTariff()
{
    if (!ensureConnected()) return;
    auto row = tariffsTable_->currentRow();
    if (row < 0) return;
    
    int id = tariffsTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить тариф?") == QMessageBox::Yes)
    {
        try
        {
            service_->DeleteTariff(id);
            refreshTariffs();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::refreshTariffs()
{
    try
    {
        auto tariffs = service_->GetAllTariffs();
        tariffsTable_->setRowCount(static_cast<int>(tariffs.size()));
        
        for (size_t i = 0; i < tariffs.size(); ++i)
        {
            const auto& t = tariffs[i];
            tariffsTable_->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
            tariffsTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t.code)));
            tariffsTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(t.name)));
            tariffsTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(t.serviceName)));
            tariffsTable_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(t.executorName)));
            tariffsTable_->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(t.dateBegin)));
            tariffsTable_->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(t.dateEnd)));
            tariffsTable_->setItem(i, 7, new QTableWidgetItem(t.isWithVat ? QString("Да (%1%)").arg(t.vatRate) : "Нет"));
            tariffsTable_->setItem(i, 8, new QTableWidgetItem(t.isActive ? "Да" : "Нет"));
        }
    }
    catch (const std::exception& e)
    {
        statusBar()->showMessage(QString::fromStdString(e.what()));
    }
}

// Заказы
void MainWindow::onAddOrder()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onEditOrder()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onDeleteOrder()
{
    if (!ensureConnected()) return;
    auto row = ordersTable_->currentRow();
    if (row < 0) return;
    
    int id = ordersTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить заказ?") == QMessageBox::Yes)
    {
        try
        {
            service_->DeleteOrder(id);
            refreshOrders();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::onCalculateOrderCost()
{
    if (!ensureConnected()) return;
    auto row = ordersTable_->currentRow();
    if (row < 0) return;
    
    int id = ordersTable_->item(row, 0)->text().toInt();
    try
    {
        double cost = service_->CalculateOrderCost(id);
        QMessageBox::information(this, "Результат", QString("Стоимость заказа: %1 руб.").arg(cost, 0, 'f', 2));
        refreshOrders();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
    }
}

void MainWindow::onValidateOrder()
{
    if (!ensureConnected()) return;
    auto row = ordersTable_->currentRow();
    if (row < 0) return;
    
    int id = ordersTable_->item(row, 0)->text().toInt();
    try
    {
        auto result = service_->ValidateOrder(id);
        if (result.isValid)
        {
            QMessageBox::information(this, "Результат", "Заказ валиден");
        }
        else
        {
            QMessageBox::warning(this, "Результат", QString::fromStdString(result.errorMessage));
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
    }
}

void MainWindow::refreshOrders()
{
    try
    {
        auto orders = service_->GetAllOrders();
        ordersTable_->setRowCount(static_cast<int>(orders.size()));
        
        for (size_t i = 0; i < orders.size(); ++i)
        {
            const auto& o = orders[i];
            ordersTable_->setItem(i, 0, new QTableWidgetItem(QString::number(o.id)));
            ordersTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(o.code)));
            ordersTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(o.serviceName)));
            ordersTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(o.orderDate)));
            ordersTable_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(o.executionDate)));
            ordersTable_->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(core::OrderStatusName(o.status))));
            ordersTable_->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(o.executorName)));
            ordersTable_->setItem(i, 7, new QTableWidgetItem(QString::fromStdString(o.tariffName)));
            ordersTable_->setItem(i, 8, new QTableWidgetItem(o.totalCost ? QString::number(*o.totalCost, 'f', 2) : ""));
        }
    }
    catch (const std::exception& e)
    {
        statusBar()->showMessage(QString::fromStdString(e.what()));
    }
}

// Параметры
void MainWindow::onAddParameter()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onEditParameter()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onDeleteParameter()
{
    if (!ensureConnected()) return;
    auto row = parametersTable_->currentRow();
    if (row < 0) return;
    
    int id = parametersTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить параметр?") == QMessageBox::Yes)
    {
        try
        {
            service_->DeleteParameter(id);
            refreshParameters();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::refreshParameters()
{
    try
    {
        auto params = service_->GetAllParameters();
        parametersTable_->setRowCount(static_cast<int>(params.size()));
        
        for (size_t i = 0; i < params.size(); ++i)
        {
            const auto& p = params[i];
            parametersTable_->setItem(i, 0, new QTableWidgetItem(QString::number(p.id)));
            parametersTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p.code)));
            parametersTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(p.name)));
            parametersTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(core::Parameter::TypeName(p.type))));
            parametersTable_->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(p.unitName)));
            parametersTable_->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(p.note)));
        }
    }
    catch (const std::exception& e)
    {
        statusBar()->showMessage(QString::fromStdString(e.what()));
    }
}

// Единицы измерения
void MainWindow::onAddUnit()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onEditUnit()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onDeleteUnit()
{
    if (!ensureConnected()) return;
    auto row = unitsTable_->currentRow();
    if (row < 0) return;
    
    int id = unitsTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить единицу измерения?") == QMessageBox::Yes)
    {
        try
        {
            service_->DeleteUnit(id);
            refreshUnits();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::refreshUnits()
{
    try
    {
        auto units = service_->GetAllUnits();
        unitsTable_->setRowCount(static_cast<int>(units.size()));
        
        for (size_t i = 0; i < units.size(); ++i)
        {
            const auto& u = units[i];
            unitsTable_->setItem(i, 0, new QTableWidgetItem(QString::number(u.id)));
            unitsTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(u.code)));
            unitsTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(u.name)));
            unitsTable_->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(u.note)));
        }
    }
    catch (const std::exception& e)
    {
        statusBar()->showMessage(QString::fromStdString(e.what()));
    }
}

// Коэффициенты
void MainWindow::onAddCoefficient()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onEditCoefficient()
{
    if (!ensureConnected()) return;
    QMessageBox::information(this, "Информация", "Функция в разработке");
}

void MainWindow::onDeleteCoefficient()
{
    if (!ensureConnected()) return;
    auto row = coefficientsTable_->currentRow();
    if (row < 0) return;
    
    int id = coefficientsTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить коэффициент?") == QMessageBox::Yes)
    {
        try
        {
            service_->DeleteCoefficient(id);
            refreshCoefficients();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::refreshCoefficients()
{
    try
    {
        auto coeffs = service_->GetAllCoefficients();
        coefficientsTable_->setRowCount(static_cast<int>(coeffs.size()));
        
        for (size_t i = 0; i < coeffs.size(); ++i)
        {
            const auto& c = coeffs[i];
            coefficientsTable_->setItem(i, 0, new QTableWidgetItem(QString::number(c.id)));
            coefficientsTable_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.code)));
            coefficientsTable_->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.name)));
            coefficientsTable_->setItem(i, 3, new QTableWidgetItem(QString::number(c.valueMin, 'f', 2)));
            coefficientsTable_->setItem(i, 4, new QTableWidgetItem(QString::number(c.valueMax, 'f', 2)));
            coefficientsTable_->setItem(i, 5, new QTableWidgetItem(QString::number(c.valueDefault, 'f', 2)));
        }
    }
    catch (const std::exception& e)
    {
        statusBar()->showMessage(QString::fromStdString(e.what()));
    }
}

// Поиск оптимального исполнителя
void MainWindow::onFindOptimalExecutor()
{
    if (!ensureConnected()) return;
    
    try
    {
        auto types = service_->GetAllServiceTypes();
        if (types.empty())
        {
            QMessageBox::warning(this, "Предупреждение", "Нет типов услуг для поиска");
            return;
        }
        
        QStringList typeNames;
        for (const auto& t : types)
        {
            typeNames << QString::fromStdString(t.name);
        }
        
        bool ok;
        QString selected = QInputDialog::getItem(this, "Поиск оптимального исполнителя", 
            "Выберите тип услуги:", typeNames, 0, false, &ok);
        
        if (!ok) return;
        
        int idx = typeNames.indexOf(selected);
        if (idx < 0) return;
        
        auto results = service_->FindOptimalExecutor(types[idx].id);
        
        if (results.empty())
        {
            QMessageBox::information(this, "Результат", "Исполнители не найдены");
            return;
        }
        
        QString msg = "Оптимальные исполнители:\n\n";
        for (const auto& r : results)
        {
            msg += QString("%1 - %2\nТариф: %3\nОценочная стоимость: %4 руб.\n\n")
                .arg(QString::fromStdString(r.executorName))
                .arg(QString::number(r.executorId))
                .arg(QString::fromStdString(r.tariffName))
                .arg(r.estimatedCost, 0, 'f', 2);
        }
        
        QMessageBox::information(this, "Результаты поиска", msg);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
    }
}
