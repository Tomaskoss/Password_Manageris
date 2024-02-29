#include "encryptindialog.h"
#include "ui_encryptindialog.h"
#include <argon2.h>
EncryptinDialog::EncryptinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EncryptinDialog)
{
    ui->setupUi(this);
}

EncryptinDialog::~EncryptinDialog()
{
    delete ui;
}

void EncryptinDialog::on_Quit_Button_clicked()
{
    close();
    parentWidget()->show();
}

