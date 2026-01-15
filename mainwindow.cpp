#include "mainwindow.h"
#include "warehouse.h"
#include "document.h"
#include "document_type.h"
#include <QDateTime>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QFont>
#include <QMenuBar>  // Добавьте эту строку
#include <iostream>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupMenu();
    
    setWindowTitle("Складская система управления - Рабочее место складского работника");
    resize(1200, 800);
    
    // Инициализация
    refreshStockTable();
    updateStatusBar();
    
    // Тестовые данные
    warehouse.printStockReport();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI() {
    // Создаем центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Главный layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Заголовок
    QLabel *titleLabel = new QLabel("СИСТЕМА УПРАВЛЕНИЯ СКЛАДОМ", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // Табы
    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);
    
    // Создаем вкладки
    setupStockTab();
    setupDocumentsTab();
    setupCreateDocTab();
    setupReportsTab();
    
    // Статус бар
    statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);
    updateStatusBar();
}

void MainWindow::setupMenu() {
    // Получаем menuBar() один раз
    QMenuBar *menuBar = this->menuBar();
    
    // Меню Файл
    fileMenu = new QMenu("Файл", this);
    menuBar->addMenu(fileMenu);
    
    saveAction = new QAction("Сохранить данные", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveData);
    fileMenu->addAction(saveAction);
    
    loadAction = new QAction("Загрузить данные", this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadData);
    fileMenu->addAction(loadAction);
    
    fileMenu->addSeparator();
    
    exitAction = new QAction("Выход", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);
    
    // Меню Справка
    helpMenu = new QMenu("Справка", this);
    menuBar->addMenu(helpMenu);
    
    aboutAction = new QAction("О программе", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupStockTab() {
    QWidget *stockTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(stockTab);
    
    // Панель инструментов
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Поиск товара...");
    toolbarLayout->addWidget(searchEdit);
    
    searchButton = new QPushButton("Найти", this);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchProduct);
    toolbarLayout->addWidget(searchButton);
    
    refreshStockButton = new QPushButton("Обновить", this);
    connect(refreshStockButton, &QPushButton::clicked, this, &MainWindow::refreshStockTable);
    toolbarLayout->addWidget(refreshStockButton);
    
    addProductButton = new QPushButton("Добавить товар", this);
    connect(addProductButton, &QPushButton::clicked, this, &MainWindow::addNewProduct);
    toolbarLayout->addWidget(addProductButton);
    
    editProductButton = new QPushButton("Изменить", this);
    connect(editProductButton, &QPushButton::clicked, this, &MainWindow::editProduct);
    toolbarLayout->addWidget(editProductButton);
    
    deleteProductButton = new QPushButton("Удалить", this);
    connect(deleteProductButton, &QPushButton::clicked, this, &MainWindow::deleteProduct);
    toolbarLayout->addWidget(deleteProductButton);
    
    updateQuantityButton = new QPushButton("Изм. кол-во", this);
    connect(updateQuantityButton, &QPushButton::clicked, this, &MainWindow::updateStockQuantity);
    toolbarLayout->addWidget(updateQuantityButton);
    
    lowStockButton = new QPushButton("Низкий остаток", this);
    connect(lowStockButton, &QPushButton::clicked, this, &MainWindow::showLowStockReport);
    toolbarLayout->addWidget(lowStockButton);
    
    exportStockButton = new QPushButton("Экспорт", this);
    connect(exportStockButton, &QPushButton::clicked, this, &MainWindow::exportStockReport);
    toolbarLayout->addWidget(exportStockButton);
    
    layout->addLayout(toolbarLayout);
    
    // Таблица товаров
    stockTable = new QTableWidget(this);
    stockTable->setColumnCount(6);
    QStringList headers = {"ID", "Наименование", "Количество", "Ед. изм.", "Цена", "Стоимость"};
    stockTable->setHorizontalHeaderLabels(headers);
    stockTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    stockTable->setSelectionMode(QAbstractItemView::SingleSelection);
    stockTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stockTable->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(stockTable);
    
    // Сводная информация
    QLabel *summaryLabel = new QLabel("Сводная информация:", this);
    layout->addWidget(summaryLabel);
    
    tabWidget->addTab(stockTab, "📦 Склад");
}

void MainWindow::setupDocumentsTab() {
    QWidget *docsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(docsTab);
    
    // Панель инструментов
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    docTypeFilter = new QComboBox(this);
    docTypeFilter->addItem("Все документы");
    docTypeFilter->addItem("Чеки");
    docTypeFilter->addItem("Накладные прихода");
    docTypeFilter->addItem("Накладные расхода");
    docTypeFilter->addItem("Акты инвентаризации");
    toolbarLayout->addWidget(docTypeFilter);
    
    refreshDocsButton = new QPushButton("Обновить", this);
    connect(refreshDocsButton, &QPushButton::clicked, this, &MainWindow::refreshDocumentsTable);
    toolbarLayout->addWidget(refreshDocsButton);
    
    createDocButton = new QPushButton("Создать", this);
    connect(createDocButton, &QPushButton::clicked, this, &MainWindow::createDocument);
    toolbarLayout->addWidget(createDocButton);
    
    viewDocButton = new QPushButton("Просмотр", this);
    connect(viewDocButton, &QPushButton::clicked, this, &MainWindow::viewDocument);
    toolbarLayout->addWidget(viewDocButton);
    
    processDocButton = new QPushButton("Провести", this);
    connect(processDocButton, &QPushButton::clicked, this, &MainWindow::processDocument);
    toolbarLayout->addWidget(processDocButton);
    
    cancelDocButton = new QPushButton("Отменить", this);
    connect(cancelDocButton, &QPushButton::clicked, this, &MainWindow::cancelDocument);
    toolbarLayout->addWidget(cancelDocButton);
    
    printDocButton = new QPushButton("Печать", this);
    connect(printDocButton, &QPushButton::clicked, this, &MainWindow::printDocument);
    toolbarLayout->addWidget(printDocButton);
    
    exportDocsButton = new QPushButton("Экспорт", this);
    connect(exportDocsButton, &QPushButton::clicked, this, &MainWindow::exportDocumentsReport);
    toolbarLayout->addWidget(exportDocsButton);
    
    layout->addLayout(toolbarLayout);
    
    // Таблица документов
    documentsTable = new QTableWidget(this);
    documentsTable->setColumnCount(6);
    QStringList headers = {"ID", "Номер", "Тип", "Дата", "Статус", "Создал"};
    documentsTable->setHorizontalHeaderLabels(headers);
    documentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    documentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    documentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    documentsTable->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(documentsTable);
    
    tabWidget->addTab(docsTab, "📄 Документы");
}

void MainWindow::setupCreateDocTab() {
    QWidget *createTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(createTab);
    
    // Форма документа
    QGroupBox *docFormGroup = new QGroupBox("Данные документа", this);
    QFormLayout *formLayout = new QFormLayout(docFormGroup);
    
    docTypeCombo = new QComboBox(this);
    docTypeCombo->addItem("Чек");
    docTypeCombo->addItem("Накладная прихода");
    docTypeCombo->addItem("Накладная расхода");
    docTypeCombo->addItem("Акт инвентаризации");
    connect(docTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDocumentTypeChanged);
    formLayout->addRow("Тип документа:", docTypeCombo);
    
    docNumberEdit = new QLineEdit(this);
    docNumberEdit->setText("ЧК-" + QString::number(QDateTime::currentSecsSinceEpoch()));
    formLayout->addRow("Номер:", docNumberEdit);
    
    docCreatedByEdit = new QLineEdit(this);
    docCreatedByEdit->setText("Складской работник");
    formLayout->addRow("Создал:", docCreatedByEdit);
    
    docDepartmentEdit = new QLineEdit(this);
    docDepartmentEdit->setText("Основной склад");
    formLayout->addRow("Подразделение:", docDepartmentEdit);
    
    docDateEdit = new QDateEdit(this);
    docDateEdit->setDate(QDate::currentDate());
    docDateEdit->setCalendarPopup(true);
    formLayout->addRow("Дата:", docDateEdit);
    
    docCommentEdit = new QTextEdit(this);
    docCommentEdit->setMaximumHeight(60);
    formLayout->addRow("Комментарий:", docCommentEdit);
    
    mainLayout->addWidget(docFormGroup);
    
    // Специфичные поля
    specificFieldsTable = new QTableWidget(this);
    specificFieldsTable->setColumnCount(2);
    specificFieldsTable->setHorizontalHeaderLabels(QStringList() << "Поле" << "Значение");
    specificFieldsTable->horizontalHeader()->setStretchLastSection(true);
    specificFieldsTable->setMaximumHeight(150);
    mainLayout->addWidget(new QLabel("Специфичные поля:", this));
    mainLayout->addWidget(specificFieldsTable);
    
    // Разделитель
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
    
    // Добавление товаров
    QHBoxLayout *itemsLayout = new QHBoxLayout();
    
    // Левая панель - доступные товары
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("Доступные товары:", this));
    
    availableProductsTable = new QTableWidget(this);
    availableProductsTable->setColumnCount(4);
    availableProductsTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Наименование" << "Цена" << "Остаток");
    availableProductsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    leftLayout->addWidget(availableProductsTable);
    
    // Количество и комментарий
    QHBoxLayout *addItemLayout = new QHBoxLayout();
    addItemLayout->addWidget(new QLabel("Количество:", this));
    
    quantitySpin = new QSpinBox(this);
    quantitySpin->setMinimum(1);
    quantitySpin->setMaximum(9999);
    quantitySpin->setValue(1);
    addItemLayout->addWidget(quantitySpin);
    
    addItemLayout->addWidget(new QLabel("Комментарий:", this));
    
    itemCommentEdit = new QLineEdit(this);
    addItemLayout->addWidget(itemCommentEdit);
    
    addItemButton = new QPushButton("Добавить", this);
    connect(addItemButton, &QPushButton::clicked, this, &MainWindow::addItemToDocument);
    addItemLayout->addWidget(addItemButton);
    
    leftLayout->addLayout(addItemLayout);
    itemsLayout->addLayout(leftLayout, 2);
    
    // Правая панель - товары в документе
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel("Товары в документе:", this));
    
    docItemsTable = new QTableWidget(this);
    docItemsTable->setColumnCount(4);
    docItemsTable->setHorizontalHeaderLabels(QStringList() << "Товар" << "Кол-во" << "Цена" << "Сумма");
    rightLayout->addWidget(docItemsTable);
    
    removeItemButton = new QPushButton("Удалить выбранный", this);
    connect(removeItemButton, &QPushButton::clicked, this, &MainWindow::removeItemFromDocument);
    rightLayout->addWidget(removeItemButton);
    
    itemsLayout->addLayout(rightLayout, 2);
    mainLayout->addLayout(itemsLayout);
    
    // Кнопки управления
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    clearDocButton = new QPushButton("Очистить", this);
    connect(clearDocButton, &QPushButton::clicked, this, &MainWindow::clearDocumentForm);
    buttonLayout->addWidget(clearDocButton);
    
    saveDocButton = new QPushButton("Сохранить документ", this);
    saveDocButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    connect(saveDocButton, &QPushButton::clicked, this, &MainWindow::saveDocument);
    buttonLayout->addWidget(saveDocButton);
    
    mainLayout->addLayout(buttonLayout);
    
    tabWidget->addTab(createTab, "➕ Создание документа");
}

