#include "orderinformaiton.h"
#include "orderhandlerform.h"
#include "ui_orderhandlerform.h"
#include <QTableWidgetItem>
#include <QList>
#include <QFile>

OrderHandlerForm::OrderHandlerForm(QWidget *parent) :
    QWidget(parent),
    Oui(new Ui::OrderHandlerForm)
{
    Oui->setupUi(this);
}

OrderHandlerForm::~OrderHandlerForm()
{
    delete Oui;
}

int OrderHandlerForm::makeoid()
{
    if(orderInfo.isEmpty())    return 10000;
    else    return orderInfo.size() + 10000;
}
