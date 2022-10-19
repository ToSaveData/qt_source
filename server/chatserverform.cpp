#include "chatserverform.h"
#include "ui_chatserverform.h"

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

ChatServerForm::ChatServerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatServerForm) // 생성자에서는 포트를 8000번으로 고정, 메뉴를 생성
{
    ui->setupUi(this);
    QList<int> sizes;
    sizes << 120 << 500;
    ui->splitter->setSizes(sizes);
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));
    if (!tcpServer->listen(QHostAddress::Any, 8000)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(tcpServer->errorString( )));
        close( );
        return;
    }

    QAction* inviteAction = new QAction(tr("&Invite"));
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), SLOT(inviteClient()));

    QAction* removeAction = new QAction(tr("&Kick out"));
    connect(removeAction, SIGNAL(triggered()), SLOT(kickOut()));

    menu = new QMenu;
    menu->addAction(inviteAction);
    menu->addAction(removeAction);
    ui->clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    qDebug() << tr("The server is running on port %1.").arg(tcpServer->serverPort( ));
}

ChatServerForm::~ChatServerForm() // 소멸자
{
    delete ui;

    tcpServer->close( );
}

void ChatServerForm::clientConnect( ) // 클라이언트와 연결
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection( ); 	// 클라이언트용 소켓 생성
    connect(clientConnection, SIGNAL(readyRead( )), SLOT(receiveData( )));	// readyRead() 시그널 발생시, receive Data 슬롯 실행
    connect(clientConnection, SIGNAL(disconnected( )), SLOT(removeClient()));	// removeClient() 시그널 발생 시, removeClient 슬롯 실행
    qDebug("new connection is established...");
}

void ChatServerForm::receiveData( )					// 데이터 받기
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));	//센더를 소켓으로 받음
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);		//read한 내용을 bytearray에 저장

    chatProtocolType data;						//구조체 변수 data 선언
    QDataStream in(&bytearray, QIODevice::ReadOnly);			//스트림 연산으로 btye array에 저장된 정보를 파싱
    in >> data.type;							// 파싱된 bytearray를 int형으로 싹다 저장
    in.readRawData(data.data, 1020);					//in의 정보를 읽어서 char[1020] 에data에 저장

    QString ip = clientConnection->peerAddress().toString();			//소켓에 저장된 ip주소를 저장
    QString name = QString::fromStdString(data.data);			// data에 저장된 정보를 string으로 반환해서 저장

    switch(data.type) {						// 데이터 타입의 경우에 따라 실행
    case Chat_Login:							// 로그인 하면
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {	//
            if(item->text(1) != "O") {					//상태가 O가 아니면
                item->setText(0, "O");					//상태를 O로 바꿈
                clientList.append(clientConnection);        // QList<QTcpSocket*> clientList;
                clientNameHash[ip] = name;					//Hash 배열에 ip와 대응되는 name을 대입
            }
        }
        break;
    case Chat_In:							//채팅에 입장하면
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {	//treeWidget에서 이름을 찾음
            if(item->text(1) != "O") {						//상태가 O가 아니면 O로 바꿔줌
                item->setText(0, "O");
            }
        }
        break;
    case Chat_Talk: {							// 채팅에서 말하면
        foreach(QTcpSocket *sock, clientList) {				// 고객 리스트에서 소켓을 뽑아서
            if(sock != clientConnection) {					// 서로 다른 클라이언트가 연결된다면
                QByteArray data("<font color=red>");        // data를 글자색을 빨간색으로 설정
                data.append(name.toStdString().data());     // name에 저장된 정보를 data에 추가
                data.append("</font> : ");                  // 빨간색을 해제하고 : data에 추가
                data.append(bytearray);                     // bytearray에 있는 내용을 data에 추가
                sock->write(data);                          // data에 있는 내용을 write
            }
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);     //트리위젯에 아이템 세팅
        item->setText(0, clientConnection->peerAddress().toString());           
        item->setText(1, QString::number(clientConnection->peerPort()));        
        item->setText(2, QString::number(clientIDHash[clientNameHash[ip]]));    
        item->setText(3, clientNameHash[ip]);
        item->setText(4, QString(data.data));
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, QString(data.data));

        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)           //열에 있는 컨텐츠의 크기로 resize
            ui->messageTreeWidget->resizeColumnToContents(i);

        ui->messageTreeWidget->addTopLevelItem(item);
    }
        break;
    case Chat_Close:                                                            //채팅창을 닫을 경우
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            if(item->text(1) == "O") {                                          // 접속 중으로 표시된 경우
                item->setText(0, "X");                                          // 접속 종료 상태로 전환
            }
        }
        break;
    case Chat_LogOut:                                                           //로그아웃을 한 경우
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {   //이름을 검색해서
            if(item->text(1) == "O") {                                          //접속 중으로 표시된 경우
                item->setText(0, "X");                                          // 접속 종료 상태로 전환
                clientList.removeOne(clientConnection);        // QList<QTcpSocket*> clientList;
            }
        }
