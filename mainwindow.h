#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtCore>
#include <qsqldatabase.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static QSqlDatabase dataBase;



private slots:
    void on_SignUp_Button_clicked();

    void on_Quit_Button_clicked();

    void on_LogIn_Button_clicked();

    void on_Back_Button_To_Login_clicked();

    void on_Register_Button_clicked();

    void on_Confirm_Button_clicked();

    void on_Back_Button_To_Register_clicked();

private:
    Ui::MainWindow *ui;
    void sendEmail(const QString &totp, const QString &recipientEmail);
    void GenerateTOTP();
    void PBKDF2_KDF();
    void Scrypt_KDF();
    void createDatabaseConnection();
    void establishEncryptedConnection(const QString& serverAddress, quint16 serverPort);
    void createTableAndStorePassword();
    void generateRandomSalt(uint8_t *salt, size_t saltSize);
    void registerUser();
    void clearData();
    void Argon_KDF();
    bool isValidEmail(QString &email);
    //QSqlDatabase dataBase;
    QSqlQuery query();
    QString usernameL;
    QString passwordL;
    QString passwordM;
    uint8_t salt[];

};


#endif // MAINWINDOW_H