void MainWindow::setupReportsTab() {
    QWidget *reportsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(reportsTab);
    
    // Панель инструментов
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    stockReportButton = new QPushButton("Отчет по складу", this);
    connect(stockReportButton, &QPushButton::clicked, this, &MainWindow::generateStockReport);
    toolbarLayout->addWidget(stockReportButton);
    
    movementReportButton = new QPushButton("Движение товаров", this);
    connect(movementReportButton, &QPushButton::clicked, this, &MainWindow::generateMovementReport);
    toolbarLayout->addWidget(movementReportButton);
    
    docsReportButton = new QPushButton("Отчет по документам", this);
    connect(docsReportButton, &QPushButton::clicked, this, &MainWindow::generateDocumentsReport);
    toolbarLayout->addWidget(docsReportButton);
    
    exportReportButton = new QPushButton("Экспорт отчета", this);
    connect(exportReportButton, &QPushButton::clicked, this, &MainWindow::exportStockReport);
    toolbarLayout->addWidget(exportReportButton);
    
    layout->addLayout(toolbarLayout);
    
    // Текстовое поле для отчетов
    reportText = new QTextEdit(this);
    reportText->setReadOnly(true);
    reportText->setFont(QFont("Courier New", 10));
    layout->addWidget(reportText);
    
    tabWidget->addTab(reportsTab, "📊 Отчеты");
}

