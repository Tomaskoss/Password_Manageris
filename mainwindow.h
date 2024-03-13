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
    void setUsernameL(const QString& username);
    QString getUsernameL() const;

    void setPasswordL(const QString& password);
    QString getPasswordL() const;

    void setPasswordM(const QString& password);
    QString getPasswordM() const;



private slots:
    void on_SignUp_Button_clicked();

    void on_Quit_Button_clicked();

    void on_LogIn_Button_clicked();

private:
    Ui::MainWindow *ui;

    //QSqlDatabase dataBase;
    QSqlQuery query();
    QString usernameL;
    QString passwordL;
    QString passwordM;
};


#endif // MAINWINDOW_H
