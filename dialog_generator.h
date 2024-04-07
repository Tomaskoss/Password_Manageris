#ifndef DIALOG_GENERATOR_H
#define DIALOG_GENERATOR_H
#include <QDialog>

namespace Ui {
class Dialog_generator;
}

class Dialog_generator : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_generator(QWidget *parent = nullptr);
     QString getGeneratedPassword() const;
    ~Dialog_generator();

private slots:


    void on_generate_Button_clicked();

    void on_close_Button_clicked();

    void on_use_Button_clicked();

private:
    Ui::Dialog_generator *ui;
    void generateRandomPassword(QString& password, size_t passwordLength);
};

#endif // DIALOG_GENERATOR_H
