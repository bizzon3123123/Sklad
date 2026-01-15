#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "warehouse.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onShowProducts();
    void onCreateDocument();

private:
    Ui::MainWindow *ui;
    Warehouse warehouse;
};

#endif // MAINWINDOW_H