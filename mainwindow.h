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
    QString getGeneratedTOTP() const { return generatedTOTP; }
    QString Argon_KDF(const QString& password, const uint8_t* salt);

private slots:
    void on_SignUp_Button_clicked();

    void on_Quit_Button_clicked();

    void on_LogIn_Button_clicked();

    void on_Back_Button_To_Login_clicked();

    void on_Register_Button_clicked();

    void on_Confirm_Button_clicked();

    void on_Back_Button_To_Register_clicked();

    void on_Back_To_Login_Button_clicked();

    void on_Confirm_Button_OTP_clicked();

private:
    Ui::MainWindow *ui;
    void sendEmail(const QString &totp, const QString &recipientEmail);
    void GenerateTOTP(QString Login_email);
    void CreateManagerWindow();
    void PBKDF2_KDF();
    void Scrypt_KDF();
    void createDatabaseConnection();
    void createTableAndStorePassword();
    void generateRandomSalt(uint8_t *salt, size_t saltSize);
    void registerUser();
    void clearData();
    bool  create_User_Table(const QString &table_name);
    bool isValidEmail(QString &email);
    bool generateRandomSecret(QByteArray &secret, int length);
    void generateRandomPassword(std::string& password, size_t passwordLength);
    bool insert_Argon2id_KDF(const QString& username, const QString& hashedPassword,const QString& algorithm_type, const QString& email);
    QString base32Encode(const QByteArray &data);
    void OTP_login();
    //QSqlDatabase dataBase;
    QSqlQuery query();
    QString generatedTOTP;

    QString usernameL;
    QString passwordL;
    QString passwordM;
    QString email ;
    uint8_t salt[];

};


#endif // MAINWINDOW_H
