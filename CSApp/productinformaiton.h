#ifndef PRODUCTINFORMAITON_H
#define PRODUCTINFORMAITON_H
#include <QTableWidgetItem>
#include <QString>

class ProductInformaiton : public QTableWidgetItem
{
public:
    ProductInformaiton(int, QString, int productPrice, QString); //생성자
    QString getProductName() const; //제품명 반환
    int getProductPrice() const; //제품 가격 반환
    QString getProductSort() const; //제품 종류 반환

private:
    int m_productID; //제품ID, PK
    QString m_productName; //제품명
    int m_productPrice; //제품 가격
    QString m_productSort; //제품 종류
};

#endif // PRODUCTINFORMAITON_H
