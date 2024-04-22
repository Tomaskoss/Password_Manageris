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
    QString PBKDF2_KDF(const QString &password, const uint8_t *salt);
    QString Scrypt_KDF(const QString &password, const uint8_t *salt);

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
    void send_Email(const QString &totp, const QString &recipientEmail);
    void Generate_TOTP(QString Login_email);
    void Create_Manager_Window();
    bool validate_Register_Credentials();
    bool is_Username_Available(const QString& username);



    void register_User();
    bool PIN_Login();
    bool OTP_login();

    QString getPIN_Login();
    void clear_Data();

    void create_Database_Connection();
    void create_Table_And_Store_Password();
    bool create_User_Table(const QString &table_name);
    bool create_User_Table_For_Logs(const QString& table_name_log);

    bool is_Valid_Email(QString &email);
    bool generate_Random_Secret(QByteArray &secret, int length);
    void generate_Random_Password(std::string& password, size_t passwordLength);
    void generate_Random_Salt(uint8_t *salt, size_t saltSize);

    bool insert_Argon2id_KDF(const QString& username, const QString& hashedPassword, const QString& algorithm_type, const QString& email, const QString &pin);
    bool insert_PBKDF2_KDF(const QString& username, const QString& hashedPassword, const QString& algorithm_type, const QString& email, const int& iterations, const QByteArray& saltByteArray, const QString &pin);
    bool insert_Scrypt_KDF(const QString& username,const QString& hashedPassword,const QString& algorithm_type,const QString& email,const uint64_t& N,const uint64_t& r,const uint64_t& p,const uint64_t maxmemory , const QByteArray& saltByteArray,const QString& pin);
    QString base32_Encode(const QByteArray &data);

    //QSqlDatabase dataBase;
    QSqlQuery query();
    QString generatedTOTP;
    QString usernameL;
    QString passwordL;
    QString passwordM;
    QString email ;
    QString pin;
    uint8_t salt[];

};


#endif // MAINWINDOW_H
