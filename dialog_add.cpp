#include "dialog_add.h"
#include "ui_dialog_add.h"

Dialog_ADD::Dialog_ADD(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_ADD)
{
    ui->setupUi(this);
}

Dialog_ADD::~Dialog_ADD()
{
    delete ui;
}

void Dialog_ADD::on_Back_Button_clicked()
{
    close();
    parentWidget()->show();

}

