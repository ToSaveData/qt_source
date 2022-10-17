#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>

namespace Ui {
class ChatForm;
}

class ChatForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatForm(QWidget *parent = nullptr);
    ~ChatForm();

private slots:
    void on_newChatPushButton_clicked();
    void isertClient(QList<QString>);

private:
    Ui::ChatForm *ui;
};

#endif // CHATFORM_H
