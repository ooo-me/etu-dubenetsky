#pragma once

#include <core/TariffService.h>
#include <db/Database.h>
#include <db/DbApi.h>

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMessageBox>
#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectDatabase();
    void onInitializeDatabase();
    
    // Типы услуг
    void onAddServiceType();
    void onEditServiceType();
    void onDeleteServiceType();
    void refreshServiceTypes();
    
    // Исполнители
    void onAddExecutor();
    void onEditExecutor();
    void onDeleteExecutor();
    void refreshExecutors();
    
    // Тарифы
    void onAddTariff();
    void onEditTariff();
    void onDeleteTariff();
    void refreshTariffs();
    
    // Заказы
    void onAddOrder();
    void onEditOrder();
    void onDeleteOrder();
    void onCalculateOrderCost();
    void onValidateOrder();
    void refreshOrders();
    
    // Параметры
    void onAddParameter();
    void onEditParameter();
    void onDeleteParameter();
    void refreshParameters();
    
    // Единицы измерения
    void onAddUnit();
    void onEditUnit();
    void onDeleteUnit();
    void refreshUnits();
    
    // Коэффициенты
    void onAddCoefficient();
    void onEditCoefficient();
    void onDeleteCoefficient();
    void refreshCoefficients();
    
    // Поиск оптимального
    void onFindOptimalExecutor();

private:
    void setupUi();
    void setupMenu();
    void createServiceTypesTab();
    void createExecutorsTab();
    void createTariffsTab();
    void createOrdersTab();
    void createParametersTab();
    void createUnitsTab();
    void createCoefficientsTab();
    
    void refreshAllTabs();
    
    bool ensureConnected();
    
    QTabWidget* tabWidget_;
    
    // Таблицы
    QTableWidget* serviceTypesTable_;
    QTableWidget* executorsTable_;
    QTableWidget* tariffsTable_;
    QTableWidget* ordersTable_;
    QTableWidget* parametersTable_;
    QTableWidget* unitsTable_;
    QTableWidget* coefficientsTable_;
    
    // Сервисы
    std::shared_ptr<db::DatabaseManager> dbManager_;
    std::shared_ptr<db::DbApi> dbApi_;
    std::shared_ptr<core::TariffService> service_;
    
    bool isConnected_ = false;
};

