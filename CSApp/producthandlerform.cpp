#include "producthandlerform.h"
#include "ui_producthandlerform.h"
#include <QList>
#include <QFile>
#include <QTableWidgetItem>
#include <QComboBox>

ProductHandlerForm::ProductHandlerForm(QWidget *parent) :
    QWidget(parent),
    Pui(new Ui::ProductHandlerForm)
{
    Pui->setupUi(this);
    QFile file("productinfo.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QVector<QTableWidget*> table;
    table << Pui->tableWidget1 << Pui->tableWidget2 << Pui->tableWidget4 << Pui->tableWidget5;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size())
        {
            int id = row[0].toInt();
            int price = row[2].toInt();
            ProductInformaiton* p = new ProductInformaiton(id, row[1], price, row[3]);
            for(int x = 0; x < 4; x++)
            {
                table[x]->setRowCount(table[x]->rowCount()+1);
                table[x]->setItem(table[x]->rowCount()-1, 0, new QTableWidgetItem(QString::number(id)));
                for (int i = 0 ; i < 3; i++)
                {
                    table[x]->setItem(table[x]->rowCount()-1, i+1, new QTableWidgetItem(row[i+1]));
                }
            }
            productInfo.insert(id, p);
        }
    }
    file.close( );
}

ProductHandlerForm::~ProductHandlerForm()
{
    QFile file("productinfo.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    Q_FOREACH(const auto& v, productInfo)
    {
        ProductInformaiton* p = v;
        out << productInfo.key(p) << ", " << p->getProductName() << ", ";
        out << p->getProductPrice() << ", " << p->getProductSort() << "\n";
    }
    file.close();
    delete Pui;
}

int ProductHandlerForm::makepid()
{
    if(productInfo.isEmpty())    return 1001;
    else    return productInfo.size() + 1001;
}

void ProductHandlerForm::setProductComboBox(QComboBox* PidBox, QComboBox* PinfoBox)
{
    Q_FOREACH(auto i, productInfo)
    {
        int key = productInfo.key(i);
        QString name = productInfo[key]->getProductName();
        QString sort = productInfo[key]->getProductSort();

        if(PidBox->findText(QString::number(key)) < 0)
            PidBox->addItem(QString::number(key));

        if(PinfoBox->findText(name + "(" + sort + ")") < 0)
            PinfoBox->addItem(name + "(" + sort + ")");
    }
}

void ProductHandlerForm::on_enrollPushButton_clicked()
{
    QVector<QTableWidget*> table;
    table << Pui->tableWidget1 << Pui->tableWidget2 << Pui->tableWidget4 << Pui->tableWidget5;

    QVector<QLineEdit*> lineEidt;
    lineEidt << Pui->nameLineEdit1 << Pui->priceLineEdit1 << Pui->sortLineEdit1;

    int key = makepid();
    int row = Pui->tableWidget1->rowCount();
    for(int x = 0; x < 4; x++)
    {
        table[x]->setRowCount(table[x]->rowCount()+1);
        table[x]->setItem(row, 0, new QTableWidgetItem(QString::number(key)));
        for (int i = 0 ; i < 3; i++)
        {
            QString s = lineEidt[i]->text();
            table[x]->setItem(row, i+1, new QTableWidgetItem(s));
        }
    }

    ProductInformaiton *p = new ProductInformaiton(key, lineEidt[0]->text(),
            lineEidt[1]->text().toInt(), lineEidt[2]->text());

    productInfo.insert(key, p);
    update();
    emit productAdded(key);

    for (int i = 0 ; i < 3; i++)    lineEidt[i]->clear();
}


void ProductHandlerForm::on_removePushButton_clicked()
{
    QVector<QTableWidget*> table;
    table << Pui->tableWidget1 << Pui->tableWidget2 << Pui->tableWidget4 << Pui->tableWidget5;

    int key = table[2]->item(table[2]->currentRow(),0)->text().toInt();
    emit productRemoved(key);

    productInfo.remove(key);
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            table[i]->takeItem(table[2]->currentRow(),j);
        }
    }
    update();
}


void ProductHandlerForm::on_searchPushButton_clicked()
{
    QTableWidget *table = Pui->tableWidget3;
    table->update();
    table->setRowCount(0); //이전 검색결과 초기화
    int key = Pui->searchLineEdit->text().toInt(); //입력된 키 값 저장

    if(productInfo[key]) //입력된 키가 저장된 정보에 있으면
    {
        QVector<QString> v;
        v << productInfo[key]->getProductName() << QString::number(productInfo[key]->getProductPrice())
          << productInfo[key]->getProductSort();

        int row = table->rowCount();
        table->setRowCount(table->rowCount()+1); //테이블에 데이터가 들어갈 행 생성
        table->setItem(row, 0, new QTableWidgetItem(QString::number(key))); //고객id 테이블에 삽입

        for(int i = 0; i < 3; i++)  //나머지 고객 정보 테이블에 삽입
        {
            table->setItem(row, i+1, new QTableWidgetItem(v[i]));
        }
    }
    update();

    Pui->searchLineEdit->clear();
}


void ProductHandlerForm::on_tableWidget5_itemClicked(QTableWidgetItem *item)
{
    QVector<QLineEdit*> lineEidt;
    lineEidt << Pui->idLineEdit << Pui->nameLineEdit2 << Pui->priceLineEdit2 << Pui->sortLineEdit2;
    item = Pui->tableWidget5->currentItem();

    for(int i = 0; i < 4; i++)
        lineEidt[i]->setText(Pui->tableWidget5->item(item->row(),i)->text());
    update();
}


void ProductHandlerForm::on_modifyPushButton_clicked()
{
    QVector<QTableWidget*> table;
    table << Pui->tableWidget1 << Pui->tableWidget2 << Pui->tableWidget4 << Pui->tableWidget5;
    QVector<QLineEdit*> lineEidt;
    lineEidt << Pui->idLineEdit << Pui->nameLineEdit2 << Pui->priceLineEdit2 << Pui->sortLineEdit2;
    int key = lineEidt[0]->text().toInt();
    int row = table[3]->currentRow();

    for(int x = 0; x < 4; x++)
    {
        for(int i = 1; i <= 3; i++)
        {
            table[x]->setItem(row, i, new QTableWidgetItem(lineEidt[i]->text()));
        }
    }

    ProductInformaiton *p = new ProductInformaiton(key, lineEidt[1]->text(),
            lineEidt[2]->text().toInt(), lineEidt[3]->text());
    productInfo.insert(key,p);
    update();

    QList<QString> pinfo;
    pinfo << p->getProductSort() << p->getProductName() << QString::number(p->getProductPrice());
    emit productModified(key, pinfo);

    for (int i = 0 ; i < 4; i++)    lineEidt[i]->clear();
}

void ProductHandlerForm::orderAddedProduct(int pid)
{
    QList<QString> pinfo;
    pinfo << productInfo[pid]->getProductSort() << productInfo[pid]->getProductName()
          << QString::number(productInfo[pid]->getProductPrice());
    emit addReturn(pinfo);
}

void ProductHandlerForm::orderSearchedProduct(int pid)
{
    QList<QString> pinfo;
    pinfo << productInfo[pid]->getProductSort() << productInfo[pid]->getProductName()
          << QString::number(productInfo[pid]->getProductPrice());
    emit searchReturn(pinfo);
}

void ProductHandlerForm::orderModifiedProduct(int pid, int row)
{
    QList<QString> pinfo;
    pinfo << productInfo[pid]->getProductSort() << productInfo[pid]->getProductName()
          << QString::number(productInfo[pid]->getProductPrice());
    emit modifyReturn(pinfo, row);
}
