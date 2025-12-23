/**
 * @file main.cpp
 * @brief Точка входа в приложение системы тарифов
 */

#include "gui/MainWindow.hpp"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Настройка приложения
    app.setApplicationName("TariffSystem");
    app.setApplicationDisplayName("Система управления тарифами");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ETU");
    
    // Установка локали для русского языка
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));
    
    // Создание и показ главного окна
    TariffSystem::MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
