#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "managerwindow.h"
#include "registerwindow.h"
#include <qsqldatabase.h>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        // here you are Data Base Parameters
    dataBase= QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("127.0.0.1"); // @ip MySql Server
    dataBase.setDatabaseName("passwordmanager"); //Database Name
    dataBase.setUserName("root"); // User Name
    dataBase.setPassword("%;`jXQG|(l8I]KDWXm,V58yQ<Oi<hq3P"); // Password
    dataBase.setPort(3306);
    dataBase.open();
    QSqlDatabase dataBase = QSqlDatabase::database();
    // Check if the MySQL driver is available
    if (!dataBase.open()) {
        qDebug() << "Error: Failed to open database:" << dataBase.lastError().text();
    }
    qDebug() << "Database opened successfully";
    QSqlQuery query(dataBase);
    query.executedQuery();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SignUp_Button_clicked()
{
    hide();
    RegisterWindow *registerWindow = new RegisterWindow(this);
    registerWindow->show();
}


void MainWindow::on_Quit_Button_clicked()
{
    QApplication::quit();
}


void MainWindow::on_LogIn_Button_clicked()
{
    QString username = ui->line_username->text();
    QString password = ui->line_password->text();
    //prerobit na sql dotazy
    if(username =="tester1" && password=="12345678"){
        ui->status_message->setText("login is sucessfull");
        close();
        ManagerWindow *managerWindow = new ManagerWindow(this);
        managerWindow->show();
    }
    else if(username.isEmpty() || password.isEmpty())
    {
        ui->status_message->setText("Username or password is blank");
    }
    else {
        ui->status_message->setText("Username or password is incorrect");
    }






}

