#include "chatserverform.h"
#include "ui_chatserverform.h"
#include "logthread.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QIcon>

ChatServerForm::ChatServerForm(QWidget *parent) :                           //생성자
    QWidget(parent),
    ui(new Ui::ChatServerForm), totalSize(0), byteReceived(0)
{
    ui->setupUi(this);                                                      //현재 클래스에 UI를 세팅

    /*현재 UI의 스플리터 사이즈를 설정*/
    QList<int> sizes;
    sizes << 120 << 500;
    ui->splitter->setSizes(sizes);

    chatServer = new QTcpServer(this);                                      //채팅 서버에 QTcp 서버 객체 생성

    /*채팅 서버에 새로운 클라이언트가 연결되면 clientConnect 함수 실행*/
    connect(chatServer, SIGNAL(newConnection()), SLOT(clientConnect()));

    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER))                //클라이언트 소켓이 연결이 안 되면
    {
        QMessageBox::critical(this, tr("Chatting Server"),                  //경고 메세지 박스 출력
                              tr("Unable to start the server: %1.")
                              .arg(chatServer->errorString()));
        return;
    }

    fileServer = new QTcpServer(this);                                      //파일 서버에 QTcp 서버 객체 생성

    /*파일 서버에 새로운 클라이언트가 연결되면 acceptConnection 함수 실행*/
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));

    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1))              //클라이언트 소켓이 연결이 안 되면
    {
        QMessageBox::critical(this, tr("Chatting Server"),                  //경고 메세지 박스 출력
                              tr("Unable to start the server: %1.")
                              .arg(fileServer->errorString()));
        return;
    }

    qDebug() << tr("Start listening ...");                                  //디버그 메세지 출력

    /*액션 생성, 객체명 지정, 신호 연결*/
    QAction* inviteAction = new QAction(tr("&Invite"));
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), SLOT(inviteClient()));

    /*액션 생성, 객체명 지정, 신호 연결*/
    QAction* removeAction = new QAction(tr("&Kick out"));
    removeAction->setObjectName("Kick out");
    connect(removeAction, SIGNAL(triggered()), SLOT(kickOut()));

    /*컨텍스트 메뉴 객체 생성 및 액션 추가*/
    menu = new QMenu;
    menu->addAction(inviteAction);
    menu->addAction(removeAction);
    ui->waittingRoomTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->chattingRoomTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    /*progressDialog 객체 생성 및 속성 설정*/
    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();                                                //autoClose가 활성화 되면 reset시 창을 닫음

    logThread = new LogThread(this);                                        //스레드 객체 생성
    logThread->start();                                                     //스레드 실행

    connect(ui->savePushButton, SIGNAL(clicked()),                          //saveButton을 누르면 로그 저장
            logThread, SLOT(saveData()));

    qDebug() << tr("The server is running on port %1.")                     //디버그 메세지 출력
                .arg(chatServer->serverPort());
}

ChatServerForm::~ChatServerForm()                                           //소멸자
{
    delete ui;                                                              //생성자에서 만든 포인터 객체 소멸

    logThread->terminate();                                                 //스레드 소멸
    chatServer->close();                                                    //채팅 서버 객체 소멸
    fileServer->close();                                                    //파일 서버 객체 소멸
}

void ChatServerForm::addClientInfo(QList<int> cIdInfo, QList<QString> cNameInfo) //고객 정보를 서버에 추가하는 함수
{
    int cnt = 0;                                                             //고객 성명 배열의 순서를 기억하기 위한 임시 변수
    Q_FOREACH(auto i, cNameInfo)                                             //고객 성명이 저장된 수만큼 반복
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();                       //트리위젯에 삽입할 아이템 생성
        item->setIcon(0, QIcon(":/icon_image/redLight.png"));                //0열에 아이콘 삽입
        item->setText(1, i + "  ");                                          //1열에 이름과 공백 2칸 삽입(오프라인임을 의미)
        ui->waittingRoomTreeWidget->addTopLevelItem(item);                   //아이템을 트리위젯에 삽입
        clientIDHash[i] = cIdInfo[cnt++];                                    //id해쉬에 이름을 키로 id를 저장
        ui->waittingRoomTreeWidget->resizeColumnToContents(0);               //트리위젯의 열 너비를 들어간 내용을 중심으로 재설정
    }
}

