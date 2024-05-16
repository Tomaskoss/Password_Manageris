#ifndef DIALOG_SERVER_H
#define DIALOG_SERVER_H

#include <QDialog>
#include <QTcpServer>
namespace Ui {
class Dialog_server;
}

class Dialog_server : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_server(QWidget *parent = nullptr);
    ~Dialog_server();

private slots:
    void onNewConnection();
  //  void onReadyRead();
     QString getSaveFilePath();

private:
    Ui::Dialog_server *ui;
     QTcpServer *server;
};

#endif // DIALOG_SERVER_H
