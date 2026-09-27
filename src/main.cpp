#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    QCoreApplication::setApplicationName("VectorForge");
    QCoreApplication::setOrganizationName("VectorForgeStudio");
    QCoreApplication::setApplicationVersion("1.0.0");

    VectorForge::MainWindow window;
    window.show();

    return app.exec();
}
