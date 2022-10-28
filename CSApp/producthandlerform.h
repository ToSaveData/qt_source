#ifndef PRODUCTHANDLERFORM_H
#define PRODUCTHANDLERFORM_H

#include <QWidget>
#include "productinformaiton.h"

namespace Ui {
class ProductHandlerForm;
}

class ProductInformaiton;
class QTableWidgetItem;
class QComboBox;

class ProductHandlerForm : public QWidget                       //제품 정보를 활용하는 클래스
{
    Q_OBJECT

public:
    explicit ProductHandlerForm(QWidget *parent = nullptr);     //생성자
    ~ProductHandlerForm();                                      //소멸자


signals:
    void productAdded(int);                                     //제품 정보가 추가됐음을 주문 정보 클래스에
                                                                //전달하는 시그널

    void productRemoved(int);                                   //제품 정보가 삭제됐음을 주문 정보 클래스에
                                                                //전달하는 시그널

    void productModified(int, QList<QString>);                  //제품 정보가 수정됐음을 주문 정보 클래스에
                                                                //전달하는 시그널

    void addReturn(QList<QString>);                             //주문 정보 등록 시 요청한 제품 정보를
                                                                //인자로 담아 반환하는 시그널

    void searchReturn(QList<QString>);                          //주문 정보 검색 시 요청한 제품 정보를
                                                                //인자로 담아 반환하는 시그널

    void modifyReturn(QList<QString>, int);                     //주문 정보 수정 시 요청한 제품 정보를
                                                                //인자로 담아 반환하는 시그널

private:
    Ui::ProductHandlerForm *Pui;                                //UI
    QMap<int, ProductInformaiton*> productInfo;                 //제품 ID를 key로 설정하여 제품 정보를 저장
    int makePid();                                              //제품 ID를 생성하는 함수

private slots:
    void on_enrollPushButton_clicked();                         //등록 버튼
    void on_searchPushButton_clicked();                         //검색 버튼
    void on_removePushButton_clicked();                         //삭제 버튼
    void on_modifyPushButton_clicked();                         //수정 버튼
    void on_tableWidget5_itemClicked(QTableWidgetItem *item);   //수정 시 선택된 데이터를 LineEdit 위젯에
                                                                //미리 채우기 위한 슬롯함수

    void orderAddedProduct(int);                                //주문 정보가 추가될 때 필요한 제품 정보를
                                                                //전달하기 위한 슬롯함수

    void orderSearchedProduct(int);                             //주문 정보를 검색할 때 필요한 제품 정보를
                                                                //전달하기 위한 슬롯함수

    void orderModifiedProduct(int, int);                        //주문 정보를 수정할 때 필요한 제품 정보를
                                                                //전달하기 위한 슬롯 함수

    void setProductComboBox(QComboBox*, QComboBox*);            //주문 정보에 들어간 제품 정보 콤보박스의
                                                                //데이터를 채우기 위한 슬롯 함수
};

#endif // PRODUCTHANDLERFORM_H