void MainWindow::refreshStockTable() {
    stockTable->setRowCount(0);
    const auto& products = warehouse.getAllProducts();
    
    for (const auto& product : products) {
        int row = stockTable->rowCount();
        stockTable->insertRow(row);
        
        stockTable->setItem(row, 0, new QTableWidgetItem(QString::number(product->getId())));
        stockTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(product->getName())));
        stockTable->setItem(row, 2, new QTableWidgetItem(QString::number(product->getQuantity())));
        stockTable->setItem(row, 3, new QTableWidgetItem("шт."));
        stockTable->setItem(row, 4, new QTableWidgetItem(QString::number(product->getPrice(), 'f', 2)));
        
        double total = product->getPrice() * product->getQuantity();
        stockTable->setItem(row, 5, new QTableWidgetItem(QString::number(total, 'f', 2)));
    }
    
    stockTable->resizeColumnsToContents();
    updateStockSummary();
}

void MainWindow::searchProduct() {
    QString searchText = searchEdit->text().toLower();
    
    for (int i = 0; i < stockTable->rowCount(); i++) {
        bool match = false;
        
        // Проверяем ID и название
        QString idText = stockTable->item(i, 0)->text().toLower();
        QString nameText = stockTable->item(i, 1)->text().toLower();
        
        if (idText.contains(searchText) || nameText.contains(searchText)) {
            match = true;
        }
        
        stockTable->setRowHidden(i, !match);
    }
}

