#include "mainwindow.h"
#include "managerwindow.h"
#include "./ui_mainwindow.h"
#include "argon2.h"
#include "registerwindow.h"
#include <qsqldatabase.h>

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>

using namespace std;
QSqlDatabase MainWindow::dataBase; // Initialize the static member
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     ui->stackedWidget->setCurrentIndex(0);
        // here you are Data Base Parameters
    dataBase= QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("127.0.0.1"); // @ip MySql Server
    dataBase.setDatabaseName("passwordmanager"); //Database Name
    dataBase.setUserName("root"); // User Name
    dataBase.setPassword("%;`jXQG|(l8I]KDWXm,V58yQ<Oi<hq3P"); // Password
    dataBase.setPort(3306);
    dataBase.open();

    // Check if the MySQL driver is available
    if (!dataBase.open()) {
        qDebug() << "Error: Failed to open database:" << dataBase.lastError().text();
    }
    qDebug() << "Database opened successfully";
    QSqlQuery query;
    query.executedQuery();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SignUp_Button_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);

}


void MainWindow::on_Quit_Button_clicked()
{
    QApplication::quit();
}


void MainWindow::on_LogIn_Button_clicked()
{
    setUsernameL( ui->line_username->text());
     passwordL = ui->line_password->text();
    size_t passwordL_lenght=passwordL.length();


    if(getUsernameL().isEmpty() || passwordL.isEmpty())
    {
        ui->status_message->setText("Username or password is blank");
        return;
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM `passwordmanager`.`login_information` WHERE Login_name = :username");
    query.bindValue(":username", getUsernameL());
    if(query.exec()){
        if(query.next()){
         passwordM = query.value("Login_master_password").toString();
        } else{
            qDebug()<<"Username not found";
        }
    }else {
        qDebug() << "Query execution error:" << query.lastError().text();
    }
    // Convert QString to const char* for passwordL
    std::string passwordL_str = passwordL.toStdString();
    const char* passwordL_char = passwordL_str.c_str();

    // Convert QString to const char* for passwordM
    std::string passwordM_str = passwordM.toStdString();
    const char* passwordM_char = passwordM_str.c_str();
    if(query.exec() && query.next())
    {
        // qDebug() << "usrL "<<getUsernameL();
        // qDebug() << "passL "<<passwordL;
        // qDebug() << "passM "<<passwordM;
       // argon2_verify(storedHashedPassword.toUtf8().constData(), password.toUtf8().constData(), password.length(),Argon2_id) == ARGON2_OK;
           int verificationResult =argon2id_verify(passwordM_char,passwordL_char,passwordL_lenght);
            if (verificationResult == ARGON2_OK)
            {
                // Passwords match
                qDebug() << "Login successful";
                 hide();
                ManagerWindow *managerWindow = new ManagerWindow(usernameL);
                managerWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure deletion on close
                managerWindow->setWindowFlags(Qt::Window); // Ensure appropriate window flags are set
                managerWindow->show();


            }
            else
            {
                // Passwords do not match
                qDebug() << "Login failed";
            }
        }
        else
        {
            // Handle query execution failure or no matching user
            qDebug() << "Login query failed:" << query.lastError().text();
        }

}

void MainWindow::setUsernameL(const QString& username)
{
    usernameL = username;
}

QString MainWindow::getUsernameL() const
{
    return usernameL;
}


