#include "cs_app.h"
#include "ui_cs_app.h"
#include "clienthandlerform.h"
#include "producthandlerform.h"
#include "orderhandlerform.h"
#include "chatserverform.h"
#include <QMdiSubWindow>
#include <QMap>

CS_App::CS_App(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CS_App)
{
    ui->setupUi(this);

    CForm = new ClientHandlerForm();
    CForm->setWindowTitle(tr("ClientInformationHandlerForm"));

    PForm = new ProductHandlerForm();
    PForm->setWindowTitle(tr("ProductInformationHandlerForm"));

    OForm = new OrderHandlerForm();
    OForm->setWindowTitle(tr("OrderInformationHandlerForm"));

    ChattingForm = new ChatServerForm();
    ChattingForm->setWindowTitle(tr("ChattingServerForm"));

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
    connect(CForm, SIGNAL(clientLoad(QList<int>, QList<QString>)), ChattingForm, SLOT(addClient(QList<int>, QList<QString>)));
    connect(CForm, SIGNAL(sendServer(QList<int>, QList<QString>)), ChattingForm, SLOT(addClient(QList<int>, QList<QString>)));
    connect(OForm, SIGNAL(orderSearchedClient(int)), CForm, SLOT(orderSearchedClient(int)));
    connect(OForm, SIGNAL(orderSearchedProduct(int)), PForm, SLOT(orderSearchedProduct(int)));
    connect(CForm, SIGNAL(searchReturn(QList<QString>)), OForm, SLOT(searchReturnClient(QList<QString>)));
    connect(PForm, SIGNAL(searchReturn(QList<QString>)), OForm, SLOT(searchReturnProduct(QList<QString>)));
    connect(OForm, SIGNAL(orderModifiedClient(int, int)), CForm, SLOT(orderModifiedClient(int, int)));
    connect(OForm, SIGNAL(orderModifiedProduct(int, int)), PForm, SLOT(orderModifiedProduct(int, int)));
    connect(CForm, SIGNAL(modifyReturn(QList<QString>, int)), OForm, SLOT(modifyReturnClient(QList<QString>, int)));
    connect(PForm, SIGNAL(modifyReturn(QList<QString>, int)), OForm, SLOT(modifyReturnProduct(QList<QString>, int)));
    connect(ChattingForm, SIGNAL(reset()), CForm, SLOT(dataload()));
    connect(CForm, SIGNAL(sendServerCModified(int, QString)), ChattingForm, SLOT(modifyClient(int, QString)));

    OForm->dataload();
    CForm->dataload();

    ui->mdiArea->addSubWindow(CForm);
    ui->mdiArea->addSubWindow(PForm);
    ui->mdiArea->addSubWindow(OForm);
    ui->mdiArea->addSubWindow(ChattingForm);

    ui->actiontr_ClientInformationForm->setIcon(QIcon(":/icon_image/client.png"));

    ui->actiontr_ProductInformationForm->setIcon(QIcon(":/icon_image/product.png"));
    ui->actiontr_OrderInformationForm->setIcon(QIcon(":/icon_image/order.png"));
    ui->actiontr_ChattingForm->setIcon(QIcon(":/icon_image/Server.png"));
    ui->actiontr_quit->setIcon(QIcon(":/icon_image/quit.png"));
    ui->toolBar->setIconSize(QSize(50, 50));

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

