#include "dialog_server.h"
#include "ui_dialog_server.h"
#include <QtNetwork>
#include <QFileDialog>

#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>
#include <QFile>

Dialog_server::Dialog_server(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Dialog_server)
{
    ui->setupUi(this);


}

Dialog_server::~Dialog_server()
{
    delete ui;
}



void Dialog_server::on_close_Button_clicked()
{
    close();
}


void Dialog_server::on_start_server_clicked()
{

    // Directory for the certificate and key files
    QString currentDir = "D:/Password_Manageris/crt";

    // Change the current directory
    if (QDir::setCurrent(currentDir)) {
        qDebug() << "Current directory changed to:" << currentDir;
    } else {
        qDebug() << "Failed to change current directory to:" << currentDir;
    }

    // Retrieve the current directory path
    QString currentDirPath = QDir::currentPath();
    qDebug() << "Current directory:" << currentDirPath;

    // Output directory for the JSON file
    QString outputDirectory = "D:/Password_Manageris/table/";

    // Command to start the OpenSSL server with specified certificate and key, and redirect output to Table_json.json
    QString command = "openssl s_server -cert " + currentDirPath + "/server-cert.pem -key " + currentDirPath + "/server-key.pem -accept 1234 -quiet > " + outputDirectory + "Table_json.json";

    // Execute the command in the system shell
    int exitCode = system(command.toStdString().c_str());

    // Check if the command ran successfully
    if (exitCode == 0) {
        qDebug() << "Server was started successfully.";
    } else {
        qDebug() << "Error starting the server.";
    }
}


void Dialog_server::on_generate_crt_clicked()
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
    QString command = "openssl req -x509 -newkey rsa:4096 -keyout " + currentDir + "/server-key.pem -out " + currentDir + "/server-cert.pem -nodes -days 365";

    // Spuštění příkazu v systémovém shellu
    int exitCode = system(command.toStdString().c_str());

    // Ověření, zda příkaz proběhl bez chyb
    if (exitCode == 0) {
        qDebug() << "Certifikát klienta byl úspěšně vygenerován.";
    } else {
        qDebug() << "Chyba při generování certifikátu klienta.";
    }
}

