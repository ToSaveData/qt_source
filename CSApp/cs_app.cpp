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
    ui->setupUi(this);                                                      //현재 클래스에 UI를 세팅

    /*각 클래스 객체를 생성하고 창 이름을 설정*/
    CForm = new ClientHandlerForm();
    CForm->setWindowTitle(tr("ClientInformationHandlerForm"));
    PForm = new ProductHandlerForm();
    PForm->setWindowTitle(tr("ProductInformationHandlerForm"));
    OForm = new OrderHandlerForm();
    OForm->setWindowTitle(tr("OrderInformationHandlerForm"));
    ChattingForm = new ChatServerForm();
    ChattingForm->setWindowTitle(tr("ChattingServerForm"));

    /*주문 정보 클래스의 콤보박스에 담을 고객, 제품 정보를 요청하는 시그널을 각 슬롯에 연결*/
    connect(OForm, SIGNAL(clientComboBox(QComboBox*, QComboBox*)), CForm,
            SLOT(setClientComboBox(QComboBox*, QComboBox*)));
    connect(OForm, SIGNAL(productComboBox(QComboBox*, QComboBox*)), PForm,
            SLOT(setProductComboBox(QComboBox*, QComboBox*)));

    /*고객 정보 최초 입력 시, 고객 정보를 채팅 서버 클래스에 보내주는 시그널을 슬롯에 연결*/
    connect(CForm, SIGNAL(clientLoad(QList<int>, QList<QString>)),
            ChattingForm, SLOT(addClient(QList<int>, QList<QString>)));

    /*고객, 제품 정보 클래스에서 정보가 추가됐다는 시그널을 주문 정보 클래스의 슬롯에 연결*/
    connect(CForm, SIGNAL(clientAdded(int)), OForm, SLOT(clientAdded()));
    connect(PForm, SIGNAL(productAdded(int)), OForm, SLOT(productAdded()));

    /*주문 정보 등록 시 고객, 제품 정보 클래스에 정보를 요청하는 시그널을 각 슬롯에 연결*/
    connect(OForm, SIGNAL(orderAddedClient(int)),
            CForm, SLOT(orderAddedClient(int)));
    connect(OForm, SIGNAL(orderAddedProduct(int)),
            PForm, SLOT(orderAddedProduct(int)));

    /*고객, 제품 정보 클래스에서 정보를 담아 주문 정보 클래스로 반환하는 시그널/슬롯 연결*/
    connect(CForm, SIGNAL(addReturn(QList<QString>)),
            OForm, SLOT(addReturnClient(QList<QString>)));
    connect(PForm, SIGNAL(addReturn(QList<QString>)),
            OForm, SLOT(addReturnProduct(QList<QString>)));

    /*주문 정보 검색 시 고객, 제품 정보 클래스에 정보를 요청하는 시그널을 각 슬롯에 연결*/
    connect(OForm, SIGNAL(orderSearchedClient(int)),
            CForm, SLOT(orderSearchedClient(int)));
    connect(OForm, SIGNAL(orderSearchedProduct(int)),
            PForm, SLOT(orderSearchedProduct(int)));

    /*고객, 제품 정보 클래스에서 정보를 담아 주문 정보 클래스로 반환하는 시그널/슬롯 연결*/
    connect(CForm, SIGNAL(searchReturn(QList<QString>)),
            OForm, SLOT(searchReturnClient(QList<QString>)));
    connect(PForm, SIGNAL(searchReturn(QList<QString>)),
            OForm, SLOT(searchReturnProduct(QList<QString>)));

    /*주문 정보 수정 시 고객, 제품 정보 클래스에 정보를 요청하는 시그널을 각 슬롯에 연결*/
    connect(OForm, SIGNAL(orderModifiedClient(int, int)),
            CForm, SLOT(orderModifiedClient(int, int)));
    connect(OForm, SIGNAL(orderModifiedProduct(int, int)),
            PForm, SLOT(orderModifiedProduct(int, int)));

    /*고객, 제품 정보 클래스에서 정보를 담아 주문 정보 클래스로 반환하는 시그널/슬롯 연결*/
    connect(CForm, SIGNAL(modifyReturn(QList<QString>, int)),
            OForm, SLOT(modifyReturnClient(QList<QString>, int)));
    connect(PForm, SIGNAL(modifyReturn(QList<QString>, int)),
            OForm, SLOT(modifyReturnProduct(QList<QString>, int)));

    /*고객, 제품 정보 클래스에서 정보가 삭제됐다는 시그널을 주문 정보 클래스의 슬롯에 연결*/
    connect(CForm, SIGNAL(clientRemoved(int)),
            OForm, SLOT(clientRemoved(int)));
    connect(PForm, SIGNAL(productRemoved(int)),
            OForm, SLOT(productRemoved(int)));

    /*고객, 제품 정보 클래스에서 정보가 수정됐다는 시그널을 주문 정보 클래스의 슬롯에 연결*/
    connect(CForm, SIGNAL(clientModified(int,QList<QString>)),
            OForm, SLOT(clientModified(int,QList<QString>)));
    connect(PForm, SIGNAL(productModified(int,QList<QString>)),
            OForm, SLOT(productModified(int,QList<QString>)));

    /*채팅 서버 클래스에 새로 등록된 고객 정보를 보내는 시그널을 슬롯에 연결*/
    connect(CForm, SIGNAL(sendServer(QList<int>, QList<QString>)),
            ChattingForm, SLOT(addClientInfo(QList<int>, QList<QString>)));

    /*채팅 서버 클래스에서 고객 목록을 다시 불러올 때 발생하는 시그널/슬롯 연결*/
    connect(ChattingForm, SIGNAL(reset()), CForm, SLOT(dataload()));

    /*고객 정보가 변경됐을 경우 채팅 서버 클래스에 변경된 정보를 보내주는 시그널/슬롯 연결*/
    connect(CForm, SIGNAL(sendServerCModified(int, QString)),
            ChattingForm, SLOT(modifyClientInfo(int, QString)));

    /*고객 정보가 삭제됐을 경우 채팅 서버 클래스에 삭제된 정보를 보내주는 시그널/슬롯 연결*/
    connect(CForm, SIGNAL(sendServerCRemoved(QString)),
            ChattingForm, SLOT(removeClientInfo(QString)));


    OForm->dataload();                                                      //주문 정보 클래스에 고객, 제품 정보 목록 입력
    CForm->dataload();                                                      //채팅 서버 클래스에 고객 목록 입력

    /*각 폼을 mdiarea에 서브 윈도우로 추가*/
    ui->mdiArea->addSubWindow(CForm);
    ui->mdiArea->addSubWindow(PForm);
    ui->mdiArea->addSubWindow(OForm);
    ui->mdiArea->addSubWindow(ChattingForm);
