#ifndef ORDERHANDLERFORM_H
#define ORDERHANDLERFORM_H

#include <QWidget>
#include "clientinformaiton.h"
#include "productinformaiton.h"

namespace Ui {
class OrderHandlerForm;
}
class OrderInformaiton;

class OrderHandlerForm : public QWidget
{
    Q_OBJECT

public:
    explicit OrderHandlerForm(QWidget *parent = nullptr);
    ~OrderHandlerForm();

public slots:
    void clientAdded(int);
    void productAdded(int);
    void clientRemoved(int);
    void productRemoved(int);
    void clientModified(int);
    void productModified(int);

private slots:
    void on_enrollPushButton_clicked();
    void orderReturnClient(QList<QString>);
    void orderReturnProduct(QList<QString>);

signals:
    void orderAddedClient(int);
    void orderAddedProduct(int);

private:
    Ui::OrderHandlerForm *Oui;
    QMap<int, OrderInformaiton*> orderInfo;
    int makeoid();
};

#endif // ORDERHANDLERFORM_H
