#include "managerwindow.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_managerwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>

ManagerWindow::ManagerWindow(const QString &login_name, QWidget *parent)
    : QMainWindow(parent),
     ui(new Ui::ManagerWindow),
    login_name(login_name)

{

    ui->setupUi(this);
     this->model = new QSqlQueryModel();

    qDebug() << "login_name:"<<login_name;
    query.prepare("SELECT * FROM `passwordmanager`.`"+login_name+"_password_data`");
    qDebug() << "usernameL"<<login_name;
    qDebug() << "usernameL"<<login_name;
    if (query.exec()) {
        model->setQuery(std::move(query));
        refreshTable();

    } else {
        qDebug() << "Query execution error:" << query.lastError().text();
    }

}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}


void ManagerWindow::on_LogOut_Button_clicked()
{
    close();
    parentWidget()->show();
    delete this->model;
}


void ManagerWindow::on_actionAdd_triggered()
{
    // Prepare the SQL statement with placeholders
    QString queryString = "INSERT INTO `passwordmanager`.`" + login_name + "_password_data` "
                                                                           "(`Name of APP`, `Username`, `Password`, `URL`, `log`) "
                                                                           "VALUES (?, ?, ?, ?, NOW());";

    // Create a QSqlQuery object
    QSqlQuery query;

    // Prepare the query with the SQL statement
    if (query.prepare(queryString)) {
        // Bind values to placeholders
        query.addBindValue("Bugisoft"); // Example value for application name
        query.addBindValue("Test");     // Example value for username
        query.addBindValue("Test123");  // Example value for password
        query.addBindValue("https://www.ubisoft.com/en-us/"); // Example value for URL

        // Execute the prepared statement
        if (query.exec()) {
            // If insertion is successful, refresh the table
            refreshTable();
        } else {
            // Handle query execution error
            qDebug() << "Query execution error:" << query.lastError().text();
        }
    } else {
        // Handle query preparation error
        qDebug() << "Query preparation error:" << query.lastError().text();
    }
}

void ManagerWindow::refreshTable() {
    // Delete previous model to avoid memory leaks
    delete model;
    model = nullptr;

    // Create a new QSqlQueryModel instance
    model = new QSqlQueryModel();

    if (model) {
        // Use a prepared statement for the query
        QSqlQuery query;
        QString queryString = "SELECT `Name of APP`, `Username`, `Password`, `URL`, `LOG` FROM `passwordmanager`.`" + login_name + "_password_data`;";
        query.prepare(queryString);

        // Execute the prepared statement
        if (query.exec()) {
            // Pass the QSqlQuery object by move to setQuery
            model->setQuery(std::move(query));
            model->setHeaderData(0, Qt::Horizontal, tr("Application"));
            model->setHeaderData(1, Qt::Horizontal, tr("Username"));
            model->setHeaderData(2, Qt::Horizontal, tr("Password"));
            model->setHeaderData(3, Qt::Horizontal, tr("URL"));
            model->setHeaderData(4, Qt::Horizontal, tr("LOG"));
            // Set the model for the table view
            ui->tableView->setModel(model);

            // Set the width of the header to x pixels
            ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
            ui->tableView->horizontalHeader()->resizeSection(3, 350);
            ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
            ui->tableView->horizontalHeader()->resizeSection(2, 150);
        } else {
            qDebug() << "Query execution error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Failed to create QSqlQueryModel object.";
    }
}

void ManagerWindow::on_actionRemove_triggered()
{
     QSqlQuery query;
    // Use the selectedRowID variable to construct the SQL query for deletion
    QString queryString = "DELETE FROM `passwordmanager`.`" + login_name + "_password_data` WHERE (`ID` = ?)";
    // QString queryString = "SELECT `Name of APP`, `Username`, `Password`, `URL`, `LOG` FROM `passwordmanager`.`" + login_name + "_password_data`;";
    // Prepare the query with the SQL statement
    if (query.prepare(queryString)) {
        // Bind the selectedRowID value to the placeholder
        query.addBindValue(selectedRowID.toInt()); // Assuming selectedRowID is a QString

        // Execute the prepared statement
        if (query.exec()) {
            qDebug() << "Row with ID" << selectedRowID << "removed successfully.";
            refreshTable();
            // Optionally, refresh the table or update the view after removal
        } else {
            qDebug() << "Error removing row:" << query.lastError().text();
        }
    } else {
        qDebug() << "Query preparation error:" << query.lastError().text();
    }
}

void ManagerWindow::on_tableView_clicked(const QModelIndex &index)
{
    // Retrieve the ID of the row
    selectedRowID = QString::number(index.row() + 1); // Add 1 to row index to make it 1-based ID
    qDebug() << "Row ID:" << selectedRowID;
    qDebug() <<login_name;
}

