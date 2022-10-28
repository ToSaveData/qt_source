#include "chatclient.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDataStream>
#include <QTcpSocket>
#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

#define BLOCK_SIZE      1024                                                //블럭 사이즈를 1024로 고정

ChatClient::ChatClient(QWidget *parent)                                     //생성자
    : QWidget{parent}, isSent(false)                                        //파일 서버 접속 여부를 false로 초기화
{
    /*연결한 서버 정보 입력을 위한 위젯들*/
    name = new QLineEdit(this);                                             //이름 입력란에 새로운 객체 대입

    serverAddress = new QLineEdit(this);                                    //서버 IP 주소 입력란에 새로운 객체 대입
    serverAddress->setText("127.0.0.1");                                    //기본 주소로 자기 자신을 세팅

    /*정규표현식과 Validator를 사용하여 IP주소 입력란에 각 칸마다 255이상의 숫자가 올 수 없도록 설정*/
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator validator(re);
    serverAddress->setPlaceholderText(tr("Server IP Address"));             //입력란이 비어있을 경우 서버 IP 주소 자리임을 표시
    serverAddress->setValidator(&validator);

    serverPort = new QLineEdit(this);                                       //서버 포트 번호 입력란에 새로운 객체 대입
    serverPort->setText(QString::number(PORT_NUMBER));                      //기본값으로 채팅 서버 포트인 8000 설정
    serverPort->setInputMask("00000;_");                                    //입력 마스크 설정
    serverPort->setPlaceholderText(tr("Server Port No"));                   //입력란이 비어있을 경우 서버 포트 번호 자리임을 표시

    connectButton = new QPushButton(tr("Log In"), this);                    //connectButton에 Log In으로 텍스트를 설정하여
                                                                            //새로운 객체 대입
    connect(name, SIGNAL(returnPressed()),                                  //이름 입력란에서 Enter 키를 누르면
            connectButton, SIGNAL(clicked()));                              //connectButton이 클릭되는 시그널을 보내도록 설정

    /*위젯들의 레이아웃을 설정*/
    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addWidget(name);
    serverLayout->addStretch(1);
    serverLayout->addWidget(serverAddress);
    serverLayout->addWidget(serverPort);
    serverLayout->addWidget(connectButton);

    message = new QTextEdit(this);                                          //메세지 출력 창에 새로운 객체 대입
    message->setReadOnly(true);                                             //읽기 전용으로 설정

    /*서버로 보낼 메시지를 위한 위젯들*/
    inputLine = new QLineEdit(this);                                        //메세지 입력란에 새로운 객체 대입

    /*메세지 입력란에서 Enter 키를 누를 경우 슬롯함수 실행*/
    connect(inputLine, SIGNAL(returnPressed()), SLOT(sendData()));          //sendData 함수 실행
    connect(inputLine, SIGNAL(returnPressed()), inputLine, SLOT(clear()));  //메세지 입력란을 비워줌

    sentButton = new QPushButton(tr("Send"), this);                         //전송 버튼에 새로운 객체 대입

    /*전송 버튼을 클릭할 경우 슬롯함수 실행*/
    connect(sentButton, SIGNAL(clicked()), SLOT(sendData()));               //sendData 함수 실행
    connect(sentButton, SIGNAL(clicked()), inputLine, SLOT(clear()));       //메세지 입력란을 비워줌

    /*로그인 전이므로 초기값으로 메세지 입력란과 전송 버튼 비활성화*/
    inputLine->setDisabled(true);
    sentButton->setDisabled(true);

    /*위젯들의 레이아웃을 설정*/
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputLine);
    inputLayout->addWidget(sentButton);

    fileButton = new QPushButton(tr("File Transfer"), this);                //파일 전송 버튼에 새로운 객체 대입
    connect(fileButton, SIGNAL(clicked()), SLOT(sendFile()));               //파일 전송 버튼을 누르면 SendFile 함수를 실행
    fileButton->setDisabled(true);                                          //로그인 전 파일 전송 버튼 비활성화

    /*종료 기능*/
    QPushButton* quitButton = new QPushButton(tr("Log Out"), this);         //종료 버튼에 Log Out 텍스트를 설정하고 객체 대입
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));             //종료 버튼을 누르면 채팅 클라이언트 폼 종료

    /*위젯들의 레이아웃을 설정*/
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(fileButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);

    /*위젯들의 레이아웃을 설정*/
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(serverLayout);
    mainLayout->addWidget(message);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    /* 채팅을 위한 소켓 */
    clientSocket = new QTcpSocket(this);                                    //클라이언트 소켓 생성

    connect(clientSocket, &QAbstractSocket::errorOccurred, this,            //소켓에서 에러 발생시
            [=]{ qDebug() << clientSocket->errorString(); });               //디버그 메세지로 에러코드 출력

    connect(clientSocket, SIGNAL(readyRead()), SLOT(receiveData()));        //소켓이 read할 준비가 되면
                                                                            //receiveData 함수 실행

    connect(clientSocket, SIGNAL(disconnected()), SLOT(disconnect()));      //소켓의 연결이 끊어지면
                                                                            //disconnect 함수 실행
    /* 파일 전송을 위한 소켓 */
    fileClient = new QTcpSocket(this);                                      //파일 전송 소켓에 새로운 객체 대입
    connect(fileClient, SIGNAL(bytesWritten(qint64)),                       //파일이 전송되기 시작하면
            SLOT(goOnSend(qint64)));                                        //goOnSend 함수 실행

    /*progressDialog 객체 생성 및 속성 설정*/
    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();                                                //autoClose가 활성화 되면
                                                                            //reset시 창을 닫음

    connect(connectButton, SIGNAL(clicked()),                               //connectButton이 클릭되면
            this, SLOT(connectButtonClicked()));                            //connectButtonClicked() 함수 실행

    setWindowTitle(tr("Chat Client"));                                      //창 이름 설정
}

