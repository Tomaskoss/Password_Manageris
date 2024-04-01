#include "managerwindow.h"
#include "mainwindow.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_managerwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>

ManagerWindow::ManagerWindow(const QString &login_name,MainWindow *mainWindow)
    : QMainWindow(),
     ui(new Ui::ManagerWindow),
    login_name(login_name),
    model(new QSqlQueryModel(this)),
    mainWindow(mainWindow)


{

    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Window);
    ui->stackedWidget->setCurrentIndex(0);
    query.prepare("SELECT * FROM `passwordmanager`.`"+login_name+"_password_data`");
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

    // Show the existing MainWindow
    if (mainWindow) {
        mainWindow->show();
    } else {
        qDebug() << "Error: MainWindow instance is null.";
    }
}


void ManagerWindow::on_actionAdd_triggered()
{

    ui->stackedWidget->setCurrentIndex(1);
    selectedRowID.clear();


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

    QString appName = model->data(model->index(index.row(), 1)).toString();
    QString username = model->data(model->index(index.row(), 2)).toString();
    QString password = model->data(model->index(index.row(), 3)).toString();
    QString url = model->data(model->index(index.row(), 4)).toString();

    ui->app_Line->setText(appName);
    ui->username_Line->setText(username);
    ui->URL_Line->setText(url);
    ui->password_Line->setText(password);
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


void ManagerWindow::on_actionChange_triggered()
{

    ui->stackedWidget->setCurrentIndex(1);
}


void ManagerWindow::on_Back_Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}


void ManagerWindow::on_Confirm_Button_clicked()
{


    ui->stackedWidget->setCurrentIndex(0);
    // Extract the ID of the selected row
    QString id = ID_Column;
    // Extract the data from the line edits
    QString appName = ui->app_Line->text();
    QString username = ui->username_Line->text();
    QString password = ui->password_Line->text();
    QString url = ui->URL_Line->text();
    if(selectedRowID.isEmpty()){
        addRecord(appName, username, password, url);
    }
    else{
        updateRecord(appName, username, password, url,  id);
    }
    refreshTable();

}

void ManagerWindow::updateRecord(const QString &appName, const QString &username, const QString &password, const QString &url, const QString &id){
    QSqlQuery query;
    QString updateQueryString = "UPDATE `passwordmanager`.`" + login_name + "_password_data` "
                                                                            "SET `Name of APP` = ?, "
                                                                            "`Username` = ?, "
                                                                            "`Password` = ?, "
                                                                            "`URL` = ?, "
                                                                            "`log` = NOW() "
                                                                            "WHERE `ID` = ?";

    // Prepare the query with the SQL statement
    if (query.prepare(updateQueryString)) {
        // Bind values to placeholders
        query.addBindValue(appName);
        query.addBindValue(username);
        query.addBindValue(password);
        query.addBindValue(url);
        query.addBindValue(id);

        // Execute the prepared statement
        if (query.exec()) {
            qDebug() << "Record updated successfully.";
            refreshTable(); // Refresh the table after updating
        } else {
            qDebug() << "Error updating record:" << query.lastError().text();
        }
    } else {
        qDebug() << "Query preparation error:" << query.lastError().text();
    }
}
void ManagerWindow::addRecord(const QString &appName, const QString &username, const QString &password, const QString &url)
{
    QSqlQuery query;
    QString insertQueryString = "INSERT INTO `passwordmanager`.`" + login_name + "_password_data` "
                                                                                 "(`Name of APP`, `Username`, `Password`, `URL`, `log`) "
                                                                                 "VALUES (?, ?, ?, ?, NOW())";

    // Prepare the query with the SQL statement
    if (query.prepare(insertQueryString)) {
        // Bind values to placeholders
        query.addBindValue(appName);
        query.addBindValue(username);
        query.addBindValue(password);
        query.addBindValue(url);

        // Execute the prepared statement
        if (query.exec()) {
            qDebug() << "Record added successfully.";
            refreshTable(); // Refresh the table after adding
            query.finish();
        } else {
            qDebug() << "Error adding record:" << query.lastError().text();
        }
    } else {
        qDebug() << "Query preparation error:" << query.lastError().text();
    }
}
