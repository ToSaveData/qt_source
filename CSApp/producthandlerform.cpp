#include "producthandlerform.h"
#include "ui_producthandlerform.h"
#include <QList>
#include <QFile>
#include <QTableWidgetItem>
#include <QComboBox>

ProductHandlerForm::ProductHandlerForm(QWidget *parent) :                   //생성자
    QWidget(parent),
    Pui(new Ui::ProductHandlerForm)
{
    Pui->setupUi(this);                                                     //현재 클래스에 UI를 세팅
    QFile file("productinfo.txt");                                          //파일 입력을 위한 파일 생성
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))                  //파일 열기 예외처리
        return;

    QVector<QTableWidget*> table;                                           //입력이 필요한 테이블 위젯 모음
    table << Pui->tableWidget1 << Pui->tableWidget2
          << Pui->tableWidget4 << Pui->tableWidget5;

    QTextStream in(&file);
    while (!in.atEnd())                                                     //스트림의 끝까지 반복
    {
        QString line = in.readLine();                                       //스트림을 한 줄씩 읽음
        QList<QString> row = line.split(", ");                              //", "를 기준으로 줄을 나눔
        if(row.size())                                                      //row에 데이터가 있을 경우
        {
            int id = row[0].toInt();                                        //id는 int형이므로 따로 저장
            int price = row[2].toInt();                                     //가격은 int형이므로 따로 저장
            ProductInformaiton* p = new ProductInformaiton(id, row[1],      //제품 정보 객체 조립
                                                        price, row[3]);
            for(int x = 0; x < 4; x++)                                      //테이블 위젯의 갯수만큼 반복
            {
                table[x]->setRowCount(table[x]->rowCount()+1);              //테이블 위젯의 행을 한 줄 늘림
                table[x]->setItem(table[x]->rowCount()-1, 0,                //현재 행의 0열에 id 삽입
                                  new QTableWidgetItem(QString::number(id)));
                for (int i = 0 ; i < 3; i++)                                //테이블 위젯의 열의 갯수만큼 반복
                {
                    table[x]->setItem(table[x]->rowCount()-1, i+1,
                                      new QTableWidgetItem(row[i+1]));
                }
            }
            productInfo.insert(id, p);                                      //제품 정보를 id를 키로 저장
        }
    }
    file.close( );                                                          //파일 입력 종료
}

ProductHandlerForm::~ProductHandlerForm()                                   //소멸자
{
    QFile file("productinfo.txt");                                          //파일 출력을 위한 파일 생성
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))                 //파일 열기 예외처리
        return;

    QTextStream out(&file);
    Q_FOREACH(const auto& v, productInfo)                                   //저장된 모든 고객 정보를
    {                                                                       //", "를 기준으로 분리
        ProductInformaiton* p = v;
        out << productInfo.key(p) << ", " << p->getProductName() << ", ";
        out << p->getProductPrice() << ", " << p->getProductSort() << "\n";
    }
    file.close();                                                           //파일 출력 종료
    delete Pui;                                                             //생성자에서 만든 포인터 객체 소멸
}

int ProductHandlerForm::makePid()                                           //제품 ID를 생성하는 함수
{
    if(productInfo.isEmpty())    return 1001;                               //첫 번째 제품 ID: 1001
    else    return productInfo.size() + 1001;                               //이후 순차적으로 +1
}

void ProductHandlerForm::setProductComboBox(QComboBox* PidBox, QComboBox* PinfoBox)
{                                                                           //주문 정보 클래스의 제품 정보 콤보박스 채우기
    Q_FOREACH(auto i, productInfo)                                          //저장된 제품 정보의 수만큼 반복
    {
        int key = productInfo.key(i);                                       //제품 id 추출
        QString name = productInfo[key]->getProductName();                  //제품명 추출
        QString sort = productInfo[key]->getProductSort();                  //제품 종류 추출

        if(PidBox->findText(QString::number(key)) < 0)                      //id 콤보박스에 중복되지 않도록
            PidBox->addItem(QString::number(key));                          //제품 id 추가

        if(PinfoBox->findText(name + "(" + sort + ")") < 0)                 //제품명(제품종류) 콤보박스에
            PinfoBox->addItem(name + "(" + sort + ")");                     //중복되지 않도록 고객 정보 추가
    }
}