ChatClient::~ChatClient()                                                   //소멸자
{
    clientSocket->close();                                                  //채팅 소켓을 닫음
}

/*connectButton의 현재 text에 따라 다른 동작을 수행*/
void ChatClient::connectButtonClicked()                                     //connectButton의 클릭 시그널을 처리하는 함수
{
    if(connectButton->text() == tr("Log In"))                               //text가 Log In이면
    {
        clientSocket->connectToHost(serverAddress->text(),                  //입력된 서버의 IP주소와 포트번호로
                                    serverPort->text().toInt());            //연결을 시도

        clientSocket->waitForConnected();                                   //연결 될 때까지 기다림
        sendProtocol(Chat_Login, name->text().toStdString().data());        //연결된 서버에 Login 프로토콜 전송
        name->clearFocus();                                                 //이름 입력 창에서 커서를 없애줌
    }
    else if(connectButton->text() == tr("Chat in"))                         //text가 Chat in이면
    {
        sendProtocol(Chat_In, name->text().toStdString().data());           //연결된 서버에 채팅방 입장 프로토콜 전송

        /*채팅방에 있는 상태*/
        connectButton->setText(tr("Chat Out"));                             //connectButton의 text를 변경
        inputLine->setEnabled(true);                                        //메세지 입력란 활성화
        sentButton->setEnabled(true);                                       //메세지 전송 버튼 활성화
        fileButton->setEnabled(true);                                       //파일 전송 버튼 활성화
    }
    else if(connectButton->text() == tr("Chat Out"))                        //text가 Chat Out이면
    {
        sendProtocol(Chat_Out, name->text().toStdString().data());          //연결된 서버에 채팅방 나가기 프로토콜 전송

        /*대기실에서 로그인 상태*/
        connectButton->setText(tr("Chat in"));                              //connectButton의 text를 변경
        inputLine->setDisabled(true);                                       //메세지 입력란 비활성화
        sentButton->setDisabled(true);                                      //메세지 전송 버튼 비활성화
        fileButton->setDisabled(true);                                      //파일 전송 버튼 비활성화
    }
}

/* 창이 닫힐 때 서버에 연결 접속 메시지를 보내고 종료 */
void ChatClient::closeEvent(QCloseEvent*)                                   //윈도우 창의 닫기 버튼에 대한 이벤트 처리
{
    sendProtocol(Chat_LogOut, name->text().toStdString().data());           //로그아웃 프로토콜 전송
    clientSocket->disconnectFromHost();                                     //소켓과 연결된 서버와 연결 끊기
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)          //만약 소켓의 연결상태가 유지된다면
        clientSocket->waitForDisconnected();                                //연결이 끊어질 때까지 기다림
}

