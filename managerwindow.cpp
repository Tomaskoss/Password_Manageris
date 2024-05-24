#include "managerwindow.h"
#include "dialog_client_site.h"
#include "dialog_generator.h"
#include "dialog_server.h"
#include "mainwindow.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_managerwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QStandardItemModel> // Add this include statement

#include <QTcpServer>

#include <QStyledItemDelegate>
#include "openssl/evp.h"
#include <QMessageBox> // Include the necessary header file

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
    logModel(new QSqlQueryModel(this)),
    mainWindow(mainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Window);
    ui->stackedWidget->setCurrentIndex(0);
    ui->Back_To_Records->hide();

    query.prepare("SELECT * FROM `passwordmanager`.`"+login_name+"_password_data`");
    if (query.exec()) {
        logging("Logged in");
        model->setQuery(std::move(query));
        refreshTable();


    } else {
        qDebug() << "Query execution error:" << query.lastError().text();
    }

}

ManagerWindow::~ManagerWindow()
{
    delete ui;
    delete model;
    delete logModel;

}

void ManagerWindow::on_LogOut_Button_clicked()
{

    close();
    logging("log out");
    // Show the existing MainWindow
    if (mainWindow) {
        mainWindow->show();

    } else {
        qDebug() << "Error: MainWindow instance is null.";
    }
}

void ManagerWindow::on_actionAdd_triggered()
{
        ui->comboBox->show();
        selectedRowID.clear();
        resetAutoIncrementAndReindex();
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
            // Set the headers to resize to contents
            QHeaderView *header = ui->tableView->horizontalHeader();
            header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
            header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
            header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
            header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
            header->setSectionResizeMode(5, QHeaderView::ResizeToContents);

            PasswordDelegate *passwordDelegate = new PasswordDelegate(this);
            ui->tableView->setItemDelegateForColumn(3, passwordDelegate); // A

        } else {
            qDebug() << "Query execution error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Failed to create QSqlQueryModel object.";
    }

    // Set the model for the table view
    ui->tableView->setModel(model);
    clearData();
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
            logging("Removed record");

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
    selectedRowID.clear();
    passwordsVisible=false;
    ui->password_Line->setEchoMode(QLineEdit::Password);

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
    ui->password_Line->setEchoMode(QLineEdit::Password);

}

void ManagerWindow::updateRecord(const QString &appName, const QString &username, const QString &password, const QString &url, const QString &id){
    QSqlQuery query;

    QString encryptionMethod = ui->comboBox->currentText();
    if (encryptionMethod == "Aes_GCM_256") {
        auto [encryptedPassword, iv, tag] = aes_GCM_ENCRYPT(password);
        if (encryptedPassword.isEmpty() || iv.isEmpty() || tag.isEmpty()) {
            qDebug() << "Encryption failed.";
            return;
        }

        QString updateQueryString = "UPDATE `passwordmanager`.`" + login_name + "_password_data` "
                                                                                "SET `Name of APP` = ?, "
                                                                                "`Username` = ?, "
                                                                                "`Password` = ?, "
                                                                                "`IV` = ?, "
                                                                                "`Tag` = ?, "
                                                                                "`URL` = ?, "
                                                                                "`log` = NOW(), "
                                                                                "`encryption_method` = ? "
                                                                                "WHERE `ID` = ?";

        if (query.prepare(updateQueryString)) {
            query.addBindValue(appName);
            query.addBindValue(username);
            query.addBindValue(encryptedPassword);
            query.addBindValue(iv);
            query.addBindValue(tag);
            query.addBindValue(url);
            query.addBindValue(encryptionMethod);
            query.addBindValue(id);

            if (query.exec()) {
                qDebug() << "Record updated successfully.";
                refreshTable();
                logging("Updated record");
            } else {
                qDebug() << "Error updating record:" << query.lastError().text();
            }
        } else {
            qDebug() << "Query preparation error:" << query.lastError().text();
        }
    } else if (encryptionMethod == "ChaCha20") {
        auto [iv, encryptedPassword] = chacha20_encrypt(password);
        if (encryptedPassword.isEmpty() || iv.isEmpty()) {
            qDebug() << "Encryption failed.";
            return;
        }

        QString updateQueryString = "UPDATE `passwordmanager`.`" + login_name + "_password_data` "
                                                                                "SET `Name of APP` = ?, "
                                                                                "`Username` = ?, "
                                                                                "`Password` = ?, "
                                                                                "`IV` = ?, "
                                                                                "`URL` = ?, "
                                                                                "`log` = NOW(), "
                                                                                "`encryption_method` = ? "
                                                                                "WHERE `ID` = ?";

        if (query.prepare(updateQueryString)) {
            query.addBindValue(appName);
            query.addBindValue(username);
            query.addBindValue(encryptedPassword);
            query.addBindValue(iv);
            query.addBindValue(url);
            query.addBindValue(encryptionMethod);
            query.addBindValue(id);

            if (query.exec()) {
                qDebug() << "Record updated successfully.";
                refreshTable();
                logging("Updated record");
            } else {
                qDebug() << "Error updating record:" << query.lastError().text();
            }
        } else {
            qDebug() << "Query preparation error:" << query.lastError().text();
        }
    }
}

