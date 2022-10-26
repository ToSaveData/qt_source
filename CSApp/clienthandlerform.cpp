#include "clienthandlerform.h"
#include "ui_clienthandlerform.h"
#include <QTableWidgetItem>
#include <QList>
#include <QFile>
#include <QComboBox>

ClientHandlerForm::ClientHandlerForm(QWidget *parent) :     //생성자
    QWidget(parent),
    Cui(new Ui::ClientHandlerForm)
{
    Cui->setupUi(this);                                     //현재 클래스에 UI를 세팅

    QFile file("clientinfo.txt");                           //파일 입력을 위한 파일 생성
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  //파일 열기 예외처리
        return;

    QVector<QTableWidget*> table;                           //입력이 필요한 테이블 위젯 모음
    table << Cui->tableWidget1 << Cui->tableWidget2
          << Cui->tableWidget4 << Cui->tableWidget5;

    QTextStream in(&file);
    while (!in.atEnd()) {                                   //스트림의 끝까지 반복
        QString line = in.readLine();                       //스트림을 한 줄씩 읽음
        QList<QString> row = line.split(", ");              //", "를 기준으로 줄을 나눔
        if(row.size())                                      //row에 데이터가 있을 경우
        {
            int id = row[0].toInt();                        //id는 int형이므로 따로 저장
            ClientInformaiton* c = new ClientInformaiton    //고객 정보 객체 조립
                    (id, row[1], row[2], row[3], row[4], row[5]);

            for(int x = 0; x < 4; x++)                      //테이블 위젯 갯수만큼 반복
            {
                table[x]->setRowCount(table[x]->rowCount()+1);  //테이블의 행을 한 줄 늘림
                table[x]->setItem(table[x]->rowCount()-1,       //현재 행의 0열에 id 삽입
                                  0, new QTableWidgetItem(QString::number(id)));

                for (int i = 0 ; i < 5; i++)                //테이블 위젯의 열의 갯수만큼 반복
                {
                    table[x]->setItem(table[x]->rowCount()-1,
                                    i+1, new QTableWidgetItem(row[i+1]));
                }
            }

            clientInfo.insert(id, c);                       //고객 정보를 id 와 키로 저장
        }
    }
    file.close( );                                          //파일 입력 종료
}

ClientHandlerForm::~ClientHandlerForm()                     //소멸자
{
    QFile file("clientinfo.txt");                           //파일 출력을 위한 파일 생성
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) //파일 열기 예외처리
        return;

    QTextStream out(&file);
    Q_FOREACH(const auto& v, clientInfo)                    //저장된 모든 고객 정보를
    {                                                       //", "를 기준으로 분리
        ClientInformaiton* c = v;
        out << clientInfo.key(c) << ", " << c->getName() << ", ";
        out << c->getBirthday() << ", " << c->getPhoneNumber() << ", ";
        out << c->getAddress() << ", " << c->getEmail() << "\n";
    }
    file.close( );                                          //파일 출력 종료
    delete Cui;                                             //생성자에서 만든 포인터 객체 소멸
}

void ClientHandlerForm::dataload()                          //서버 클래스의 파일 입력에 필요한 정보를
{                                                           //담기 위한 슬롯 함수
    QList<QString> cNameList;
    QList<int> cIdList;
    Q_FOREACH(auto c, clientInfo)
    {
        int key = clientInfo.key(c);                        //고객 ID 추출
        cNameList << c->getName();                          //고객 성명 저장
        cIdList << key;                                     //고객 ID 저장
    }


    emit clientLoad(cIdList, cNameList);                    //서버 클래스의 고객 목록 입력에 필요한 시그널 방출
}

int ClientHandlerForm::makecid()                            //고객 ID를 생성하는 함수
{
    if(clientInfo.isEmpty())    return 5001;                //첫 번째 고객 ID: 5001
    else    return clientInfo.size() + 5001;                //이후 순차적으로 +1
}

void ClientHandlerForm::setClientComboBox(QComboBox* CidBox, QComboBox* CinfoBox)
{                                                           //주문 정보 클래스의 고객 정보 콤보박스 채우기
    Q_FOREACH(auto i, clientInfo)                           //저장된 고객 정보의 수만큼 반복
    {
        int key = clientInfo.key(i);                        //고객 id 추출
        QString name = clientInfo[key]->getName();          //고객 성명 추출
        QString phoneNum = clientInfo[key]->getPhoneNumber();   //고객 전화번호 추출

        if(CidBox->findText(QString::number(key)) < 0)      //id 콤보박스에 중복되지 않도록
            CidBox->addItem(QString::number(key));          //고객 id 추가

        if(CinfoBox->findText(name + "(" + phoneNum + ")") < 0) //이름(전화번호) 콤보박스에
            CinfoBox->addItem(name + "(" + phoneNum + ")");     //중복되지 않도록 고객 정보 추가
    }
}