/* 데이터를 받을 때 */
void ChatClient::receiveData()                                              //서버에서 데이터를 받는 함수
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender());        //서버의 소켓을 sender로 설정
    if (clientSocket->bytesAvailable() > BLOCK_SIZE) return;                //소켓이 받을 수 있는 용량보다
                                                                            //보내는 데이터의 용량이 크면 함수 종료

    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);                  //소켓의 데이터를 blocksize(1024)만큼 read

    Chat_Status type;                                                       //채팅의 목적
    char data[1020];                                                        //전송되는 메시지/데이터
    memset(data, 0, 1020);                                                  //쓰레기 값을 0으로 초기화

    QDataStream in(&bytearray, QIODevice::ReadOnly);                        //스트림에 bytearray를 담음
    in.device()->seek(0);                                                   //스트림의 처음으로 위치 설정
    in >> type;                                                             //첫 블럭 read(프로토콜 read)
    in.readRawData(data, 1020);                                             //나머지 데이터를 read

    switch(type)                                                            //프로토콜에 따른 데이터 처리
    {
    case Chat_logCheck:                                                     //프로토콜이 로그인 확인인 경우
        /*대기실에서 로그인 상태*/
        connectButton->setText(tr("Chat in"));                              //connectButton의 text를 변경
        name->setReadOnly(true);                                            //로그인한 이름을 바꿀 수 없도록 설정
        break;

        /*다른 사용자의 메세지를 받았음을 의미*/
    case Chat_Talk:                                                         //프로토콜이 메세지 보내기인 경우
        message->append(QString(data));                                     //받은 메시지를 화면에 표시
        inputLine->setEnabled(true);                                        //메세지 입력란 활성화
        sentButton->setEnabled(true);                                       //메세지 전송 버튼 활성화
        fileButton->setEnabled(true);                                       //파일 전송 버튼 활성화
        break;
    case Chat_KickOut:                                                      //프로토콜이 강퇴인 경우
        QMessageBox::critical(this, tr("Chatting Client"),                  //메세지 박스 출력
                              tr("Kick out from Server. "
                                 "Please quit the program "
                                 "and reconnect to server."));

        /*대기실에서 로그인 상태*/
        connectButton->setText(tr("Chat in"));                              //connectButton의 text를 변경
        inputLine->setDisabled(true);                                       //메세지 입력란 비활성화
        sentButton->setDisabled(true);                                      //메세지 전송 버튼 비활성화
        fileButton->setDisabled(true);                                      //파일 전송 버튼 비활성화

        /*재접속을 유도*/
        connectButton->setDisabled(true);                                   //connectButton 비활성화
        break;
    case Chat_Invite:                                                       //프로토콜이 초대인 경우
        QMessageBox::information(this, tr("Chatting Client"),               //메세지 박스 출력
                                 tr("Invited from Server"));

        /*채팅방에 있는 상태*/
        connectButton->setText(tr("Chat Out"));                             //connectButton의 text를 변경
        inputLine->setEnabled(true);                                        //메세지 입력란 활성화
        sentButton->setEnabled(true);                                       //메세지 전송 버튼 활성화
        fileButton->setEnabled(true);                                       //파일 전송 버튼 활성화
        name->setReadOnly(true);                                            //로그인한 이름을 바꿀 수 없도록 설정
        break;
    };
}

/* 연결이 끊어졌을 때 : 상태 변경 */
void ChatClient::disconnect()                                               //서버와의 연결을 끊는 함수
{
    QMessageBox::critical(this, tr("Chatting Client"),                      //메세지 박스 출력
                          tr("Disconnect from Server"));
    /*로그아웃 상태*/
    connectButton->setText(tr("Log in"));                                   //connectButton의 text를 변경
    inputLine->setEnabled(false);                                           //메세지 입력란 비활성화
    name->setReadOnly(false);                                               //이름 입력창 활성화
    sentButton->setEnabled(false);                                          //전송 버튼 비활성화

    sendProtocol(Chat_LogOut, name->text().toStdString().data());           //로그아웃 프로토콜 전송
    clientSocket->disconnectFromHost();                                     //서버와 소켓의 연결을 끊음
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)          //만약 소켓의 연결상태가 유지된다면
        clientSocket->waitForDisconnected();                                //연결이 끊어질 때까지 기다림
}

