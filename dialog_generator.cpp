#include "dialog_generator.h"
#include "ui_dialog_generator.h"
#include <random>

Dialog_generator::Dialog_generator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_generator)
{
    ui->setupUi(this);
}

Dialog_generator::~Dialog_generator()
{
    delete ui;
}




void Dialog_generator::on_generate_Button_clicked()
{
    QString password;
    size_t passwordLength= ui->spinBox->value();
    generateRandomPassword(password,passwordLength);
    ui->password_generator_line->setText(password);

}


void Dialog_generator::on_close_Button_clicked()
{
    close();
}

void Dialog_generator::generateRandomPassword(QString& password, size_t passwordLength){
    std::string CHARACTERS;
    const std::string Alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string alphabet="abcdefghijklmnopqrstuvwxyz";
    const std::string numbers="0123456789";
    const std::string special="!@#$%^&*()-_=";
    // Construct character set based on checked buttons
    if (ui->Alphabet_Button->isChecked()) {
        CHARACTERS += Alphabet;
    }
    if (ui->alphabet_Button->isChecked()) {
        CHARACTERS += alphabet;
    }
    if (ui->number_Button->isChecked()) {
        CHARACTERS += numbers;
    }
    if (ui->special_Button->isChecked()) {
        CHARACTERS += special;
    }
    if (CHARACTERS.empty()) {
        // No character set selected, use default characters
        CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()-_=+";
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> distribution(0, CHARACTERS.size() - 1);

    password.clear(); // Clear the password string to ensure it's empty before generating the new password

    for (size_t i = 0; i < passwordLength; ++i) {
        password.append(QChar(CHARACTERS[distribution(gen)]));
    }
}

void Dialog_generator::on_use_Button_clicked()
{
    QString password;
    password =ui->password_generator_line->text();
    QString generatedPassword = getGeneratedPassword();
    qDebug()<<"generated password:"<<generatedPassword;
    accept();

}
QString Dialog_generator::getGeneratedPassword() const
{
    return ui->password_generator_line->text();
}
