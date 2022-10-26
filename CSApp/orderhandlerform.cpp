#include "orderinformaiton.h"
#include "orderhandlerform.h"
#include "ui_orderhandlerform.h"
#include <QTableWidgetItem>
#include <QList>
#include <QFile>
#include <QComboBox>

OrderHandlerForm::OrderHandlerForm(QWidget *parent) :                       //생성자
    QWidget(parent),
    Oui(new Ui::OrderHandlerForm)
{
    Oui->setupUi(this);                                                     //현재 클래스에 UI를 세팅

    QFile file("orderinfo.txt");                                            //파일 입력을 위한 파일 생성
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))                  //파일 열기 예외처리
        return;

    QTextStream in(&file);

    /*콤보박스에 "선택하세요" 항목 추가*/
    Oui->clientIDComboBox1->addItem(tr("select item"));
    Oui->clientInfoComboBox->addItem(tr("select item"));
    Oui->productIDComboBox1->addItem(tr("select item"));
    Oui->productInfoComboBox->addItem(tr("select item"));

    while (!in.atEnd()) {                                                   //스트림의 끝까지 반복
        QString line = in.readLine();                                       //스트림을 한 줄씩 읽음
        QList<QString> row = line.split(", ");                              //", "를 기준으로 줄을 나눔

        QVector<QTableWidget*> table;                                       //입력이 필요한 테이블 위젯 모음
        table << Oui->tableWidget2 << Oui->tableWidget4
              << Oui->tableWidget5;
        if(row.size())                                                      //row에 데이터가 있을 경우
        {
            /*스플릿 된 데이터를 각 자료형에 맞는 변수에 저장*/
            int oId = row[0].toInt();
            QString date = row[1];
            int quantity = row[2].toInt();
            int cid = row[3].toInt();
            int pid = row[4].toInt();

            OrderInformaiton* o = new OrderInformaiton                      //주문 정보 객체 조립
                    (oId, date, quantity, cid, pid);

            int itemRow = Oui->tableWidget1->rowCount();                    //입력될 데이터의 행을 저장
            Oui->tableWidget1->setRowCount(itemRow + 1);                    //테이블 위젯의 행을 한 줄 늘림
            Oui->tableWidget1->setItem(itemRow, 0,                          //현재 행의 0열에 id 삽입
                                       new QTableWidgetItem(QString::number(oId)));

            for (int i = 0 ; i < 4; i++)                                    //테이블 위젯1의 열의 수만큼 반복
            {
                Oui->tableWidget1->setItem(itemRow, i + 1,                  //각 열에 해당하는 정보 삽입
                                           new QTableWidgetItem(row[i+1]));
            }

            /*현재 테이블 위젯에서 채워지지 않은 부분은 슬롯함수에서 채울 예정*/
            for(int x = 0; x < 3; x++)                                      //입력이 필요한 테이블 위젯의 갯수만큼 반복
            {
                table[x]->setRowCount(table[x]->rowCount()+1);
                table[x]->setItem(itemRow, 0,
                                  new QTableWidgetItem(QString::number(oId)));
                table[x]->setItem(itemRow, 1,
                                  new QTableWidgetItem(date));
                table[x]->setItem(itemRow, 8,
                                  new QTableWidgetItem(QString::number(quantity)));
            }

            orderInfo.insert(oId, o);                                       //주문 정보를 id를 키로 저장
        }
    }
    file.close( );                                                          //파일 출력 종료
}

OrderHandlerForm::~OrderHandlerForm()                                       //소멸자
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

void OrderHandlerForm::dataload()
{
    Q_FOREACH(const auto& v, orderInfo)
    {
        OrderInformaiton* o = v;
        int cid = o->getCID();
        int pid = o->getPID();
        emit orderAddedClient(cid);
        emit orderAddedProduct(pid);
    }
    emit clientComboBox(Oui->clientIDComboBox1, Oui->clientInfoComboBox);
    emit productComboBox(Oui->productIDComboBox1, Oui->productInfoComboBox);
}


int OrderHandlerForm::makeoid()
{
    if(orderInfo.isEmpty())    return 100001;
    else    return orderInfo.size() + 100001;
}

void OrderHandlerForm::clientAdded()
{
    emit clientComboBox(Oui->clientIDComboBox1, Oui->clientInfoComboBox);
}

