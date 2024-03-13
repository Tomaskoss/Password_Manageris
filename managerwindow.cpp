#include "managerwindow.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_managerwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>

ManagerWindow::ManagerWindow(const QString &login_name, QWidget *parent)
    : QMainWindow(parent),
     ui(new Ui::ManagerWindow)

{
    ui->setupUi(this);
     this->model = new QSqlQueryModel();
    QSqlQuery query;

    qDebug() << "login_name:"<<login_name;
    query.prepare("SELECT * FROM `passwordmanager`.`"+login_name+"_password_data`");
    qDebug() << "usernameL"<<login_name;
    if (query.exec()) {
        model->setQuery(std::move(query));

        this->model->setHeaderData(0,Qt::Horizontal,tr("Application"));
        this->model->setHeaderData(1,Qt::Horizontal,tr("Username"));
        this->model->setHeaderData(2,Qt::Horizontal,tr("Password"));
        this->model->setHeaderData(3,Qt::Horizontal,tr("URL"));
        this->model->setHeaderData(4,Qt::Horizontal,tr("LOG"));
        ui->tableView->setModel(model);

    } else {
        qDebug() << "Query execution error:" << query.lastError().text();
    }

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
    delete this->model;
}




void ManagerWindow::on_actionAdd_triggered()
{

}

