#ifndef ENCRYPTINDIALOG_H
#define ENCRYPTINDIALOG_H
#include <argon2.h>
#include <QDialog>
namespace Ui {
class EncryptinDialog;
}

class EncryptinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EncryptinDialog(const QString &login_name,const QString &masterPassword,QWidget *parent = nullptr);
    ~EncryptinDialog();

private slots:
    void on_Quit_Button_clicked();

    void on_Back_Button_clicked();

    void on_Confirm_Button_clicked();



private:
    Ui::EncryptinDialog *ui;
    QString passwordM;
    QString loginM;
};

#endif // ENCRYPTINDIALOG_H
