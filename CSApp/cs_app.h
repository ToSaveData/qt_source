#ifndef CS_APP_H
#define CS_APP_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CS_App; }
QT_END_NAMESPACE

class ClientHandlerForm;
class ProductHandlerForm;
class OrderHandlerForm;
class ChatServerForm;

class CS_App : public QMainWindow                                           //메인 윈도우 클래스
{
    Q_OBJECT

public:
    CS_App(QWidget *parent = nullptr);                                      //생성자
    ~CS_App();                                                              //소멸자

private slots:
    void on_actiontr_ClientHandlerForm_triggered();                         //고객 정보 폼 액션이 호출될 때

    void on_actiontr_ProductHandlerForm_triggered();                        //제품 정보 폼 액션이 호출될 때

    void on_actiontr_OrderHandlerForm_triggered();                          //주문 정보 폼 액션이 호출될 때

    void on_actiontr_ChatServerForm_triggered();                            //채팅 서버 폼 액션이 호출될 때

private:
    Ui::CS_App *ui;                                                         //UI
    ClientHandlerForm *CForm;                                               //고객 정보 관리 폼
    ProductHandlerForm *PForm;                                              //제품 정보 관리 폼
    OrderHandlerForm *OForm;                                                //주문 정보 관리 폼
    ChatServerForm *ChattingForm;                                           //채팅 서버 관리 폼
};
#endif // CS_APP_H
