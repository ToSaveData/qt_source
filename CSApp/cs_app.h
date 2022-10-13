#ifndef CS_APP_H
#define CS_APP_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CS_App; }
QT_END_NAMESPACE

class ClientHandlerForm;
class ProductHandlerForm;
class OrderHandlerForm;
class CS_App : public QMainWindow
{
    Q_OBJECT

public:
    CS_App(QWidget *parent = nullptr);
    ~CS_App();

private slots:

    void on_actiontr_ClientInformationForm_triggered();

    void on_actiontr_ProductInformationForm_triggered();

    void on_actiontr_OrderInformationForm_triggered();

private:
    Ui::CS_App *ui;
    ClientHandlerForm *CForm;
    ProductHandlerForm *PForm;
    OrderHandlerForm *OForm;
};
#endif // CS_APP_H