void ChatServerForm::modifyClientInfo(int id, QString name)                      //고객 정보가 수정된 경우
{
    QString lastName = clientIDHash.key(id);                                 //수정 전 이름을 저장

    /*수정 전 이름을 가진 클라이언트가 접속중이라면 kick out 프로토콜을 보내 재접속 유도*/
    if(clientSocketHash.contains(lastName))
    {
        QTcpSocket* sock = clientSocketHash[lastName];
        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out << Chat_KickOut;
        out.writeRawData("", 1020);
        sock->write(sendArray);
    }

    clientIDHash[name] = id;                                                 //수정된 이름을 key로 id 저장
    clientIDHash.remove(lastName);                                           //이전 이름을 key로 하는 정보 삭제

    Q_FOREACH(auto item, ui->chattingRoomTreeWidget->                        //이전 이름이 채팅방에 있는 경우
              findItems(lastName, Qt::MatchFixedString, 1))
    {
        if(ui->chattingRoomTreeWidget->
                findItems(lastName,
                          Qt::MatchFixedString, 1).count() > 0)
            ui->chattingRoomTreeWidget->                                     //채팅방 목록에서 제거
                    takeTopLevelItem(ui->chattingRoomTreeWidget->
                                     indexOfTopLevelItem(item));
    }

    Q_FOREACH(auto item, ui->waittingRoomTreeWidget->                        //이전 이름이 대기실에 로그인 상태인 경우
              findItems(lastName + " ", Qt::MatchFixedString, 1))
    {
        if(ui->waittingRoomTreeWidget->
                findItems(lastName + " ",
                          Qt::MatchFixedString, 1).count() > 0)
            ui->waittingRoomTreeWidget->                                     //대기실 목록에서 제거
                    takeTopLevelItem(ui->waittingRoomTreeWidget->
                                     indexOfTopLevelItem(item));
    }

    Q_FOREACH(auto item, ui->waittingRoomTreeWidget->                        //이전 이름이 대기실에 로그아웃 상태인 경우
              findItems(lastName + "  ", Qt::MatchFixedString, 1))
    {
        if(ui->waittingRoomTreeWidget->
                findItems(lastName + "  ",
                          Qt::MatchFixedString, 1).count() > 0)
            ui->waittingRoomTreeWidget->                                     //대기실 목록에서 제거
                    takeTopLevelItem(ui->waittingRoomTreeWidget->
                                     indexOfTopLevelItem(item));
    }

    QTreeWidgetItem *item = new QTreeWidgetItem;                             //새로운 목록을 만들기 위한 아이템 생성
    item->setIcon(0, QIcon(":/icon_image/redLight.png"));                    //로그아웃 상태 아이콘 설정
    item->setText(1, name + "  ");                                           //변경된 이름 설정
    ui->waittingRoomTreeWidget->addTopLevelItem(item);                       //대기실에 새로운 고객 정보 추가
}

