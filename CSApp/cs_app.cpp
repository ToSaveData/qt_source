#include "cs_app.h"
#include "ui_cs_app.h"
#include "clienthandlerform.h"
#include "producthandlerform.h"
#include "orderhandlerform.h"
#include "chatform.h"
#include <QMdiSubWindow>
#include <QMap>

CS_App::CS_App(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CS_App)
{
    ui->setupUi(this);

    ChattingForm = new ChatForm();
    ui->mdiArea->addSubWindow(ChattingForm);
    ChattingForm->setWindowTitle(tr("ChattingForm"));

    CForm = new ClientHandlerForm();

    ui->mdiArea->addSubWindow(CForm);
    CForm->setWindowTitle(tr("ClientInformationHandlerForm"));

    PForm = new ProductHandlerForm();

    ui->mdiArea->addSubWindow(PForm);
    PForm->setWindowTitle(tr("ProductInformationHandlerForm"));

    OForm = new OrderHandlerForm();
    ui->mdiArea->addSubWindow(OForm);
    OForm->setWindowTitle(tr("OrderInformationHandlerForm"));

    connect(CForm, SIGNAL(clientAdded(int)), OForm, SLOT(clientAdded()));
    connect(PForm, SIGNAL(productAdded(int)), OForm, SLOT(productAdded()));
    connect(OForm, SIGNAL(orderAddedClient(int)), CForm, SLOT(orderAddedClient(int)));
    connect(OForm, SIGNAL(orderAddedProduct(int)), PForm, SLOT(orderAddedProduct(int)));
    connect(CForm, SIGNAL(addReturn(QList<QString>)), OForm, SLOT(addReturnClient(QList<QString>)));
    connect(PForm, SIGNAL(addReturn(QList<QString>)), OForm, SLOT(addReturnProduct(QList<QString>)));
    connect(CForm, SIGNAL(clientRemoved(int)), OForm, SLOT(clientRemoved(int)));
    connect(PForm, SIGNAL(productRemoved(int)), OForm, SLOT(productRemoved(int)));
    connect(OForm, SIGNAL(clientComboBox(QComboBox*, QComboBox*)), CForm,
            SLOT(setClientComboBox(QComboBox*, QComboBox*)));
    connect(OForm, SIGNAL(productComboBox(QComboBox*, QComboBox*)), PForm,
            SLOT(setProductComboBox(QComboBox*, QComboBox*)));
    connect(CForm, SIGNAL(clientModified(int,QList<QString>)), OForm, SLOT(clientModified(int,QList<QString>)));
    connect(PForm, SIGNAL(productModified(int,QList<QString>)), OForm, SLOT(productModified(int,QList<QString>)));
    connect(CForm, SIGNAL(clientLoad(QList<QString>)), ChattingForm, SLOT(addClient(QList<QString>)));
    connect(CForm, SIGNAL(sendServer(QList<QString>)), ChattingForm, SLOT(addClient(QList<QString>)));
    connect(OForm, SIGNAL(orderSearchedClient(int)), CForm, SLOT(orderSearchedClient(int)));
    connect(OForm, SIGNAL(orderSearchedProduct(int)), PForm, SLOT(orderSearchedProduct(int)));
    connect(CForm, SIGNAL(searchReturn(QList<QString>)), OForm, SLOT(searchReturnClient(QList<QString>)));
    connect(PForm, SIGNAL(searchReturn(QList<QString>)), OForm, SLOT(searchReturnProduct(QList<QString>)));
    connect(OForm, SIGNAL(orderModifiedClient(int, int)), CForm, SLOT(orderModifiedClient(int, int)));
    connect(OForm, SIGNAL(orderModifiedProduct(int, int)), PForm, SLOT(orderModifiedProduct(int, int)));
    connect(CForm, SIGNAL(modifyReturn(QList<QString>, int)), OForm, SLOT(modifyReturnClient(QList<QString>, int)));
    connect(PForm, SIGNAL(modifyReturn(QList<QString>, int)), OForm, SLOT(modifyReturnProduct(QList<QString>, int)));
    OForm->dataload();
    CForm->dataload();
}

CS_App::~CS_App()
{
    delete ui;
}



void CS_App::on_actiontr_ClientInformationForm_triggered()
{
    CForm->setFocus();
}

void CS_App::on_actiontr_ProductInformationForm_triggered()
{
    PForm->setFocus();
}


void CS_App::on_actiontr_OrderInformationForm_triggered()
{
    OForm->setFocus();
}


void CS_App::on_actiontr_ChattingForm_triggered()
{
    ChattingForm->setFocus();
}

