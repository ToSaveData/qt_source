#include "chatform.h"
#include "ui_chatform.h"
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

ChatForm::ChatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatForm), totalSize(0), byteReceived(0)
{
    ui->setupUi(this);
    QList<int> sizes;
    sizes << 120 << 500;
    ui->splitter->setSizes(sizes);

    chatServer = new QTcpServer(this);
    connect(chatServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));
    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(chatServer->errorString( )));
        close( );
        return;
    }

    fileServer = new QTcpServer(this);
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(fileServer->errorString( )));
        close( );
        return;
    }

    qDebug("Start listening ...");

    QAction* inviteAction = new QAction(tr("&Invite"));
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), SLOT(inviteClient()));

    QAction* removeAction = new QAction(tr("&Kick out"));
    connect(removeAction, SIGNAL(triggered()), SLOT(kickOut()));

    menu = new QMenu;
    menu->addAction(inviteAction);
    menu->addAction(removeAction);
    ui->waittingRoomTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->chattingRoomTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    logThread = new LogThread(this);
    logThread->start();

    connect(ui->savePushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));

    qDebug() << tr("The server is running on port %1.").arg(chatServer->serverPort( ));
}

ChatForm::~ChatForm()
{
    delete ui;

    logThread->terminate();
    chatServer->close( );
    fileServer->close( );
}

void ChatForm::addClient(QList<int> cIdInfo, QList<QString> cNameInfo)
{
    int cnt = 0;
    Q_FOREACH(auto i, cNameInfo)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setIcon(0, QIcon(":/icon_image/redLight.png"));
        item->setText(1, i + "  ");
        ui->waittingRoomTreeWidget->addTopLevelItem(item);
        clientIDHash[i] = cIdInfo[cnt];
        ui->waittingRoomTreeWidget->resizeColumnToContents(0);
    }
}

void ChatForm::clientConnect( )
{
    QTcpSocket *clientConnection = chatServer->nextPendingConnection( );
    connect(clientConnection, SIGNAL(readyRead( )), SLOT(receiveData( )));
    connect(clientConnection, SIGNAL(disconnected( )), SLOT(removeClient()));
    qDebug("new connection is established...");
}

void ChatForm::receiveData( )
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    Chat_Status type;       // 채팅의 목적
    char data[1020];        // 전송되는 메시지/데이터
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    QString ip = clientConnection->peerAddress().toString();
    quint16 port = clientConnection->peerPort();
    QString name = QString::fromStdString(data);

    qDebug() << ip << " : " << type;

    switch(type) {
    case Chat_Login:
        qDebug() << ui->waittingRoomTreeWidget->findItems(name + "  ", Qt::MatchFixedString, 1).count();
        if(ui->waittingRoomTreeWidget->findItems(name + "  ", Qt::MatchFixedString, 1).count() <= 0) {
            return;
        }   else {
            QByteArray sendArray;
            QDataStream out(&sendArray, QIODevice::WriteOnly);
            out << Chat_logCheck;
            out.writeRawData("", 1020);
            clientConnection->write(sendArray);
        }

        foreach(auto item, ui->waittingRoomTreeWidget->findItems(name + "  ", Qt::MatchFixedString, 1)) {
            if(item->text(1) != name + " ") {
                item->setIcon(0, QIcon(":/icon_image/greenLight.png"));
                item->setText(1, name + " ");
                clientList.append(clientConnection);        // QList<QTcpSocket*> clientList;
                clientSocketHash[name] = clientConnection;
            }
        }
        break;
    case Chat_In:
        foreach(auto item, ui->waittingRoomTreeWidget->findItems(name + " ", Qt::MatchFixedString, 1)) {
            if(item->text(1) != name) {
                item->setIcon(0, QIcon(":/icon_image/chatting1.png"));
                item->setText(1, name);
            }
            clientNameHash[port] = name;
            int index = ui->waittingRoomTreeWidget->indexOfTopLevelItem(item);
            ui->waittingRoomTreeWidget->takeTopLevelItem(index);
            ui->chattingRoomTreeWidget->addTopLevelItem(item);
            for(int i = 0; i < ui->waittingRoomTreeWidget->columnCount(); i++)
                ui->chattingRoomTreeWidget->resizeColumnToContents(i);
        }
        break;
    case Chat_Talk: {
        foreach(QTcpSocket *sock, clientList) {
            if(clientNameHash.contains(sock->peerPort()) && sock != clientConnection) {
                QByteArray sendArray;
                sendArray.clear();
                QDataStream out(&sendArray, QIODevice::WriteOnly);
                out << Chat_Talk;
                sendArray.append("<font color=lightsteelblue>");
                sendArray.append(clientNameHash[port].toStdString().data());
                sendArray.append("</font> : ");
                sendArray.append(name.toStdString().data());
                sock->write(sendArray);
                qDebug() << sock->peerPort();
            }
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);
        item->setText(0, ip);
        item->setText(1, QString::number(port));
        item->setText(2, QString::number(clientIDHash[clientNameHash[port]]));
        item->setText(3, clientNameHash[port]);
        item->setText(4, QString(data));
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, QString(data));
        ui->logTreeWidget->addTopLevelItem(item);

        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++)
            ui->logTreeWidget->resizeColumnToContents(i);

        logThread->appendData(item);
    }
        break;
    case Chat_Out:
        foreach(auto item, ui->chattingRoomTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            if(item->text(1) != name + " ") {
                item->setIcon(0, QIcon(":/icon_image/greenLight.png"));
                item->setText(1, name + " ");
            }
            clientNameHash.remove(port);
            int index = ui->chattingRoomTreeWidget->indexOfTopLevelItem(item);
            ui->chattingRoomTreeWidget->takeTopLevelItem(index);
            ui->waittingRoomTreeWidget->addTopLevelItem(item);
        }
        break;
    case Chat_LogOut:
        foreach(auto item, ui->waittingRoomTreeWidget->findItems(name + " ", Qt::MatchContains, 1)) {
            if(item->text(1) != name + "  ") {
                item->setIcon(0, QIcon(":/icon_image/redLight.png"));
                item->setText(1, name + "  ");
                clientList.removeOne(clientConnection);        // QList<QTcpSocket*> clientList;
                clientSocketHash.take(name);
            }
        }
        break;
    }
}