void OrderHandlerForm::productAdded()
{
    emit productComboBox(Oui->productIDComboBox1, Oui->productInfoComboBox);
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

    QVector<QTableWidget*> table;
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    for(int x = 0; x < 3; x++)
    {
        table[x]->setRowCount(table[x]->rowCount()+1);
        table[x]->setItem(row, 0, new QTableWidgetItem(QString::number(key)));
        table[x]->setItem(row, 1, new QTableWidgetItem(date));
        table[x]->setItem(row, 8, new QTableWidgetItem(QString::number(quantity)));
    }

    OrderInformaiton *o = new OrderInformaiton(key, date, quantity, cid, pid);

    orderInfo.insert(key, o);

    emit orderAddedClient(cid);
    emit orderAddedProduct(pid);

    update();
    Oui->orderDateLineEdit1->clear();
    Oui->orderQuantityLineEdit1->clear();
    Oui->clientIDComboBox1->setCurrentText(tr("select item"));
    Oui->productIDComboBox1->setCurrentText(tr("select item"));
}

void OrderHandlerForm::addReturnClient(QList<QString> cinfo)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    for(int x = 0; x < 3; x++)
    {
        table[x]->setItem(cnt, 2, new QTableWidgetItem(name));
        table[x]->setItem(cnt, 3, new QTableWidgetItem(phoneNum));
        table[x]->setItem(cnt, 4, new QTableWidgetItem(address));
    }
}

void OrderHandlerForm::addReturnProduct(QList<QString> pinfo)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();
    int quantity = table[0]->item(cnt,8)->text().toInt();

    for(int x = 0; x < 3; x++)
    {
        table[x]->setItem(cnt, 5, new QTableWidgetItem(sort));
        table[x]->setItem(cnt, 6, new QTableWidgetItem(name));
        table[x]->setItem(cnt, 7, new QTableWidgetItem(QString::number(price)));
        table[x]->setItem(cnt, 9, new QTableWidgetItem(QString::number(price * quantity)));
    }
    cnt++;
}

void OrderHandlerForm::clientRemoved(int cid)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget1 << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;
    QVector<int> keys;

    QVector<QTableWidgetItem*> items = Oui->tableWidget1->findItems(QString::number(cid), Qt::MatchExactly);
    Q_FOREACH(auto v, items)
    {
        int key = table[0]->item(v->row(),0)->text().toInt();
        keys << key;

        table[0]->takeItem(key-100001, 0);
        table[0]->takeItem(key-100001, 1);
        table[0]->takeItem(key-100001, 2);
        table[0]->takeItem(key-100001, 3);
        table[0]->takeItem(key-100001, 4);

        for(int i = 1; i < 4; i++)
        {
            for(int j = 0; j < 10; j++)
            {
                table[i]->takeItem(key-100001, j);
            }
        }
    }

    Q_FOREACH(auto v, keys)
    {
        orderInfo.take(v);
    }
}
void OrderHandlerForm::productRemoved(int pid)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget1 << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;
    QVector<int> keys;

    QVector<QTableWidgetItem*> items = Oui->tableWidget1->findItems(QString::number(pid), Qt::MatchExactly);
    Q_FOREACH(auto v, items)
    {
        int key = table[0]->item(v->row(),0)->text().toInt();
        keys << key;

        table[0]->takeItem(key-100001, 0);
        table[0]->takeItem(key-100001, 1);
        table[0]->takeItem(key-100001, 2);
        table[0]->takeItem(key-100001, 3);
        table[0]->takeItem(key-100001, 4);

        for(int i = 1; i < 4; i++)
        {
            for(int j = 0; j < 10; j++)
            {
                table[i]->takeItem(key-100001, j);
            }
        }
    }

    Q_FOREACH(auto v, keys)
    {
        orderInfo.take(v);
    }
}
void OrderHandlerForm::clientModified(int cid, QList<QString> cinfo)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget1 << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    QVector<int> keys;
    QVector<int> rows;

    QVector<QTableWidgetItem*> items = Oui->tableWidget1->findItems(QString::number(cid), Qt::MatchExactly);
    Q_FOREACH(auto v, items)
    {
        int key = table[0]->item(v->row(),0)->text().toInt();
        keys << key;
        int row = v->row();
        rows << row;
    }

    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    for(int x = 0; x < keys.length(); x++)
    {
        int row = rows[x];
        for(int i = 1; i < 4; i++)
        {
            table[i]->setItem(row, 2, new QTableWidgetItem(name));
            table[i]->setItem(row, 3, new QTableWidgetItem(phoneNum));
            table[i]->setItem(row, 4, new QTableWidgetItem(address));
        }
    }
    emit clientComboBox(Oui->clientIDComboBox1, Oui->clientInfoComboBox);
}
void OrderHandlerForm::productModified(int pid, QList<QString> pinfo)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget1 << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    QVector<int> keys;
    QVector<int> rows;

    QVector<QTableWidgetItem*> items = Oui->tableWidget1->findItems(QString::number(pid), Qt::MatchExactly);
    Q_FOREACH(auto v, items)
    {
        int key = table[0]->item(v->row(),0)->text().toInt();
        keys << key;
        int row = v->row();
        rows << row;
    }

    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();


    for(int x = 0; x < keys.length(); x++) //변경된 제품의 주문정보와 관련된 모든 데이터에서 정보를 수정
    {
        int row = rows[x];
        int quantity = table[0]->item(row,8)->text().toInt();
        for(int i = 1; i < 4; i++) //테이블 위젯 2, 4, 5의 제품 관련 데이터 채우기
        {
            table[i]->setItem(row, 5, new QTableWidgetItem(sort)); //제품 종류
            table[i]->setItem(row, 6, new QTableWidgetItem(name)); //제품명
            table[i]->setItem(row, 7, new QTableWidgetItem(QString::number(price))); //제품 가격
            table[i]->setItem(row, 9, new QTableWidgetItem(QString::number(price * quantity)));
            //총 가격 = 제품 가격 * 주문수량
        }
    }
    emit productComboBox(Oui->productIDComboBox1, Oui->productInfoComboBox);
}

