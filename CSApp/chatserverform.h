#ifndef CHATSERVERFORM_H
#define CHATSERVERFORM_H

#include <QWidget>
#include <QList>
#include <QHash>

class QLabel;
class QTcpServer;
class QTcpSocket;
class QFile;
class QProgressDialog;
class LogThread;

namespace Ui {
class ChatServerForm;
}

typedef enum {                                          //프로토콜을 enum클래스로 저장
    Chat_Login,                                         // 로그인(서버 접속)   --> 초대를 위한 정보 저장
    Chat_logCheck,                                      // 로그인 시 이름 확인
    Chat_In,                                            // 채팅방 입장
    Chat_Talk,                                          // 채팅
    Chat_Out,                                           // 채팅방 퇴장         --> 초대 가능
    Chat_LogOut,                                        // 로그 아웃(서버 단절) --> 초대 불가능
    Chat_Invite,                                        // 초대
    Chat_KickOut,                                       // 강퇴
} Chat_Status;

class ChatServerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatServerForm(QWidget *parent = nullptr); //생성자
    ~ChatServerForm();                                  //소멸자

signals:
    void reset();                                       //서버의 고객 목록을 초기화하는 함수

private slots:
    void acceptConnection();                            //파일 서버의 소켓을 연결하는 함수
    void readClient();                                  //클라이언트가 보내는 파일을 read하는 함수
    void clientConnect();                               //서버와 클라이언트 소켓을 연결하는 함수
    void receiveData();                                 //클라이언트의 프로토콜을 받고 처리하는 함수
    void removeClient();                                //서버와의 연결이 끊어질 경우 소켓 목록에서 삭제하는 함수
    void addClientInfo(QList<int>, QList<QString>);     //고객 정보를 추가하는 함수
    void modifyClientInfo(int, QString);                //고객 정보를 수정하는 함수
    void removeClientInfo(QString);                     //고객 정보를 삭제하는 함수
    void inviteClient();                                //클라이언트에게 초대 프로토콜을 보내는 함수
    void kickOut();                                     //클라이언트에게 강퇴 프로토콜을 보내는 함수

    /*고객 목록에서 컨텍스트 메뉴를 띄우는 함수*/
    void on_waittingRoomTreeWidget_customContextMenuRequested(const QPoint &pos);
    void on_chattingRoomTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_resetPushButton_clicked();                  //고객 목록을 다시 불러오는 함수

private:
    const int BLOCK_SIZE = 1024;                        //한 번에 보낼 바이트의 크기를 1024로 고정
    const int PORT_NUMBER = 8000;                       //서버의 포트번호를 8000으로 고정

    Ui::ChatServerForm *ui;                             //UI
    QTcpServer *chatServer;                             //채팅용 서버
    QTcpServer *fileServer;                             //파일 전송용 서버
    QList<QTcpSocket*> clientList;                      //로그인 한 고객의 소켓을 저장하는 배열
    //QList<int> clientIDList;                            //고객의 ID를 저장하는 배열
    QHash<quint16, QString> clientNameHash;             //고객이 접속한 소켓의 포트번호를 key로 이름을 저장하는 해쉬
    QHash<QString, QTcpSocket*> clientSocketHash;       //고객의 이름을 key로 소켓을 저장하는 해쉬
    QHash<QString, int> clientIDHash;                   //고객의 이름을 key로 id를 저장하는 해쉬
    QMenu* menu;                                        //컨텍스트 메뉴를 만들기 위한 메뉴 객체
    QFile* file;                                        //파일 전송을 위한 file 객체
    QProgressDialog* progressDialog;                    //진행률을 나타내는 progressDialog
    qint64 totalSize;                                   //파일의 총 사이즈
    qint64 byteReceived;                                //현재까지 읽어진 파일의 크기
    QByteArray inBlock;                                 //파일을 읽어서 전송할 byteArray
    LogThread* logThread;                               //로그를 저장하는 스레드
};

#endif // CHATSERVERFORM_H
