#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QValidator>

#include "definition.h";

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_butCrear_clicked();
    void on_butLine_clicked();
    void on_actionLin_triggered();
    void on_actionSin_triggered();
    void on_butAuto_clicked(bool checked);
    void on_butSet_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
