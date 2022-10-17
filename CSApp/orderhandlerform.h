#ifndef ORDERHANDLERFORM_H
#define ORDERHANDLERFORM_H

#include <QWidget>

namespace Ui {
class OrderHandlerForm;
}
class OrderInformaiton;
class QTableWidgetItem;
class QComboBox;
class OrderHandlerForm : public QWidget
{
    Q_OBJECT

public:
    explicit OrderHandlerForm(QWidget *parent = nullptr);
    ~OrderHandlerForm();
    void dataload();
    int cnt = 0;

public slots:
    void clientAdded();
    void productAdded();
    void clientRemoved(int);
    void productRemoved(int);
    void clientModified(int, QList<QString>);
    void productModified(int, QList<QString>);

private slots:
    void on_enrollPushButton_clicked();
    void addReturnClient(QList<QString>);
    void addReturnProduct(QList<QString>);
    void searchReturnClient(QList<QString>);
    void searchReturnProduct(QList<QString>);

    void on_tableWidget5_itemClicked(QTableWidgetItem *item);

    void on_searchPushButton_clicked();

    void on_removePushButton_clicked();

    void on_modifyPushButton_clicked();

signals:
    void orderAddedClient(int);
    void orderAddedProduct(int);
    void orderSearchedClient(int);
    void orderSearchedProduct(int);
    void clientComboBox(QComboBox*, QComboBox*);
    void productComboBox(QComboBox*, QComboBox*);

private:
    Ui::OrderHandlerForm *Oui;
    QMap<int, OrderInformaiton*> orderInfo;
    int makeoid();
};

#endif // ORDERHANDLERFORM_H
