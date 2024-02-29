#include "managerwindow.h"
#include "ui_managerwindow.h"
#include <QSqlDatabase>
ManagerWindow::ManagerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ManagerWindow)
{
    ui->setupUi(this);
}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}

void ManagerWindow::on_tableView_activated(const QModelIndex &index)
{

}


void ManagerWindow::on_LogOut_Button_clicked()
{
    close();
    parentWidget()->show();
}