void ChatServerForm::removeClientInfo(QString name)                              //고객 정보가 삭제된 경우
{
    /*삭제된 이름을 가진 클라이언트가 접속중이라면 kick out 프로토콜을 보내 프로그램 종료를 유도*/
    if(clientSocketHash.contains(name))
    {
        QTcpSocket* sock = clientSocketHash[name];

        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out << Chat_KickOut;
        out.writeRawData("", 1020);
        sock->write(sendArray);
    }

    clientIDHash.remove(name);                                               //삭제된 이름을 key로 하는 정보 삭제

    Q_FOREACH(auto item, ui->chattingRoomTreeWidget->                        //삭제된 이름이 채팅방에 있는 경우
              findItems(name, Qt::MatchFixedString, 1))
    {
        if(ui->chattingRoomTreeWidget->
                findItems(name, Qt::MatchFixedString, 1).count() > 0)
            ui->chattingRoomTreeWidget->                                     //채팅방 목록에서 제거
                    takeTopLevelItem(ui->chattingRoomTreeWidget->
                                     indexOfTopLevelItem(item));
    }

    Q_FOREACH(auto item, ui->waittingRoomTreeWidget->                        //삭제된 이름이 대기방에 로그인 상태인 경우
              findItems(name + " ", Qt::MatchFixedString, 1))
    {
        if(ui->waittingRoomTreeWidget->
                findItems(name + " ", Qt::MatchFixedString, 1).count() > 0)
            ui->waittingRoomTreeWidget->                                     //대기실 목록에서 제거
                    takeTopLevelItem(ui->waittingRoomTreeWidget->
                                     indexOfTopLevelItem(item));
    }

    Q_FOREACH(auto item, ui->waittingRoomTreeWidget->                        //삭제된 이름이 대기방에 로그아웃 상태인 경우
              findItems(name + "  ", Qt::MatchFixedString, 1))
    {
        if(ui->waittingRoomTreeWidget->
                findItems(name + "  ", Qt::MatchFixedString, 1).count() > 0)
            ui->waittingRoomTreeWidget->                                     //대기실 목록에서 제거
                    takeTopLevelItem(ui->waittingRoomTreeWidget->
                                     indexOfTopLevelItem(item));
    }
}

void ChatServerForm::clientConnect()                                         //서버와 클라이언트 소켓을 연결하는 함수
{
    QTcpSocket *clientConnection = chatServer->nextPendingConnection();      //연결된 클라이언트 소켓과 채팅 서버를 연결

    /*소켓이 read할 준비가 되면 receiveData 함수를 연결*/
    connect(clientConnection, SIGNAL(readyRead()), SLOT(receiveData()));

    /*소켓의 연결이 끊어지면 removeClient 함수를 연결*/
    connect(clientConnection, SIGNAL(disconnected()), SLOT(removeClient()));

    qDebug() << tr("new connection is established...");                      //디버그 메세지 출력
}

