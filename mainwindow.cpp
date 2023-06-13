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

        QString t= nullptr;
        if(i!= ui->spinBox->value())
        {
            t = QString("X");
            t += QString::number(i);
        }
        else
        {
            t =  QString("B");
        }
        ui->tableWidget->setItem(0, i, new QTableWidgetItem(t));
    }
    QString t = QString("Bi");
    ui->tableWidget->setItem(ui->tableWidget->columnCount(), 0, new QTableWidgetItem(t));

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
    SetTimeDate();
    SendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
    SetTimeDate();
    SendToServer(ui->lineEdit->text());
}


void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{
    for(int i = ui->tableWidget->rowCount()-1; ui->spinBox->value() > i; i = ui->tableWidget->rowCount()-1)
    {
        ui->tableWidget->insertColumn(i);
        ui->tableWidget->insertRow(i);
        QString t = QString("X");
        t += QString::number(i);
        ui->tableWidget->setItem(0, i, new QTableWidgetItem(t));
    }
    for(int i = ui->tableWidget->rowCount()-1; ui->spinBox->value() < i; i = ui->tableWidget->rowCount()-1)
    {
        ui->tableWidget->removeColumn(i-1);
        ui->tableWidget->removeRow(i);
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    SetTimeDate();
    QString str = GetDataFromForm();
    SendToServer(str);
}

QString MainWindow::GetDataFromForm()
{
    QString str;
    ui->textBrowser_2->clear();
    for(int i = 1; i< ui->tableWidget->rowCount(); i++)
    {
        for(int j = 0; j< ui->tableWidget->columnCount() - 1 ; j++)
        {
            QTableWidgetItem *item1(ui->tableWidget->item(i,j));
            if (item1)
            {
                QString R1 = item1->text();
                bool b = !R1.isEmpty() && !R1.isNull();
                if(b)
                {
                    // Make query to server to send and get data
                     str += QString::number( ui->tableWidget->item(i,j)->text().toInt() ); //some data
                     str += " ";
                }
            }
        }
        //Show the result
        //QString t = QString("X");
        //t+= QString::number(i)+"= "+QString::number(0) ;
        //ui->textBrowser_2->append(t);
    }
    str +="|";
    int columncount = ui->tableWidget->columnCount();
    for(int i = 1; i< ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *item1(ui->tableWidget->item(i,columncount-1));
        if (item1)
        {
            QString R1 = item1->text();
            bool b = !R1.isEmpty() && !R1.isNull();
            if(b)
            {
                str += QString::number( ui->tableWidget->item(i, columncount-1)->text().toInt() );
                str += " ";
            }
        }
    }
     return str;
}


void MainWindow::SetTimeDate()
{
    ui->textBrowser_3->clear();
    ui->textBrowser_3->append(QDateTime::currentDateTime().toString());
}
