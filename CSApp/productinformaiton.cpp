#include "productinformaiton.h"

ProductInformaiton::ProductInformaiton(int ID, QString productName, int productPrice, QString productSort):
    m_productID(ID), m_productName(productName), m_productPrice(productPrice), m_productSort(productSort){}
//생성자

QString ProductInformaiton::getProductName() const { return m_productName; } //제품명 반환

int ProductInformaiton::getProductPrice() const { return m_productPrice; } //제품 가격 반환

QString ProductInformaiton::getProductSort() const { return m_productSort; } //제품 종류 반환