//        ui->inviteComboBox->addItem(name);
        break;
    }
    //    qDebug() << bytearray;
}

void ChatServerForm::removeClient()                                                 //고객을 삭제하는 경우
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));           //센더를 고객 연결 소켓으로 선언
    clientList.removeOne(clientConnection);                                         //현재 연결된 고객 소켓 목록에서 삭제
    clientConnection->deleteLater();                                                //현재 소켓도 지울 수 있을 때 삭제

    QString name = clientNameHash[clientConnection->peerAddress().toString()];          //해쉬에서 이름을 찾아서 name에 저장
    foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {   //이름을 찾아서 접속 종료 상태로 표시
        item->setText(0, "X");                                                              
    }
}

void ChatServerForm::addClient(int id, QString name)                                    //고객 추가
{
    clientIDList << id;                                                                 //idList 배열에 id 추가
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->clientTreeWidget);                  //현재 트리위젯을 아이템으로 선언
    item->setText(0, "X");                                                              //접속 해제 상태로 추가
    item->setText(1, name);                                                             //이름 추가
    ui->clientTreeWidget->addTopLevelItem(item);                                        //아이템 추가
    clientIDHash[name] = id;                                                            // clientIDHash.insert(name,id)와 동일, 키에 대응되는 값을 대입
    ui->clientTreeWidget->resizeColumnToContents(0);                                    //사이즈 정렬
}

void ChatServerForm::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos)  //컨텍스트 메뉴에 액션 추가
{
    foreach(QAction *action, menu->actions()) {                                         
        if(action->objectName() == "Invite")
            action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) != "O");
        else
            action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) == "O");
    }
    QPoint globalPos = ui->clientTreeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

void ChatServerForm::kickOut()                                                              //강퇴 기능 구현
{
    QString name = ui->clientTreeWidget->currentItem()->text(1);                            //트리위젯에서 이름 뽑아서 저장
    QString ip = clientNameHash.key(name);                                                  //hash에서 이름(키)으로 ip 출력

    chatProtocolType data;                                                                  //구조체 변수 선언
    data.type = Chat_KickOut;                                                               //상태를 Kick_out으로 설정
    qstrcpy(data.data, "");                                                                 //data를 초기화
    QByteArray sendArray;                                                                   //QByteArray 객체 선언
    QDataStream out(&sendArray, QIODevice::WriteOnly);                                      //sendArray의 내용을 
    out << data.type;                                                                       //데이터의 타입을 출력
    out.writeRawData(data.data, 1020);                                                      //data를 1020만큼 out에 write

    foreach(QTcpSocket* sock, clientList) {                                                 //각 클라이언트 소켓의 ip 주소를 sendArray에 write
        if(sock->peerAddress().toString() == ip){
//            sock->disconnectFromHost();
            sock->write(sendArray);
        }
    }
    ui->clientTreeWidget->currentItem()->setText(0, "X");                                   //현재 선택된 클라이언트를 접속 해제 상태로 변경
//    clientIDList.append(clientIDHash[name]);
//    ui->inviteComboBox->addItem(name);
}

void ChatServerForm::inviteClient()                                                         //초대하기 기능
{
    if(ui->clientTreeWidget->topLevelItemCount()) {                                         //트리위젯에 아이템이 있으면
        QString name = ui->clientTreeWidget->currentItem()->text(1);                        //트리위젯에서 이름 가져옴
        QString ip = clientNameHash.key(name, "");                                          //name에 맞는 ip 저장

        chatProtocolType data;                                                              //구조체 변수 선언
        data.type = Chat_Invite;                                                            //타입에 초대 상태 저장
        qstrcpy(data.data, "");                                                             //데이터 값 초기화
        QByteArray sendArray;                                                               //byteArray 배열 선언
        QDataStream out(&sendArray, QIODevice::WriteOnly);                                  //쓰기전용으로 sendArray에 쓰는 DataStream 객체 선언
        out << data.type;                                                                   //현재 상태를 out에 투입
        out.writeRawData(data.data, 1020);                                                  //out에 있는 정보에서 1020만큼 data에 write

        foreach(QTcpSocket* sock, clientList) {                                             //현재 ip와 일치하는 소켓을 찾아서
            if(sock->peerAddress().toString() == ip){
                sock->write(sendArray);                                                     //소켓에 sendArray의 내용을 write
//                clientList.append(sock);        // QList<QTcpSocket*> clientList;
                foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {
                    if(item->text(1) != "O") {                                              //트리위젯에서 이름이 맞는 아이템을 찾아서
                        item->setText(0, "O");                                              //접속 상태로 변경
                        clientList.append(sock);        // QList<QTcpSocket*> clientList;   //고객 소켓 목록에 추가
//                        clientNameHash[ip] = name;
                    }
                }
            }
        }
    }
}

