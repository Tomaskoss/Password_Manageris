#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include "mainwindow.h"
#include "qsqlquery.h"
#include "qsqlquerymodel.h"
#include <QMainWindow>

namespace Ui {
class ManagerWindow;
}

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagerWindow(const QString &login_name, MainWindow *mainWindow);

    ~ManagerWindow();
    void refreshTable();
    void generateAndSetPassword(QString& password, size_t passwordLength) {
        generateRandomPassword(password, passwordLength);
    }
private slots:

    void on_LogOut_Button_clicked();
    void on_actionAdd_triggered();
    void on_actionRemove_triggered();
    void resetAutoIncrementAndReindex();
    void on_tableView_clicked(const QModelIndex &index);
    void on_actionChange_triggered();
    void on_Back_Button_clicked();
    void on_Confirm_Button_clicked();

    void on_show_Password_Button_clicked();

    void on_show_Password_Edit_Button_clicked();

    void on_actionPassword_generator_triggered();

    void on_generate_Button_clicked();

    void on_close_Button_clicked();

private:
    std::tuple<QString, QString, QString> aes_GCM_ENCRYPT(const QString &plaintext);
    void addRecord(const QString &appName, const QString &username, const QString &password, const QString &url);
    void clearData();
    void updateRecord(const QString &appName, const QString &username, const QString &password, const QString &url, const QString &id);
    void generateRandomPassword(QString &password, size_t passwordLength);
    void handleErrors(void);
    QString Get_KDF_From_Database(const QString &login_name);
    QString aes_GCM_DECRYPT(const QString &base64Ciphertext, const QString &base64IV, const QString &base64Tag);
    Ui::ManagerWindow *ui;
    QString login_name;
    QSqlQueryModel *model;
    QString selectedRowID;
    QSqlQuery query;
    QString ID_Column;
    MainWindow *mainWindow;
};

#endif // MANAGERWINDOW_H
