#include "mainwindow.h"
#include "managerwindow.h"
#include "./ui_mainwindow.h"
#include <qsqldatabase.h>
#include "argon2.h"

#include <stdio.h>
#include <random>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>

#define HASHLEN 32
#define SALTLEN 16
using namespace std;
QSqlDatabase MainWindow::dataBase; // Initialize the static member
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    createDatabaseConnection();

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
    QSqlDatabase::database().close();
    QApplication::quit();
}


void MainWindow::on_LogIn_Button_clicked()
{
    setUsernameL( ui->line_username->text());
     passwordL = ui->line_password->text();
    size_t passwordL_lenght=passwordL.length();


    if(getUsernameL().isEmpty() || passwordL.isEmpty())
    {
        ui->status_message_log->setText("Username or password is blank");
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



void MainWindow::on_Back_Button_To_Login_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}


void MainWindow::on_Register_Button_clicked()
{
    registerUser();
}


void MainWindow::on_Confirm_Button_clicked()
{
    createTableAndStorePassword();

}

void MainWindow::createTableAndStorePassword(){
    QSqlQuery query;
    QString database_name= usernameL;
    QString table_name= database_name+"_password_data";
    uint8_t salt[SALTLEN];
    generateRandomSalt(salt, SALTLEN);
    uint32_t pwdlen = passwordM.size();
    uint32_t t_cost = 2;            // 2-pass computation
    uint32_t m_cost = (1<<16);      // 64 mebibytes memory usage
    uint32_t parallelism = 1;       // number of threads and lanes
    char encodedPasswordHash[HASHLEN*8 +1];
    std::string passwordString = passwordM.toStdString();
    const char* passwordCString = passwordString.c_str();
    qDebug()<<passwordCString;
    qDebug()<<"Print current selected"<<ui->comboBox->currentText();
    if(ui->comboBox->currentText()=="Argon2id"){
        // Check if the MySQL driver is available
        if (!dataBase.isOpen()) {
            qDebug() << "Error: Failed to open database:" << dataBase.lastError().text();
            return;
        }
        else {
            qDebug() << "openned database:" << dataBase.lastError().text();
            // Prepare the SQL query with placeholders
            query.prepare("CREATE TABLE IF NOT EXISTS `passwordmanager`.`" + table_name + "` ("
                                                                                          "`ID` INT NOT NULL AUTO_INCREMENT,"
                                                                                          "`Name of APP` VARCHAR(48) NULL,"
                                                                                          "`Username` VARCHAR(48) NULL,"
                                                                                          "`Password` VARCHAR(64) NULL,"
                                                                                          "`URL` VARCHAR(512) NULL,"
                                                                                          "`log` DATETIME NULL,"
                                                                                          "PRIMARY KEY (`ID`),"
                                                                                          "UNIQUE INDEX `ID_UNIQUE` (`ID` ASC))");
            // Execute the prepared statement
            if (query.exec()) {
                argon2id_hash_encoded(t_cost, m_cost, parallelism, passwordCString, pwdlen, salt, SALTLEN, HASHLEN, encodedPasswordHash, sizeof(encodedPasswordHash));
                qDebug() << "argon2idHash:"<<encodedPasswordHash;
                QString storedPasswordQString = QString::fromUtf8(encodedPasswordHash);
                qDebug() << "Table created successfully";
                query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password) VALUES (:username, :password)");
                query.bindValue(":username", usernameL);
                query.bindValue(":password", storedPasswordQString);
                //query.bindValue(":salt", hexSalt);
                if(query.exec()){
                    qDebug() << "Insert created successfully";
                    ui->stackedWidget->setCurrentIndex(0);

                }
                else{
                    qDebug() << "Error: " << query.lastError().text();
                }
            } else {
                qDebug() << "Error: " << query.lastError().text();
            }
            return;
        }
    }
}
void MainWindow::generateRandomSalt(uint8_t *salt, size_t saltSize)
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> distribution(0, CHARACTERS.size() - 1);

    for (size_t i = 0; i < saltSize; ++i)
    {
        salt[i] = CHARACTERS[distribution(gen)];
    }

}
void MainWindow::registerUser(){
    QSqlQuery query;
    usernameL          = ui->line_login->text();
    passwordM     = ui->line_master_password->text();
    QString confirm_password    = ui->line_confirm_password->text();
    QString email               = ui->Email->text();
    if(usernameL.length()>3 && passwordM.length()>16 && confirm_password.length()>16 && email.length()>4 && isValidEmail(email)){
        if(passwordM==confirm_password){
            query.prepare("SELECT Login_name FROM `passwordmanager`.`login_information` WHERE Login_name = :username");
            query.bindValue(":username", usernameL);
            ui->status_message_reg->setText("Username is already taken");
            if(query.exec()){
                qDebug() << "Query executed to";
                if(query.next()){
                    qDebug() << "Error: Username already exists.";
                    return;
                }
                else if (!query.next()){
                    ui->stackedWidget->setCurrentIndex(2);
                }
                else {
                    qDebug() << "Error: " << query.lastError().text();
                    return;
                }
            }

        }
        else{
            ui->status_message_reg->setText("Passwords are not matching");
        }

    }
    else if (usernameL.length()<3){
        ui->status_message_reg->setText("Login name is short");
    }
    else if (passwordM.length()<16){
        ui->status_message_reg->setText("master password is short");
    }
    else if (confirm_password.length()<16){
        ui->status_message_reg->setText("confirm password is short");
    }
    else if (email.length()<4){
        ui->status_message_reg->setText("Email name is short");
    }
    else if(!isValidEmail(email)){
        ui->status_message_reg->setText("Wrong email format");
    }

}

bool MainWindow::isValidEmail(QString &email) {
    QRegularExpression rx("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", QRegularExpression::CaseInsensitiveOption);
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    int pos = 0;
    return validator->validate(email, pos) == QValidator::Acceptable;
}

void MainWindow::on_Back_Button_To_Register_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::createDatabaseConnection(){
    // here you are Data Base Parameters
    dataBase= QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("127.0.0.1"); // @ip MySql Server
    dataBase.setDatabaseName("passwordmanager"); //Database Name
    dataBase.setUserName("root"); // User Name
    dataBase.setPassword("%;`jXQG|(l8I]KDWXm,V58yQ<Oi<hq3P"); // Password
    dataBase.setPort(3306);

    // Check if the MySQL driver is available
    if (!dataBase.open()) {
        qDebug() << "Error: Failed to open database:" << dataBase.lastError().text();
    }
    else{qDebug() << "Database opened successfully";}

}
