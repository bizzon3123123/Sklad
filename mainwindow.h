#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QMenuBar> 
#include <QTextEdit>
#include <QTabWidget>
#include <QGroupBox>
#include <QDateEdit>
#include <QStatusBar>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include "warehouse.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Вкладка "Склад"
    void refreshStockTable();
    void searchProduct();
    void addNewProduct();
    void editProduct();
    void deleteProduct();
    void updateStockQuantity();
    void showLowStockReport();
    void exportStockReport();
    
    // Вкладка "Документы"
    void refreshDocumentsTable();
    void createDocument();
    void viewDocument();
    void processDocument();
    void cancelDocument();
    void printDocument();
    void exportDocumentsReport();
    
    // Вкладка "Создание документа"
    void onDocumentTypeChanged(int index);
    void addItemToDocument();
    void removeItemFromDocument();
    void clearDocumentForm();
    void saveDocument();
    
    // Вкладка "Отчеты"
    void generateStockReport();
    void generateMovementReport();
    void generateDocumentsReport();
    
    // Общие
    void updateStatusBar();
    void about();
    void saveData();
    void loadData();

private:
    Warehouse warehouse;
    
    // Виджеты
    QTabWidget *tabWidget;
    
    // Вкладка "Склад"
    QTableWidget *stockTable;
    QLineEdit *searchEdit;
    QPushButton *searchButton;
    QPushButton *refreshStockButton;
    QPushButton *addProductButton;
    QPushButton *editProductButton;
    QPushButton *deleteProductButton;
    QPushButton *updateQuantityButton;
    QPushButton *lowStockButton;
    QPushButton *exportStockButton;
    
    // Вкладка "Документы"
    QTableWidget *documentsTable;
    QComboBox *docTypeFilter;
    QPushButton *refreshDocsButton;
    QPushButton *createDocButton;
    QPushButton *viewDocButton;
    QPushButton *processDocButton;
    QPushButton *cancelDocButton;
    QPushButton *printDocButton;
    QPushButton *exportDocsButton;
    
    // Вкладка "Создание документа"
    QComboBox *docTypeCombo;
    QLineEdit *docNumberEdit;
    QLineEdit *docCreatedByEdit;
    QLineEdit *docDepartmentEdit;
    QDateEdit *docDateEdit;
    QTextEdit *docCommentEdit;
    QTableWidget *availableProductsTable;
    QTableWidget *docItemsTable;
    QSpinBox *quantitySpin;
    QLineEdit *itemCommentEdit;
    QPushButton *addItemButton;
    QPushButton *removeItemButton;
    QPushButton *clearDocButton;
    QPushButton *saveDocButton;
    QTableWidget *specificFieldsTable;
    
    // Вкладка "Отчеты"
    QTextEdit *reportText;
    QPushButton *stockReportButton;
    QPushButton *movementReportButton;
    QPushButton *docsReportButton;
    QPushButton *exportReportButton;
    
    // Статус бар
    QLabel *statusLabel;
    
    // Меню
    QMenu *fileMenu;
    QMenu *helpMenu;
    
    // Действия
    QAction *saveAction;
    QAction *loadAction;
    QAction *exitAction;
    QAction *aboutAction;
    
    void setupUI();
    void setupMenu();
    void setupStockTab();
    void setupDocumentsTab();
    void setupCreateDocTab();
    void setupReportsTab();
    
    void updateStockSummary();
    void updateSpecificFieldsTable();
    void showDocumentDetails(int docId);
};

#endif // MAINWINDOW_H