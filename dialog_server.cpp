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
    ui(new Ui::Dialog_server),
    server(new QTcpServer(this))
{
    ui->setupUi(this);

    // Connect server signals to slots
    connect(server, &QTcpServer::newConnection, this, &Dialog_server::onNewConnection);

    // Load SSL configuration
    if (loadSslConfig()) {
        // Start the server on a specified port (e.g., 1234)
        if (server->listen(QHostAddress::Any, 1234)) {
            qDebug() << "Server started on port 1234.";
        } else {
            qDebug() << "Failed to start server: " << server->errorString();
        }
    } else {
        qDebug() << "Failed to load SSL configuration.";
    }
}

Dialog_server::~Dialog_server()
{
    delete ui;
}

bool Dialog_server::loadSslConfig()
{
    // Load the server's certificate
    QFile certFile("D:/Password_Manageris/libs/p2p/server.crt");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open certificate file.";
        return false;
    }
    QSslCertificate certificate(&certFile);
    certFile.close();

    // Load the server's private key
    QFile keyFile("D:/Password_Manageris/libs/p2p/privatekey.key");
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open key file.";
        return false;
    }
    QSslKey privateKey(&keyFile, QSsl::Rsa);
    keyFile.close();

    // Set the SSL configuration
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(privateKey);
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone); // Adjust as needed
    sslConfiguration.setProtocol(QSsl::TlsV1_2);

    return true;

}

void Dialog_server::onNewConnection()
{
    QTcpSocket *clientConnection = server->nextPendingConnection();
    QSslSocket *sslSocket = qobject_cast<QSslSocket *>(clientConnection);

    if (sslSocket) {
        // Set the SSL configuration for the socket
        sslSocket->setSslConfiguration(sslConfiguration);

        // Connect signals to handle SSL events
        connect(sslSocket, &QSslSocket::encrypted, this, &Dialog_server::onSslEncrypted);
        connect(sslSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
                this, &Dialog_server::onSslErrors);

        // Start the SSL handshake
        sslSocket->startServerEncryption();
    } else {
        // Handle non-SSL connection (if necessary)
        qDebug() << "Received a non-SSL connection.";
    }
}

void Dialog_server::onSslEncrypted()
{
    QSslSocket *sslSocket = qobject_cast<QSslSocket *>(sender());
    if (sslSocket) {
        qDebug() << "SSL handshake completed.";
        // Now you can communicate securely with sslSocket
    }
}

void Dialog_server::onSslErrors(const QList<QSslError> &errors)
{
    QSslSocket *sslSocket = qobject_cast<QSslSocket *>(sender());
    if (sslSocket) {
        qDebug() << "SSL errors occurred:";
        for (const QSslError &error : errors) {
            qDebug() << error.errorString();
        }

        // You might want to ignore certain errors for testing purposes
        // sslSocket->ignoreSslErrors();
    }
}

void Dialog_server::on_close_Button_clicked()
{
    close();
}

