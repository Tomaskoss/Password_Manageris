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
    explicit ManagerWindow(const QString &login_name);
    ~ManagerWindow();
    void refreshTable();
private slots:

    void on_LogOut_Button_clicked();
    void on_actionAdd_triggered();
    void on_actionRemove_triggered();
    void resetAutoIncrementAndReindex();
    void on_tableView_clicked(const QModelIndex &index);
    void on_actionChange_triggered();
    void on_Back_Button_clicked();
    void on_Confirm_Button_clicked();

private:
    void addRecord(const QString &appName, const QString &username, const QString &password, const QString &url);

    void updateRecord(const QString &appName, const QString &username, const QString &password, const QString &url, const QString &id);
    Ui::ManagerWindow *ui;
    QString login_name;
    QSqlQueryModel *model;
    QString selectedRowID;
    QSqlQuery query;
    QString ID_Column;
};

#endif // MANAGERWINDOW_H
