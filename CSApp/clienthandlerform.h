#ifndef CLIENTHANDLERFORM_H
#define CLIENTHANDLERFORM_H
#include "clientinformaiton.h"

#include <QWidget>

namespace Ui {
class ClientHandlerForm;
}

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
    void orderAddedClient(int);

private:
    int makecid();
    QMap<int, ClientInformaiton*> clientInfo;
    Ui::ClientHandlerForm *Cui;

signals:
    void clientAdded(int);
    void clientRemoved(int);
    void clientModified(int);
    void orderReturn(QList<QString>);
};

#endif // CLIENTHANDLERFORM_H
