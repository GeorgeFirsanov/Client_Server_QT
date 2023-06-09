#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    for(int i =0; i<ui->spinBox->value() + 1; i++)
    {
        ui->tableWidget->insertColumn(i);
        ui->tableWidget->insertRow(i);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    uint port = 1111;
    socket ->connectToHost("127.0.0.1", port);
}

void MainWindow::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    out << str;
    socket -> write(Data);
    ui->lineEdit->clear();
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    if(in.status()== QDataStream::Ok)
    {
        QString str;
        in >>str;
        ui->textBrowser->append(str);
    }
    else
    {
        ui->textBrowser->append("read error");
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    SendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
    SendToServer(ui->lineEdit->text());
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    for(; ui->spinBox->value() > ui->tableWidget->rowCount() - 1;)
    {
        ui->tableWidget->insertColumn(ui->tableWidget->rowCount() - 1);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    }

    for(; ui->spinBox->value() < ui->tableWidget->rowCount() - 1;)
    {
        ui->tableWidget->removeColumn(ui->tableWidget->rowCount() - 1);
        ui->tableWidget->removeRow(ui->tableWidget->rowCount());
        break;

    }
}


void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{
    for(; ui->spinBox->value() > ui->tableWidget->rowCount() - 1;)
    {
        ui->tableWidget->insertColumn(ui->tableWidget->rowCount() - 1);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    }

    for(; ui->spinBox->value() < ui->tableWidget->rowCount() - 1;)
    {
        ui->tableWidget->removeColumn(ui->tableWidget->rowCount() - 1);
        ui->tableWidget->removeRow(ui->tableWidget->rowCount());
        break;
    }
}