//    CForm->show();
//    PForm->show();
//    OForm->show();
//    ChattingForm->show();

    /*각 액션에 대해서 아이콘 추가*/
    ui->actiontr_ClientInformationForm->setIcon(QIcon(":/icon_image/client.png"));
    ui->actiontr_ProductInformationForm->setIcon(QIcon(":/icon_image/product.png"));
    ui->actiontr_OrderInformationForm->setIcon(QIcon(":/icon_image/order.png"));
    ui->actiontr_ChattingForm->setIcon(QIcon(":/icon_image/Server.png"));
    ui->actiontr_quit->setIcon(QIcon(":/icon_image/quit.png"));

    ui->toolBar->setIconSize(QSize(50, 50));                                //아이콘 크기 설정
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);           //아이콘 아래에 액션의 이름이 나오도록 설정
}

CS_App::~CS_App()                                                           //소멸자
{
    delete ui;                                                              //생성자에서 만든 포인터 객체 소멸
}


void CS_App::on_actiontr_ClientInformationForm_triggered()                  //고객 정보 폼 액션을 선택했을 경우
{
    CForm->setFocus();                                                      //고객 정보 폼으로 화면 전환
}

void CS_App::on_actiontr_ProductInformationForm_triggered()                 //제품 정보 폼 액션을 선택했을 경우
{
    PForm->setFocus();                                                      //제품 정보 폼으로 화면 전환
}

void CS_App::on_actiontr_OrderInformationForm_triggered()                   //주문 정보 폼 액션을 선택했을 경우
{
    OForm->setFocus();                                                      //주문 정보 폼으로 화면 전환
}

void CS_App::on_actiontr_ChattingForm_triggered()                           //채팅 서버 폼 액션을 선택했을 경우
{
    ChattingForm->setFocus();                                               //채팅 서버 폼으로 화면 전환
}
