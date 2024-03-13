#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include "qsqlquerymodel.h"
#include <QMainWindow>

namespace Ui {
class ManagerWindow;
}

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagerWindow(const QString &login_name,QWidget *parent = nullptr);
    ~ManagerWindow();

private slots:
    void on_tableView_activated(const QModelIndex &index);

    void on_LogOut_Button_clicked();

    void on_actionAdd_triggered();

private:
    Ui::ManagerWindow *ui;
    QString usernameL;
    QSqlQueryModel *model;
};

#endif // MANAGERWINDOW_H
