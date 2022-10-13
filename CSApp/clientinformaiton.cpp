#include "clientinformaiton.h"

ClientInformaiton::ClientInformaiton
(int id, QString name, QString birthday, QString phoneNumber, QString address, QString email)
    : m_clientid(id), m_name(name), m_birthday(birthday), m_phoneNumber(phoneNumber), m_address(address), m_email(email)
{}    //생성자

QString ClientInformaiton::getName() const { return m_name; } //고객 성명 반환

QString ClientInformaiton::getBirthday() const { return m_birthday; } //고객 생년월일 반환

QString ClientInformaiton::getPhoneNumber() const { return m_phoneNumber; } //고객 전화번호 반환

QString ClientInformaiton::getAddress() const { return m_address; } //고객 주소 반환

QString ClientInformaiton::getEmail() const { return m_email; } //고객 이메일주소 반환
