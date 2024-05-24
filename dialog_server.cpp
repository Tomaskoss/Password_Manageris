#include "dialog_server.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_dialog_server.h"
#include <QtNetwork>
#include <QFileDialog>
#include <QMessageBox>
#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>
#include <QFile>

Dialog_server::Dialog_server(const QString &login_name,QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Dialog_server),
    login_name(login_name)
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
    QString currentDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + QDir::separator() + "crt";

    // Check if the directory exists, and if not, create it
    QDir dir(currentDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create directory:" << currentDir;
            return;
        }
    }
    // Change the current directory
    if (QDir::setCurrent(currentDir)) {
        qDebug() << "Current directory changed to:" << currentDir;
    } else {
        qDebug() << "Failed to change current directory to:" << currentDir;
        return;
    }

    // Retrieve the current directory path
    QString currentDirPath = QDir::currentPath();
    qDebug() << "Current directory:" << currentDirPath;

    // Output directory for the JSON file
    QString outputDirectory = QString("%1%2Table_export.json").arg(currentDirPath, QDir::separator());

    // Command to start the OpenSSL server with specified certificate and key, and redirect output to Table_json.json
    QString command = QString("openssl s_server -cert %1/server-cert.pem -key %1/server-key.pem -accept 1234 -quiet > %2").arg(currentDirPath).arg(outputDirectory);

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
    // Directory for the certificate and key files
    QString currentDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + QDir::separator() + "crt";

    // Check if the directory exists, and if not, create it
    QDir dir(currentDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create directory:" << currentDir;
            return;
        }
    }

    if (QDir::setCurrent(currentDir)) {
        qDebug() << "Current directory changed to:" << currentDir;
    } else {
        qDebug() << "Failed to change current directory to:" << currentDir;
        return;
    }

    // Retrieve the current directory path
    QString currentDirPath = QDir::currentPath();
    qDebug() << "current dir:" << currentDirPath;

    // Constructing the command for generating the certificate
    QString command = QString("openssl req -x509 -newkey rsa:4096 -keyout %1/server-key.pem -out %1/server-cert.pem -nodes -days 365 -subj /CN=netpass").arg(currentDirPath);

    int exitCode = system(command.toStdString().c_str());

    // Checking if the command ran successfully
    if (exitCode == 0) {
        qDebug() << "Certificate generation successful.";
    } else {
        qDebug() << "Error generating certificate.";
    }
}

void Dialog_server::on_load_data_clicked()
{
    // Read data from JSON file
    QString filePath = QFileDialog::getOpenFileName(this, "Open JSON File", QDir::homePath(), "JSON Files (*.json)");
    if (filePath.isEmpty()) {
        qDebug() << "No file selected.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Failed to open JSON file: " + file.errorString());
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (!jsonDoc.isArray()) {
        QMessageBox::critical(this, "Error", "Invalid JSON data format.");
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    // Connect to the database
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Error", "Database connection not open.");
        return;
    }

    // Truncate the table
    QString truncateQuery = "TRUNCATE TABLE `passwordmanager`.`" + login_name + "_password_data`";
    QSqlQuery truncateSqlQuery;
    if (!truncateSqlQuery.exec(truncateQuery)) {
        QMessageBox::critical(this, "Error", "Failed to truncate table: " + truncateSqlQuery.lastError().text());
        return;
    }

    // Insert data into the table
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO `passwordmanager`.`" + login_name + "_password_data` (`ID`, `Name of APP`, `Username`, `Password`, `URL`, `log`, `IV`, `Tag`) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    foreach (const QJsonValue &value, jsonArray) {
        QJsonObject obj = value.toObject();
        insertQuery.addBindValue(obj["ID"].toString());
        insertQuery.addBindValue(obj["Name of APP"].toString());
        insertQuery.addBindValue(obj["Username"].toString());
        insertQuery.addBindValue(obj["Password"].toString());
        insertQuery.addBindValue(obj["URL"].toString());
        insertQuery.addBindValue(obj["log"].toString());
        insertQuery.addBindValue(obj["IV"].toString());
        insertQuery.addBindValue(obj["Tag"].toString());

        if (!insertQuery.exec()) {
            QMessageBox::critical(this, "Error", "Failed to insert data into table: " + insertQuery.lastError().text());
            return;
        }
    }

    QMessageBox::information(this, "Success", "Data loaded successfully.");
    logging("Loaded Data");
}

void Dialog_server::logging(QString logType){
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
