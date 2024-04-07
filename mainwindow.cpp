#include "mainwindow.h"
#include "managerwindow.h"
#include "./ui_mainwindow.h"
#include "qsqldatabase.h"
#include "argon2.h"
#include <iostream>
#include <stdio.h>


#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/hmac.h>

#include <libqotp/qotp.h>


// #include <include/smtpmime/smtpclient.h>
// #include <include/smtpmime/mimemessage.h>
// #include <include/smtpmime/mimetext.h>

#include <include/smtpmime/SmtpMime>

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
     usernameL= ui->line_username->text();
     passwordL = ui->line_password->text();
    size_t passwordL_lenght=passwordL.length();
    QByteArray storedSaltQString;
    int iterations=0;
    unsigned char generated_hash[HASHLEN];
    QString algorithm_type;
    QString Login_email;
    int r_BLOCK_SIZE,p_PARALLELISM_FACTOR ,MAX_MEMORY;


    if(usernameL.isEmpty() || passwordL.isEmpty())
    {
        ui->status_message_log->setText("Username or password is blank");
        return;
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM `passwordmanager`.`login_information` WHERE Login_name = :username");
    query.bindValue(":username", usernameL);
    if(query.exec()){
        if(query.next()){
         passwordM = query.value("Login_master_password").toString();
         storedSaltQString= query.value("Login_salt").toByteArray();
         iterations = query.value("Login_iterations").toInt();
         algorithm_type = query.value("Kdf_algorithm").toString();
         r_BLOCK_SIZE = query.value("Login_r").toInt();
         p_PARALLELISM_FACTOR = query.value("Login_p").toInt();
         MAX_MEMORY= query.value("Login_maxmemory").toInt();
         Login_email=query.value("Login_email").toString();
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
    QByteArray saltData = storedSaltQString;

    const unsigned char *salt = reinterpret_cast<const unsigned char *>(saltData.constData());
    if(query.exec() && query.next())
    {
        if(algorithm_type=="Argon2id"){
           int verificationResult =argon2id_verify(passwordM_char,passwordL_char,passwordL_lenght);
            if (verificationResult == ARGON2_OK)
            {
                // Passwords match
                qDebug() << "Login successful";
                 GenerateTOTP(Login_email);
                 ui->stackedWidget->setCurrentIndex(3);


               // CreateManagerWindow();
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
                     GenerateTOTP(Login_email);
                     ui->stackedWidget->setCurrentIndex(3);
                     //CreateManagerWindow();
                    qDebug()<<"variables "<<usernameL<<passwordM_char<<passwordL_char<<passwordL_str<<passwordM_str<<salt<<generated_hash<<iterations;
                }
                else{
                    qDebug()<<"Wrong password";
                }
        }else if (algorithm_type=="Scrypt"){
              int result = EVP_PBE_scrypt(passwordL_char, passwordL_lenght, salt, SALTLEN, iterations, r_BLOCK_SIZE, p_PARALLELISM_FACTOR, MAX_MEMORY, generated_hash, HASHLEN);
             QString generatedHashStr = QByteArray(reinterpret_cast<const char*>(generated_hash), HASHLEN).toHex();
              if (result !=1){
                 qDebug()<<"EVP_PBE_scrypt Failed to proceed";
                  return;}
              else {
                    if(generatedHashStr==passwordM_char){
                         // Passwords match
                         qDebug() << "Login successful";
                          GenerateTOTP(Login_email);
                         //CreateManagerWindow();
                          ui->stackedWidget->setCurrentIndex(3);
                         qDebug()<<"variables "<<usernameL<<passwordM_char<<passwordL_char<<passwordL_str<<passwordM_str<<salt<<generated_hash<<iterations;

                    }
                    else{
                        qDebug()<<"Wrong password";
            }   }
        }
    }
    qDebug()<<"sdasda";

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


void MainWindow::registerUser(){
    QSqlQuery query;
    usernameL   = ui->line_login->text();
    passwordM   = ui->line_master_password->text();
    QString confirm_password    = ui->line_confirm_password->text();
     email               = ui->Email->text();
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
                    ui->line_login->clear();
                    ui->line_confirm_password->clear();
                    ui->line_master_password->clear();
                    ui->line_login->clear();
                    ui->Email->clear();

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
    else if (passwordM.length()<=16){
        ui->status_message_reg->setText("master password is short");
    }
    else if (confirm_password.length()<=16){
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

    if (RAND_bytes(salt, saltSize) != 1) {
        // Handle error: unable to generate random bytes
        // You can choose to throw an exception, log an error, or exit the program
        // based on your application's error handling strategy.
        // Here, we'll just print an error message to the console.
        std::cerr << "Error generating random bytes." << std::endl;
        exit(EXIT_FAILURE);
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
                qDebug()<<email<<":email";
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
                    query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password,Kdf_algorithm,Login_email) VALUES (:username, :password, :algorithm_type, :Login_email)");
                    query.bindValue(":username", usernameL);
                    query.bindValue(":password", storedPasswordQString);
                    query.bindValue(":algorithm_type",algorithm_type);
                    query.bindValue(":Login_email",email);
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
            query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password,Login_salt,Login_iterations,Login_SALTLEN,Login_HASHLEN,Kdf_algorithm,Login_email)"
                          " VALUES (:username, :password, :login_salt, :login_iterations, :login_saltlen, :login_hashlen, :algorithm_type, :Login_email)");
            query.bindValue(":username", usernameL);
            query.bindValue(":password", hashedPasswordString);
            query.bindValue(":login_iterations", iterations);
            query.bindValue(":login_salt", saltByteArray);
            query.bindValue(":login_saltlen", SALTLEN);
            query.bindValue(":login_hashlen", HASHLEN);
            query.bindValue(":algorithm_type",algorithm_type);
            query.bindValue(":Login_email",email);
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
        query.prepare("INSERT INTO `passwordmanager`.`login_information` (Login_name, Login_master_password, Login_salt, Login_iterations, Login_r, Login_p, Login_maxmemory, Login_SALTLEN, Login_HASHLEN, Kdf_algorithm,Login_email)"
                      " VALUES (:username, :password, :login_salt, :login_iterations, :login_r, :login_p, :login_maxmemory, :login_saltlen, :login_hashlen, :algorithm_type, :Login_email)");
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
        query.bindValue(":Login_email",email);
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

void MainWindow::GenerateTOTP(QString Login_email){
    const int secretLength = 20; // Adjust the length as needed
    QByteArray secret;
    QSqlQuery query;

    // Check if the secret key has already been generated and stored for the user
    query.prepare("SELECT Shared_secret FROM login_information WHERE Login_name = :Login_name");
    query.bindValue(":Login_name", usernameL);
    if (query.exec()) {
        if (query.next()) {
            // Check if the query returned any rows
            secret = query.value(0).toByteArray();
            if (!secret.isEmpty()) {
                // Secret key already exists for the user
                qDebug() << "Secret key loaded from database";
            } else {
                // Secret key is NULL, generate a new one
                if (!generateRandomSecret(secret, secretLength)) {
                    qDebug() << "Error generating secret key";
                    return;
                }

                // Update the existing record in the database
                query.prepare("UPDATE login_information SET Shared_secret = :Shared_secret, Shared_secret_string = :Shared_secret_string WHERE Login_name = :Login_name");
                query.bindValue(":Shared_secret", secret);
                query.bindValue(":Shared_secret_string", base32Encode(secret));
                query.bindValue(":Login_name", usernameL);
                if (!query.exec()) {
                    qDebug() << "Error updating secret key in the database:" << query.lastError().text();
                    return;
                }
            }
        } else {
            // Secret key does not exist, generate a new one
            if (!generateRandomSecret(secret, secretLength)) {
                qDebug() << "Error generating secret key";
                return;
            }

            // Insert a new record into the database
            query.prepare("INSERT INTO login_information (Login_name, Shared_secret, Shared_secret_string) VALUES (:Login_name, :Shared_secret, :Shared_secret_string)");
            query.bindValue(":Login_name", usernameL);
            query.bindValue(":Shared_secret", secret);
            query.bindValue(":Shared_secret_string", base32Encode(secret));
            if (!query.exec()) {
                qDebug() << "Error inserting secret key into the database:" << query.lastError().text();
                return;
            }
        }
    } else {
        qDebug() << "Error executing SQL query:" << query.lastError().text();
        return;
    }

    // Generate TOTP
    quint64 currentUnixTime = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    unsigned int timeStep = 30;
    unsigned int digits = 6;
    unsigned int digitMinimum = 6;
    unsigned int digitMaximum = 6;
    quint64 epoch = 0;
    generatedTOTP = libqotp::totp(secret, currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum);

    // Send TOTP via email
    qDebug()<<"generated totp:"<<generatedTOTP;
   // sendEmail(generatedTOTP, Login_email);
}

void MainWindow::sendEmail(const QString &totp, const QString &recipientEmail) {

    // Now we create a MimeMessage object. This is the email.
    MimeMessage message;

    EmailAddress sender("netbk.pass@gmail.com", "Password Manageris");
    message.setSender(sender);

    EmailAddress to(recipientEmail, "Recipient Name");
    message.addRecipient(to);

    message.setSubject("TOTP-PasswordManageris");

    // Now add some text to the email.
    // First we create a MimeText object.
    MimeText* text = new MimeText("Hi,\nThis is your TOTP password for login: " + totp + ".\n");


    // Now add it to the mail
    message.addPart(text);

    // Now we can send the mail
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    smtp.connectToHost();
    if (!smtp.waitForReadyConnected()) {
        qDebug() << "Failed to connect to host!";
        return;
    }

    smtp.login("netbk.pass@gmail.com", "uqfz zajb nufm uqzy");
    if (!smtp.waitForAuthenticated()) {
        qDebug() << "Failed to login!";
        return;
    }

    smtp.sendMail(message);
    if (!smtp.waitForMailSent()) {
        qDebug() << "Failed to send mail!";
        return;
    }

    // smtp.disconnect();
    // qDebug()<<"discoected ";
    // // smtp.waitForDisconnected();
    // qDebug()<<"wait";
    smtp.quit();
        qDebug()<<"quit";
}

void MainWindow::CreateManagerWindow(){
    hide();
    ManagerWindow *managerWindow = new ManagerWindow(usernameL,this);
    managerWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure deletion on close
    managerWindow->setWindowFlags(Qt::Window); // Ensure appropriate window flags are set
    managerWindow->show();
    ui->line_password->clear();
    ui->line_username->clear();
}

void MainWindow::on_Back_To_Login_Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_Confirm_Button_OTP_clicked()
{

    OTP_login();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::OTP_login(){
    // Retrieve the TOTP generated for the user (assuming it's stored somewhere accessible)
    QString generatedTOTP = getGeneratedTOTP(); // Implement this function to retrieve the generated TOTP
    QString enteredTOTP= ui->otp_line->text();
    qDebug() << "Entered OTP:" << enteredTOTP;
    qDebug() << "Generated TOTP:" << generatedTOTP;
    libqotp::totp_expire_time();
    if (enteredTOTP == generatedTOTP) {
        CreateManagerWindow(); // OTP verification successful, proceed to manager window
    }
    else {
        ui->otp_label->setText("Wrong TOTP");
    }
}

bool MainWindow::generateRandomSecret(QByteArray &secret, int length) {
    secret.resize(length);
    if (RAND_bytes(reinterpret_cast<unsigned char *>(secret.data()), length) != 1) {
        return false; // Error generating random bytes
    }
    return true;
}

QString MainWindow::base32Encode(const QByteArray &data) {
    static const char *base32chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    QString result;
    int buffer = 0;
    int bitsRemaining = 0;

    for (char byte : data) {
        buffer = (buffer << 8) | (byte & 0xFF);
        bitsRemaining += 8;
        while (bitsRemaining >= 5) {
            result += base32chars[(buffer >> (bitsRemaining - 5)) & 0x1F];
            bitsRemaining -= 5;
        }
    }

    if (bitsRemaining > 0) {
        buffer <<= (5 - bitsRemaining);
        result += base32chars[buffer & 0x1F];
    }

    // Remove padding '=' characters
    while (result.endsWith(QLatin1Char('='))) {
        result.chop(1);
    }

    return result;
}