void MainWindow::addNewProduct() {
    bool ok;
    QString name = QInputDialog::getText(this, "Добавить товар", 
                                         "Наименование:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    
    double price = QInputDialog::getDouble(this, "Добавить товар", 
                                          "Цена:", 0, 0, 1000000, 2, &ok);
    if (!ok) return;
    
    int quantity = QInputDialog::getInt(this, "Добавить товар", 
                                       "Количество:", 0, 0, 10000, 1, &ok);
    if (!ok) return;
    
    auto product = warehouse.addProduct(name.toStdString(), price, quantity);
    if (product) {
        refreshStockTable();
        QMessageBox::information(this, "Успех", 
            QString("Товар добавлен:\nID: %1\n%2").arg(product->getId()).arg(name));
    }
}

void MainWindow::editProduct() {
    QList<QTableWidgetItem*> selected = stockTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите товар для редактирования");
        return;
    }
    
    int row = selected.first()->row();
    int id = stockTable->item(row, 0)->text().toInt();
    
    QMessageBox::information(this, "Информация", 
        "Для изменения товара нужно создать новый с новыми параметрами");
}

void MainWindow::deleteProduct() {
    QList<QTableWidgetItem*> selected = stockTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите товар для удаления");
        return;
    }
    
    int row = selected.first()->row();
    int id = stockTable->item(row, 0)->text().toInt();
    QString name = stockTable->item(row, 1)->text();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Удаление товара", 
                                 QString("Удалить товар:\n%1 (ID: %2)?").arg(name).arg(id),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (warehouse.removeProduct(id)) {
            refreshStockTable();
            QMessageBox::information(this, "Успех", "Товар удален");
        }
    }
}

void MainWindow::updateStockQuantity() {
    QList<QTableWidgetItem*> selected = stockTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите товар");
        return;
    }
    
    int row = selected.first()->row();
    int id = stockTable->item(row, 0)->text().toInt();
    QString name = stockTable->item(row, 1)->text();
    int currentQty = stockTable->item(row, 2)->text().toInt();
    
    bool ok;
    int newQuantity = QInputDialog::getInt(this, "Изменить количество", 
                                          QString("Новое количество для %1:").arg(name),
                                          currentQty, 0, 10000, 1, &ok);
    if (!ok) return;
    
    if (warehouse.updateProductQuantity(id, newQuantity)) {
        refreshStockTable();
        QMessageBox::information(this, "Успех", "Количество обновлено");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось обновить количество");
    }
}