void ManagerWindow::addRecord(const QString &appName, const QString &username, const QString &password, const QString &url)
{
    QString encryptionMethod = ui->comboBox->currentText();
    if (encryptionMethod == "Aes_GCM_256") {
        auto [encryptedPassword, iv, tag] = aes_GCM_ENCRYPT(password);
        if (encryptedPassword.isEmpty() || iv.isEmpty() || tag.isEmpty()) {
            qDebug() << "Encryption failed.";
            return;
        }

        QSqlQuery query;
        QString insertQueryString = "INSERT INTO `passwordmanager`.`" + login_name + "_password_data` "
                                                                                     "(`Name of APP`, `Username`, `Password`, `URL`, `log`, `IV`, `Tag`, `encryption_method`) "
                                                                                     "VALUES (?, ?, ?, ?, NOW(), ?, ?, ?)";

        // Prepare the query with the SQL statement
        if (query.prepare(insertQueryString)) {
            // Bind values to placeholders
            query.addBindValue(appName);
            query.addBindValue(username);
            query.addBindValue(encryptedPassword); // Bind the encrypted password
            query.addBindValue(url);
            query.addBindValue(iv); // Bind IV
            query.addBindValue(tag); // Bind tag
            query.addBindValue(encryptionMethod); // Bind encryption method

            // Execute the prepared statement
            if (query.exec()) {
                qDebug() << "Record added successfully.";
                refreshTable(); // Refresh the table after adding
                query.finish();
                logging("Added record");
            } else {
                qDebug() << "Error adding record:" << query.lastError().text();
            }
        } else {
            qDebug() << "Query preparation error:" << query.lastError().text();
        }
    }else if (encryptionMethod == "ChaCha20") {
        auto [iv, encryptedPassword] = chacha20_encrypt(password);
        if (encryptedPassword.isEmpty() || iv.isEmpty()) {
            qDebug() << "Encryption failed.";
            return;
        }

        QSqlQuery query;
        QString insertQueryString = "INSERT INTO `passwordmanager`.`" + login_name + "_password_data` "
                                                                                     "(`Name of APP`, `Username`, `Password`, `URL`, `log`, `IV`,  `encryption_method`) "
                                                                                     "VALUES (?, ?, ?, ?, NOW(), ?, ?)";

        if (query.prepare(insertQueryString)) {
            query.addBindValue(appName);
            query.addBindValue(username);
            query.addBindValue(encryptedPassword);
            query.addBindValue(url);
            query.addBindValue(iv);
            query.addBindValue(encryptionMethod);

            // Execute the prepared statement
            if (query.exec()) {
                qDebug() << "Record added successfully.";
                refreshTable();
                query.finish();
                logging("Added record");
            } else {
                qDebug() << "Error adding record:" << query.lastError().text();
            }
        } else {
            qDebug() << "Query preparation error:" << query.lastError().text();
        }
    }

}

