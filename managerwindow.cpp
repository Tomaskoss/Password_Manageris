#include "managerwindow.h"
#include "dialog_generator.h"
#include "mainwindow.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_managerwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>

#include <QStyledItemDelegate>
#include "openssl/evp.h"

bool passwordsVisible = false;
class PasswordDelegate : public QStyledItemDelegate {

public:
    PasswordDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        if (index.column() == 3) { // Assuming password column is at index 3
            QString password = index.data(Qt::DisplayRole).toString();

            // Check if passwords are currently visible
            if (!passwordsVisible) {
                // If passwords are not visible, obscure them
                password = QString(password.length(), '*');
            }

            // Get the widget associated with the painter
            const QWidget *widget = option.widget;
            if (widget) {
                // Draw the password data using the style associated with the widget
                QStyleOptionViewItem opt = option;
                opt.text = password;
                const_cast<QWidget *>(widget)->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
            }
        } else {
            // For other columns, use default painting
            QStyledItemDelegate::paint(painter, option, index);
        }
    }
};

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
        selectedRowID.clear();
        ui->stackedWidget->setCurrentIndex(1); // Change the index to 1

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
            // ui->tableView->setItemDelegateForColumn(3,);
            ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
            ui->tableView->horizontalHeader()->resizeSection(3, 350);
            ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
            ui->tableView->horizontalHeader()->resizeSection(2, 150);
            // Set the custom delegate for the password column
            PasswordDelegate *passwordDelegate = new PasswordDelegate(this);
            ui->tableView->setItemDelegateForColumn(3, passwordDelegate); // A

        } else {
            qDebug() << "Query execution error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Failed to create QSqlQueryModel object.";
    }
}

void ManagerWindow::on_actionRemove_triggered()
{

    if (!selectedRowID.isEmpty()) { // Check if a row is selected



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
    selectedRowID.clear();
    }
    else{
        qDebug()<<"no selected row"<<selectedRowID;
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
    if (!selectedRowID.isEmpty()) { // Check if a row is selected
        ui->stackedWidget->setCurrentIndex(1); // Change the index to 1
    }
    else{
        qDebug()<<"no row selected"<<selectedRowID;
    }
}


void ManagerWindow::on_Back_Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    clearData();
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
    clearData();
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


void ManagerWindow::on_show_Password_Button_clicked()
{
    passwordsVisible = !passwordsVisible;
    // Refresh the table view to reflect the updated visibility
    ui->tableView->viewport()->update();
}


void ManagerWindow::on_show_Password_Edit_Button_clicked()
{
    // Toggle the visibility of passwords
    passwordsVisible = !passwordsVisible;

    if (passwordsVisible) {
        // If passwords are now visible, set echo mode to Normal
        ui->password_Line->setEchoMode(QLineEdit::Normal);
    } else {
        // If passwords are not visible, set echo mode to Password
        ui->password_Line->setEchoMode(QLineEdit::Password);
    }
}

void ManagerWindow::clearData(){
    ui->password_Line->clear();
    ui->URL_Line->clear();
    ui->username_Line->clear();
    ui->app_Line->clear();
}

void ManagerWindow::aes_GCM_ENCRYPT(){

}

void ManagerWindow::generateRandomPassword(QString& password, size_t passwordLength){
    std::string CHARACTERS;
    const std::string Alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string alphabet="abcdefghijklmnopqrstuvwxyz";
    const std::string numbers="0123456789";
    const std::string special="!@#$%^&*()-_=";
    // Construct character set based on checked buttons
    if (ui->Alphabet_Button->isChecked()) {
        CHARACTERS += Alphabet;
    }
    if (ui->alphabet_Button->isChecked()) {
        CHARACTERS += alphabet;
    }
    if (ui->number_Button->isChecked()) {
        CHARACTERS += numbers;
    }
    if (ui->special_Button->isChecked()) {
        CHARACTERS += special;
    }
    if (CHARACTERS.empty()) {
        // No character set selected, use default characters
        CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()-_=+";
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> distribution(0, CHARACTERS.size() - 1);

    password.clear(); // Clear the password string to ensure it's empty before generating the new password

    for (size_t i = 0; i < passwordLength; ++i) {
        password.append(QChar(CHARACTERS[distribution(gen)]));
    }
}
void ManagerWindow::on_actionPassword_generator_triggered()
{
   // generateRandomPassword(password,);
    if(ui->stackedWidget->currentIndex()==0){
        ui->stackedWidget->setCurrentIndex(2);

    }
    else if(ui->stackedWidget->currentIndex()==1){
        Dialog_generator dialog(this); // Pass a reference to ManagerWindow
        dialog.isModal();
        // Show the dialog as modal and wait for it to be closed
        if (dialog.exec() == QDialog::Accepted) {
            // If the dialog was accepted, get the generated password and set it to password_Line
            QString generatedPassword = dialog.getGeneratedPassword();
            ui->password_Line->setText(generatedPassword);
            qDebug() << "generated password manwin" << generatedPassword;
        } else {
            // Handle the case where the dialog was not accepted (e.g., if the user clicked "Cancel")
            qDebug() << "Dialog was not accepted";
        }
    }
}


void ManagerWindow::on_generate_Button_clicked()
{
    QString password;
    size_t passwordLenght= ui->spinBox->value();
    generateRandomPassword(password,passwordLenght);
    ui->password_generator_line->setText(password);

}


void ManagerWindow::on_close_Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