void ClientHandlerForm::on_enrollPushButton_clicked()           //등록 버튼 눌렸을 때
{
    QVector<QTableWidget*> table;                               //입력돼야 하는 테이블 4개 모음
    table << Cui->tableWidget1 << Cui->tableWidget2
          << Cui->tableWidget4 << Cui->tableWidget5;

    QVector<QLineEdit*> lineEidt;                               //입력된 5개의 lineEdit위젯 모음
    lineEidt << Cui->nameLineEdit1 << Cui->birthdayLineEdit1
             << Cui->phoneNumLineEdit1 << Cui->addressLineEdit1
             << Cui->emailLineEdit1;
    int key = makecid();                                        //고객 ID 생성
    int row = Cui->tableWidget1->rowCount();                    //입력돼야 하는 행 저장

    for(int x = 0; x < 4; x++)                                  //테이블 위젯 갯수만큼 반복
    {
        table[x]->setRowCount(table[x]->rowCount()+1);          //입력될 새로운 행 추가
        table[x]->setItem(row, 0,                               //고객 ID 0번 열에 입력
                          new QTableWidgetItem(QString::number(key)));
        for (int i = 0 ; i < 5; i++)                            //입력될 열의 수만큼 반복
        {
            QString s = lineEidt[i]->text();                    //입력될 데이터 추출
            table[x]->setItem(row, i+1,                         //각 열에 고객 정보 입력
                              new QTableWidgetItem(s));
        }
    }

    ClientInformaiton *c = new ClientInformaiton(key,           //고객 정보 객체 생성
                            lineEidt[0]->text(), lineEidt[1]->text(),
                            lineEidt[2]->text(), lineEidt[3]->text(),
                            lineEidt[4]->text());

    clientInfo.insert(key, c);                                  //id를 key로 정보 저장
    update();                                                   //테이블 위젯 정보 최신화

    emit clientAdded(key);                                      //주문 정보 클래스에 새 고객 정보가
                                                                //추가 됐다는 시그널 방출
    QList<QString> cNameInfo;                                   //서버 클래스에 보낼 고객 성명을 담을 배열
    cNameInfo << lineEidt[0]->text();                           //고객 성명을 저장
    QList<int> cIdInfo;                                         //서버 클래스에 보낼 고객 ID를 담을 배열
    cIdInfo << key;                                             //고객 ID를 저장
    emit sendServer(cIdInfo, cNameInfo);                        //서버 클래스에 저장할 고객 정보 시그널 방출

    for (int i = 0 ; i < 5; i++)    lineEidt[i]->clear();       //입력란 초기화
}

void ClientHandlerForm::on_searchPushButton_clicked()           //검색 버튼 눌렸을 때
{
    QTableWidget *table = Cui->tableWidget3;                    //검색 탭에 있는 테이블 위젯 저장
    table->setRowCount(0);                                      //이전 검색결과 초기화
    int key = Cui->searchLineEdit->text().toInt();              //입력된 고객 ID 저장

    if(clientInfo[key])                                         //입력된 고객 ID가 저장된 정보에 있으면
    {
        QVector<QString> v;                                     //고객 정보를 QString 형태로 저장
        v << clientInfo[key]->getName() << clientInfo[key]->getBirthday()
          << clientInfo[key]->getPhoneNumber() << clientInfo[key]->getAddress()
          << clientInfo[key]->getEmail();

        int row = table->rowCount();                            //테이블 위젯의 현재 행의 수 저장
        table->setRowCount(table->rowCount()+1);                //검색 결과 데이터가 들어갈 행 생성
        table->setItem(row, 0,                                  //고객id 테이블에 삽입
                       new QTableWidgetItem(QString::number(key)));

        for(int i = 0; i < 5; i++)
            table->setItem(row, i+1, new QTableWidgetItem(v[i])); //나머지 고객 정보 테이블에 삽입
    }
    update();                                                   //테이블 정보 최신화
    Cui->searchLineEdit->clear();                               //입력란 초기화
}


void ClientHandlerForm::on_removePushButton_clicked()           //삭제 버튼을 눌렀을 때
{

    QVector<QTableWidget*> table;                               //삭제될 정보가 있는 테이블 위젯 저장
    table << Cui->tableWidget1 << Cui->tableWidget2
          << Cui->tableWidget4 << Cui->tableWidget5;

    int key =table[2]->item(table[2]->currentRow(),0)           //삭제될 창에서 선택된 고객 정보의 ID 저장
            ->text().toInt();
    emit clientRemoved(key);                                    //주문 정보 클래스에 고객 정보가
                                                                //삭제됐다는 시그널 방출

    clientInfo.remove(key);                                     //ID로 저장된 고객 정보 삭제

    for(int i = 0; i < 4; i++)                                  //테이블 위젯의 수만큼 반복
    {
        for(int j = 0; j < 6; j++)                              //테이블의 열 갯수만큼 반복
        {
            table[i]->takeItem(table[2]->currentRow(),j);       //현재 행의 데이터를 순차적으로 삭제
        }
    }
    update();                                                   //테이블 정보 최신화
}


