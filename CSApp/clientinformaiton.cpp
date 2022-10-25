#include "clientinformaiton.h"

ClientInformaiton::ClientInformaiton                                    //생성자
(int id, QString name, QString birthday, QString phoneNumber,
 QString address, QString email) : m_clientid(id), m_name(name),
    m_birthday(birthday), m_phoneNumber(phoneNumber),
    m_address(address), m_email(email)  {}

QString ClientInformaiton::getName() const                              //고객 성명 반환
{
    return m_name;
}

QString ClientInformaiton::getBirthday() const                          //고객 생년월일 반환
{
    return m_birthday;
}

QString ClientInformaiton::getPhoneNumber() const                       //고객 전화번호 반환
{
    return m_phoneNumber;
}

QString ClientInformaiton::getAddress() const                           //고객 주소 반환
{
    return m_address;
}

QString ClientInformaiton::getEmail() const                             //고객 이메일주소 반환
{
    return m_email;
}
