#ifndef DIALOG_SERVER_H
#define DIALOG_SERVER_H

#include "qsslconfiguration.h"
#include <QDialog>
#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>


namespace Ui {
class Dialog_server;
}

class Dialog_server : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_server(QWidget *parent = nullptr);
    ~Dialog_server();

private slots:
    void onNewConnection();
    void onSslEncrypted();
    void onSslErrors(const QList<QSslError> &errors);


    void on_close_Button_clicked();

private:
    bool loadSslConfig();

    Ui::Dialog_server *ui;
    QTcpServer *server;
    QSslConfiguration sslConfiguration; // Declare sslConfiguration here


};



#endif // DIALOG_SERVER_H
