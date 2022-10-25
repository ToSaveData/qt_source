#include "orderinformaiton.h"

OrderInformaiton::OrderInformaiton                              //생성자
(int oid, QString orderDate, int orderNumber, int cid, int pid)
    : m_OID(oid), m_orderDate(orderDate), m_orderNumber(orderNumber),
      m_CID(cid), m_PID(pid)    {}

QString OrderInformaiton::getOrderDate()                        //주문 일자 반환
{
    return m_orderDate;
}

int OrderInformaiton::getOrderNumber()                          //주문 수량 반환
{
    return m_orderNumber;
}

int OrderInformaiton::getCID()                                  //고객 ID(ClientInformation 클래스 PK) 반환
{
    return m_CID;
}

int OrderInformaiton::getPID()                                  //제품 ID(ProductInformation 클래스 PK) 반환
{
    return m_PID;
}

