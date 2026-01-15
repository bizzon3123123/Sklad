#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Устанавливаем стиль Fusion для лучшего вида
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    MainWindow window;
    window.setWindowTitle("Складская система - Рабочее место");
    window.resize(1200, 800);
    window.show();
    
    return app.exec();
}