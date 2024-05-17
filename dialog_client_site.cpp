#include "dialog_client_site.h"
#include "ui_dialog_client_site.h"
#include <QSslSocket> // Include QSslSocket for SSL/TLS support
#include <QSslKey>
#include <QSslCertificate>
#include <QFileDialog>
#include <QMessageBox>

Dialog_client_site::Dialog_client_site(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_client_site)
{
    ui->setupUi(this);
    ui->send_file_button->hide();

    // Instantiate clientSocket
    clientSocket = new QSslSocket(this); // Assuming QSslSocket is the correct type
    if (!clientSocket) {
        qDebug() << "Failed to create clientSocket instance.";
        return; // or handle the error appropriately
    }

    // // Connect UI buttons to slots
     connect(ui->connect_button, &QPushButton::clicked, this, &::Dialog_client_site::on_connect_button_clicked);
     connect(ui->send_file_button, &QPushButton::clicked, this, &::Dialog_client_site::on_send_file_button_clicked);

    // Connect signals to slots
    connect(clientSocket, &QSslSocket::encrypted, this, &Dialog_client_site::onSslEncrypted);
     connect(clientSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
             this, &Dialog_client_site::onSslErrors);
     connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred),
             this, &Dialog_client_site::onError);


    // Load SSL configuration
    if (!loadSslConfig()) {
        qDebug() << "Failed to load SSL configuration.";
    }
}

Dialog_client_site::~Dialog_client_site()
{
    delete ui;
}

bool Dialog_client_site::loadSslConfig()
{
    // Load the client's certificate (if needed)
    QFile certFile("D:/Password_Manageris/libs/p2p/client.crt");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open certificate file.";
        return false;
    }
    QSslCertificate certificate(&certFile);
    certFile.close();

    // Load the client's private key (if needed)
    QFile keyFile("D:/Password_Manageris/libs/p2p/clientkey.key");
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open key file.";
        return false;
    }
    QSslKey privateKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    keyFile.close();

    // Set the SSL configuration
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(privateKey);
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyPeer); // Adjust as needed
    sslConfiguration.setProtocol(QSsl::TlsV1_2);

    return true;
}



void Dialog_client_site::on_connect_button_clicked()
{
    QString serverAddress = ui->line_sever_ip->text();
    quint16 serverPort = 1234;

    clientSocket->setSslConfiguration(sslConfiguration);
    clientSocket->connectToHostEncrypted(serverAddress, serverPort);

    if (!clientSocket->waitForEncrypted()) {
        QMessageBox::critical(this, "Connection Error", "Could not establish SSL connection: " + clientSocket->errorString());
    } else {
        qDebug() << "SSL connection established.";
    }
}


void Dialog_client_site::on_send_file_button_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File to Send");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "File Error", "Could not open file for reading.");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    clientSocket->write(fileData);
    clientSocket->flush();
}

void Dialog_client_site::onSslEncrypted()
{
    qDebug() << "SSL handshake completed. Ready to send data.";
}

void Dialog_client_site::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "SSL errors occurred:";
    for (const QSslError &error : errors) {
        qDebug() << error.errorString();
    }

    // You might want to ignore certain errors for testing purposes
    // clientSocket->ignoreSslErrors();
}
void Dialog_client_site::on_close_Button_clicked()
{
    close();
}

void Dialog_client_site::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error occurred:" << clientSocket->errorString();
}