void ManagerWindow::on_show_Password_Button_clicked()
{
     //ui->comboBox->hide();
    if (selectedRowID.isEmpty()) {
        qDebug()<<"rowid for show:"<<selectedRowID;
        qDebug()<<"No row selected nothing to show";
    }
    else{

        on_actionChange_triggered();
    }




}

void ManagerWindow::on_show_Password_Edit_Button_clicked(){

    // Toggle the visibility of passwords
    passwordsVisible = !passwordsVisible;

    QString decryptedText;
    if (passwordsVisible) {
        auto encryptionData = Get_Database_encryption_data();
        QString ivString = std::get<0>(encryptionData);
        QString encryptedPassword = std::get<1>(encryptionData); // Change to encryptedPassword
        QString tagString = std::get<2>(encryptionData);
        QString encryptionMethod = std::get<3>(encryptionData);
        qDebug()<<"encryption: "<<encryptionMethod;
        if (encryptionMethod == "Aes_GCM_256") {
            decryptedText = aes_GCM_DECRYPT(ivString, encryptedPassword, tagString);
            qDebug() << "Aes decryption";
        } else if (encryptionMethod == "ChaCha20") {
            // Decrypt ChaCha20 using both IV and encrypted password
            decryptedText = chacha20_decrypt(encryptedPassword, ivString);
            qDebug() << "ChaCha20 decryption";
        } else if (encryptionMethod.isEmpty()){

            decryptedText= ui->password_Line->text();
        }
        else{
            qDebug() << "Unsupported encryption algorithm selected.";
        }

        qDebug() << "Decrypted text:" << decryptedText;
           ui->password_Line->setText(decryptedText);
    }

    if (passwordsVisible) {
        ui->password_Line->setEchoMode(QLineEdit::Normal);
    } else {
        ui->password_Line->setEchoMode(QLineEdit::Password);
    }

}

void ManagerWindow::clearData(){
    ui->password_Line->clear();
    ui->URL_Line->clear();
    ui->username_Line->clear();
    ui->app_Line->clear();
}

std::tuple<QString, QString, QString> ManagerWindow::aes_GCM_ENCRYPT(const QString &plaintext) {
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Parameters for AES-GCM
    const int key_len = 32; // 256-bit key
    const int iv_len = 12;  // 96-bit IV
    const int tag_len = 16; // 128-bit tag

    // Generate a random IV
    unsigned char iv[iv_len];
    RAND_bytes(iv, iv_len);

    // Retrieve KDF from the database using the login_name
    QString loginMasterPassword = Get_KDF_From_Database(login_name);
    if (loginMasterPassword.isEmpty()) {
        qDebug() << "Failed to retrieve KDF from the database.";
        return std::make_tuple("", "", ""); // Return empty values indicating failure
    }

    // Convert QString to QByteArray to use with OpenSSL
    QByteArray keyBytes = loginMasterPassword.toUtf8();
    unsigned char *key = reinterpret_cast<unsigned char*>(keyBytes.data());

    // Encrypt plaintext using AES-GCM
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);

    // Enable encryption padding (not required for GCM mode)
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    // Calculate ciphertext length
    int ciphertext_len = plaintext.length() + EVP_CIPHER_CTX_block_size(ctx);
    unsigned char *ciphertext = new unsigned char[ciphertext_len];

    int len;
    EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char*)plaintext.toStdString().c_str(), plaintext.length());
    int ciphertext_len_final = len;

    // Finalize encryption (compute tag)
    unsigned char tag[tag_len];
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, tag_len, tag);

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    // Convert ciphertext, IV, and tag to QString and return as a tuple
    QString encryptedText = QByteArray(reinterpret_cast<const char*>(ciphertext), ciphertext_len_final).toBase64();
    QString ivString = QByteArray(reinterpret_cast<const char*>(iv), iv_len).toBase64();
    QString tagString = QByteArray(reinterpret_cast<const char*>(tag), tag_len).toBase64();

    return std::make_tuple(ivString, encryptedText, tagString);
}

