#ifndef DIALOG_CLIENT_SITE_H
#define DIALOG_CLIENT_SITE_H

#include <QtNetwork>
#include <QDialog>

namespace Ui {
class Dialog_client_site;
}

class Dialog_client_site : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_client_site(QWidget *parent = nullptr);
    ~Dialog_client_site();

private slots:

    void onSslEncrypted();
    void onSslErrors(const QList<QSslError> &errors);
    void onError(QAbstractSocket::SocketError socketError);

    void on_connect_button_clicked();

    void on_send_file_button_clicked();

    void on_close_Button_clicked();

private:
    bool loadSslConfig();

    Ui::Dialog_client_site *ui;
    QSslSocket *clientSocket;
    QSslConfiguration sslConfiguration; // Declare sslConfiguration here
};

#endif // DIALOG_CLIENT_SITE_H
