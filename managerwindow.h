#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

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
    explicit ManagerWindow(const QString &login_name,QWidget *parent = nullptr);
    ~ManagerWindow();
    void refreshTable();
private slots:

    void on_LogOut_Button_clicked();

    void on_actionAdd_triggered();

    void on_actionRemove_triggered();


    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::ManagerWindow *ui;
    QString login_name;
    QSqlQueryModel *model;
    QString selectedRowID;
    QSqlQuery query;
};

#endif // MANAGERWINDOW_H