/* 프로토콜을 생성해서 서버로 전송 */
void ChatClient::sendProtocol(Chat_Status type, char* data, int size)       //서버에 프로토콜을 보내는 함수
{
    /*소켓에 프로토콜과 data를 write*/
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data, size);
    clientSocket->write(dataArray);
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

/* 메시지 보내기 */
void ChatClient::sendData()                                                 //서버로 메세지를 보내는 함수
{
    QString str = inputLine->text();                                        //메세지 입력란에 있는 text를 저장
    if(str.length())                                                        //메세지 입력란에 입력 정보가 있으면
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();                                           //bytearray에 담긴 정보를 utf8형식으로 변경

        /* 화면에 표시 : 앞에 '나'라고 추가 */
        message->append("<font color=red>" + tr("me") + "</font> : " + str);
        sendProtocol(Chat_Talk, bytearray.data());                          //Chat_Talk 프로토콜과 데이터를 전송
    }
}

/* 파일 전송시 여러번 나눠서 전송 */
void ChatClient::goOnSend(qint64 numBytes)                                  //numBytes씩 나눠서 파일을 전송
{
    byteToWrite -= numBytes;                                                //byteToWrite에 남은 파일의 크기를 저장
    outBlock = file->read(qMin(byteToWrite, numBytes));                     //전송할 파일의 크기만큼 read
    fileClient->write(outBlock);                                            //소켓에 write

    progressDialog->setMaximum(totalSize);                                  //progressDialog에 최대 크기 설정
    progressDialog->setValue(totalSize-byteToWrite);                        //현재 진행된 값을 progressDialog에 설정

    if (byteToWrite == 0)                                                   //파일 전송이 완료되면
    {
        qDebug() << tr("File sending completed!");                          //디버그 메세지 출력
        progressDialog->reset();                                            //progressDialog 초기화 및 닫기
    }
}

/* 파일 보내기 */
void ChatClient::sendFile()                                                 //파일을 보내는 함수
{
    /*파일 관련 정보를 담을 변수 초기화*/
    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    QString filename = QFileDialog::getOpenFileName(this);                  //파일 이름 저장
    if(filename.length())                                                   //파일이 있으면
    {
        file = new QFile(filename);                                         //새로운 file 객체 생성
        file->open(QFile::ReadOnly);                                        //파일 열기

        qDebug() << QString(tr("file %1 is opened")).arg(filename);         //디버그 메세지 출력

        /*progressDialog의 값을 초기화*/
        progressDialog->setValue(0);

        if (!isSent)                                                        //파일 서버에 소켓이 연결되지 않았다면
        {
            fileClient->connectToHost(serverAddress->text(),                //소켓을 파일 전송 서버에 연결
                                      serverPort->text().toInt() + 1);
            isSent = true;                                                  //파일 서버 접속 완료
        }

        byteToWrite = totalSize = file->size();                             //남은 파일의 크기 저장
        loadSize = 1024;                                                    //한 번에 1024바이트씩 전송

        /*스트림에 qint64 자료형 크기의 자리를 미리 만들어 둠*/
        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out << qint64(0) << qint64(0) << filename << name->text();

        /*qint64 자료형에 데이터를 채워주고*/
        totalSize += outBlock.size();                                       //파일의 총 크기에 데이터의 크기를 추가함
        byteToWrite += outBlock.size();                                     //남은 파일의 크기에 데이터의 크기를 추가

        /*seek(0)으로 해서 파일의 처음으로 포인터 옮기고*/
        out.device()->seek(0);                                              //파일 포인터를 제일 앞으로 설정

        /*qint64 자료형을 순차적으로 채우면 딱 맞게 들어감*/
        out << totalSize << qint64(outBlock.size());                        //qint64 2개의 크기만큼 데이터 추가


        fileClient->write(outBlock);                                        //소켓에 보낼 파일을 wirte함

        progressDialog->setMaximum(totalSize);                              //progressDialog에 최대치 설정
        progressDialog->setValue(totalSize-byteToWrite);                    //progressDialog에 현재 진행된 만큼 값 설정
        progressDialog->show();                                             //progressDialog 출력
    }
    qDebug() << QString(tr("Sending file %1")).arg(filename);               //디버그 메세지 출력
}