void ChatForm::removeClient()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));

    QString name = clientNameHash[clientConnection->peerPort()];
    foreach(auto item, ui->chattingRoomTreeWidget->findItems(name, Qt::MatchContains, 1)) {
        int index = ui->chattingRoomTreeWidget->indexOfTopLevelItem(item);
        item->setIcon(0, QIcon(":/icon_image/redLight.png"));
        item->setText(1, name + "  ");
        ui->chattingRoomTreeWidget->takeTopLevelItem(index);
        ui->waittingRoomTreeWidget->addTopLevelItem(item);
    }
    foreach(auto item, ui->waittingRoomTreeWidget->findItems(name + " ", Qt::MatchContains, 1)) {
        item->setIcon(0, QIcon(":/icon_image/redLight.png"));
        item->setText(1, name + "  ");
    }


    clientList.removeOne(clientConnection);
    clientConnection->deleteLater();
}

void ChatForm::kickOut()
{
    QString name = ui->waittingRoomTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientSocketHash[name.left(3)];

    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_KickOut;
    out.writeRawData("", 1020);
    sock->write(sendArray);

    ui->waittingRoomTreeWidget->currentItem()->setIcon(0, QIcon(":/icon_image/greenLight.png"));
}

void ChatForm::inviteClient()
{
    if(ui->waittingRoomTreeWidget->topLevelItemCount()) {
        QString name = ui->waittingRoomTreeWidget->currentItem()->text(1);


        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out << Chat_Invite;
        out.writeRawData("", 1020);
        QTcpSocket* sock = clientSocketHash[name.left(3)];
        sock->write(sendArray);
        qDebug() << sock->peerPort();

        quint16 port = sock->peerPort();
        clientNameHash[port] = name.left(3);

        foreach(auto item, ui->waittingRoomTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {
            if(item->text(1) != name.left(3)) {
                item->setIcon(0, QIcon(":/icon_image/chatting1.png"));
                item->setText(1, name.left(3));
                int index = ui->waittingRoomTreeWidget->indexOfTopLevelItem(item);
                ui->waittingRoomTreeWidget->takeTopLevelItem(index);
                ui->chattingRoomTreeWidget->addTopLevelItem(item);
            }
        }
    }
}

/* 파일 전송 */
void ChatForm::acceptConnection()
{
    qDebug("Connected, preparing to receive files!");

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

void ChatForm::readClient()
{
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));
    QString filename, name;

    if (byteReceived == 0) { // just started to receive data, this data is file information
        progressDialog->reset();
        progressDialog->show();

        QString ip = receivedSocket->peerAddress().toString();
        quint16 port = receivedSocket->peerPort();

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> name;
        progressDialog->setMaximum(totalSize);

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);
        item->setText(0, ip);
        item->setText(1, QString::number(port));
        item->setText(2, QString::number(clientIDHash[name]));
        item->setText(3, name);
        item->setText(4, filename);
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, filename);

        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++)
            ui->logTreeWidget->resizeColumnToContents(i);

        ui->logTreeWidget->addTopLevelItem(item);

        logThread->appendData(item);

        QFileInfo info(filename);
        QString currentFileName = info.fileName();
        file = new QFile(currentFileName);
        file->open(QFile::WriteOnly);
    } else { // Officially read the file content
        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        file->write(inBlock);
        file->flush();
    }

    progressDialog->setValue(byteReceived);

    if (byteReceived == totalSize) {
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();
        progressDialog->hide();
        file->close();
        delete file;
    }
}

void ChatForm::on_waittingRoomTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->waittingRoomTreeWidget->currentItem() != nullptr)
    {
    QString name = ui->waittingRoomTreeWidget->currentItem()->text(1).left(3);
    foreach(QAction *action, menu->actions()) {
        if(action->objectName() == "Invite")
            action->setEnabled(ui->waittingRoomTreeWidget->currentItem()->text(1) != name);
        else
            action->setEnabled(ui->waittingRoomTreeWidget->currentItem()->text(0) == name);
    }
    QPoint globalPos = ui->waittingRoomTreeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
    }
}


void ChatForm::on_chattingRoomTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->chattingRoomTreeWidget->currentItem() != nullptr)
    {
    QString name = ui->chattingRoomTreeWidget->currentItem()->text(1);
    foreach(QAction *action, menu->actions()) {
        if(action->objectName() == "Invite")
            action->setEnabled(ui->chattingRoomTreeWidget->currentItem()->text(1) == name);
        else
            action->setEnabled(ui->chattingRoomTreeWidget->currentItem()->text(0) != name);
    }
    QPoint globalPos = ui->waittingRoomTreeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
    }
}

void ChatForm::on_resetPushButton_clicked()
{
    ui->waittingRoomTreeWidget->clear();
    emit reset();
}

