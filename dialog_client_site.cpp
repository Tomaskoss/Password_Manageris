#include "dialog_client_site.h"
#include "ui_dialog_client_site.h"
#include <QtNetwork>
#include <QFileDialog>
Dialog_client_site::Dialog_client_site(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_client_site)
{
    ui->setupUi(this);
    ui->send_file_button->hide();
}

Dialog_client_site::~Dialog_client_site()
{
    delete ui;
}

void Dialog_client_site::on_connect_button_clicked()
{
    QString server_ip = ui->line_sever_ip->text();
    qDebug() << "Server IP:" << server_ip;

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Dialog_client_site::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Dialog_client_site::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Dialog_client_site::onReadyRead);
    //connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Dialog_client_site::onError);

    socket->connectToHost(server_ip, 1234); // Replace "server_address" with the actual server address
}

// Define slots to handle various events
void Dialog_client_site::onConnected() {
    qDebug() << "Connected to server!";
    ui->send_file_button->show();
    ui->connect_button->hide();
    ui->line_sever_ip->hide();
}

void Dialog_client_site::onDisconnected() {
    qDebug() << "Disconnected from server!";
}

void Dialog_client_site::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "Received data:" << data;
}

void Dialog_client_site::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socketError;
}


void Dialog_client_site::on_close_Button_clicked()
{
    // Disconnect from the server
    socket->disconnectFromHost();
    close();
}


void Dialog_client_site::on_send_file_button_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Select File to Send"), QDir::homePath());

    if (!file_path.isEmpty()) {
        QFile file(file_path);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open file for reading:" << file.errorString();
            return;
        }

        QByteArray file_data = file.readAll();
        file.close();

        qint64 bytes_written = socket->write(file_data);
        if (bytes_written == -1) {
            qDebug() << "Failed to send file data:" << socket->errorString();
            return;
        }

        qDebug() << "File sent successfully!";
    }
}

