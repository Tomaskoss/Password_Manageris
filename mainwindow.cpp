#include "mainwindow.h"
#include "managerwindow.h"
#include "./ui_mainwindow.h"
#include "qsqldatabase.h"
#include "argon2.h"
#include <stdio.h>


#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <random>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#define HASHLEN 32
#define SALTLEN 16

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
    QString storedSaltQString;
    int iterations=0;
    unsigned char generated_hash[HASHLEN];
    QString algorithm_type;


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
         storedSaltQString= query.value("Login_salt").toString();
         iterations = query.value("Login_iterations").toInt();
         algorithm_type = query.value("Kdf_algorithm").toString();

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

    // Convert QString to QByteArray and get the raw data
    QByteArray saltData = storedSaltQString.toUtf8();

    const unsigned char *salt = reinterpret_cast<const unsigned char *>(saltData.constData());
    if(query.exec() && query.next())
    {
        if(algorithm_type=="Argon2id"){
           int verificationResult =argon2id_verify(passwordM_char,passwordL_char,passwordL_lenght);
            if (verificationResult == ARGON2_OK)
            {
                // Passwords match
                qDebug() << "Login successful";
                 hide();
                ManagerWindow *managerWindow = new ManagerWindow(usernameL,this);
                managerWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure deletion on close
                managerWindow->setWindowFlags(Qt::Window); // Ensure appropriate window flags are set
                managerWindow->show();
                ui->line_password->clear();
                ui->line_username->clear();
            }
        }
        else if (algorithm_type=="PBKDF2"){
                // Passwords do not match
                PKCS5_PBKDF2_HMAC(passwordL_char,passwordL.length(),salt,SALTLEN,iterations,EVP_sha512(),HASHLEN,generated_hash);
                // Convert passwordM QString to QByteArray
                QString generatedHashStr = QByteArray(reinterpret_cast<const char*>(generated_hash), HASHLEN).toHex();
                if (generatedHashStr==passwordM_char) {
                    // Passwords match
                    qDebug() << "Login successful";
                    hide();
                    ManagerWindow *managerWindow = new ManagerWindow(usernameL,this);
                    managerWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure deletion on close
                    managerWindow->setWindowFlags(Qt::Window); // Ensure appropriate window flags are set
                    managerWindow->show();
                    ui->line_password->clear();
                    ui->line_username->clear();
                    qDebug()<<"variables "<<usernameL<<passwordM_char<<passwordL_char<<passwordL_str<<passwordM_str<<salt<<generated_hash<<iterations;
                }
                else{
                    qDebug()<<"Wrong password";
                }
        }else if (algorithm_type=="Scrypt"){

        }
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

    if(ui->comboBox->currentText()=="Argon2id"){
        Argon_KDF();}

    else if (ui->comboBox->currentText()=="PBKDF2"){
        PBKDF2_KDF();
    }
    else if(ui->comboBox->currentText()=="Scrypt"){
        Scrypt_KDF();
        }
 }
// void generateRandomSalt(uint8_t *salt, size_t saltSize)
// {
//     std::random_device rd;
//     std::mt19937_64 gen(rd());
//     std::uniform_int_distribution<uint8_t> distribution(0, 255);

//     for (size_t i = 0; i < saltSize; ++i)
//     {
//         salt[i] = distribution(gen);
//     }

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

void MainWindow::Argon_KDF(){
    QSqlQuery query;
    QString database_name= usernameL;
    QString table_name= database_name+"_password_data";
    QString algorithm_type;
    uint8_t salt[SALTLEN];
    generateRandomSalt(salt, SALTLEN);
    uint32_t pwdlen = passwordM.size();
    uint32_t t_cost = 2;            // 2-pass computation
    uint32_t m_cost = (1<<16);      // 64 mebibytes memory usage
    uint32_t parallelism = 1;       // number of threads and lanes
    char encodedPasswordHash[HASHLEN*8 +1];
    std::string passwordString = passwordM.toStdString();
    const char* passwordCString = passwordString.c_str();
    algorithm_type=ui->comboBox->currentText();
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
                                                                                              "PRIMARY KEY (`ID`))");
                // Execute the prepared statement
                if (query.exec()) {
                    argon2id_hash_encoded(t_cost, m_cost, parallelism, passwordCString, pwdlen, salt, SALTLEN, HASHLEN, encodedPasswordHash, sizeof(encodedPasswordHash));
                    qDebug() << "argon2idHash:"<<encodedPasswordHash;
                    QString storedPasswordQString = QString::fromUtf8(encodedPasswordHash);
                    qDebug() << "Table created successfully";
                    query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password,Kdf_algorithm) VALUES (:username, :password, :algorithm_type)");
                    query.bindValue(":username", usernameL);
                    query.bindValue(":password", storedPasswordQString);
                    query.bindValue(":algorithm_type",algorithm_type);
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

