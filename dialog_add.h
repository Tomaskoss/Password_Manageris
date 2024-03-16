#ifndef DIALOG_ADD_H
#define DIALOG_ADD_H

#include <QDialog>

namespace Ui {
class Dialog_ADD;
}

class Dialog_ADD : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ADD(QWidget *parent = nullptr);
    ~Dialog_ADD();

private slots:
    void on_Back_Button_clicked();

private:
    Ui::Dialog_ADD *ui;
};

#endif // DIALOG_ADD_H
