#include "clienthandlerform.h"
#include "ui_clienthandlerform.h"
#include <QTableWidgetItem>
#include <QList>
#include <QFile>
#include <QComboBox>

ClientHandlerForm::ClientHandlerForm(QWidget *parent) :
    QWidget(parent),
    Cui(new Ui::ClientHandlerForm)
{
    Cui->setupUi(this);

    QFile file("clientinfo.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QVector<QTableWidget*> table;
    table << Cui->tableWidget1 << Cui->tableWidget2 << Cui->tableWidget4 << Cui->tableWidget5;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size())
        {
            int id = row[0].toInt();
            ClientInformaiton* c = new ClientInformaiton(id, row[1], row[2], row[3], row[4], row[5]);
            for(int x = 0; x < 4; x++)
            {
                table[x]->setRowCount(table[x]->rowCount()+1);
                table[x]->setItem(table[x]->rowCount()-1, 0, new QTableWidgetItem(QString::number(id)));
                for (int i = 0 ; i < 5; i++)
                {
                    table[x]->setItem(table[x]->rowCount()-1, i+1, new QTableWidgetItem(row[i+1]));
                }
            }
            clientInfo.insert(id, c);
        }
    }
    file.close( );
}

ClientHandlerForm::~ClientHandlerForm()
{
    QFile file("clientinfo.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    Q_FOREACH(const auto& v, clientInfo)
    {
        ClientInformaiton* c = v;
        out << clientInfo.key(c) << ", " << c->getName() << ", ";
        out << c->getBirthday() << ", " << c->getPhoneNumber() << ", ";
        out << c->getAddress() << ", " << c->getEmail() << "\n";
    }
    file.close( );
    delete Cui;
}

void ClientHandlerForm::dataload()
{
    QList<QString> clist;
    Q_FOREACH(auto c, clientInfo)
    {
        clist << c->getName();
        clientInfo[clientInfo.key(c)]->getName();
    }

    emit clientLoad(clist);
}

int ClientHandlerForm::makecid()
{
    if(clientInfo.isEmpty())    return 5001;
    else    return clientInfo.size() + 5001;
}
void ClientHandlerForm::setclientComboBox(QComboBox* CidBox, QComboBox* CinfoBox)
{
    Q_FOREACH(auto i, clientInfo)
    {
        int key = clientInfo.key(i);
        QString name = clientInfo[key]->getName();
        QString phoneNum = clientInfo[key]->getPhoneNumber();

        if(CidBox->findText(QString::number(key)) < 0)
            CidBox->addItem(QString::number(key));

        if(CinfoBox->findText(name + "(" + phoneNum + ")") < 0)
            CinfoBox->addItem(name + "(" + phoneNum + ")");
    }
}

void ClientHandlerForm::on_enrollPushButton_clicked()
{
    QVector<QTableWidget*> table;
    table << Cui->tableWidget1 << Cui->tableWidget2 << Cui->tableWidget4 << Cui->tableWidget5;

    QVector<QLineEdit*> lineEidt;
    lineEidt << Cui->nameLineEdit1 << Cui->birthdayLineEdit1 << Cui->phoneNumLineEdit1 <<
         Cui->addressLineEdit1 << Cui->emailLineEdit1;
    int key = makecid();
    int row = Cui->tableWidget1->rowCount();
    for(int x = 0; x < 4; x++)
    {
        table[x]->setRowCount(table[x]->rowCount()+1);
        for (int i = 0 ; i < 5; i++)
        {
            QString s = lineEidt[i]->text();
            table[x]->setItem(row, 0, new QTableWidgetItem(QString::number(key)));
            table[x]->setItem(row, i+1, new QTableWidgetItem(s));
        }
    }

    ClientInformaiton *c = new ClientInformaiton(key, lineEidt[0]->text(), lineEidt[1]->text(),
            lineEidt[2]->text(), lineEidt[3]->text(), lineEidt[4]->text());

    clientInfo.insert(key, c);
    update();
    emit clientAdded(key);

    for (int i = 0 ; i < 5; i++)    lineEidt[i]->clear();
}

void ClientHandlerForm::on_searchPushButton_clicked()
{
    QTableWidget *table = Cui->tableWidget3;
    table->update();
    table->setRowCount(0); //이전 검색결과 초기화
    int key = Cui->searchLineEdit->text().toInt(); //입력된 키 값 저장

    if(clientInfo[key]) //입력된 키가 저장된 정보에 있으면
    {
        QVector<QString> v;
        v << clientInfo[key]->getName() << clientInfo[key]->getBirthday() << clientInfo[key]->getPhoneNumber() <<
             clientInfo[key]->getAddress() << clientInfo[key]->getEmail();

        int row = table->rowCount();
        table->setRowCount(table->rowCount()+1); //테이블에 데이터가 들어갈 행 생성
        table->setItem(row, 0, new QTableWidgetItem(QString::number(key))); //고객id 테이블에 삽입

        for(int i = 0; i < 5; i++)
            table->setItem(row, i+1, new QTableWidgetItem(v[i])); //나머지 고객 정보 테이블에 삽입
    }
    update();
    Cui->searchLineEdit->clear();
}


void ClientHandlerForm::on_removePushButton_clicked()
{

    QVector<QTableWidget*> table;
    table << Cui->tableWidget1 << Cui->tableWidget2 << Cui->tableWidget4 << Cui->tableWidget5;

    int key =table[2]->item(table[2]->currentRow(),0)->text().toInt();
    emit clientRemoved(key);

    clientInfo.remove(key);
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            table[i]->takeItem(table[2]->currentRow(),j);
        }
    }
    update();
}


