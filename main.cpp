#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <iostream>

// Простой заглушки для тестирования Qt
class Warehouse {
public:
    Warehouse() {
        std::cout << "Склад создан" << std::endl;
    }
    
    void printAllProducts() {
        std::cout << "Список товаров:" << std::endl;
        std::cout << "1. Ноутбук - 65000 руб." << std::endl;
        std::cout << "2. Мышь - 1500 руб." << std::endl;
        std::cout << "3. Клавиатура - 4500 руб." << std::endl;
    }
    
    void createTestDocument() {
        std::cout << "Тестовый документ создан!" << std::endl;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Создаем главное окно
    QMainWindow window;
    window.setWindowTitle("Warehouse System");
    window.resize(600, 400);
    
    // Создаем центральный виджет
    QWidget *centralWidget = new QWidget(&window);
    window.setCentralWidget(centralWidget);
    
    // Создаем layout
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    // Заголовок
    QLabel *label = new QLabel("Система управления складом", centralWidget);
    label->setAlignment(Qt::AlignCenter);
    QFont font = label->font();
    font.setPointSize(16);
    font.setBold(true);
    label->setFont(font);
    layout->addWidget(label);
    
    // Информация
    QLabel *infoLabel = new QLabel("Для работы с системой используйте кнопки ниже:", centralWidget);
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);
    
    // Создаем склад
    Warehouse warehouse;
    
    // Кнопки управления
    QPushButton *btnProducts = new QPushButton("📦 Показать товары (консоль)", centralWidget);
    QPushButton *btnDoc = new QPushButton("📄 Создать тестовый документ", centralWidget);
    
    layout->addWidget(btnProducts);
    layout->addWidget(btnDoc);
    
    // Таблица для товаров (пустая для демонстрации)
    QTableWidget *table = new QTableWidget(centralWidget);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(QStringList() << "ID" << "Название" << "Цена");
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setRowCount(3);
    
    // Заполняем таблицу тестовыми данными
    table->setItem(0, 0, new QTableWidgetItem("1"));
    table->setItem(0, 1, new QTableWidgetItem("Ноутбук"));
    table->setItem(0, 2, new QTableWidgetItem("65000 руб."));
    
    table->setItem(1, 0, new QTableWidgetItem("2"));
    table->setItem(1, 1, new QTableWidgetItem("Мышь"));
    table->setItem(1, 2, new QTableWidgetItem("1500 руб."));
    
    table->setItem(2, 0, new QTableWidgetItem("3"));
    table->setItem(2, 1, new QTableWidgetItem("Клавиатура"));
    table->setItem(2, 2, new QTableWidgetItem("4500 руб."));
    
    layout->addWidget(table);
    
    // Подключаем кнопки
    QObject::connect(btnProducts, &QPushButton::clicked, [&warehouse]() {
        warehouse.printAllProducts();
        QMessageBox::information(nullptr, "Товары", 
            "Список товаров выведен в консоль.\nПосмотрите терминал.");
    });
    
    QObject::connect(btnDoc, &QPushButton::clicked, [&warehouse]() {
        warehouse.createTestDocument();
        QMessageBox::information(nullptr, "Документ", 
            "Тестовый документ создан!\nИнформация в консоли.");
    });
    
    // Информация о запуске
    std::cout << "=== ЗАПУСК СИСТЕМЫ УПРАВЛЕНИЯ СКЛАДОМ ===" << std::endl;
    std::cout << "Используйте кнопки в приложении для управления" << std::endl;
    
    // Показать окно
    window.show();
    
    return app.exec();
}