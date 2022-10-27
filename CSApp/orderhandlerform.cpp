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

    while (!in.atEnd())                                                     //스트림의 끝까지 반복
    {
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
    QFile file("orderinfo.txt");                                            //파일 출력을 위한 파일 생성
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))                 //파일 열기 예외처리
        return;

    QTextStream out(&file);
    Q_FOREACH(const auto& v, orderInfo)                                     //저장된 모든 고객 정보를
    {                                                                       //", "를 기준으로 분리
        OrderInformaiton* o = v;
        out << orderInfo.key(o) << ", " << o->getOrderDate() << ", ";
        out << o->getOrderNumber() << ", " << o->getCID() << ", " << o->getPID() << "\n";
    }
    file.close();                                                           //파일 출력 종료

    delete Oui;                                                             //생성자에서 만든 포인터 객체 소멸
}

void OrderHandlerForm::dataload()                                           //주문 정보 클래스에서 채울 수 없는
{                                                                           //고객 정보, 제품 정보를 시그널로 불러서 채우는 함수
    Q_FOREACH(const auto& v, orderInfo)                                     //저장된 주문 정보의 수만큼 반복
    {
        /*주문 정보에 담김 고객 ID와 제품 ID를 시그널로 방출*/
        OrderInformaiton* o = v;
        int cid = o->getCID();
        int pid = o->getPID();
        emit orderAddedClient(cid);
        emit orderAddedProduct(pid);
    }
    emit clientComboBox(Oui->clientIDComboBox1,                             //고객 정보와 관련된 콤보박스를 시그널로 방출
                        Oui->clientInfoComboBox);
    emit productComboBox(Oui->productIDComboBox1,                           //제품 정보와 관련된 콤보박스를 시그널로 방출
                         Oui->productInfoComboBox);
}

int OrderHandlerForm::makeoid()                                             //주문 ID를 생성하는 함수
{
    if(orderInfo.isEmpty())    return 100001;                               //첫 번째 주문 ID: 100001
    else    return orderInfo.size() + 100001;                               //이후 순차적으로 +1
}

void OrderHandlerForm::on_enrollPushButton_clicked()                        //등록 버튼을 눌렀을 때
{
    int key = makeoid();                                                    //주문 id 생성 후 저장

    /*LineEdit에 입력된 내용을 각 변수에 맞게 저장*/
    QString date = Oui->orderDateLineEdit1->text();
    int quantity = Oui->orderQuantityLineEdit1->text().toInt();
    int cid = Oui->clientIDComboBox1->currentText().toInt();
    int pid = Oui->productIDComboBox1->currentText().toInt();

    int row = Oui->tableWidget1->rowCount();                                //현재 테이블 위젯의 행의 갯수를 저장
    Oui->tableWidget1->setRowCount(Oui->tableWidget1->rowCount()+1);        //테이블 위젯의 행을 하나 늘림

    /*각 열에 맞는 정보를 테이블 위젯에 삽입*/
    Oui->tableWidget1->setItem(row, 0, new QTableWidgetItem
                                            (QString::number(key)));
    Oui->tableWidget1->setItem(row, 1, new QTableWidgetItem(date));
    Oui->tableWidget1->setItem(row, 2, new QTableWidgetItem
                                            (QString::number(quantity)));
    Oui->tableWidget1->setItem(row, 3, new QTableWidgetItem
                                            (QString::number(cid)));
    Oui->tableWidget1->setItem(row, 4, new QTableWidgetItem
                                            (QString::number(pid)));

    QVector<QTableWidget*> table;
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;   //입력돼야 하는 테이블 모음

    for(int x = 0; x < 3; x++)                                              //테이블 위젯의 갯수만큼 반복
    {
        table[x]->setRowCount(table[x]->rowCount()+1);                      //테이블 위젯의 행을 하나 늘림

        /*각 열에 맞는 정보를 테이블 위젯에 삽입*/
        table[x]->setItem(row, 0, new QTableWidgetItem
                                        (QString::number(key)));
        table[x]->setItem(row, 1, new QTableWidgetItem(date));
        table[x]->setItem(row, 8, new QTableWidgetItem
                                        (QString::number(quantity)));
    }

    OrderInformaiton *o = new OrderInformaiton(key, date,                   //주문 정보 객체 생성
                                               quantity, cid, pid);

    orderInfo.insert(key, o);                                               //id를 key로 정보 저장

    /*주문정보가 추가됐다는 시그널을 고객 정보 클래스와 제품 정보 클래스에 방출*/
    emit orderAddedClient(cid);
    emit orderAddedProduct(pid);

    update();                                                               //테이블 위젯 정보 최신화
    /*입력란 초기화*/
    Oui->orderDateLineEdit1->clear();
    Oui->orderQuantityLineEdit1->clear();

    /*콤보박스 상태 초기화*/
    Oui->clientIDComboBox1->setCurrentText(tr("select item"));
    Oui->productIDComboBox1->setCurrentText(tr("select item"));
}