void ProductHandlerForm::on_enrollPushButton_clicked()                      //등록 버튼을 눌렀을 때
{
    QVector<QTableWidget*> table;
    table << Pui->tableWidget1 << Pui->tableWidget2                         //입력돼야 하는 테이블 위젯 모음
          << Pui->tableWidget4 << Pui->tableWidget5;

    QVector<QLineEdit*> lineEidt;
    lineEidt << Pui->nameLineEdit1 << Pui->priceLineEdit1                   //입력된 3개의 lineEdit위젯 모음
             << Pui->sortLineEdit1;

    int key = makePid();                                                    //제품 ID 생성
    int row = Pui->tableWidget1->rowCount();                                //입력돼야 하는 행 저장

    for(int x = 0; x < 4; x++)                                              //테이블 위젯의 수를 반복
    {
        table[x]->setRowCount(table[x]->rowCount()+1);                      //입력될 새로운 행 추가
        table[x]->setItem(row, 0,                                           //제품 ID를 0열에 추가
                          new QTableWidgetItem(QString::number(key)));
        for (int i = 0 ; i < 3; i++)                                        //입력될 열의 수만큼 반복
        {
            QString s = lineEidt[i]->text();                                //입력될 데이터 추출
            table[x]->setItem(row, i+1, new QTableWidgetItem(s));           //각 열에 제품 정보 입력
        }
    }

    ProductInformaiton *p = new ProductInformaiton(key,                     //새로운 제품 정보 객체 생성
    lineEidt[0]->text(), lineEidt[1]->text().toInt(), lineEidt[2]->text());

    productInfo.insert(key, p);                                             //id를 key로 정보 저장
    update();                                                               //테이블 위젯의 정보 최신화
    emit productAdded(key);                                                 //주문 정보 클래스에 새 고객 정보가
                                                                            //추가됐다는 시그널 방출
    for (int i = 0 ; i < 3; i++)    lineEidt[i]->clear();                   //입력란 초기화
}

void ProductHandlerForm::on_searchPushButton_clicked()                      //검색 버튼을 눌렀을 때
{
    QTableWidget *table = Pui->tableWidget3;                                //검색 탭에 있는 테이블 위젯 저장
    table->setRowCount(0);                                                  //이전 검색결과 초기화
    int key = Pui->searchLineEdit->text().toInt();                          //입력된 제품 ID 값 저장

    if(productInfo[key])                                                    //입력된 키가 저장된 정보에 있으면
    {
        QVector<QString> v;                                                 //제품 정보를 QString 형태로 저장
        v << productInfo[key]->getProductName()
          << QString::number(productInfo[key]->getProductPrice())
          << productInfo[key]->getProductSort();

        int row = table->rowCount();                                        //테이블 위젯의 현재 행의 수 저장
        table->setRowCount(table->rowCount()+1);                            //테이블 위젯에 데이터가 들어갈 행 생성
        table->setItem(row, 0, new QTableWidgetItem(QString::number(key))); //제품id를 테이블 위젯에 삽입

        for(int i = 0; i < 3; i++)                                          //나머지 제품 정보 테이블 위젯에 삽입
        {
            table->setItem(row, i+1, new QTableWidgetItem(v[i]));
        }
    }
    update();                                                               //테이블 위젯 정보 최신화

    Pui->searchLineEdit->clear();                                           //입력란 초기화
}

void ProductHandlerForm::on_removePushButton_clicked()                      //삭제 버튼을 눌렀을 때
{
    QVector<QTableWidget*> table;                                           //삭제될 정보가 있는 테이블 위젯 저장
    table << Pui->tableWidget1 << Pui->tableWidget2
          << Pui->tableWidget4 << Pui->tableWidget5;

    int key = table[2]->item(table[2]->currentRow(),0)->text().toInt();     //삭제될 창에서 선택된 제품 정보의 ID 저장
    emit productRemoved(key);                                               //주문 정보 클래스에 제품 정보가
                                                                            //삭제됐다는 시그널 방출

    productInfo.remove(key);                                                //ID로 저장된 고객 정보 삭제

    for(int i = 0; i < 4; i++)                                              //테이블 위젯의 수만큼 반복
    {
        for(int j = 0; j < 4; j++)                                          //열의 갯수만큼 반복
        {
            table[i]->takeItem(table[2]->currentRow(),j);                   //현재 행의 데이터를 순차적으로 삭제
        }
    }
    update();                                                               //테이블 위젯 정보 최신화
}

