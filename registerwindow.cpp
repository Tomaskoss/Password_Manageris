#include "registerwindow.h"
#include "qregularexpression.h"
#include "qvalidator.h"
#include "ui_registerwindow.h"
#include "encryptindialog.h"
RegisterWindow::RegisterWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::on_Back_Button_clicked()
{
    close();
    parentWidget()->show();
}


void RegisterWindow::on_SignUp_Button_clicked()
{

    QString login_name          = ui->line_login->text();
    QString master_password     = ui->line_master_password->text();
    QString confirm_password    = ui->line_confirm_password->text();
    QString email               = ui->Email->text();
    if(login_name.length()>3 && master_password.length()>16 && confirm_password.length()>16 && email.length()>4 && isValidEmail(email)){
        if(master_password==confirm_password){
            EncryptinDialog *encryptindialog = new EncryptinDialog(this);
            encryptindialog->show();
        }
        else{
            ui->status_message->setText("Passwords are not matching");
        }

    }
    else if (login_name.length()<3){
        ui->status_message->setText("Login name is short");
    }
    else if (master_password.length()<16){
        ui->status_message->setText("master password is short");
    }
    else if (confirm_password.length()<16){
        ui->status_message->setText("confirm password is short");
    }
    else if (email.length()<4){
        ui->status_message->setText("Email name is short");
    }
    else if(!isValidEmail(email)){
        ui->status_message->setText("Wrong email format");
    }



}
// Implementation of the email validation function
bool RegisterWindow::isValidEmail(QString &email) {
    QRegularExpression rx("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", QRegularExpression::CaseInsensitiveOption);
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    int pos = 0;
    return validator->validate(email, pos) == QValidator::Acceptable;
}

