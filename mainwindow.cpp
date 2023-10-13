#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->setRegression(LINE);
    ui->butAuto->setCheckable(true);
    ui->groupBoxSetArgs->setFixedHeight(60);
    ui->groupBoxSetArgs->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butCrear_clicked()
{
    ui->widget->clearPoints();
}

void MainWindow::on_butLine_clicked()
{
    ui->widget->calculateLine();
}

void MainWindow::on_actionLin_triggered()
{
    ui->widget->setRegression(LINE);
}

void MainWindow::on_actionSin_triggered()
{
    ui->widget->setRegression(SINUSA);
}

void MainWindow::on_butAuto_clicked(bool checked)
{
    ui->groupBoxSetArgs->setEnabled(checked);
    ui->widget->setManual(checked);

    float *arg = ui->widget->getParametrs();
    ui->lineEditArg1->setText(QString::number(arg[0], 'f', 2));
    ui->lineEditArg2->setText(QString::number(arg[1], 'f', 2));
    ui->lineEditArg3->setText(QString::number(arg[2], 'f', 2));
    ui->lineEditArg4->setText(QString::number(arg[3], 'f', 2));
}

void MainWindow::on_butSet_clicked()
{
    float a = ui->lineEditArg1->text().toFloat();
    float b = ui->lineEditArg2->text().toFloat();
    float c = ui->lineEditArg3->text().toFloat();
    float d = ui->lineEditArg4->text().toFloat();

    ui->widget->setParametrs(a, b, c, d);
}
