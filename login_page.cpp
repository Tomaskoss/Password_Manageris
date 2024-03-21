#include "login_page.h"
#include "managerwindow.h"
#include "qsqlerror.h"

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>

login_page::login_page(QWidget *parent)
    : QWidget{parent}
{
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        statusMessageLabel->setText("Username or password is blank");
        return;
    }

    query.prepare("SELECT * FROM `passwordmanager`.`login_information` WHERE Login_name = :username");
    query.bindValue(":username", username);

    if (query.exec()) {
        if (query.next()) {
            QString storedPassword = query.value("Login_master_password").toString();
            // Perform password verification here
            // For demonstration purposes, let's print the stored password
            qDebug() << "Stored Password:" << storedPassword;

            // Check if passwords match (replace this with your actual password verification logic)
            if (storedPassword == password) {
                // Passwords match
                qDebug() << "Login successful";
                hide();
                ManagerWindow *managerWindow = new ManagerWindow(username);
                managerWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure deletion on close
                managerWindow->setWindowFlags(Qt::Window); // Ensure appropriate window flags are set
                managerWindow->show();
            } else {
                // Passwords do not match
                qDebug() << "Login failed";
                statusMessageLabel->setText("Incorrect username or password");
            }
        } else {
            qDebug() << "Username not found";
            statusMessageLabel->setText("Username not found");
        }
    } else {
        qDebug() << "Query execution error:" << query.lastError().text();
        statusMessageLabel->setText("Query execution error");
    }


}