void ChatServerForm::receiveData()                                           //클라이언트의 프로토콜을 받고 처리하는 함수
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender());     //클라이언트 소켓을 sender로 설정
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);               //소켓의 데이터를 blocksize(1024)만큼 read

    Chat_Status type;                                                        // 채팅의 목적
    char data[1020];                                                         // 전송되는 메시지/데이터
    memset(data, 0, 1020);                                                   //쓰레기 값을 0으로 초기화


    QDataStream in(&bytearray, QIODevice::ReadOnly);                         //스트림에 bytearray를 담음
    in.device()->seek(0);                                                    //스트림의 처음으로 위치 설정
    in >> type;                                                              //첫 블럭 read(프로토콜 read)
    in.readRawData(data, 1020);                                              //나머지 데이터를 read

    QString ip = clientConnection->peerAddress().toString();                 //소켓과 연결된 IP주소 저장
    quint16 port = clientConnection->peerPort();                             //소켓의 포트번호 저장
    QString name = QString::fromStdString(data);                             //데이터로 넘어온 이름 저장

    qDebug() << ip << " : " << type;                                         //디버그 메세지 출력

    switch(type)                                                             //프로토콜에 따른 데이터 처리
    {
    case Chat_Login:                                                         //프로토콜이 로그인인 경우
        if(ui->waittingRoomTreeWidget->
                findItems(name + "  ",                                       //로그인 한 이름이 고객 목록에 없을 경우
                          Qt::MatchFixedString, 1).count() <= 0)
        {
            return;                                                          //receiveData 함수 중단(로그인 불가)
        }
        else                                                                 //로그인 한 이름이 고객 목록에 있을 경우
        {
            /*로그인을 허락하는 프로토콜을 클라이언트 소켓에 보냄*/
            QByteArray sendArray;
            QDataStream out(&sendArray, QIODevice::WriteOnly);
            out << Chat_logCheck;
            out.writeRawData("", 1020);
            clientConnection->write(sendArray);
        }

        foreach(auto item, ui->waittingRoomTreeWidget->                      //대기실에서 로그아웃 상태인 이름 찾기
                findItems(name + "  ", Qt::MatchFixedString, 1)) {
            if(item->text(1) != name + " ")                                  //로그아웃 상태(공백 2개)이면
            {
                item->setIcon(0, QIcon(":/icon_image/greenLight.png"));      //로그인 아이콘 설정
                item->setText(1, name + " ");                                //로그인 상태 설정(공백 1개)
                clientList.append(clientConnection);                         //로그인한 고객의 소켓을 리스트에 저장
                clientSocketHash[name] = clientConnection;                   //이름을 key로 소켓을 해쉬에 저장
            }
        }
        break;
    case Chat_In:                                                            //프로토콜이 채팅방 입장인 경우
        foreach(auto item, ui->waittingRoomTreeWidget->                      //대기실에서 로그인 상태인 이름 찾기
                findItems(name + " ", Qt::MatchFixedString, 1)) {
            if(item->text(1) != name)                                        //채팅방 입장 상태(공백 0개)가 아니면
            {
                item->setIcon(0, QIcon(":/icon_image/chatting1.png"));       //채팅방 입장 상태 아이콘 설정
                item->setText(1, name);                                      //채팅방 입장 상태 설정(공백 0개)
            }
            clientNameHash[port] = name;                                     //이름을 key로 포트 번호를 저장

            /*대기실에 있는 고객 아이템을 제거하고, 채팅방 목록에 아이템 추가*/
            int index = ui->waittingRoomTreeWidget->indexOfTopLevelItem(item);
            ui->waittingRoomTreeWidget->takeTopLevelItem(index);
            ui->chattingRoomTreeWidget->addTopLevelItem(item);

            /*채팅방 목록의 열너비를 아이템 내용에 맞춤*/
            for(int i = 0; i < ui->waittingRoomTreeWidget->columnCount(); i++)
                ui->chattingRoomTreeWidget->resizeColumnToContents(i);
        }
        break;
    case Chat_Talk:                                                          //프로토콜이 메세지 보내기인 경우
    {
        foreach(QTcpSocket *sock, clientList)                                //접속한 클라이언트 수만큼 반복
        {
            if(clientNameHash.contains(sock->peerPort())                     //자신을 제외하고 채팅방에 있는 소켓에게
                    && sock != clientConnection)
            {
                /*메세지를 담아서 보냄*/
                QByteArray sendArray;
                sendArray.clear();                                           //바이트 어레이 초기화
                QDataStream out(&sendArray, QIODevice::WriteOnly);
                out << Chat_Talk;                                            //채팅보내기 프로토콜 삽입
                sendArray.append("<font color=lightsteelblue>");             //옅은 파란색으로
                sendArray.append(clientNameHash[port].toStdString().data()); //이름
                sendArray.append("</font> : ");                              //폰트 종료, : 출력
                sendArray.append(name.toStdString().data());                 //메세지 추가
                sock->write(sendArray);                                      //모든 데이터 write
                qDebug() << sock->peerPort();                                //디버그 메세지 출력
            }
        }

        /*로그를 기록*/
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);      //로그를 기록할 아이템 생성
        item->setText(0, ip);                                                //아이피 주소
        item->setText(1, QString::number(port));                             //포트 번호
        item->setText(2,                                                     //고객 ID
                      QString::number(clientIDHash[clientNameHash[port]]));
        item->setText(3, clientNameHash[port]);                              //고객 성명
        item->setText(4, QString(data));                                     //메세지 내용
        item->setText(5, QDateTime::currentDateTime().toString());           //현재 시간
        item->setToolTip(4, QString(data));                                  //메세지가 길 수 있으므로 툴팁으로 메세지 보여줌
        ui->logTreeWidget->addTopLevelItem(item);                            //채팅 로그 추가

        /*채팅방 목록의 열너비를 아이템 내용에 맞춤*/
        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++)
            ui->logTreeWidget->resizeColumnToContents(i);

        logThread->appendData(item);                                         //로그스레드에 저장할 아이템 추가
    }
        break;
    case Chat_Out:                                                           //프로토콜이 채팅방에서 나가기인 경우
        foreach(auto item, ui->chattingRoomTreeWidget->                      //채팅방 목록에서 이름 찾기
                findItems(name, Qt::MatchFixedString, 1))
        {
            if(item->text(1) != name + " ")                                  //이름이 로그인 상태(공백 1개)가 아니라면
            {
                item->setIcon(0, QIcon(":/icon_image/greenLight.png"));      //로그인 상태 아이콘 설정
                item->setText(1, name + " ");                                //로그인 상태 설정(공백 1개)
            }
            clientNameHash.remove(port);                                     //채팅방 입장시 추가했던 해쉬에서 제거

            /*채팅방 목록에서 제거 후 대기실 목록에 추가*/
            int index = ui->chattingRoomTreeWidget->
                    indexOfTopLevelItem(item);
            ui->chattingRoomTreeWidget->takeTopLevelItem(index);
            ui->waittingRoomTreeWidget->addTopLevelItem(item);
        }
        break;
    case Chat_LogOut:                                                        //프로토콜이 로그아웃인 경우
        foreach(auto item, ui->waittingRoomTreeWidget->
                findItems(name + " ", Qt::MatchFixedString, 1))              //대기실에서 로그인 상태인 이름 찾기
        {
            if(item->text(1) != name + "  ")                                 //로그아웃 상태(공백 2개)가 아닌 경우
            {
                item->setIcon(0, QIcon(":/icon_image/redLight.png"));        //로그아웃 상태 아이콘 설정
                item->setText(1, name + "  ");                               //로그아웃 상태 설정(공백 2개)
                clientList.removeOne(clientConnection);                      //로그인 시 추가했던 리스트에서 제거
                clientSocketHash.take(name);                                 //로그인 시 추가했던 해쉬에서 제거
            }
        }
        break;
    }
}

