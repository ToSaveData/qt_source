#include "logthread.h"

#include <QTreeWidgetItem>
#include <QFile>
#include <QDateTime>

LogThread::LogThread(QObject *parent)                                   //생성자
    : QThread{parent}
{
    QString format = "yyyyMMdd_hhmmss";                                 //년월일_시분초 형식 저장
    filename = QString("log_%1.txt").                                   //파일 이름을 현재 시간으로 설정
                arg(QDateTime::currentDateTime().toString(format));
}

void LogThread::run()                                                   //로그를 저장하는 함수
{
    Q_FOREVER
    {
        saveData();
        sleep(60);                                                      // 1분마다 저장
    }
}

void LogThread::appendData(QTreeWidgetItem* item)                       //저장할 로그를 배열에 추가하는 함수
{
    itemList.append(item);                                              //배열에 아이템을 저장
}

void LogThread::saveData()                                              //로그를 파일로 출력하는 함수
{
    if(itemList.count() > 0)                                            //저장할 로그가 있으면
    {
        QFile file(filename);                                           //파일 생성
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))         //파일 열기 예외처리
            return;

        QTextStream out(&file);
        foreach(auto item, itemList)                                    //저장된 아이템의 수만큼 반복
        {                                                               //", "를 기준으로 분리
            out << item->text(0) << ", ";
            out << item->text(1) << ", ";
            out << item->text(2) << ", ";
            out << item->text(3) << ", ";
            out << item->text(4) << ", ";
            out << item->text(5) << "\n";
        }
        file.close();                                                   //파일 출력 종료
    }
}