void OrderHandlerForm::on_tableWidget5_itemClicked(QTableWidgetItem *item)
{
    Q_UNUSED(item);
    QVector<QLineEdit*> lineEdit;
    lineEdit << Oui->orderIDLineEdit << Oui->orderDateLineEdit2 << Oui->orderQuantityLineEdit2;
    int row = Oui->tableWidget5->currentItem()->row();
    QString cName = Oui->tableWidget5->item(row, 2)->text();
    QString cPhoneNum = Oui->tableWidget5->item(row, 3)->text();
    QString pSort = Oui->tableWidget5->item(row, 5)->text();
    QString pName = Oui->tableWidget5->item(row, 6)->text();

    lineEdit[0]->setText(Oui->tableWidget5->item(row, 0)->text());
    lineEdit[1]->setText(Oui->tableWidget5->item(row, 1)->text());
    lineEdit[2]->setText(Oui->tableWidget5->item(row, 8)->text());
    Oui->clientInfoComboBox->setCurrentText(cName + "(" + cPhoneNum + ")");
    Oui->productInfoComboBox->setCurrentText(pName + "(" + pSort + ")");
    update();
}


void OrderHandlerForm::on_searchPushButton_clicked()
{
    QTableWidget *table = Oui->tableWidget3;
    table->update();
    table->setRowCount(0); //이전 검색결과 초기화
    int key = Oui->searchLineEdit->text().toInt(); //입력된 키 값 저장

    if(orderInfo[key]) //입력된 키가 저장된 정보에 있으면
    {
        QVector<QString> lineEdit;
        lineEdit << orderInfo[key]->getOrderDate() << QString::number(orderInfo[key]->getOrderNumber());

        int row = table->rowCount();
        table->setRowCount(table->rowCount()+1); //테이블에 데이터가 들어갈 행 생성
        table->setItem(row, 0, new QTableWidgetItem(QString::number(key))); //주문id를 테이블에 삽입
        table->setItem(row, 1, new QTableWidgetItem(lineEdit[0])); //주문 일자를 테이블에 삽입
        table->setItem(row, 8, new QTableWidgetItem(lineEdit[1])); //주문 수량을 테이블에 삽입
        emit orderSearchedClient(orderInfo[key]->getCID());
        emit orderSearchedProduct(orderInfo[key]->getPID());
    }
    update();
    Oui->searchLineEdit->clear();
}