void ChatServerForm::removeClient()                                          //서버와 연결이 끊어질 경우 소켓 목록에서
{                                                                            //삭제하는 함수
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender());     //클라이언트 소켓을 센터로 설정

    QString name = clientNameHash[clientConnection->peerPort()];             //클라이언트 소켓의 이름을 저장
    foreach(auto item, ui->chattingRoomTreeWidget->                          //클라이언트의 이름이 채팅방에 있는 경우
            findItems(name, Qt::MatchFixedString, 1))
    {
        int index = ui->chattingRoomTreeWidget->indexOfTopLevelItem(item);   //현재 아이템의 인덱스 저장
        item->setIcon(0, QIcon(":/icon_image/redLight.png"));                //로그아웃 상태 아이콘 설정
        item->setText(1, name + "  ");                                       //로그아웃 상태 설정(공백 2개)
        ui->chattingRoomTreeWidget->takeTopLevelItem(index);                 //채팅방 목록에서 제거
        ui->waittingRoomTreeWidget->addTopLevelItem(item);                   //대기실 목록에 추가
    }
    foreach(auto item, ui->waittingRoomTreeWidget->                          //클라이언트의 이름이 대기실에 있는 경우
            findItems(name + " ", Qt::MatchFixedString, 1))
    {
        item->setIcon(0, QIcon(":/icon_image/redLight.png"));                //로그아웃 상태 아이콘 설정
        item->setText(1, name + "  ");                                       //로그아웃 상태 설정(공백 2개)
    }

    clientList.removeOne(clientConnection);                                  //로그인 시 추가했던 리스트에서 제거
    clientConnection->deleteLater();                                         //소켓을 삭제할 수 있을 때 삭제
}

