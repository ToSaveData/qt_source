#include "chattingclient.h"
#include <QtGui>
#include <QtNetwork>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTcpSocket>


#define BLOCK_SIZE 1024

ChattingClient::ChattingClient(QWidget *parent)
    : QWidget{parent}
{
    //연결한 서버 정보를 위한 위젯들
    QLineEdit *serverAddress = new QLineEdit(this);
    serverAddress->setText("127.0.0.1");

    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|");
    QRegularExpressionValidator validator(re);
    serverAddress->setPlaceholderText("Server IP Address");
    QLineEdit *serverPort = new QLineEdit(this);
    serverPort->setInputMask("00000;_");
    serverPort->setPlaceholderText("Server Port No");

    QPushButton *connectButton = new QPushButton("connect", this);
    connect(connectButton, &QPushButton::clicked, [=]{clientSocket->connectToHost(serverAddress->text(),
                                                                                  serverPort->text().toInt());});
    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addStretch(1);
    serverLayout->addWidget(serverAddress);
    serverLayout->addWidget(serverPort);
    serverLayout->addWidget(connectButton);

    message = new QTextEdit(this);                          //서버에서 오는 메시지 표시용
    message->setReadOnly(true);

    //서버로 보낼 메시지를 위한 것들
    inputLine = new QLineEdit(this);
    QPushButton *sentButton = new QPushButton("Send", this);
    connect(sentButton, SIGNAL(clicked()), SLOT(sendData()));
    connect(inputLine, SIGNAL(returnPressed()), this, SLOT(sendData()));
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputLine);
    inputLayout->addWidget(sentButton);

    //종료기능
//    QPushButton *quitButton = new QPushButton("Quit", this);
//    connect(quitButton, SIGNAL(clicked()), qapp, SLOT(quit()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
//    buttonLayout->addWidget(quitButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(serverLayout);
    mainLayout->addWidget(message);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    clientSocket = new QTcpSocket(this);                        //클라이언트 소켓 생성
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{qDebug() << clientSocket->errorString();});
    connect(clientSocket, SIGNAL(readyRead()), SLOT(echoData()));
    setWindowTitle(tr("Echo Client"));
}

ChattingClient::~ChattingClient()
{
    clientSocket->close();
}

void ChattingClient::echoData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if(clientSocket->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);
    message->append(QString(bytearray));
}

void ChattingClient::sendData()
{
    QString str = inputLine->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        clientSocket->write(bytearray);
        inputLine->clear();
    }

    message->append(str);
}
