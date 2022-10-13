#ifndef CLIENTHANDLERFORM_H
#define CLIENTHANDLERFORM_H

#include <QWidget>
#include <QMap>
namespace Ui {
class ClientHandlerForm;
}

class ClientInformaiton;
class QTableWidgetItem;

class ClientHandlerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientHandlerForm(QWidget *parent = nullptr);
    ~ClientHandlerForm();

private slots:
    void on_enrollPushButton_clicked();

    void on_searchPushButton_clicked();

    void on_removePushButton_clicked();

    void on_modifyPushButton_clicked();

    void on_tableWidget5_itemClicked(QTableWidgetItem *item);

private:
    int makecid();
    QMap<int, ClientInformaiton*> clientInfo;
    Ui::ClientHandlerForm *Cui;

signals:
    void clientAdded(int);

};

#endif // CLIENTHANDLERFORM_H
