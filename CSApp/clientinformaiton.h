#ifndef CLIENTINFORMAITON_H
#define CLIENTINFORMAITON_H
#include <QString>
#include <QWidget>

class ClientInformaiton : public QWidget                           //고객 정보를 저장하는 클래스
{
public:
    ClientInformaiton(int, QString, QString, QString, QString, QString);    // 생성자
    QString getName() const;                                                //고객 성명 반환
    QString getBirthday() const;                                            //고객 생년월일 반환
    QString getPhoneNumber() const;                                         //고객 전화번호 반환
    QString getAddress() const;                                             //고객 주소 반환
    QString getEmail() const;                                               //고객 이메일주소 반환
private:
    int m_clientid;                                                         //고객 ID, PK
    QString m_name;                                                         //고객 성명
    QString m_birthday;                                                     //고객 생년월일
    QString m_phoneNumber;                                                  //고객 전화번호
    QString m_address;                                                      //고객 주소
    QString m_email;                                                        //고객 이메일주소

};

#endif // CLIENTINFORMAITON_H