void ProductHandlerForm::on_modifyPushButton_clicked()                      //수정 버튼을 눌렀을 때
{
    QVector<QTableWidget*> table;                                           //수정될 정보가 있는 테이블 위젯 모음
    table << Pui->tableWidget1 << Pui->tableWidget2
          << Pui->tableWidget4 << Pui->tableWidget5;
    QVector<QLineEdit*> lineEidt;                                           //수정될 데이터가 있는 LineEdit 위젯 모음
    lineEidt << Pui->idLineEdit << Pui->nameLineEdit2
             << Pui->priceLineEdit2 << Pui->sortLineEdit2;
    int key = lineEidt[0]->text().toInt();                                  //id를 저장
    int row = table[3]->currentRow();                                       //변경될 데이터가 있는 행 저장

    for(int x = 0; x < 4; x++)                                              //테이블 위젯의 수만큼 반복
    {
        for(int i = 1; i <= 3; i++)                                         //수정될 열의 수만큼 반복
        {
            table[x]->setItem(row, i,                                       //lineEdit에 적힌 text를 item에 삽입
                              new QTableWidgetItem(lineEidt[i]->text()));
        }
    }

    ProductInformaiton *p = new ProductInformaiton(key, lineEidt[1]->text(), //수정된 내용으로
            lineEidt[2]->text().toInt(), lineEidt[3]->text());              //새로운 ProductInformation 객체 생성
    productInfo.insert(key,p);                                              //제품 정보 배열에 저장
    update();                                                               //테이블 위젯 정보 최산화

    QList<QString> pinfo;                                                   //주문 정보 클래스에 보낼 제품 정보 배열
    pinfo << p->getProductSort() << p->getProductName()                     //제품 정보를 담음
          << QString::number(p->getProductPrice());
    emit productModified(key, pinfo);                                       //주문 정보 클래스에 제품 정보가
                                                                            //수정됐다는 시그널 방출

    for (int i = 0 ; i < 4; i++)    lineEidt[i]->clear();                   //입력란 초기화
}

void ProductHandlerForm::on_tableWidget5_itemClicked(QTableWidgetItem *item) //수정할 고객 정보를 선택했을 경우
{
    QVector<QLineEdit*> lineEidt;                                            //현재 제품 정보를 대입할 LineEdit 위젯 저장
    lineEidt << Pui->idLineEdit << Pui->nameLineEdit2
             << Pui->priceLineEdit2 << Pui->sortLineEdit2;
    item = Pui->tableWidget5->currentItem();                                 //현재 선택된 아이템 저장

    for(int i = 0; i < 4; i++)                                              //LineEdit 위젯의 수만큼 반복
        lineEidt[i]->setText(Pui->tableWidget5->
                             item(item->row(),i)->text());
    update();                                                               //위젯 정보 최신화
}

void ProductHandlerForm::orderAddedProduct(int pid)                         //새로운 주문 정보를 등록할 경우
{                                                                           //고객 정보를 담아서 보내주는 슬롯함수
    QList<QString> pinfo;                                                   //고객 정보를 담을 배열
    pinfo << productInfo[pid]->getProductSort()
          << productInfo[pid]->getProductName()
          << QString::number(productInfo[pid]->getProductPrice());
    emit addReturn(pinfo);                                                   //담은 제품 정보를 시그널로 방출
}

void ProductHandlerForm::orderSearchedProduct(int pid)                       //주문 정보 클래스에서 검색할 경우
{                                                                            //필요한 제품 정보를 담아서 보내주는 슬롯함수
    QList<QString> pinfo;                                                    //제품 정보를 담을 배열
    pinfo << productInfo[pid]->getProductSort()
          << productInfo[pid]->getProductName()
          << QString::number(productInfo[pid]->getProductPrice());
    emit searchReturn(pinfo);                                                //담은 제품 정보를 시그널로 방출
}

void ProductHandlerForm::orderModifiedProduct(int pid, int row)              //주문 정보 클래스에서 주문 정보를 수정할 경우
{                                                                            //필요한 제품 정보를 담아서 보내주는 슬롯함수
    QList<QString> pinfo;                                                    //제품 정보를 담을 배열
    pinfo << productInfo[pid]->getProductSort()
          << productInfo[pid]->getProductName()
          << QString::number(productInfo[pid]->getProductPrice());
    emit modifyReturn(pinfo, row);                                           //담은 제품 정보를 시그널로 방출
}
