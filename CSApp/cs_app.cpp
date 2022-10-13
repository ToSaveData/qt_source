#include "cs_app.h"
#include "ui_cs_app.h"
#include "clienthandlerform.h"
#include "producthandlerform.h"
#include "orderhandlerform.h"
#include <QMdiSubWindow>

CS_App::CS_App(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CS_App), CForm(nullptr), PForm(nullptr), OForm(nullptr)
{
    ui->setupUi(this);

    CForm = new ClientHandlerForm();
    ui->mdiArea->addSubWindow(CForm);
    CForm->setWindowTitle(tr("ClientInformationHandlerForm"));

    PForm = new ProductHandlerForm();
    ui->mdiArea->addSubWindow(PForm);
    PForm->setWindowTitle(tr("ProductInformationHandlerForm"));

    OForm = new OrderHandlerForm();
    ui->mdiArea->addSubWindow(OForm);
    OForm->setWindowTitle(tr("OrderInformationHandlerForm"));
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