std::tuple<QString, QString> ManagerWindow::chacha20_encrypt(const QString &plaintext) {
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Parameters for ChaCha20
    const int key_len = 32; // 256-bit key
    const int iv_len = 16;  // 96-bit nonce

    // Generate a random IV (nonce)
    unsigned char iv[iv_len];
    if (RAND_bytes(iv, iv_len) != 1) {
        qDebug() << "Failed to generate random IV.";
        return std::make_tuple("", ""); // Return empty values indicating failure
    }

    // Retrieve KDF from the database using the login_name
    QString loginMasterPassword = Get_KDF_From_Database(login_name);
    if (loginMasterPassword.isEmpty()) {
        qDebug() << "Failed to retrieve KDF from the database.";
        return std::make_tuple("", ""); // Return empty values indicating failure
    }

    // Convert QString to QByteArray to use with OpenSSL
    QByteArray keyBytes = loginMasterPassword.toUtf8();
    unsigned char *key = reinterpret_cast<unsigned char*>(keyBytes.data());

    // Encrypt plaintext using ChaCha20
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qDebug() << "Failed to create EVP_CIPHER_CTX.";
        return std::make_tuple("", "");
    }

    if (EVP_EncryptInit_ex(ctx, EVP_chacha20(), NULL, key, iv) != 1) {
        qDebug() << "Failed to initialize ChaCha20 cipher.";
        EVP_CIPHER_CTX_free(ctx);
        return std::make_tuple("", "");
    }

    // Allocate memory for ciphertext
    int plaintext_len = plaintext.toUtf8().size();
    unsigned char *ciphertext = new unsigned char[plaintext_len];

    int len;
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, (unsigned char*)plaintext.toStdString().c_str(), plaintext_len) != 1) {
        qDebug() << "Encryption failed.";
        EVP_CIPHER_CTX_free(ctx);
        delete[] ciphertext;
        return std::make_tuple("", "");
    }

    int ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        qDebug() << "Failed to finalize encryption.";
        EVP_CIPHER_CTX_free(ctx);
        delete[] ciphertext;
        return std::make_tuple("", "");
    }

    ciphertext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    // Convert ciphertext and IV to QString and return as a tuple
    QString encryptedText = QByteArray(reinterpret_cast<const char*>(ciphertext), ciphertext_len).toBase64();
    QString ivString = QByteArray(reinterpret_cast<const char*>(iv), iv_len).toBase64();

    delete[] ciphertext;

    return std::make_tuple(ivString, encryptedText);
}

