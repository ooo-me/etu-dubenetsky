#include "MainWindow.h"

#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Система тарифов на услуги");
    app.setOrganizationName("ЛЭТИ");
    app.setApplicationVersion("1.0.0");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

