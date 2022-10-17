#include "chattingserver.h"
#include <QLabel>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include <QList>
#include <QHBoxLayout>
#include <QApplication>
#include <QMessageBox>

#define BLOCK_SIZE 1024

ChattingServer::ChattingServer(QWidget *parent)
    : QWidget{parent}
{
    infoLabel = new QLabel(this);
    QPushButton* quitButton = new QPushButton("Quit", this);
    connect(quitButton, SIGNAL(clicked( )), qApp, SLOT(quit( )));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));
    if (!tcpServer->listen(QHostAddress::Any, 3333)) {
        QMessageBox::critical(this, tr("Echo Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(tcpServer->errorString( )));
        close( );
        return;
    }

    infoLabel->setText(tr("The server is running on port %1.")
                       .arg(tcpServer->serverPort( )));
    setWindowTitle(tr("Chat Server(%1)").arg(tcpServer->serverPort( )));
}

void ChattingServer::clientConnect( )
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection( );
    connect(clientConnection, SIGNAL(readyRead( )), SLOT(echoData( )));
    connect(clientConnection, SIGNAL(disconnected( )), SLOT(removeItem()));
    infoLabel->setText(tr("new connection is established..."));

    clientList.append(clientConnection);        // QList<QTcpSocket*> clientList;
}

void ChattingServer::echoData( )
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    foreach(QTcpSocket *sock, clientList) {
        if(sock != clientConnection)
            sock->write(bytearray);
    }
    infoLabel->setText(QString(bytearray));
}

void ChattingServer::removeItem()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    clientList.removeOne(clientConnection);
    clientConnection->deleteLater();
}

QString ChattingServer::getInfoLable()
{
    QString label = infoLabel->text();
    return label;
}
