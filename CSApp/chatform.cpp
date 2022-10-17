#include "chatform.h"
#include "ui_chatform.h"
#include "chattingserver.h"
#include "chattingclient.h"
#include <QLineEdit>
#include <QTreeWidgetItem>

ChatForm::ChatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);
    ChattingServer *server = new ChattingServer;
    ui->currentServerLabel->setText(server->getInfoLable());
}

ChatForm::~ChatForm()
{
    delete ui;
}

void ChatForm::on_newChatPushButton_clicked()
{
    ChattingClient *chat = new ChattingClient(0);
    chat->show();
}

void ChatForm::isertClient(QList<QString> clist)
{
    Q_FOREACH(auto i, clist)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, "X");
        item->setText(1, i);
        ui->clientTreeWidget->addTopLevelItem(item);
    }
}