QString ManagerWindow::chacha20_decrypt(const QString &encryptedText, const QString &ivString) {
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Parameters for ChaCha20
    const int iv_len = 16;  // 96-bit nonce

    // Decode the base64 encoded IV and ciphertext
    QByteArray iv = QByteArray::fromBase64(ivString.toUtf8());
    QByteArray ciphertext = QByteArray::fromBase64(encryptedText.toUtf8());

    if (iv.size() != iv_len) {
        qDebug() << "Invalid IV length.";
        return ""; // Return empty QString indicating failure
    }

    // Retrieve KDF from the database using the login_name
    QString loginMasterPassword = Get_KDF_From_Database(login_name);
    if (loginMasterPassword.isEmpty()) {
        qDebug() << "Failed to retrieve KDF from the database.";
        return ""; // Return empty QString indicating failure
    }

    // Convert QString to QByteArray to use with OpenSSL
    QByteArray keyBytes = loginMasterPassword.toUtf8();
    unsigned char *key = reinterpret_cast<unsigned char*>(keyBytes.data());

    // Decrypt ciphertext using ChaCha20
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qDebug() << "Failed to create EVP_CIPHER_CTX.";
        return "";
    }

    if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), NULL, key, reinterpret_cast<unsigned char*>(iv.data())) != 1) {
        qDebug() << "Failed to initialize ChaCha20 cipher.";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Allocate memory for plaintext
    int ciphertext_len = ciphertext.size();
    unsigned char *plaintext = new unsigned char[ciphertext_len];

    int len;
    if (EVP_DecryptUpdate(ctx, plaintext, &len, reinterpret_cast<unsigned char*>(ciphertext.data()), ciphertext_len) != 1) {
        qDebug() << "Decryption failed.";
        EVP_CIPHER_CTX_free(ctx);
        delete[] plaintext;
        return "";
    }

    int plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
        qDebug() << "Failed to finalize decryption.";
        EVP_CIPHER_CTX_free(ctx);
        delete[] plaintext;
        return "";
    }

    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    // Convert plaintext to QString and return
    QString decryptedText = QString::fromUtf8(reinterpret_cast<const char*>(plaintext), plaintext_len);

    delete[] plaintext;

    return decryptedText;
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
    logging("Generated password");
}

void ManagerWindow::on_close_Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ManagerWindow::handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

QString ManagerWindow::Get_KDF_From_Database(const QString &login_name) {
    QSqlQuery query;
    QString loginMasterPassword;
    query.prepare("SELECT Login_master_password FROM `passwordmanager`.`login_information` WHERE Login_name = :username");
    query.bindValue(":username", login_name);
    if(query.exec()) {
        if (query.next()) {
            loginMasterPassword = query.value(0).toString();
            qDebug() << "Login Master Password: " << loginMasterPassword;
        }
    } else {
        qDebug() << "Query execution error: " << query.lastError();
    }
    return loginMasterPassword;
}

QString ManagerWindow::aes_GCM_DECRYPT(const QString &base64Ciphertext, const QString &base64IV, const QString &base64Tag) {
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Parameters for AES-GCM
    const int key_len = 32; // 256-bit key
    const int iv_len = 12;  // 96-bit IV
    const int tag_len = 16; // 128-bit tag

    // Retrieve KDF from the database using the login_name
    QString loginMasterPassword = Get_KDF_From_Database(login_name);
    qDebug()<<"loginMasterPassword Aes:"<<loginMasterPassword;
    if (loginMasterPassword.isEmpty()) {
        qDebug() << "Failed to retrieve KDF from the database.";
        return ""; // Return empty string indicating failure
    }

    // Convert QString to QByteArray to use with OpenSSL
    QByteArray keyBytes = loginMasterPassword.toUtf8();
    unsigned char *key = reinterpret_cast<unsigned char*>(keyBytes.data());

    // Decode base64 encoded IV and tag
    QByteArray ivBytes = QByteArray::fromBase64(base64IV.toUtf8());
    QByteArray tagBytes = QByteArray::fromBase64(base64Tag.toUtf8());
    // Decode base64 encoded ciphertext
    QByteArray ciphertextBytes = QByteArray::fromBase64(base64Ciphertext.toUtf8());

    // Decrypt ciphertext using AES-GCM
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, (const unsigned char*)ivBytes.constData());

    // Enable encryption padding (not required for GCM mode)
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    // Allocate memory for plaintext
    unsigned char *plaintext = new unsigned char[ciphertextBytes.length() + EVP_CIPHER_CTX_block_size(ctx)];
    int len;
    int plaintext_len;

    // Decrypt the ciphertext
    if (!EVP_DecryptUpdate(ctx, plaintext, &len, (const unsigned char*)ciphertextBytes.constData(), ciphertextBytes.length())) {
        // Decryption failed
        EVP_CIPHER_CTX_free(ctx);
        delete[] plaintext;
        return "";
    }
    plaintext_len = len;

    // Set the tag
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag_len, (void*)tagBytes.constData())) {
        // Decryption failed
        EVP_CIPHER_CTX_free(ctx);
        delete[] plaintext;
        return "";
    }

    // Finalize decryption
    if (!EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        // Decryption failed
        EVP_CIPHER_CTX_free(ctx);
        delete[] plaintext;
        return "";
    }
    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    qDebug()<<plaintext<<"plaintext";

    // Convert decrypted plaintext to QString and return
    QString decryptedText = QString::fromUtf8(reinterpret_cast<char*>(plaintext), plaintext_len);
    qDebug()<<decryptedText<<":decryptedText";
    delete[] plaintext;
    return decryptedText;
}

