#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>
#include <QList>

class QTreeWidgetItem;

class LogThread : public QThread                            //로그를 저장할 스레드를 활용하는 클래스
{
    Q_OBJECT
public:
    explicit LogThread(QObject *parent = nullptr);          //생성자

private:
    void run();                                             //로그를 저장하는 함수

    QList<QTreeWidgetItem*> itemList;                       //로그의 내용을 저장하는 배열
    QString filename;                                       //파일의 이름

public slots:
    void appendData(QTreeWidgetItem*);                      //저장할 로그를 배열에 추가하는 함수
    void saveData();                                        //로그를 txt파일로 출력하는 함수
};

#endif // LOGTHREAD_H
