#ifndef PRODUCTHANDLERFORM_H
#define PRODUCTHANDLERFORM_H

#include <QWidget>
#include "productinformaiton.h"

namespace Ui {
class ProductHandlerForm;
}

class ProductInformaiton;
class QTableWidgetItem;

class ProductHandlerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProductHandlerForm(QWidget *parent = nullptr);
    ~ProductHandlerForm();

private slots:
    void on_enrollPushButton_clicked();
    void on_removePushButton_clicked();
    void on_searchPushButton_clicked();
    void on_tableWidget5_itemClicked(QTableWidgetItem *item);
    void on_modifyPushButton_clicked();
    void orderAddedProduct(int);

private:
    Ui::ProductHandlerForm *Pui;
    QMap<int, ProductInformaiton*> productInfo;
    int makepid();

signals:
    void productAdded(int);
    void productRemoved(int);
    void productModified(int);
    void orderReturn(QList<QString>);
};

#endif // PRODUCTHANDLERFORM_H