void MainWindow::showLowStockReport() {
    warehouse.printLowStockReport();
    QMessageBox::information(this, "Низкий остаток", 
        "Отчет сгенерирован. Проверьте консоль для просмотра.");
}

void MainWindow::exportStockReport() {
    QString filename = QFileDialog::getSaveFileName(this, "Экспорт отчета", 
                                                   "stock_report.txt", 
                                                   "Текстовые файлы (*.txt)");
    if (filename.isEmpty()) return;
    
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "ОТЧЕТ ПО СКЛАДУ\n";
        out << "Дата: " << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm") << "\n";
        out << "========================================\n\n";
        
        const auto& products = warehouse.getAllProducts();
        for (const auto& product : products) {
            out << QString::fromStdString(product->getName()) << "\t"
                << product->getQuantity() << " шт.\t"
                << QString::number(product->getPrice(), 'f', 2) << " руб.\n";
        }
        
        file.close();
        QMessageBox::information(this, "Успех", "Отчет экспортирован в файл");
    }
}

void MainWindow::refreshDocumentsTable() {
    documentsTable->setRowCount(0);
    const auto& docs = warehouse.getAllDocuments();
    
    for (const auto& doc : docs) {
        int row = documentsTable->rowCount();
        documentsTable->insertRow(row);
        
        documentsTable->setItem(row, 0, new QTableWidgetItem(QString::number(doc->getId())));
        documentsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(doc->getNumber())));
        documentsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(doc->getTypeName())));
        
        // Исправляем localtime
        time_t docTime = doc->getDate();
        std::tm* tm_info = std::localtime(&docTime);
        char dateBuffer[80];
        std::strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y %H:%M", tm_info);
        documentsTable->setItem(row, 3, new QTableWidgetItem(QString(dateBuffer)));
        
        documentsTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(doc->getStatus())));
        documentsTable->setItem(row, 5, new QTableWidgetItem("Складской работник"));
    }
    
    documentsTable->resizeColumnsToContents();
}

void MainWindow::createDocument() {
    tabWidget->setCurrentIndex(2); // Переходим на вкладку создания документа
}

void MainWindow::viewDocument() {
    QList<QTableWidgetItem*> selected = documentsTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите документ");
        return;
    }
    
    int row = selected.first()->row();
    int docId = documentsTable->item(row, 0)->text().toInt();
    showDocumentDetails(docId);
}

void MainWindow::showDocumentDetails(int docId) {
    auto doc = warehouse.getDocumentById(docId);
    if (!doc) return;
    
    QString details;
    details += "=== " + QString::fromStdString(doc->getTypeName()) + " ===\n";
    details += "Номер: " + QString::fromStdString(doc->getNumber()) + "\n";
    details += "Статус: " + QString::fromStdString(doc->getStatus()) + "\n";
    
    // Исправляем localtime
    time_t docTime = doc->getDate();
    std::tm* tm_info = std::localtime(&docTime);
    char dateBuffer[80];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y %H:%M", tm_info);
    details += "Дата: " + QString(dateBuffer) + "\n";
    
    QMessageBox::information(this, "Просмотр документа", details);
}

void MainWindow::processDocument() {
    QList<QTableWidgetItem*> selected = documentsTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите документ");
        return;
    }
    
    int row = selected.first()->row();
    int docId = documentsTable->item(row, 0)->text().toInt();
    
    if (warehouse.processDocument(docId)) {
        refreshDocumentsTable();
        QMessageBox::information(this, "Успех", "Документ проведен");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось провести документ");
    }
}

void MainWindow::cancelDocument() {
    QList<QTableWidgetItem*> selected = documentsTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите документ");
        return;
    }
    
    int row = selected.first()->row();
    int docId = documentsTable->item(row, 0)->text().toInt();
    QString docNumber = documentsTable->item(row, 1)->text();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Отмена документа", 
                                 QString("Отменить документ №%1?").arg(docNumber),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (warehouse.cancelDocument(docId)) {
            refreshDocumentsTable();
            QMessageBox::information(this, "Успех", "Документ отменен");
        }
    }
}

void MainWindow::printDocument() {
    QMessageBox::information(this, "Печать", "Функция печати в разработке");
}