void OrderHandlerForm::addReturnClient(QList<QString> cinfo)                //등록 탭에서 보낸 시그널로 넘어온 고객 정보로
{                                                                           //테이블 위젯을 채우는 슬롯함수

    QVector<QTableWidget*> table;                                           //입력이 필요한 테이블 위젯 모음
    table << Oui->tableWidget2 << Oui->tableWidget4
          << Oui->tableWidget5;

    /*넘어온 고객 정보를 각 변수로 저장 */
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    for(int x = 0; x < 3; x++)                                              //테이블 위젯의 수만큼 반복
    {
        /*각 열에 맞는 정보를 테이블 위젯에 삽입, cnt는 행을 카운트하는 멤버변수*/
        table[x]->setItem(cnt, 2, new QTableWidgetItem(name));
        table[x]->setItem(cnt, 3, new QTableWidgetItem(phoneNum));
        table[x]->setItem(cnt, 4, new QTableWidgetItem(address));
    }
}

void OrderHandlerForm::addReturnProduct(QList<QString> pinfo)               //등록 탭에서 보낸 시그널로 넘어온 제품 정보로
{                                                                           //테이블 위젯을 채우는 슬롯함수

    QVector<QTableWidget*> table;                                           //입력이 필요한 테이블 위젯 모음
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    /*넘어온 고객 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();

    int quantity = table[0]->item(cnt,8)->text().toInt();                   //총 가격을 계산하기 위한 수량

    for(int x = 0; x < 3; x++)                                              //테이블 위젯의 수만큼 반복
    {
        /*각 열에 맞는 정보를 테이블 위젯에 삽입, cnt는 행을 카운트하는 멤버변수*/
        table[x]->setItem(cnt, 5, new QTableWidgetItem(sort));
        table[x]->setItem(cnt, 6, new QTableWidgetItem(name));
        table[x]->setItem(cnt, 7, new QTableWidgetItem
                                        (QString::number(price)));
        table[x]->setItem(cnt, 9, new QTableWidgetItem
                                        (QString::number(price * quantity)));
    }
    cnt++;                                                                  //다음 행 입력을 위한 cnt 증가
}

void OrderHandlerForm::on_searchPushButton_clicked()                        //검색 버튼을 눌렀을 때
{
    QTableWidget *table = Oui->tableWidget3;                                //검색 탭에 있는 테이블 위젯 저장
    table->setRowCount(0);                                                  //이전 검색결과 초기화
    int key = Oui->searchLineEdit->text().toInt();                          //입력된 키 값 저장

    if(orderInfo[key])                                                      //입력된 키가 저장된 정보에 있으면
    {
        QVector<QString> v;                                                 //주문 정보를 QString 형태로 저장
        v << orderInfo[key]->getOrderDate()
                 << QString::number(orderInfo[key]->getOrderNumber());

        int row = table->rowCount();
        table->setRowCount(table->rowCount()+1);                            //테이블에 데이터가 들어갈 행 생성
        table->setItem(row, 0, new QTableWidgetItem(QString::number(key))); //주문id를 테이블에 삽입
        table->setItem(row, 1, new QTableWidgetItem(v[0]));                 //주문 일자를 테이블에 삽입
        table->setItem(row, 8, new QTableWidgetItem(v[1]));                 //주문 수량을 테이블에 삽입

        emit orderSearchedClient(orderInfo[key]->getCID());                 //고객 정보 클래스에 고객 정보를
                                                                            //요청하는 시그널 방출
        emit orderSearchedProduct(orderInfo[key]->getPID());                //제품 정보 클래스에 제품 정보를
                                                                            //요청하는 시그널 방출
    }
    update();                                                               //테이블 위젯 정보 최신화
    Oui->searchLineEdit->clear();                                           //입력란 초기화
}

