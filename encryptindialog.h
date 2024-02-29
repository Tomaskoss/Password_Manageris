#ifndef ENCRYPTINDIALOG_H
#define ENCRYPTINDIALOG_H

#include <QDialog>

namespace Ui {
class EncryptinDialog;
}

class EncryptinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EncryptinDialog(QWidget *parent = nullptr);
    ~EncryptinDialog();

private slots:
    void on_Quit_Button_clicked();

private:
    Ui::EncryptinDialog *ui;
};

#endif // ENCRYPTINDIALOG_H
