/**
 * @file MainWindow.hpp
 * @brief Главное окно приложения системы тарифов
 * 
 * Главное окно приложения с меню, панелью инструментов и статус-баром.
 */

#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <memory>

namespace TariffSystem {

class TariffSystemRepository;

/**
 * @brief Главное окно приложения
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна
     * @param parent Родительский виджет
     */
    explicit MainWindow(QWidget* parent = nullptr);
    
    /**
     * @brief Деструктор
     */
    ~MainWindow() override;

private slots:
    /**
     * @brief Открыть справочник услуг
     */
    void openServiceCatalog();
    
    /**
     * @brief Открыть справочник тарифов
     */
    void openTariffCatalog();
    
    /**
     * @brief Создать новый заказ
     */
    void createOrder();
    
    /**
     * @brief Открыть поиск оптимального тарифа
     */
    void openOptimalSearch();
    
    /**
     * @brief Показать окно "О программе"
     */
    void showAbout();
    
    /**
     * @brief Подключиться к базе данных
     */
    void connectToDatabase();

private:
    /**
     * @brief Создать меню приложения
     */
    void createMenus();
    
    /**
     * @brief Создать панель инструментов
     */
    void createToolBars();
    
    /**
     * @brief Создать статус-бар
     */
    void createStatusBar();

    QTabWidget* centralTabWidget_;
    std::shared_ptr<TariffSystemRepository> repository_;
};

} // namespace TariffSystem