void ChatServerForm::kickOut()                                               //클라이언트에게 강퇴 프로토콜을 보내는 함수
{
    QString name = ui->chattingRoomTreeWidget->currentItem()->text(1);       //채팅 중인 클라이언트의 이름을 저장
    QTcpSocket* sock = clientSocketHash[name];                               //이름과 연결된 소켓 저장

    /*소켓에 강퇴 프로토콜 write*/
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_KickOut;
    out.writeRawData("", 1020);
    sock->write(sendArray);

    foreach(auto item, ui->chattingRoomTreeWidget->                          //채팅방 목록에서 이름 찾기
            findItems(name, Qt::MatchFixedString, 1))
    {
        int index = ui->chattingRoomTreeWidget->                             //채팅방에서 아이템의 인덱스 저장
                indexOfTopLevelItem(item);
        ui->chattingRoomTreeWidget->takeTopLevelItem(index);                 //채팅방 목록에서 이름과 일치하는 아이템 제거
        QTreeWidgetItem *waittingItem = new QTreeWidgetItem;                 //대기실에 추가할 새 아이템 생성
        waittingItem->setIcon(0, QIcon(":/icon_image/greenLight.png"));      //로그인 상태 아이콘 설정
        waittingItem->setText(1, name + " ");                                //로그인 상태 설정(공백 1개)
        ui->waittingRoomTreeWidget->addTopLevelItem(item);                   //대기실에 새 아이템 추가
    }
}

void ChatServerForm::inviteClient()                                          //클라이언트에게 초대 프로토콜을 보내는 함수
{
    QString name = ui->waittingRoomTreeWidget->currentItem()->               //대기실에 있는 이름 3글자만 저장
            text(1).left(3);
    if(ui->waittingRoomTreeWidget->currentItem()->text(1) != name + " ")     //현재 저장된 이름이 로그인 상태가 아니라면
    {
        return;                                                              //inviteClient함수 중단(초대 불가)
    }

    /*소켓에 초대 프로토콜 write*/
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Invite;
    out.writeRawData("", 1020);
    QTcpSocket* sock = clientSocketHash[name.left(3)];
    sock->write(sendArray);

    qDebug() << sock->peerPort();                                            //디버그 메세지 출력

    quint16 port = sock->peerPort();                                         //현재 소켓의 포트 번호를 저장
    clientNameHash[port] = name;                                             //초대된 클라이언트의 이름을 포트번호로 저장

    foreach(auto item, ui->waittingRoomTreeWidget->                          //대기실에서 로그인 상태인 이름 찾기
            findItems(name + " ", Qt::MatchFixedString, 1))
    {
        if(item->text(1) != name)                                            //채팅방 입장 상태(공백 0개)가 아니면
        {
            item->setIcon(0, QIcon(":/icon_image/chatting1.png"));           //채팅방 입장 상태 아이콘 설정
            item->setText(1, name);                                          //채팅방 입장 상태 설정(공백 0개)
            int index = ui->waittingRoomTreeWidget->                         //대기실에서 아이템의 인덱스 저장
                    indexOfTopLevelItem(item);
            ui->waittingRoomTreeWidget->takeTopLevelItem(index);             //대기실 목록에서 아이템 제거
            ui->chattingRoomTreeWidget->addTopLevelItem(item);               //채팅방 목록에 아이템 추가
        }
    }
}

