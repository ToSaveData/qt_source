#ifndef CHATTINGCLIENT_H
#define CHATTINGCLIENT_H

#include <QWidget>

class QTextEdit;
class QLineEdit;
class QTcpSocket;
class ChattingClient : public QWidget
{
    Q_OBJECT
public:
    explicit ChattingClient(QWidget *parent = nullptr);
    ~ChattingClient();

private slots:
    void echoData();
    void sendData();

private:
    QTextEdit *message;
    QLineEdit *inputLine;
    QTcpSocket *clientSocket;
};

#endif // CHATTINGCLIENT_H