void ClientHandlerForm::on_modifyPushButton_clicked()
{
    QVector<QTableWidget*> table;
    table << Cui->tableWidget1 << Cui->tableWidget2 << Cui->tableWidget4 << Cui->tableWidget5;
    QVector<QLineEdit*> lineEidt;
    lineEidt << Cui->idLineEdit << Cui->nameLineEdit2 << Cui->birthdayLineEdit2
      << Cui->phoneNumLineEdit2 << Cui->addressLineEdit2 << Cui->emailLineEdit2;
    int key = lineEidt[0]->text().toInt();
    int row = table[3]->currentRow();

    for(int x = 0; x < 4; x++)
    {
        for(int i = 1; i <= 5; i++)
        {
            table[x]->setItem(row, i, new QTableWidgetItem(lineEidt[i]->text()));
        }
    }

    ClientInformaiton *c = new ClientInformaiton(key, lineEidt[1]->text(), lineEidt[2]->text(),
            lineEidt[3]->text(), lineEidt[4]->text(), lineEidt[5]->text());
    clientInfo.insert(key,c);
    update();

    QList<QString> cinfo;
    cinfo << c->getName() << c->getPhoneNumber() << c->getAddress();
    emit clientModified(key, cinfo);
    for (int i = 0 ; i < 6; i++)    lineEidt[i]->clear();
}


void ClientHandlerForm::on_tableWidget5_itemClicked(QTableWidgetItem *item)
{
    QVector<QLineEdit*> lineEidt;
    lineEidt << Cui->idLineEdit << Cui->nameLineEdit2 << Cui->birthdayLineEdit2
      << Cui->phoneNumLineEdit2 << Cui->addressLineEdit2 << Cui->emailLineEdit2;
    item = Cui->tableWidget5->currentItem();

    for(int i = 0; i < 6; i++)
        lineEidt[i]->setText(Cui->tableWidget5->item(item->row(),i)->text());
    update();
}

void ClientHandlerForm::orderAddedClient(int cid)
{
    QList<QString> cinfo;
    cinfo << clientInfo[cid]->getName() << clientInfo[cid]->getPhoneNumber() << clientInfo[cid]->getAddress();
    emit addReturn(cinfo);
}

void ClientHandlerForm::ordersearchedClient(int cid)
{
    QList<QString> cinfo;
    cinfo << clientInfo[cid]->getName() << clientInfo[cid]->getPhoneNumber() << clientInfo[cid]->getAddress();
    emit searchReturn(cinfo);
}
