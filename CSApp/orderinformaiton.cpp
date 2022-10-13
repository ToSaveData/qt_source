#include "orderinformaiton.h"

OrderInformaiton::OrderInformaiton(int oid, QString orderDate, int orderNumber, int cid, int pid)
    : m_OID(oid), m_orderDate(orderDate), m_orderNumber(orderNumber),
m_CID(cid), m_PID(pid)
{}

int OrderInformaiton::getOrderCode() { return m_OID; } //주문코드 반환

QString OrderInformaiton::getOrderDate() { return m_orderDate; } //주문일자 반환

int OrderInformaiton::getOrderNumber() { return m_orderNumber; } //주문수량 반환

int OrderInformaiton::getCID() { return m_CID; } //고객 전화번호(Client 클래스 PK) 반환

int OrderInformaiton::getPID() { return m_PID; } //제품ID(Product클래스 PK) 반환

