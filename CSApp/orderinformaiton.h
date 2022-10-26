#ifndef ORDERINFORMAITON_H
#define ORDERINFORMAITON_H
#include <QWidget>
#include <QString>

class OrderInformaiton : public QWidget                             //제품 정보를 저장하는 클래스
{
public:
    OrderInformaiton(int, QString, int,	int, int);                  //생성자
    QString getOrderDate();                                         //주문일자 반환
    int getOrderNumber();                                           //주문수량 반환
    int getCID();                                                   //고객 ID(ClientInformation 클래스 PK) 반환
    int getPID();                                                   //제품 ID(ProductInformation 클래스 PK) 반환

private:
    int m_OID;                                                      //주문코드, PK
    QString m_orderDate;                                            //주문일자
    int m_orderNumber;                                              //주문수량
    int m_CID;                                                      //고객 ID(ClientInformation 클래스 PK)
    int m_PID;                                                      //제품ID(ProductInformation 클래스 PK)
};

#endif // ORDERINFORMAITON_H
