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
    explicit Dialog_client_site(const QString &login_name, QWidget *parent = nullptr);
    ~Dialog_client_site();

private slots:



    void on_generate_client_crt_clicked();

    void on_table_export_clicked();

    void on_send_file_button_clicked();


    void on_close_Button_clicked();

private:
    Ui::Dialog_client_site *ui;
    QString login_name;
    void logging(QString logType);
};

#endif // DIALOG_CLIENT_SITE_H