void OrderHandlerForm::searchReturnClient(QList<QString> cinfo)             //검색 탭에서 보낸 시그널로 넘어온 고객 정보로
{                                                                           //테이블 위젯을 채우는 슬롯함수

    QTableWidget *table = Oui->tableWidget3;                                //검색 탭에 있는 테이블 위젯 저장

    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    int row = table->rowCount() - 1;                                        //정보가 들어갈 행 저장

    /*각 열에 맞는 정보를 테이블 위젯에 삽입*/
    table->setItem(row, 2, new QTableWidgetItem(name));
    table->setItem(row, 3, new QTableWidgetItem(phoneNum));
    table->setItem(row, 4, new QTableWidgetItem(address));
}

void OrderHandlerForm::searchReturnProduct(QList<QString> pinfo)            //검색 탭에서 보낸 시그널로 넘어온 제품 정보로
{                                                                           //테이블 위젯을 채우는 슬롯함수

    QTableWidget *table = Oui->tableWidget3;                                //검색 탭에 있는 테이블 위젯 저장

    /*넘어온 고객 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();

    int row = table->rowCount() - 1;                                        //정보가 들어갈 행 저장
    int quantity = table->item(row,8)->text().toInt();                      //총 가격을 계산하기 위한 현재 행의 주문 수량

    /*각 열에 맞는 정보를 테이블 위젯에 삽입*/
    table->setItem(row, 5, new QTableWidgetItem(sort));
    table->setItem(row, 6, new QTableWidgetItem(name));
    table->setItem(row, 7, new QTableWidgetItem(QString::number(price)));
    table->setItem(row, 9, new QTableWidgetItem(QString::number(price * quantity)));

}

void OrderHandlerForm::on_removePushButton_clicked()                        //삭제 버튼을 눌렀을 때
{
    QVector<QTableWidget*> table;                                           //삭제될 정보가 있는 테이블 위젯 모음
    table << Oui->tableWidget1 << Oui->tableWidget2
          << Oui->tableWidget4 << Oui->tableWidget5;

    int row = table[2]->currentRow();                                       //선택된 주문 정보의 현재 행 저장
    int key =table[2]->item(row,0)->text().toInt();                         //삭제될 창에서 선택된 주문 정보의 ID 저장

    orderInfo.remove(key);                                                  //ID로 저장된 주문 정보 삭제

    for(int i = 0; i < 5; i++)                                              //테이블 위젯1의 모든 정보 삭제
    {
        table[0]->takeItem(row, i);
    }

    for(int i = 1; i < 4; i++)                                              //3개의 테이블에 대해서 반복
    {
        for(int j = 0; j < 10; j++)                                         //테이블 위젯의 열 갯수만큼 반복
        {
            table[i]->takeItem(table[2]->currentRow(),j);
        }
    }
    update();                                                               //테이블 위젯 정보 최신화
}

void OrderHandlerForm::on_modifyPushButton_clicked()                        //수정 버튼을 눌렀을 때
{
    QVector<QTableWidget*> table;                                           //삭제될 정보가 있는 테이블 위젯 모음
    table << Oui->tableWidget1 << Oui->tableWidget2
          << Oui->tableWidget4 << Oui->tableWidget5;

    QVector<QLineEdit*> lineEidt;                                           //수정될 정보가 적힌 LineEdit 위젯 모음
    lineEidt << Oui->orderIDLineEdit << Oui->orderDateLineEdit2
             << Oui->orderQuantityLineEdit2;

    /*현재 lineEdit에 있는 정보를 각 변수에 저장*/
    int key = lineEidt[0]->text().toInt();
    QString date = lineEidt[1]->text();
    int quantity = lineEidt[2]->text().toInt();

    int row = table[3]->currentRow();                                       //현재 선택된 아이템의 행을 저장

    for(int x = 1; x < 4; x++)                                              //3개의 테이블에 대해서 반복
    {
        table[x]->setItem(row, 1, new QTableWidgetItem(date));              //현재 입력된 날짜로 변경
        table[x]->setItem(row, 8, new QTableWidgetItem                      //현재 입력된 수량으로 변경
                                        (QString::number(quantity)));
    }

    int cBoxIndex = Oui->clientInfoComboBox->currentIndex();                //고객 정보 콤보박스에서 현재 선택된
                                                                            //데이터의 index 출력
    int pBoxIndex = Oui->productInfoComboBox->currentIndex();               //제품 정보 콤보박스에서 현재 선택된
                                                                            //데이터의 index 출력
    /*고객 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용*/
    int cid = Oui->clientIDComboBox1->itemText(cBoxIndex).toInt();          //고객 ID 콤보박스에서 현재 선택된
                                                                            //데이터의 cid를 출력
    /*제품 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용*/
    int pid = Oui->productIDComboBox1->itemText(pBoxIndex).toInt();         //제품 ID 콤보박스에서 현재 선택된
                                                                            //데이터의 pid를 출력

    emit orderModifiedClient(cid, row);                                     //고객 정보 클래스에 주문 정보 변경에
                                                                            //필요한 고객 정보를 요청하는 시그널 방출
    emit orderModifiedProduct(pid, row);                                    //제품 정보 클래스에 주문 정보 변경에
                                                                            //필요한 제품 정보를 요청하는 시그널 방출

    /*테이블 위젯1에 각 열에 맞는 정보 삽입*/
    table[0]->setItem(row, 1, new QTableWidgetItem(date));
    table[0]->setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
    table[0]->setItem(row, 3, new QTableWidgetItem(QString::number(cid)));
    table[0]->setItem(row, 4, new QTableWidgetItem(QString::number(pid)));

    OrderInformaiton *o = new OrderInformaiton(key, date,                   //수정된 내용으로 새로운 OrderInformation 객체 생성
                                               quantity, cid, pid);
    orderInfo.insert(key, o);                                               //주문 정보 배열에 저장

    update();                                                               //테이블 위젯 정보 최신화

    for (int i = 0 ; i < 3; i++)    lineEidt[i]->clear();                   //입력란 초기화
}

