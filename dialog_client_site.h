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
    void on_connect_button_clicked();

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

    void on_close_Button_clicked();

    void on_send_file_button_clicked();

private:
    Ui::Dialog_client_site *ui;
    QTcpSocket *socket;
};

#endif // DIALOG_CLIENT_SITE_H
