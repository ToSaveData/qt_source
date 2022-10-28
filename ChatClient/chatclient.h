#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QWidget>
#include <QDataStream>

class QTextEdit;
class QLineEdit;
class QTcpSocket;
class QPushButton;
class QFile;
class QProgressDialog;

typedef enum {
    Chat_Login,                                         // 로그인(서버 접속) --> 초대를 위한 정보 저장
    Chat_logCheck,                                      // 로그인을 확인하는 프로토콜
    Chat_In,                                            // 채팅방 입장
    Chat_Talk,                                          // 채팅
    Chat_Out,                                           // 채팅방 퇴장 --> 초대 가능
    Chat_LogOut,                                        // 로그 아웃(서버 단절) --> 초대 불가능
    Chat_Invite,                                        // 초대
    Chat_KickOut,                                       // 강퇴
} Chat_Status;

class ChatClient : public QWidget                       //채팅 클라이언트 클래스
{
    Q_OBJECT
public:
    const int PORT_NUMBER = 8000;                       //채팅용 서버의 포트를 8000번으로 고정

    explicit ChatClient(QWidget *parent = nullptr);     //생성자
    ~ChatClient();                                      //소멸자

private slots:
    void connectButtonClicked();                        //connectButton의 클릭 시그널을 처리하는 함수
    void receiveData();                                 //서버에서 데이터를 받는 함수
    void sendData();                                    //서버로 메세지를 보내는 함수
    void disconnect();                                  //서버와의 연결을 끊는 함수
    void sendProtocol(Chat_Status, char*, int = 1020);  //서버에 프로토콜을 보내는 함수
    void sendFile();                                    //파일을 보내는 함수
    void goOnSend(qint64);                              //파일을 여러 번 나눠서 보내기 위한 함수

private:
    void closeEvent(QCloseEvent*) override;             //창이 닫힐 때 이벤트를 처리하기 위한 재구현함수

    /*채팅 클라이언트의 입력 및 출력 위젯 모음*/
    QLineEdit *name;                                    //서버에 접속할 이름 입력란
    QLineEdit* serverAddress;                           //접속할 서버의 IP 주소 입력란
    QLineEdit* serverPort;                              //접속할 서버의 포트번호 입력란
    QLineEdit *inputLine;                               //서버로 보내는 메시지 입력란
    QTextEdit *message;                                 //서버에서 오는 메세지 표시용

    /*채팅 클라이언트의 버튼 모음*/
    QPushButton *connectButton;                         //서버 로그인 등 접속 처리 버튼
    QPushButton *sentButton;                            //메시지 전송 버튼
    QPushButton* fileButton;                            //파일 전송 버튼

    /*채팅 클라이언트의 소켓 모음*/
    QTcpSocket *clientSocket;                           //채팅용 소켓
    QTcpSocket *fileClient;                             //파일 전송용 소켓

    /*파일 전송에 필요한 객체 모음*/
    QProgressDialog* progressDialog;                    //파일 진행 확인
    QFile* file;                                        //서버로 보내는 파일
    qint64 loadSize;                                    //파일의 크기
    qint64 byteToWrite;                                 //남은 파일의 크기
    qint64 totalSize;                                   //전체 파일의 크기
    QByteArray outBlock;                                //전송을 위한 데이터
    bool isSent;                                        //파일 서버에 접속되었는지 확인

};

#endif // CHATCLIENT_H