void OrderHandlerForm::modifyReturnClient(QList<QString> cinfo, int row)    //수정 탭에서 보낸 시그널로 넘어온 고객 정보로
{                                                                           //테이블 위젯을 채우는 슬롯함수

    QVector<QTableWidget*> table;                                           //수정이 필요한 테이블 위젯 모음
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    for(int x = 0; x < 3; x++)                                              //테이블 위젯의 갯수만큼 반복
    {
        /*테이블 위젯에 각 열에 맞는 정보 삽입*/
        table[x]->setItem(row, 2, new QTableWidgetItem(name));
        table[x]->setItem(row, 3, new QTableWidgetItem(phoneNum));
        table[x]->setItem(row, 4, new QTableWidgetItem(address));
    }
}

void OrderHandlerForm::modifyReturnProduct(QList<QString> pinfo, int row)
{
    QVector<QTableWidget*> table;                                           //수정이 필요한 테이블 위젯 모음
    table << Oui->tableWidget2 << Oui->tableWidget4 << Oui->tableWidget5;

    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();
    int quantity = table[0]->item(row,8)->text().toInt();                   //총 가격을 계산하기 위한 현재 행의 주문 수량

    for(int x = 0; x < 3; x++)                                              //테이블 위젯의 갯수만큼 반복
    {
        /*테이블 위젯에 각 열에 맞는 정보 삽입*/
        table[x]->setItem(row, 5, new QTableWidgetItem(sort));
        table[x]->setItem(row, 6, new QTableWidgetItem(name));
        table[x]->setItem(row, 7, new QTableWidgetItem
                                        (QString::number(price)));
        table[x]->setItem(row, 9, new QTableWidgetItem
                                        (QString::number(price * quantity)));
    }
}

void OrderHandlerForm::on_tableWidget5_itemClicked(QTableWidgetItem *item)  //수정할 주문 정보를 선택했을 경우
{
    QVector<QLineEdit*> lineEdit;                                           //현재 주문 정보를 대입할 LineEdit 위젯 저장
    lineEdit << Oui->orderIDLineEdit << Oui->orderDateLineEdit2
             << Oui->orderQuantityLineEdit2;
    int row = item->row();                                                  //현재 행을 저장

    /*콤보박스에 들어갈 고객, 제품 정보를 저장*/
    QString cName = Oui->tableWidget5->item(row, 2)->text();
    QString cPhoneNum = Oui->tableWidget5->item(row, 3)->text();
    QString pSort = Oui->tableWidget5->item(row, 5)->text();
    QString pName = Oui->tableWidget5->item(row, 6)->text();

    /*현재 선택된 테이블 위젯의 정보를 각 위젯에 설정*/
    lineEdit[0]->setText(Oui->tableWidget5->item(row, 0)->text());
    lineEdit[1]->setText(Oui->tableWidget5->item(row, 1)->text());
    lineEdit[2]->setText(Oui->tableWidget5->item(row, 8)->text());
    Oui->clientInfoComboBox->setCurrentText(cName + "(" + cPhoneNum + ")"); //이름(전화번호) -> 동명이인 구분을 위함
    Oui->productInfoComboBox->setCurrentText(pName + "(" + pSort + ")");    //제품명(제품 종류) -> 제품 정보 식별을 위함
    update();                                                               //위젯 정보 업데이트
}

