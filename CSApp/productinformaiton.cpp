#include "productinformaiton.h"

ProductInformaiton::ProductInformaiton                                      //생성자
(int ID, QString productName, int productPrice, QString productSort):
    m_productID(ID), m_productName(productName),
    m_productPrice(productPrice), m_productSort(productSort)    {}


QString ProductInformaiton::getProductName() const                          //제품명 반환
{
    return m_productName;
}

int ProductInformaiton::getProductPrice() const                             //제품 가격 반환
{
    return m_productPrice;
}

QString ProductInformaiton::getProductSort() const                          //제품 종류 반환
{
    return m_productSort;
}
