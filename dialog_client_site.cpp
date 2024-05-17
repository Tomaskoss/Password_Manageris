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
#include <fstream>
#include <QProcess>



#include <fstream>
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
    QString certificateDir = "D:/Password_Manageris/crt";
    if (QDir::setCurrent(certificateDir)) {
        qDebug() << "Current directory changed to:" << certificateDir;
    } else {
        qDebug() << "Failed to change current directory to:" << certificateDir;
    }

    // Get the current directory path
    QString currentDir = QDir::currentPath();
    qDebug() << "Current directory:" << currentDir;

    // File to send
    QString fileToSend = "Table_export.json";

    // Command to send the file to the server using OpenSSL client with specified IP, port, and certificates
    QString command = "openssl s_client -connect " + ip_address_server + ":" + QString::number(port) + " -cert " + currentDir + "/server-cert.pem -key " + currentDir + "/server-key.pem < " + fileToSend;

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
    QString newDir = "D:/Password_Manageris/crt";
    if (QDir::setCurrent(newDir)) {
        qDebug() << "Current directory changed to:" << newDir;
    } else {
        qDebug() << "Failed to change current directory to:" << newDir;
    }
    // Získání aktuálního pracovního adresáře
    QString currentDir = QDir::currentPath();
    qDebug()<<"current dir:"<<currentDir;
    // Příkaz pro generování certifikátu pomocí OpenSSL
    QString command = "openssl req -x509 -newkey rsa:4096 -keyout " + currentDir + "/client-key.pem -out " + currentDir + "/client-cert.pem -nodes -days 365";

    // Spuštění příkazu v systémovém shellu
    int exitCode = system(command.toStdString().c_str());

    // Ověření, zda příkaz proběhl bez chyb
    if (exitCode == 0) {
        qDebug() << "Certifikát klienta byl úspěšně vygenerován.";
    } else {
        qDebug() << "Chyba při generování certifikátu klienta.";
    }


}


void Dialog_client_site::on_table_export_clicked()
{
    // Assuming you have already established a valid MySQL connection named 'db_connection'
    // and you have the login_name variable storing the user's login name
    // Assuming line_login_name is where the user inputs their login name

    QString queryString = "SELECT `ID`, `Name of APP`, `Username`, `Password`, `URL`, `LOG` FROM `" + login_name + "_password_data`;";
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
        jsonObject["LOG"] = query.value("LOG").toString();
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
        } else {
            QMessageBox::critical(this, "Error", "Failed to open file for writing.");
        }
    }
}


void Dialog_client_site::on_close_Button_clicked()
{
    close();
}

