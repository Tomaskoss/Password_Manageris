#include "dialog_server.h"
#include "ui_dialog_server.h"
#include <QtNetwork>
#include <QFileDialog>


Dialog_server::Dialog_server(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Dialog_server),
    server(new QTcpServer(this))
{
    ui->setupUi(this);

    // Attempt to start the server
    if (!server->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server could not start! Error:" << server->errorString();
        // You might want to handle this error more gracefully, like disabling functionality or providing a message to the user.
    } else {
        qDebug() << "Server started!";
    }

    // Connect the newConnection signal to a slot
    connect(server, &QTcpServer::newConnection, this, &Dialog_server::onNewConnection);
}

Dialog_server::~Dialog_server()
{
    delete ui;
}

void Dialog_server::onNewConnection()
{
    // Handle new incoming connections here
    while (server->hasPendingConnections()) {
        QTcpSocket *clientSocket = server->nextPendingConnection();
        qDebug() << "New connection established with client:" << clientSocket->peerAddress().toString();

        // Read file data sent by the client
        QByteArray file_data;
        while (clientSocket->bytesAvailable() > 0) {
            file_data.append(clientSocket->readAll());
        }

        // Prompt user for file path
        QString save_path = getSaveFilePath();

        // Save the received file to disk
        QFile file(save_path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(file_data);
            file.close();
            qDebug() << "File received and saved successfully:" << save_path;
        } else {
            qDebug() << "Failed to open file for writing:" << file.errorString();
        }

        // Close the client connection
        clientSocket->close();
        clientSocket->deleteLater(); // Cleanup
    }
}

QString Dialog_server::getSaveFilePath()
{
    return QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath());
}

void Dialog_server::on_close_Button_clicked()
{
    // Stop the server
    server->close();

    // Close the dialog window
    close();
}