void OrderHandlerForm::clientAdded()                                        //고객 정보가 추가 됐다는 시그널을 받는 슬롯함수
{
    emit clientComboBox(Oui->clientIDComboBox1,                             //콤보박스에 고객 정보를 추가하기 위한 시그널 방출
                        Oui->clientInfoComboBox);
}

void OrderHandlerForm::productAdded()                                       //제품 정보가 추가 됐다는 시그널을 받는 슬롯함수
{
    emit productComboBox(Oui->productIDComboBox1,                           //콤보박스에 제품 정보를 추가하기 위한 시그널 방출
                         Oui->productInfoComboBox);
}

void OrderHandlerForm::clientRemoved(int cid)                               //고객 정보가 삭제됐다는 시그널을 받은 슬롯함수
{
    QVector<QTableWidget*> table;                                           //삭제될 정보가 있는 테이블 위젯 모음
    table << Oui->tableWidget1 << Oui->tableWidget2
          << Oui->tableWidget4 << Oui->tableWidget5;

    QVector<int> keys;                                                      //삭제된 고객 id가 포함된 주문 정보의
                                                                            //id를 저장하는 배열

    Q_FOREACH(auto v, Oui->tableWidget1->findItems                          //삭제된 고객 id가 포함된 아이템의 수만큼 반복
              (QString::number(cid), Qt::MatchExactly))
    {
        int key = table[0]->item(v->row(),0)->text().toInt();               //삭제될 아이템에서 주문 id 저장
        keys << key;                                                        //주문 id를 배열에 저장

        /*테이블 위젯1에서 해당 아이템 제거, key-100001은 id 값을 이용헤 행을 도출하는 부분*/
        table[0]->takeItem(key-100001, 0);
        table[0]->takeItem(key-100001, 1);
        table[0]->takeItem(key-100001, 2);
        table[0]->takeItem(key-100001, 3);
        table[0]->takeItem(key-100001, 4);

        for(int i = 1; i < 4; i++)                                          //3개의 테이블에 대해서 반복
        {
            for(int j = 0; j < 10; j++)                                     //테이블 위젯의 열 갯수만큼 반복
            {
                table[i]->takeItem(key-100001, j);
            }
        }
    }

    Q_FOREACH(auto v, keys)                                                 //배열에 저장된 정보의 수만큼 반복
    {
        orderInfo.take(v);                                                  //ID로 저장된 주문 정보 삭제
    }
}

void OrderHandlerForm::productRemoved(int pid)                              //제품 정보가 삭제됐다는 시그널을 받은 슬롯함수
{
    QVector<QTableWidget*> table;                                           //삭제될 정보가 있는 테이블 위젯 모음
    table << Oui->tableWidget1 << Oui->tableWidget2
          << Oui->tableWidget4 << Oui->tableWidget5;
    QVector<int> keys;                                                      //삭제된 제품 id가 포함된 주문 정보의
                                                                            //id를 저장하는 배열

    Q_FOREACH(auto v, Oui->tableWidget1->findItems                          //삭제된 제품 id가 포함된 아이템의 수만큼 반복
              (QString::number(pid), Qt::MatchExactly))
    {
        int key = table[0]->item(v->row(),0)->text().toInt();               //삭제될 아이템의 주문 id 저장
        keys << key;                                                        //주문 id를 배열에 저장

        /*테이블 위젯1에서 해당 아이템 제거, key-100001은 id 값을 이용헤 행을 도출하는 부분*/
        table[0]->takeItem(key-100001, 0);
        table[0]->takeItem(key-100001, 1);
        table[0]->takeItem(key-100001, 2);
        table[0]->takeItem(key-100001, 3);
        table[0]->takeItem(key-100001, 4);

        for(int i = 1; i < 4; i++)                                          //3개의 테이블에 대해서 반복
        {
            for(int j = 0; j < 10; j++)                                     //테이블 위젯의 열 갯수만큼 반복
            {
                table[i]->takeItem(key-100001, j);
            }
        }
    }

    Q_FOREACH(auto v, keys)                                                 //ID로 저장된 주문 정보 삭제
    {
        orderInfo.take(v);
    }
}

