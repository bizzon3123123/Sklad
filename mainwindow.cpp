#include "mainwindow.h"
#include "ui_mainwindow.h"  // Измените эту строку
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Система управления складом");
    
    // Подключаем кнопки
    connect(ui->btnShowProducts, &QPushButton::clicked, this, &MainWindow::onShowProducts);
    connect(ui->btnCreateDocument, &QPushButton::clicked, this, &MainWindow::onCreateDocument);
    
    // Тестовый вывод
    std::cout << "Приложение запущено!" << std::endl;
    warehouse.printAllProducts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onShowProducts()
{
    QMessageBox::information(this, "Товары", 
        QString("Всего товаров: %1").arg(warehouse.getAllProducts().size()));
    
    warehouse.printAllProducts();
}

void MainWindow::onCreateDocument()
{
    warehouse.createTestDocument();
    warehouse.printAllDocuments();
    QMessageBox::information(this, "Документ", "Тестовый документ создан!");
}