void OrderHandlerForm::searchReturnClient(QList<QString> cinfo)
{
    //여기부터 해야 함
    QTableWidget *table = Oui->tableWidget3;
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];
    int row = table->rowCount() - 1;

    table->setItem(row, 2, new QTableWidgetItem(name));
    table->setItem(row, 3, new QTableWidgetItem(phoneNum));
    table->setItem(row, 4, new QTableWidgetItem(address));
}

void OrderHandlerForm::searchReturnProduct(QList<QString> pinfo)
{
    QTableWidget *table = Oui->tableWidget3;
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();
    int row = table->rowCount() - 1;
    int quantity = table->item(row,8)->text().toInt();

    table->setItem(row, 5, new QTableWidgetItem(sort)); //제품 종류
    table->setItem(row, 6, new QTableWidgetItem(name)); //제품명
    table->setItem(row, 7, new QTableWidgetItem(QString::number(price))); //제품 가격
    table->setItem(row, 9, new QTableWidgetItem(QString::number(price * quantity)));
    //총 가격 = 제품 가격 * 주문수량
}

void OrderHandlerForm::on_removePushButton_clicked()
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget1 << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    int row = table[2]->currentRow();
    int key =table[2]->item(row,0)->text().toInt();

    orderInfo.remove(key);
    for(int i = 0; i < 5; i++)
    {
        table[0]->takeItem(row, i);
    }
    for(int i = 1; i < 4; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            table[i]->takeItem(table[2]->currentRow(),j);
        }
    }
    update();
}


void OrderHandlerForm::on_modifyPushButton_clicked()
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget1 << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    QVector<QLineEdit*> lineEidt;
    lineEidt << Oui->orderIDLineEdit << Oui->orderDateLineEdit2 << Oui->orderQuantityLineEdit2;

    int key = lineEidt[0]->text().toInt();
    QString date = lineEidt[1]->text();
    int quantity = lineEidt[2]->text().toInt();
    int row = table[3]->currentRow();

    for(int x = 1; x < 4; x++)
    {
        table[x]->setItem(row, 1, new QTableWidgetItem(date));
        table[x]->setItem(row, 8, new QTableWidgetItem(QString::number(quantity)));
    }

    int cBoxIndex = Oui->clientInfoComboBox->currentIndex();    //고객 정보 콤보박스에서 현재 선택된 데이터의 index 출력
    int pBoxIndex = Oui->productInfoComboBox->currentIndex();   //제품 정보 콤보박스에서 현재 선택된 데이터의 index 출력
    int cid = Oui->clientIDComboBox1->itemText(cBoxIndex).toInt();  //고객 ID 콤보박스에서 현재 선택된 데이터의 cid를 출력
                                                            //고객 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용
    int pid = Oui->productIDComboBox1->itemText(pBoxIndex).toInt(); //제품 ID 콤보박스에서 현재 선택된 데이터의 pid를 출력
                                                            //제품 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용
    emit orderModifiedClient(cid, row);
    emit orderModifiedProduct(pid, row);
    table[0]->setItem(row, 1, new QTableWidgetItem(date));
    table[0]->setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
    table[0]->setItem(row, 3, new QTableWidgetItem(QString::number(cid)));
    table[0]->setItem(row, 4, new QTableWidgetItem(QString::number(pid)));

    OrderInformaiton *o = new OrderInformaiton(key, date, quantity, cid, pid);
    orderInfo.insert(key, o);

    update();

    for (int i = 0 ; i < 3; i++)    lineEidt[i]->clear();           //라인에디터의 입력 내용 지우기
}
void OrderHandlerForm::modifyReturnClient(QList<QString> cinfo, int row)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    for(int x = 0; x < 3; x++)
    {
        table[x]->setItem(row, 2, new QTableWidgetItem(name));
        table[x]->setItem(row, 3, new QTableWidgetItem(phoneNum));
        table[x]->setItem(row, 4, new QTableWidgetItem(address));
    }
}
void OrderHandlerForm::modifyReturnProduct(QList<QString> pinfo, int row)
{
    QVector<QTableWidget*> table;
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();
    int quantity = table[0]->item(row,8)->text().toInt();

    for(int x = 0; x < 3; x++)
    {
        table[x]->setItem(row, 5, new QTableWidgetItem(sort));
        table[x]->setItem(row, 6, new QTableWidgetItem(name));
        table[x]->setItem(row, 7, new QTableWidgetItem(QString::number(price)));
        table[x]->setItem(row, 9, new QTableWidgetItem(QString::number(price * quantity)));
    }
}
