#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include "qsqlquery.h"
#include <QMainWindow>

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();
   bool isValidEmail(QString &email);

private slots:
    void on_Back_Button_clicked();

    void on_SignUp_Button_clicked();

private:
    Ui::RegisterWindow *ui;
    QSqlQuery query;
};

#endif // REGISTERWINDOW_H