void OrderHandlerForm::clientModified(int cid, QList<QString> cinfo)        //고객 정보가 수정됐다는 시그널을 받는 슬롯함수
{
    QVector<QTableWidget*> table;                                           //수정될 정보가 있는 테이블 위젯 모음
    table << Oui->tableWidget1 << Oui->tableWidget2
          << Oui->tableWidget4 << Oui->tableWidget5;

    QVector<int> keys;                                                      //수정된 고객 id가 포함된 주문 정보의
                                                                            //id를 저장하는 배열

    QVector<int> rows;                                                      //수정된 고객 id가 포함된 주문 정보의
                                                                            //행을 저장하는 배열

    Q_FOREACH(auto v, Oui->tableWidget1->findItems                          //수정된 고객 id가 포함된 아이템의 수만큼 반복
              (QString::number(cid), Qt::MatchExactly))
    {
        int key = table[0]->item(v->row(),0)->text().toInt();               //수정될 아이템의 주문 id 저장
        keys << key;                                                        //주문 id를 배열에 저장

        int row = v->row();                                                 //수정될 아이템의 행 저장
        rows << row;                                                        //행을 배열에 저장
    }

    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    for(int x = 0; x < keys.length(); x++)                                  //수정될 아이템의 주문 id의 수만큼 반복
    {
        int row = rows[x];                                                  //해당 아이템의 행 저장

        for(int i = 1; i < 4; i++)                                          //3개의 테이블 위젯에 대해서 반복
        {
            /*테이블 위젯에 각 열에 맞는 정보 삽입*/
            table[i]->setItem(row, 2, new QTableWidgetItem(name));
            table[i]->setItem(row, 3, new QTableWidgetItem(phoneNum));
            table[i]->setItem(row, 4, new QTableWidgetItem(address));
        }
    }
    emit clientComboBox(Oui->clientIDComboBox1, Oui->clientInfoComboBox);   //수정된 정보를 콤보박스에도 적용하기 위한 시그널 방출
}

void OrderHandlerForm::productModified(int pid, QList<QString> pinfo)       //주문 정보가 수정됐다는 시그널을 받는 슬롯함수
{
    QVector<QTableWidget*> table;                                           //수정될 정보가 있는 테이블 위젯 모음
    table << Oui->tableWidget1 << Oui->tableWidget2
          << Oui->tableWidget4 << Oui->tableWidget5;

    QVector<int> keys;                                                      //수정된 제품 id가 포함된 주문 정보의
                                                                            //id를 저장하는 배열

    QVector<int> rows;                                                      //수정된 제품 id가 포함된 주문 정보의
                                                                            //행을 저장하는 배열

    Q_FOREACH(auto v, Oui->tableWidget1->findItems                          //수정된 제품 id가 포함된 아이템의 수만큼 반복
              (QString::number(pid), Qt::MatchExactly))
    {
        int key = table[0]->item(v->row(),0)->text().toInt();               //수정될 아이템의 주문 id 저장
        keys << key;                                                        //주문 id를 배열에 저장

        int row = v->row();                                                 //수정될 아이템의 행 저장
        rows << row;                                                        //행을 배열에 저장
    }

    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();

    for(int x = 0; x < keys.length(); x++)                                  //수정될 아이템의 주문 id의 수만큼 반복
    {
        int row = rows[x];                                                  //해당 아이템의 행 저장
        int quantity = table[0]->item(row,8)->text().toInt();               //총 가격을 계산하기 위한 현재 행의 주문 수량

        for(int i = 1; i < 4; i++)                                          //3개의 테이블 위젯에 대해서 반복
        {
            /*테이블 위젯에 각 열에 맞는 정보 삽입*/
            table[i]->setItem(row, 5, new QTableWidgetItem(sort));
            table[i]->setItem(row, 6, new QTableWidgetItem(name));
            table[i]->setItem(row, 7, new QTableWidgetItem
                                            (QString::number(price)));
            table[i]->setItem(row, 9, new QTableWidgetItem
                                            (QString::number(price * quantity)));
        }
    }
    emit productComboBox(Oui->productIDComboBox1, Oui->productInfoComboBox);    //수정된 정보를 콤보박스에도 적용하기
                                                                                //위한 시그널 방출
}
