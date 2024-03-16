#include "encryptindialog.h"
#include "qsqlquery.h"
#include "mainwindow.h"
#include <QSqlQuery>
#include "ui_encryptindialog.h"
#include <argon2.h>
#include <random>
#include "QSql"
#include <stdio.h>
#define HASHLEN 32
#define SALTLEN 16
EncryptinDialog::EncryptinDialog(const QString &login_name,const QString &masterPassword, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EncryptinDialog),
    passwordM(masterPassword),
    loginM(login_name)

{
    qDebug() << "login_name:"<<login_name;
    ui->setupUi(this);
}

EncryptinDialog::~EncryptinDialog()
{
    delete ui;

}

void generateRandomSalt(uint8_t *salt, size_t saltSize)
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

void EncryptinDialog::on_Confirm_Button_clicked()
{
    QSqlDatabase dataBase = MainWindow::dataBase;
    QSqlQuery query(dataBase);
    QString database_name= loginM;
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
                "`Name of APP` VARCHAR(48) NULL,"
                "`Username` VARCHAR(48) NULL,"
                "`Password` VARCHAR(64) NULL,"
                "`URL` VARCHAR(512) NULL,"
                "`log` DATETIME NULL)");
            // Execute the prepared statement
            if (query.exec()) {
                argon2id_hash_encoded(t_cost, m_cost, parallelism, passwordCString, pwdlen, salt, SALTLEN, HASHLEN, encodedPasswordHash, sizeof(encodedPasswordHash));
                qDebug() << "argon2idHash:"<<encodedPasswordHash;
                QString storedPasswordQString = QString::fromUtf8(encodedPasswordHash);
                qDebug() << "Table created successfully";
                query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password) VALUES (:username, :password)");
                query.bindValue(":username", loginM);
                query.bindValue(":password", storedPasswordQString);
                //query.bindValue(":salt", hexSalt);
                if(query.exec()){
                    close();
                    qDebug() << "Insert created successfully";
                    // MainWindow *mainWindow = new MainWindow(this);
                    // mainWindow->show();
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


void EncryptinDialog::on_Quit_Button_clicked()
{
   QApplication::quit();
}


void EncryptinDialog::on_Back_Button_clicked()
{
    close();
    parentWidget()->show();
}


