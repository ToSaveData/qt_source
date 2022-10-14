#include "orderinformaiton.h"
#include "orderhandlerform.h"
//#include "clienthandlerform.h"
//#include "producthandlerform.h"
#include "ui_orderhandlerform.h"
#include <QTableWidgetItem>
#include <QList>
#include <QFile>

OrderHandlerForm::OrderHandlerForm(QWidget *parent) :
    QWidget(parent),
    Oui(new Ui::OrderHandlerForm)
{
    Oui->setupUi(this);

    QFile file("orderinfo.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size())
        {
            int itemRow = Oui->tableWidget1->rowCount();
            int id = row[0].toInt();
            int orderNumber = row[2].toInt();
            int cid = row[3].toInt();
            int pid = row[4].toInt();
            OrderInformaiton* o = new OrderInformaiton(id, row[1], orderNumber, cid, pid);
                Oui->tableWidget1->setRowCount(itemRow + 1);
                Oui->tableWidget1->setItem(itemRow - 1, 0, new QTableWidgetItem(QString::number(id)));
                for (int i = 0 ; i < 4; i++)
                {
                    Oui->tableWidget1->setItem(itemRow - 1, i + 1, new QTableWidgetItem(row[i+1]));
                }
            if(Oui->clientIDComboBox1->findText(QString::number(cid)) < 0)
                Oui->clientIDComboBox1->addItem(QString::number(cid));
            if(Oui->productIDComboBox1->findText(QString::number(pid)) < 0)
                Oui->productIDComboBox1->addItem(QString::number(pid));
            orderInfo.insert(id, o);
        }
    }
    file.close( );
}

OrderHandlerForm::~OrderHandlerForm()
{
    QFile file("orderinfo.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    Q_FOREACH(const auto& v, orderInfo)
    {
        OrderInformaiton* o = v;
        out << orderInfo.key(o) << ", " << o->getOrderDate() << ", ";
        out << o->getOrderNumber() << ", " << o->getCID() << ", " << o->getPID() << "\n";
    }
    file.close();

    delete Oui;
}

int OrderHandlerForm::makeoid()
{
    if(orderInfo.isEmpty())    return 10000;
    else    return orderInfo.size() + 10000;
}

void OrderHandlerForm::clientAdded(int cid)
{
    Oui->clientIDComboBox1->addItem(QString::number(cid));
}

void OrderHandlerForm::productAdded(int pid)
{
    Oui->productIDComboBox1->addItem(QString::number(pid));
}

void OrderHandlerForm::on_enrollPushButton_clicked()
{
    int key = makeoid();
    QString date = Oui->orderDateLineEdit1->text();
    int quantity = Oui->orderQuantityLineEdit1->text().toInt();
    int cid = Oui->clientIDComboBox1->currentText().toInt();
    int pid = Oui->productIDComboBox1->currentText().toInt();

    int row = Oui->tableWidget1->rowCount();
    Oui->tableWidget1->setRowCount(Oui->tableWidget1->rowCount()+1);
    Oui->tableWidget1->setItem(row, 0, new QTableWidgetItem(QString::number(key)));
    Oui->tableWidget1->setItem(row, 1, new QTableWidgetItem(date));
    Oui->tableWidget1->setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
    Oui->tableWidget1->setItem(row, 3, new QTableWidgetItem(QString::number(cid)));
    Oui->tableWidget1->setItem(row, 4, new QTableWidgetItem(QString::number(pid)));

    QVector<QTableWidget*> w;
    w << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    for(int x = 0; x < 3; x++)
    {
        w[x]->setRowCount(w[x]->rowCount()+1);
        w[x]->setItem(row, 0, new QTableWidgetItem(QString::number(key)));
        w[x]->setItem(row, 1, new QTableWidgetItem(date));
        w[x]->setItem(row, 8, new QTableWidgetItem(QString::number(quantity)));
        w[x]->setItem(row, 9, new QTableWidgetItem(QString::number(0)));
    }

    OrderInformaiton *o = new OrderInformaiton(key, date, quantity, cid, pid);

    orderInfo.insert(key, o);

    emit orderAddedClient(cid);
    emit orderAddedProduct(pid);

    update();
}

void OrderHandlerForm::orderReturnClient(QList<QString> cinfo)
{
    QVector<QTableWidget*> w;
    w << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    int row = Oui->tableWidget2->rowCount() -1;
    for(int x = 0; x < 3; x++)
    {
        w[x]->setItem(row, 2, new QTableWidgetItem(cinfo[0]));
        w[x]->setItem(row, 3, new QTableWidgetItem(cinfo[1]));
        w[x]->setItem(row, 4, new QTableWidgetItem(cinfo[2]));
    }
}

void OrderHandlerForm::orderReturnProduct(QList<QString> pinfo)
{
    QVector<QTableWidget*> w;
    w << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    int quantity = Oui->orderQuantityLineEdit1->text().toInt();
    int row = Oui->tableWidget2->rowCount() -1;

qDebug() << pinfo[0] << pinfo[1] << pinfo[2];
    for(int x = 0; x < 3; x++)
    {
        w[x]->setItem(row, 5, new QTableWidgetItem(pinfo[0]));
        w[x]->setItem(row, 6, new QTableWidgetItem(pinfo[1]));
        w[x]->setItem(row, 7, new QTableWidgetItem(pinfo[2]));
        w[x]->setItem(row, 9, new QTableWidgetItem(QString::number(pinfo[2].toInt() * quantity)));
    }
}

void OrderHandlerForm::clientRemoved(int cid)
{
    //    order
}
void OrderHandlerForm::productRemoved(int pid)
{

}
void OrderHandlerForm::clientModified(int cid)
{

}
void OrderHandlerForm::productModified(int pid)
{

}

