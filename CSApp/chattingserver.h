#ifndef CHATTINGSERVER_H
#define CHATTINGSERVER_H

#include <QWidget>

class QLabel;
class QTcpServer;
class QTcpSocket;

class ChattingServer : public QWidget
{
    Q_OBJECT

public:
    explicit ChattingServer(QWidget *parent = nullptr);
    QString getInfoLable();

private slots:
    void clientConnect( );                       /* 에코 서버 */
    void echoData( );
    void removeItem( );
private:
    QLabel *infoLabel;
    QTcpServer *tcpServer;

    QList<QTcpSocket*> clientList;
};

#endif // CHATTINGSERVER_H
