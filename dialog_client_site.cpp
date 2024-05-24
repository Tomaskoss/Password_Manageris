#include "dialog_client_site.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_dialog_client_site.h"

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>



Dialog_client_site::Dialog_client_site(const QString &login_name,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_client_site),
    login_name(login_name)
{
    ui->setupUi(this);
    SSL_library_init();
    SSL_load_error_strings();
}

Dialog_client_site::~Dialog_client_site()
{
    delete ui;
    ERR_free_strings();
    EVP_cleanup();
}



void Dialog_client_site::on_send_file_button_clicked()
{
    // Get the server IP address and port
    QString ip_address_server = ui->line_server_ip->text();
    int port = 1234;

    // Set the current directory to the certificate directory
    QString certificateDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + QDir::separator() + "crt";
    if (QDir::setCurrent(certificateDir)) {
        qDebug() << "Current directory changed to:" << certificateDir;
    } else {
        qDebug() << "Failed to change current directory to:" << certificateDir;
    }

    // Get the current directory path
    QString currentDir = QDir::currentPath();
    qDebug() << "Current directory:" << currentDir;

    // Let the user choose the file to send
    QString fileToSend = QFileDialog::getOpenFileName(this, "Select File to Send", QDir::homePath(), "JSON Files (*.json);;All Files (*)");
    if (fileToSend.isEmpty()) {
        qDebug() << "No file selected.";
        return;
    }

    // Command to send the file to the server using OpenSSL client with specified IP, port, and certificates
    QString command = QString("openssl s_client -connect %1:%2 -cert %3/server-cert.pem -key %3/server-key.pem < %4")
                          .arg(ip_address_server)
                          .arg(port)
                          .arg(currentDir)
                          .arg(fileToSend);

    // Execute the command in the system shell
    int exitCode = system(command.toStdString().c_str());

    // Check if the command ran successfully
    if (exitCode == 0) {
        qDebug() << "File was successfully sent to the server.";
    } else {
        qDebug() << "Error sending the file to the server.";
    }
}

void Dialog_client_site::on_generate_client_crt_clicked()
{
    // Set the current directory dynamically based on the application directory path
    QString newDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + QDir::separator() + "crt";

    // Check if the directory exists, and if not, create it
    QDir dir(newDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create directory:" << newDir;
            return;
        }
    }

    if (QDir::setCurrent(newDir)) {
        qDebug() << "Current directory changed to:" << newDir;
    } else {
        qDebug() << "Failed to change current directory to:" << newDir;
        return;
    }

    QString currentDir = QDir::currentPath();
    qDebug() << "Current directory:" << currentDir;

    // Command to generate the client certificate using OpenSSL
    QString command = QString("openssl req -x509 -newkey rsa:4096 -keyout %1/client-key.pem -out %1/client-cert.pem -nodes -days 365 -subj /CN=netpass")
                          .arg(currentDir);

    // Execute the command in the system shell
    int exitCode = system(command.toStdString().c_str());

    // Check if the command ran successfully
    if (exitCode == 0) {
        qDebug() << "Client certificate was successfully generated.";
    } else {
        qDebug() << "Error generating client certificate.";
    }
}

void Dialog_client_site::on_table_export_clicked()
{

    QString queryString = "SELECT `ID`, `Name of APP`, `Username`, `Password`, `URL`, `log`, `IV`, `Tag` FROM `" + login_name + "_password_data`;";

    QSqlQuery query;
    if (!query.exec(queryString)) {
        QMessageBox::critical(this, "Error", "Failed to execute query: " + query.lastError().text());
        return;
    }

    // Construct JSON object
    QJsonArray jsonArray;
    while (query.next()) {
        QJsonObject jsonObject;
        jsonObject["ID"] = query.value("ID").toString();
        jsonObject["Name of APP"] = query.value("Name of APP").toString();
        jsonObject["Username"] = query.value("Username").toString();
        jsonObject["Password"] = query.value("Password").toString();
        jsonObject["URL"] = query.value("URL").toString();
        jsonObject["log"] = query.value("log").toString();
        jsonObject["IV"] = query.value("IV").toString();
        jsonObject["Tag"] = query.value("Tag").toString();
        jsonArray.append(jsonObject);
    }

    // Serialize JSON object to string
    QJsonDocument jsonDoc(jsonArray);
    QByteArray jsonData = jsonDoc.toJson();

    // Export data to file
    QString filePath = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(jsonData);
            file.close();
            QMessageBox::information(this, "Success", "User table exported successfully.");
            logging("Exportovanie dát");
        } else {
            QMessageBox::critical(this, "Error", "Failed to open file for writing.");
        }
    }
}

void Dialog_client_site::on_close_Button_clicked()
{
    close();
}

void Dialog_client_site::logging(QString logType){
    QSqlQuery query;

    // Get current timestamp
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString(Qt::ISODate);

    // Construct the INSERT query
    QString queryString = "INSERT INTO `passwordmanager`.`" + login_name + "_log_data` (timestamp, log) VALUES (:timestamp, :log)";
    query.prepare(queryString);
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":log", logType);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error inserting log for" << logType << ":" << query.lastError().text();
        return;
    }

    qDebug() << "Successfully logged '" << logType << "' action with timestamp" << timestamp;
}