void ClientHandlerForm::on_modifyPushButton_clicked()           //수정 버튼을 눌렀을 때
{
    QVector<QTableWidget*> table;                               //수정될 정보가 있는 테이블 위젯 모음
    table << Cui->tableWidget1 << Cui->tableWidget2
          << Cui->tableWidget4 << Cui->tableWidget5;
    QVector<QLineEdit*> lineEidt;                               //수정될 정보가 적힌 LineEdit 위젯 모음
    lineEidt << Cui->idLineEdit << Cui->nameLineEdit2
             << Cui->birthdayLineEdit2 << Cui->phoneNumLineEdit2
             << Cui->addressLineEdit2 << Cui->emailLineEdit2;
    int key = lineEidt[0]->text().toInt();                      //id를 저장
    int row = table[3]->currentRow();                           //변경될 데이터가 있는 행 저장

    for(int x = 0; x < 4; x++)                                  //테이블 위젯의 수만큼 반복
    {
        for(int i = 1; i <= 5; i++)                             //수정될 열의 수만큼 반복
        {
            table[x]->setItem(row, i,                           //lineEdit에 적힌 text를 item에 삽입
                              new QTableWidgetItem(lineEidt[i]->text()));
        }
    }

    ClientInformaiton *c =                                      //수정된 내용으로 새로운 ClientInformation 객체 생성
            new ClientInformaiton(key, lineEidt[1]->text(),
            lineEidt[2]->text(), lineEidt[3]->text(),
            lineEidt[4]->text(), lineEidt[5]->text());
    clientInfo.insert(key,c);                                   //고객 정보 배열에 저장
    update();                                                   //테이블 위젯 정보 최신화

    QList<QString> cinfo;                                       //주문 정보 클래스에 보낼 고객 정보 배열
    cinfo << c->getName() << c->getPhoneNumber() << c->getAddress(); //고객 정보를 담음
    emit clientModified(key, cinfo);                            //주문 정보 클래스에 고객 정보가 수정됐다는 시그널 방출
    emit sendServerCModified(key, c->getName());                //서버 클래스에 고객 정보가 수정됐다는 시그널 방출

    for (int i = 0 ; i < 6; i++)    lineEidt[i]->clear();       //입력란 초기화
}


void ClientHandlerForm::on_tableWidget5_itemClicked(QTableWidgetItem *item) //수정할 고객 정보를 선택했을 경우
{
    QVector<QLineEdit*> lineEidt;                                           //현재 고객 정보를 대입할 LineEdit 위젯 저장
    lineEidt << Cui->idLineEdit << Cui->nameLineEdit2
             << Cui->birthdayLineEdit2 << Cui->phoneNumLineEdit2
             << Cui->addressLineEdit2 << Cui->emailLineEdit2;
    item = Cui->tableWidget5->currentItem();                                //현재 선택된 아이템 저장

    for(int i = 0; i < 6; i++)                                              //lineEdit 위젯의 수만큼 반복
        lineEidt[i]->setText(Cui->tableWidget5->                            //각 lineEdit에 맞는 고객 정보 삽입
                             item(item->row(),i)->text());
    update();                                                               //lineEdit 상태 최신화
}

void ClientHandlerForm::orderAddedClient(int cid)                           //새로운 주문 정보를 등록할 경우
{                                                                           //고객 정보를 담아서 보내주는 슬롯함수
    QList<QString> cinfo;                                                   //고객 정보를 담을 배열
    cinfo << clientInfo[cid]->getName() << clientInfo[cid]->getPhoneNumber()
          << clientInfo[cid]->getAddress();
    emit addReturn(cinfo);                                                  //담은 고객 정보를 시그널로 방출
}

void ClientHandlerForm::orderSearchedClient(int cid)                        //주문 정보 클래스에서 검색할 경우
{                                                                           //필요한 고객 정보를 담아서 보내주는 슬롯함수
    QList<QString> cinfo;                                                   //고객 정보를 담을 배열
    cinfo << clientInfo[cid]->getName() << clientInfo[cid]->getPhoneNumber()
          << clientInfo[cid]->getAddress();
    emit searchReturn(cinfo);                                               //담은 고객 정보를 시그널로 방출
}

void ClientHandlerForm::orderModifiedClient(int cid, int row)               //주문 정보 클래스에서 주문 정보를 수정할 경우
{                                                                           //필요한 고객 정보를 담아서 보내주는 슬롯함수
    QList<QString> cinfo;                                                   //고객 정보를 담을 배열
    cinfo << clientInfo[cid]->getName() << clientInfo[cid]->getPhoneNumber()
          << clientInfo[cid]->getAddress();
    emit modifyReturn(cinfo, row);                                          //담은 고객 정보를 시그널로 방출
}
