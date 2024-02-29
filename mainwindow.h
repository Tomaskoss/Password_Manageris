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


private slots:
    void on_SignUp_Button_clicked();

    void on_Quit_Button_clicked();

    void on_LogIn_Button_clicked();

private:
    Ui::MainWindow *ui;

    QSqlDatabase dataBase;
    QSqlQuery query();

};
#endif // MAINWINDOW_H
