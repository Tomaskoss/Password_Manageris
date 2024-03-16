#include "managerwindow.h"
#include "dialog_add.h"
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
    hide();
    Dialog_ADD *dialog_add = new Dialog_ADD();
    dialog_add->show();
    // Prepare the SQL statement with placeholders
    QString queryString = "INSERT INTO `passwordmanager`.`" + login_name + "_password_data` "
                                                                           "(`Name of APP`, `Username`, `Password`, `URL`, `log`) "
                                                                           "VALUES (?, ?, ?, ?, NOW());";
    // Create a QSqlQuery object
    QSqlQuery query;
    // Use the selectedRowID variable to construct the SQL query for deletion
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
            resetAutoIncrementAndReindex();
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
        QString queryString = "SELECT `ID`, `Name of APP`, `Username`, `Password`, `URL`, `LOG` FROM `passwordmanager`.`" + login_name + "_password_data`;";
        query.prepare(queryString);

        // Execute the prepared statement
        if (query.exec()) {
            // Pass the QSqlQuery object by move to setQuery
            model->setQuery(std::move(query));

            model->setHeaderData(1, Qt::Horizontal, tr("Application"));
            model->setHeaderData(2, Qt::Horizontal, tr("Username"));
            model->setHeaderData(3, Qt::Horizontal, tr("Password"));
            model->setHeaderData(4, Qt::Horizontal, tr("URL"));
            model->setHeaderData(5, Qt::Horizontal, tr("LOG"));
            // Set the model for the table view
            ui->tableView->setModel(model);
            // Hide the column containing the row IDs
            ui->tableView->hideColumn(0);
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
     QString deleteQueryString = "DELETE FROM `passwordmanager`.`" + login_name + "_password_data` WHERE (`ID` = ?)"; // Replace ColumnName with the actual name of the first column
    // Prepare the query with the SQL statement
    if (query.prepare(deleteQueryString)) {

        // Bind the selectedRowID value to the placeholder
        query.addBindValue(ID_Column); // Assuming selectedRowID is a QString
        // Execute the prepared statement
        if (query.exec()) {
            qDebug() << "Row with ID" << selectedRowID << "removed successfully.";
            qDebug() << "Row with ID in database" << ID_Column << "removed successfully.";
            selectedRowID=NULL;
            ID_Column=NULL;
            resetAutoIncrementAndReindex();
            refreshTable();

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
    // Retrieve data from the model
    QVariant data = index.model()->data(index.model()->index(index.row(), 0)); // Assuming the first column is at index 0
    ID_Column = data.toString();
    //qDebug() << "Value of the first column:" << ID_Column;
}

void ManagerWindow::resetAutoIncrementAndReindex()
{
    QString alterQueryString = "ALTER TABLE `passwordmanager`.`" + login_name + "_password_data` AUTO_INCREMENT = 1";
    QString reindexQueryString = "SET @id := 0; UPDATE `passwordmanager`.`" + login_name + "_password_data` SET `ID` = @id := @id + 1";
    // Create QSqlQuery objects for each query
    QSqlQuery alterQuery;
    QSqlQuery reindexQuery;
    // Reset the auto-increment value
    if (alterQuery.exec(alterQueryString)) {
        qDebug() << "Table auto-increment reset successfully.";
    } else {
        qDebug() << "Error resetting table auto-increment:" << alterQuery.lastError().text();
    }

    // Re-index the ID column
    if (reindexQuery.exec(reindexQueryString)) {
        qDebug() << "Table re-indexed successfully.";
    } else {
        qDebug() << "Error re-indexing table:" << reindexQuery.lastError().text();
    }
}
