#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qsqlquery.h"
#include <QWidget>

class login_page : public QWidget
{
    Q_OBJECT
public:
    explicit login_page(QWidget *parent = nullptr);

signals:

private slots:
    void on_LogIn_Button_clicked();
private:
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginButton;
    QLabel *statusMessageLabel;
    QSqlQuery query;
};

#endif // LOGIN_PAGE_H