std::tuple<QString, QString, QString, QString> ManagerWindow::Get_Database_encryption_data() {
    QSqlQuery query;
    QString ivString;
    QString tagString;
    QString encryptedText;
    QString encryptionMethod; // Add variable for encryption method
    query.prepare("SELECT Password, IV, Tag, encryption_method FROM `passwordmanager`.`" + login_name + "_password_data` WHERE id = :id");
    query.bindValue(":id", selectedRowID);
    if(query.exec()) {
        if (query.next()) {
            encryptedText = query.value(0).toString();
            ivString = query.value(1).toString();
            tagString = query.value(2).toString();
            encryptionMethod = query.value(3).toString(); // Retrieve encryption method
            qDebug() << "Login Master Password: " << encryptedText;
            qDebug() << "IV: " << ivString;
            qDebug() << "Tag: " << tagString;
            qDebug() << "Encryption Method: " << encryptionMethod; // Output encryption method
        }
    } else {
        qDebug() << "Query execution error: " << query.lastError();
    }
    return std::make_tuple(ivString, encryptedText, tagString, encryptionMethod);
}

void ManagerWindow::logging(QString logType){
    QSqlQuery query;

    // Get current timestamp
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString(Qt::ISODate);

    // Construct the INSERT query
    QString queryString = "INSERT INTO `passwordmanager`.`" + login_name + "_log_data` (timestamp, log) VALUES (:timestamp, :log)";
    query.prepare(queryString);
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":log", logType);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error inserting log for" << logType << ":" << query.lastError().text();
        return;
    }

    qDebug() << "Successfully logged '" << logType << "' action with timestamp" << timestamp;
}

void ManagerWindow::on_button_To_Logs_clicked()
{
    // Set the logModel as the model for the QTableView
    ui->tableView->setModel(logModel);

    // Retrieve log data from the MySQL database
    logModel->setQuery("SELECT * FROM `passwordmanager`.`"+login_name+"_log_data`");
    if (logModel->lastError().isValid()) {
        qDebug() << "Error loading log data:" << logModel->lastError().text();
    }
    ui->Back_To_Records->show();
    ui->button_To_Logs->hide();
}

void ManagerWindow::on_Back_To_Records_clicked()
{
    ui->tableView->setModel(model);

    ui->Back_To_Records->hide();
    ui->button_To_Logs->show();

}

void ManagerWindow::on_server_start_clicked()
{
    // Create a dialog window to display file receiving status
    Dialog_server dialog(login_name,this);
    dialog.setModal(false);
    // Show the dialog as modal and wait for it to be closed
    if (dialog.exec() == QDialog::Accepted) {
        // If the dialog was accepted, handle accordingly
    } else {
        // Handle the case where the dialog was not accepted
        qDebug() << "Dialog was not accepted";
    }

}

void ManagerWindow::on_klient_start_clicked()
{
    Dialog_client_site dialog(login_name,this); // Pass a reference to ManagerWindow
    dialog.setModal(false); // Make the dialog modal
    // Show the dialog as modal and wait for it to be closed
    if (dialog.exec() == QDialog::Accepted) {
        // If the dialog was accepted, get the generated password and set it to password_Line
    } else {
        // Handle the case where the dialog was not accepted (e.g., if the user clicked "Cancel")
        qDebug() << "Dialog was not accepted";
    }
}