/* 파일 전송 */
void ChatServerForm::acceptConnection()                                      //파일 서버의 소켓을 연결하는 함수
{
    qDebug() << tr("Connected, preparing to receive files!");                //디버그 메세지 출력

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();        //연결된 클라이언트 소켓과 파일 서버를 연결

    /*소켓이 read할 준비가 되면 readClient 함수를 연결*/
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

void ChatServerForm::readClient()                                            //클라이언트가 보내는 파일을 read하는 함수
{
    qDebug() << tr("Receiving file ...");                                    //디버그 메세지 출력
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender());       //클라이언트의 소켓을 센더로 설정
    QString filename, name;                                                  //파일명과 클라이언트의 이름을 선언

    /*just started to receive data, this data is file information*/
    if (byteReceived == 0)                                                   //read할 파일의 크기가 0인 경우
    {
        progressDialog->reset();                                             //progressDialog 초기화
        progressDialog->show();                                              //progressDialog 출력

        QString ip = receivedSocket->peerAddress().toString();               //현재 소켓의 ip주소 저장
        quint16 port = receivedSocket->peerPort();                           //현재 소켓의 포트를 저장

        /*스트림에 파일의 총 크기, 현재 받은 파일 크기, 파일명, 클라이언트 이름을 대입*/
        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> name;

        progressDialog->setMaximum(totalSize);                               //파일의 총 크기를 100%로 progressDialog에 설정

        /*로그를 기록*/
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);
        item->setText(0, ip);
        item->setText(1, QString::number(port));
        item->setText(2, QString::number(clientIDHash[name]));
        item->setText(3, name);
        item->setText(4, filename);
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, filename);                                       //메세지가 길 수 있으므로 툴팁으로 메세지 보여줌

        /*채팅방 목록의 열너비를 아이템 내용에 맞춤*/
        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++)
            ui->logTreeWidget->resizeColumnToContents(i);

        ui->logTreeWidget->addTopLevelItem(item);                            //로그 추가

        logThread->appendData(item);                                         //로그스레드에 저장할 아이템 추가

        QFileInfo info(filename);                                            //파일의 정보를 저장
        QString currentFileName = info.fileName();                           //보안을 위해 절대경로 대신 파일 이름만 표시
        file = new QFile(currentFileName);                                   //현재 파일을 file 객체에 삽입
        file->open(QFile::WriteOnly);                                        //파일 열기
    }
    /* Officially read the file content*/
    else                                                                     //넘어온 데이터에 대한 내용 처리
    {
        inBlock = receivedSocket->readAll();                                 //소켓의 데이터를 모두 read해서 저장

        byteReceived += inBlock.size();                                      //read한 내용의 크기를 저장
        file->write(inBlock);                                                //파일에 받은 데이터를 write
        file->flush();                                                       //write한 내용은 버림
    }

    progressDialog->setValue(byteReceived);                                  //데이터의 크기만큼 progressDialog에 값을 설정

    if (byteReceived == totalSize)                                           //데이터를 다 받으면
    {
        qDebug() << QString(tr("%1 receive completed")).arg(filename);       //디버그 메세지 출력

        /*데이터를 수신하는 데 사용했던 변수들 초기화*/
        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();

        progressDialog->hide();                                              //progressDialog 숨김
        file->close();                                                       //파일 닫기
        delete file;                                                         //파일 객체 삭제
    }
}

/*대기실에서 컨텍스트 메뉴를 호출할 때*/
void ChatServerForm::on_waittingRoomTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->waittingRoomTreeWidget->currentItem() == nullptr)    return;      //현재 아이템이 선택되지 않은 경우 예외처리
    QString name = ui->waittingRoomTreeWidget->                              //현재 아이템의 이름 저장
            currentItem()->text(1).left(3);
    foreach(QAction *action, menu->actions())                                //액션의 수만큼 반복
    {
        if(action->objectName() == "Invite")                                 //초대 액션
            action->setEnabled                                               //로그인 상태인 이름에만 활성화
                    (ui->waittingRoomTreeWidget->currentItem()->text(1) == name + " ");
        else                                                                 //추방 액션
            action->setDisabled(true);                                          //비활성화
    }
    QPoint globalPos = ui->waittingRoomTreeWidget->mapToGlobal(pos);         //현재 커서의 위치
    menu->exec(globalPos);                                                   //컨텍스트 메뉴 출력
}

/*채팅방에서 컨텍스트 메뉴를 호출할 때*/
void ChatServerForm::on_chattingRoomTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->chattingRoomTreeWidget->currentItem() == nullptr)    return;      //현재 아이템이 선택되지 않은 경우 예외처리

    foreach(QAction *action, menu->actions())                                //액션의 수만큼 반복
    {
        if(action->objectName() == "Kick out")                               //추방 액션
            action->setEnabled(true);                                        //활성화
        else                                                                 //초대 액션
            action->setDisabled(true);                                       //비활성화
    }
    QPoint globalPos = ui->chattingRoomTreeWidget->mapToGlobal(pos);         //현재 커서의 위치
    menu->exec(globalPos);                                                   //컨텍스트 메뉴 출력

}

void ChatServerForm::on_resetPushButton_clicked()                            //reset 버튼이 클릭될 경우
{
    ui->waittingRoomTreeWidget->clear();                                     //대기실 목록을 비움
    emit reset();                                                            //reset 시그널 방출
}
