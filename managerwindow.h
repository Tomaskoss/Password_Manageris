#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QMainWindow>

namespace Ui {
class ManagerWindow;
}

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagerWindow(QWidget *parent = nullptr);
    ~ManagerWindow();

private slots:
    void on_tableView_activated(const QModelIndex &index);

    void on_LogOut_Button_clicked();

private:
    Ui::ManagerWindow *ui;
};

#endif // MANAGERWINDOW_H
