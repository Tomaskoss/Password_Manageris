#ifndef DIALOG_SERVER_H
#define DIALOG_SERVER_H

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
    explicit Dialog_server(const QString &login_name, QWidget *parent = nullptr);
    ~Dialog_server();

private slots:

    void on_close_Button_clicked();

    void on_start_server_clicked();

    void on_generate_crt_clicked();

    void on_load_data_clicked();

private:
    bool loadSslConfig();
    QString login_name;
    Ui::Dialog_server *ui;


};



#endif // DIALOG_SERVER_H