void MainWindow::PBKDF2_KDF(){

    QSqlQuery query;
    QString database_name= usernameL;
    QString table_name= database_name+"_password_data";
    QString algorithm_type;
    uint32_t pwdlen = passwordM.size();
    uint8_t salt[SALTLEN];
    generateRandomSalt(salt, SALTLEN);
    std::string passwordString = passwordM.toStdString();
    const char* passwordCString = passwordString.c_str();
    algorithm_type=ui->comboBox->currentText();
        if (!dataBase.isOpen()) {
            qDebug() << "Error: Failed to open database:" << dataBase.lastError().text();
            return;
        }
        else {
            qDebug() << "Opened database:" << dataBase.lastError().text();
            query.prepare("CREATE TABLE IF NOT EXISTS `passwordmanager`.`" + table_name + "` ("
                                                                                          "`ID` INT NOT NULL AUTO_INCREMENT,"
                                                                                          "`Name of APP` VARCHAR(48) NULL,"
                                                                                          "`Username` VARCHAR(48) NULL,"
                                                                                          "`Password` VARCHAR(64) NULL,"
                                                                                          "`URL` VARCHAR(512) NULL,"
                                                                                          "`log` DATETIME NULL,"
                                                                                          "PRIMARY KEY (`ID`))");
        }
        if (query.exec()) {
            int iterations=600000;
            unsigned char pwdout[HASHLEN];
            PKCS5_PBKDF2_HMAC(passwordCString, pwdlen, salt, SALTLEN, iterations,EVP_sha512(),HASHLEN, pwdout);
            QByteArray saltByteArray(reinterpret_cast<const char*>(salt), SALTLEN);
            qDebug() << "pbkdf2:";
            QString hashedPasswordString;
            for (int i = 0; i < HASHLEN; ++i) {
                hashedPasswordString.append(QString("%1").arg(pwdout[i], 2, 16, QLatin1Char('0')));
            }
            qDebug() << hashedPasswordString;
            query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password,Login_salt,Login_iterations,Login_SALTLEN,Login_HASHLEN,Kdf_algorithm)"
                          " VALUES (:username, :password, :login_salt, :login_iterations, :login_saltlen, :login_hashlen, :algorithm_type)");
            query.bindValue(":username", usernameL);
            query.bindValue(":password", hashedPasswordString);
            query.bindValue(":login_iterations", iterations);
            query.bindValue(":login_salt", saltByteArray);
            query.bindValue(":login_saltlen", SALTLEN);
            query.bindValue(":login_hashlen", HASHLEN);
            query.bindValue(":algorithm_type",algorithm_type);
        }
        if(query.exec()){
            qDebug() << "Insert created successfully";
            ui->stackedWidget->setCurrentIndex(0);
        }
        else{
            qDebug() << "Error: " << query.lastError().text();
        }
}

void MainWindow::Scrypt_KDF(){

    QSqlQuery query;
    QString database_name= usernameL;
    QString table_name= database_name+"_password_data";
    QString algorithm_type;
    uint8_t salt[SALTLEN];
    generateRandomSalt(salt, SALTLEN);
    uint32_t pwdlen = passwordM.size();
    // Define named constants for parameters
    constexpr uint64_t N_ITERATIONS = 2048;
    constexpr uint64_t BLOCK_SIZE = 1;
    constexpr uint64_t PARALLELISM_FACTOR = 1;
    constexpr uint64_t MAX_MEMORY = 1024 * 1024;
    const uint64_t N = N_ITERATIONS;
    const uint64_t r = BLOCK_SIZE;
    const uint64_t p = PARALLELISM_FACTOR;
    const uint64_t maxmemory = MAX_MEMORY;
    unsigned char pwdout[HASHLEN];
    std::string passwordString = passwordM.toStdString();
    const char* passwordCString = passwordString.c_str();
    algorithm_type=ui->comboBox->currentText();
    int result=0;
     result = EVP_PBE_scrypt(passwordCString, pwdlen, salt, SALTLEN, N, r, p, maxmemory, pwdout, HASHLEN);
    if (result != 1) {
        qDebug()<<ERR_get_error();
        qDebug() << "Error: EVP_PBE_scrypt failed with code" << result;
        return ;
    }
    QByteArray saltByteArray(reinterpret_cast<const char*>(salt), SALTLEN);
    QString hashedPasswordString;
    for (int i = 0; i < HASHLEN; ++i) {
        hashedPasswordString.append(QString("%1").arg(pwdout[i], 2, 16, QLatin1Char('0')));
    }
    qDebug()<<"hasshed string"<< hashedPasswordString;
    query.prepare("CREATE TABLE IF NOT EXISTS `passwordmanager`.`" + table_name + "` ("
                                                                                  "`ID` INT NOT NULL AUTO_INCREMENT,"
                                                                                  "`Name of APP` VARCHAR(48) NULL,"
                                                                                  "`Username` VARCHAR(48) NULL,"
                                                                                  "`Password` VARCHAR(64) NULL,"
                                                                                  "`URL` VARCHAR(512) NULL,"
                                                                                  "`log` DATETIME NULL,"
                                                                                  "PRIMARY KEY (`ID`))");

    if (query.exec()) {
        query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password, Login_salt, Login_iterations, Login_r, Login_p, Login_maxmemory, Login_SALTLEN, Login_HASHLEN, Kdf_algorithm)"
                      " VALUES (:username, :password, :login_salt, :login_iterations, :login_r, :login_p, :login_maxmemory, :login_saltlen, :login_hashlen, :algorithm_type)");
        query.bindValue(":username", usernameL);
        query.bindValue(":password", hashedPasswordString);
        query.bindValue(":login_salt", saltByteArray);
        query.bindValue(":login_iterations", N);
        query.bindValue(":login_r", r);
        query.bindValue(":login_p", p);
        query.bindValue(":login_maxmemory", maxmemory);
        query.bindValue(":login_saltlen", SALTLEN);
        query.bindValue(":login_hashlen", HASHLEN);
        query.bindValue(":algorithm_type", algorithm_type);

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
}