void MainWindow::exportDocumentsReport() {
    QMessageBox::information(this, "Экспорт", "Функция экспорта в разработке");
}

void MainWindow::onDocumentTypeChanged(int index) {
    specificFieldsTable->setRowCount(0);
    
    switch(index) {
        case 0: // Чек
            specificFieldsTable->setRowCount(4);
            specificFieldsTable->setItem(0, 0, new QTableWidgetItem("Сотрудник"));
            specificFieldsTable->setItem(0, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(1, 0, new QTableWidgetItem("Смена"));
            specificFieldsTable->setItem(1, 1, new QTableWidgetItem("Дневная"));
            specificFieldsTable->setItem(2, 0, new QTableWidgetItem("Номер заказа"));
            specificFieldsTable->setItem(2, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(3, 0, new QTableWidgetItem("Тип оплаты"));
            specificFieldsTable->setItem(3, 1, new QTableWidgetItem("Наличные"));
            break;
            
        case 1: // Накладная прихода
            specificFieldsTable->setRowCount(4);
            specificFieldsTable->setItem(0, 0, new QTableWidgetItem("Поставщик"));
            specificFieldsTable->setItem(0, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(1, 0, new QTableWidgetItem("Договор"));
            specificFieldsTable->setItem(1, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(2, 0, new QTableWidgetItem("Счет-фактура"));
            specificFieldsTable->setItem(2, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(3, 0, new QTableWidgetItem("Склад приемки"));
            specificFieldsTable->setItem(3, 1, new QTableWidgetItem("Основной"));
            break;
            
        case 2: // Накладная расхода
            specificFieldsTable->setRowCount(4);
            specificFieldsTable->setItem(0, 0, new QTableWidgetItem("Получатель"));
            specificFieldsTable->setItem(0, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(1, 0, new QTableWidgetItem("Основание"));
            specificFieldsTable->setItem(1, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(2, 0, new QTableWidgetItem("Склад отгрузки"));
            specificFieldsTable->setItem(2, 1, new QTableWidgetItem("Основной"));
            specificFieldsTable->setItem(3, 0, new QTableWidgetItem("Транспорт"));
            specificFieldsTable->setItem(3, 1, new QTableWidgetItem(""));
            break;
            
        case 3: // Акт инвентаризации
            specificFieldsTable->setRowCount(4);
            specificFieldsTable->setItem(0, 0, new QTableWidgetItem("Комиссия"));
            specificFieldsTable->setItem(0, 1, new QTableWidgetItem(""));
            specificFieldsTable->setItem(1, 0, new QTableWidgetItem("Склад"));
            specificFieldsTable->setItem(1, 1, new QTableWidgetItem("Основной"));
            specificFieldsTable->setItem(2, 0, new QTableWidgetItem("Причина"));
            specificFieldsTable->setItem(2, 1, new QTableWidgetItem("Плановая инвентаризация"));
            specificFieldsTable->setItem(3, 0, new QTableWidgetItem("Результат"));
            specificFieldsTable->setItem(3, 1, new QTableWidgetItem("Не проведена"));
            break;
    }
    
    // Обновляем таблицу доступных товаров
    availableProductsTable->setRowCount(0);
    const auto& products = warehouse.getAllProducts();
    
    for (const auto& product : products) {
        int row = availableProductsTable->rowCount();
        availableProductsTable->insertRow(row);
        
        availableProductsTable->setItem(row, 0, new QTableWidgetItem(QString::number(product->getId())));
        availableProductsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(product->getName())));
        availableProductsTable->setItem(row, 2, new QTableWidgetItem(QString::number(product->getPrice(), 'f', 2)));
        availableProductsTable->setItem(row, 3, new QTableWidgetItem(QString::number(product->getQuantity())));
    }
    
    availableProductsTable->resizeColumnsToContents();
}

void MainWindow::addItemToDocument() {
    QList<QTableWidgetItem*> selected = availableProductsTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите товар");
        return;
    }
    
    int row = selected.first()->row();
    int productId = availableProductsTable->item(row, 0)->text().toInt();
    QString productName = availableProductsTable->item(row, 1)->text();
    double price = availableProductsTable->item(row, 2)->text().toDouble();
    int availableQty = availableProductsTable->item(row, 3)->text().toInt();
    
    int quantity = quantitySpin->value();
    if (quantity > availableQty && docTypeCombo->currentIndex() == 2) { // Накладная расхода
        QMessageBox::warning(this, "Ошибка", 
            QString("Недостаточно товара на складе.\nДоступно: %1 шт.").arg(availableQty));
        return;
    }
    
    // Добавляем в таблицу товаров документа
    int docRow = docItemsTable->rowCount();
    docItemsTable->insertRow(docRow);
    
    docItemsTable->setItem(docRow, 0, new QTableWidgetItem(productName));
    docItemsTable->setItem(docRow, 1, new QTableWidgetItem(QString::number(quantity)));
    docItemsTable->setItem(docRow, 2, new QTableWidgetItem(QString::number(price, 'f', 2)));
    
    double total = price * quantity;
    docItemsTable->setItem(docRow, 3, new QTableWidgetItem(QString::number(total, 'f', 2)));
    
    // Очищаем поле комментария
    itemCommentEdit->clear();
    
    docItemsTable->resizeColumnsToContents();
}

void MainWindow::removeItemFromDocument() {
    QList<QTableWidgetItem*> selected = docItemsTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите товар для удаления");
        return;
    }
    
    int row = selected.first()->row();
    docItemsTable->removeRow(row);
}

void MainWindow::clearDocumentForm() {
    docItemsTable->setRowCount(0);
    docCommentEdit->clear();
    docNumberEdit->setText("ЧК-" + QString::number(QDateTime::currentSecsSinceEpoch()));
}

void MainWindow::saveDocument() {
    // Создаем документ
    DocumentType type;
    switch(docTypeCombo->currentIndex()) {
        case 0: type = DocumentType::RECEIPT; break;
        case 1: type = DocumentType::INCOME_INVOICE; break;
        case 2: type = DocumentType::OUTCOME_INVOICE; break;
        case 3: type = DocumentType::INVENTORY; break;
        default: return;
    }
    
    std::shared_ptr<DocumentBase> doc;
    switch(type) {
        case DocumentType::RECEIPT:
            doc = warehouse.createReceipt(
                docNumberEdit->text().toStdString(),
                docCreatedByEdit->text().toStdString(),
                docDepartmentEdit->text().toStdString(),
                docCommentEdit->toPlainText().toStdString());
            break;
        case DocumentType::INCOME_INVOICE:
            doc = warehouse.createIncomeInvoice(
                docNumberEdit->text().toStdString(),
                docCreatedByEdit->text().toStdString(),
                docDepartmentEdit->text().toStdString(),
                docCommentEdit->toPlainText().toStdString());
            break;
        case DocumentType::OUTCOME_INVOICE:
            doc = warehouse.createOutcomeInvoice(
                docNumberEdit->text().toStdString(),
                docCreatedByEdit->text().toStdString(),
                docDepartmentEdit->text().toStdString(),
                docCommentEdit->toPlainText().toStdString());
            break;
        case DocumentType::INVENTORY:
            doc = warehouse.createInventory(
                docNumberEdit->text().toStdString(),
                docCreatedByEdit->text().toStdString(),
                docDepartmentEdit->text().toStdString(),
                docCommentEdit->toPlainText().toStdString());
            break;
    }
    
    if (!doc) {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать документ");
        return;
    }
    
    // Устанавливаем специфичные поля
    for (int i = 0; i < specificFieldsTable->rowCount(); i++) {
        QString fieldName = specificFieldsTable->item(i, 0)->text();
        QString fieldValue = specificFieldsTable->item(i, 1)->text();
        doc->setSpecificField(fieldName.toStdString(), fieldValue.toStdString());
    }
    
    // Сохраняем документ в файл
    QString filename = "documents/" + docNumberEdit->text() + ".txt";
    doc->saveToFile(filename.toStdString());
    
    // Обновляем таблицы
    refreshDocumentsTable();
    clearDocumentForm();
    
    QMessageBox::information(this, "Успех", 
        QString("Документ создан и сохранен в файл:\n%1").arg(filename));
}

void MainWindow::generateStockReport() {
    QString report;
    report += "ОТЧЕТ ПО СКЛАДУ\n";
    report += "Дата формирования: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm") + "\n";
    report += "========================================\n\n";
    
    const auto& products = warehouse.getAllProducts();
    report += QString("Всего наименований: %1\n").arg(products.size());
    
    int totalQty = 0;
    double totalValue = 0;
    
    for (const auto& product : products) {
        double value = product->getPrice() * product->getQuantity();
        totalQty += product->getQuantity();
        totalValue += value;
        
        report += QString("%1 | %2 шт. | %3 руб. | %4 руб.\n")
            .arg(QString::fromStdString(product->getName()).left(30))
            .arg(product->getQuantity(), 6)
            .arg(product->getPrice(), 10, 'f', 2)
            .arg(value, 12, 'f', 2);
    }
    
    report += "\n========================================\n";
    report += QString("Итого: %1 шт. | Общая стоимость: %2 руб.\n")
        .arg(totalQty, 8)
        .arg(totalValue, 15, 'f', 2);
    
    reportText->setText(report);
    tabWidget->setCurrentIndex(3); // Переходим на вкладку отчетов
}

void MainWindow::generateMovementReport() {
    QString report;
    report += "ОТЧЕТ ПО ДВИЖЕНИЮ ТОВАРОВ\n";
    report += "Период: " + QDate::currentDate().addDays(-30).toString("dd.MM.yyyy") + 
              " - " + QDate::currentDate().toString("dd.MM.yyyy") + "\n";
    report += "========================================\n\n";
    report += "Отчет в разработке...\n";
    
    reportText->setText(report);
    tabWidget->setCurrentIndex(3);
}

void MainWindow::generateDocumentsReport() {
    QString report;
    report += "ОТЧЕТ ПО ДОКУМЕНТАМ\n";
    report += "Дата: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm") + "\n";
    report += "========================================\n\n";
    
    const auto& docs = warehouse.getAllDocuments();
    report += QString("Всего документов: %1\n\n").arg(docs.size());
    
    for (const auto& doc : docs) {
        // Исправляем localtime
        time_t docTime = doc->getDate();
        std::tm* tm_info = std::localtime(&docTime);
        char dateBuffer[80];
        std::strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y", tm_info);
        
        report += QString("%1 №%2 | %3 | %4\n")
            .arg(QString::fromStdString(doc->getTypeName()), 25)
            .arg(QString::fromStdString(doc->getNumber()), 15)
            .arg(QString(dateBuffer), 12)
            .arg(QString::fromStdString(doc->getStatus()));
    }
    
    reportText->setText(report);
    tabWidget->setCurrentIndex(3);
}

void MainWindow::updateStatusBar() {
    int totalProducts = warehouse.getTotalProductsCount();
    int totalItems = warehouse.getTotalItemsCount();
    double totalValue = warehouse.getTotalInventoryValue();
    
    QString status = QString("Товаров: %1 | Единиц: %2 | Стоимость: %3 руб. | Пользователь: Складской работник")
        .arg(totalProducts)
        .arg(totalItems)
        .arg(totalValue, 0, 'f', 2);
    
    statusLabel->setText(status);
}

void MainWindow::updateStockSummary() {
    // Можно добавить виджет с суммарной информацией
}

void MainWindow::about() {
    QMessageBox::about(this, "О программе",
        "<h2>Складская система управления</h2>"
        "<p>Версия 1.0</p>"
        "<p>Система для управления складскими операциями.</p>"
        "<p>Функционал:</p>"
        "<ul>"
        "<li>Учет товаров на складе</li>"
        "<li>Создание документов (чеки, накладные)</li>"
        "<li>Формирование отчетов</li>"
        "<li>Контроль остатков</li>"
        "</ul>"
        "<p>© 2024 Складские системы</p>");
}

void MainWindow::saveData() {
    if (warehouse.saveToFile()) {
        QMessageBox::information(this, "Сохранение", "Данные успешно сохранены");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные");
    }
}

void MainWindow::loadData() {
    if (warehouse.loadFromFile()) {
        refreshStockTable();
        refreshDocumentsTable();
        updateStatusBar();
        QMessageBox::information(this, "Загрузка", "Данные успешно загружены");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить данные");
    }
}