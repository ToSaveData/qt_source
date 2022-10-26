#ifndef ORDERHANDLERFORM_H
#define ORDERHANDLERFORM_H

#include <QWidget>

namespace Ui {
class OrderHandlerForm;
}
class OrderInformaiton;
class QTableWidgetItem;
class QComboBox;
class OrderHandlerForm : public QWidget                             //주문 정보를 활용하는 클래스
{
    Q_OBJECT

public:
    explicit OrderHandlerForm(QWidget *parent = nullptr);           //생성자
    ~OrderHandlerForm();                                            //소멸자
    void dataload();                                                //파일 입력 함수
    int cnt = 0;                                                    //고객 정보 및 제품정보의 행을 순서대로 넣기 위한 변수

public slots:
    void clientAdded();                                             //고객 정보가 추가됐다는 시그널을 받는 슬롯함수
    void productAdded();                                            //제품 정보가 추가됐다는 시그널을 받는 슬롯함수

    void clientRemoved(int);                                        //고객 정보가 삭제됐다는 시그널을 받는 슬롯함수
    void productRemoved(int);                                       //제품 정보가 삭제됐다는 시그널을 받는 슬롯함수

    void clientModified(int, QList<QString>);                       //고객 정보가 수정됐다는 시그널을 받는 슬롯함수
    void productModified(int, QList<QString>);                      //제품 정보가 수정됐다는 시그널을 받는 슬롯함수

signals:
    void orderAddedClient(int);                                     //주문 정보를 추가할 때, 고객 정보 클래스에 보내는 시그널
    void orderAddedProduct(int);                                    //주문 정보를 추가할 때, 제품 정보 클래스에 보내는 시그널

    void orderSearchedClient(int);                                  //주문 정보를 검색할 때, 고객 정보 클래스에 보내는 시그널
    void orderSearchedProduct(int);                                 //주문 정보를 검색할 때, 제품 정보 클래스에 보내는 시그널

    void orderModifiedClient(int, int);                             //주문 정보를 변경할 때, 고객 정보 클래스에 보내는 시그널
    void orderModifiedProduct(int, int);                            //주문 정보를 변경할 때, 제품 정보 클래스에 보내는 시그널

    void clientComboBox(QComboBox*, QComboBox*);                    //고객 정보 콤보박스를 채울 때, 고객 정보 클래스에 보내는 시그널
    void productComboBox(QComboBox*, QComboBox*);                   //제품 정보 콤보박스를 채울 때, 제품 정보 클래스에 보내는 시그널

private slots:
    void addReturnClient(QList<QString>);                           //주문 정보를 추가할 때, 고객 정보 클래스에서 돌아온
                                                                    //시그널을 받는 슬롯함수
    void addReturnProduct(QList<QString>);                          //주문 정보를 추가할 때, 제품 정보 클래스에서 돌아온
                                                                    //시그널을 받는 슬롯함수

    void searchReturnClient(QList<QString>);                        //주문 정보를 검색할 때, 고객 정보 클래스에서 돌아온
                                                                    //시그널을 받는 슬롯함수
    void searchReturnProduct(QList<QString>);                       //주문 정보를 검색할 때, 제품 정보 클래스에서 돌아온
                                                                    //시그널을 받는 슬롯함수

    void modifyReturnClient(QList<QString>, int);                   //주문 정보를 수정할 때, 고객 정보 클래스에서 돌아온
                                                                    //시그널을 받는 슬롯함수
    void modifyReturnProduct(QList<QString>, int);                  //주문 정보를 수정할 때, 제품 정보 클래스에서 돌아온
                                                                    //시그널을 받는 슬롯함수

    void on_enrollPushButton_clicked();                             //등록 버튼을 눌렀을 때
    void on_searchPushButton_clicked();                             //검색 버튼을 눌렀을 때
    void on_removePushButton_clicked();                             //삭제 버튼을 눌렀을 때
    void on_modifyPushButton_clicked();                             //수정 버튼을 눌렀을 때
    void on_tableWidget5_itemClicked(QTableWidgetItem *item);       //수정 시 선택된 데이터를 LineEdit 위젯에
                                                                    //미리 채우기 위한 슬롯함수
private:
    Ui::OrderHandlerForm *Oui;                                      //UI
    QMap<int, OrderInformaiton*> orderInfo;                         //주문 ID를 key로 설정하여 주문 정보를 저장
    int makeoid();                                                  //주문 ID를 생성하는 함수
};

#endif // ORDERHANDLERFORM_H
