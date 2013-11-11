#include <QtGui>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <telinformationpskeys.h>
#include "cphonepubsubproxy.h"

class CarModeTester : public HbWidget
{
    Q_OBJECT

public:
    CarModeTester();

private slots:
    void setCarModeOn();
    void setCarModeOff();
};

int main(int argc, char *argv[])
{
    // Initialization
    HbApplication app(argc, argv);

    // Main window widget. 
    // Includes decorators such as signal strength and battery life indicator.
    HbMainWindow mainWindow;

    CarModeTester* cmt = new CarModeTester;
    mainWindow.addView(cmt);

    // Show widget
    mainWindow.show();

    // Enter event loop
    return app.exec();
}

CarModeTester::CarModeTester() {
    HbPushButton* button = new HbPushButton("Set Car Mode ON", this);
    button->setMaximumHeight(50);
    connect(button,SIGNAL(clicked()),SLOT(setCarModeOn()));

    HbPushButton* button2 = new HbPushButton("Set Car Mode OFF", this);
    button2->setMaximumHeight(50);
    connect(button2,SIGNAL(clicked()),SLOT(setCarModeOff()));

    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->addItem(button);
    layout->addItem(button2);
    setLayout(layout);
}

void CarModeTester::setCarModeOn()
{
    CPhonePubSubProxy::Instance()->ChangePropertyValue(KPSUidTelCarMode, KTelCarMode, EPSCarModeOn);
}

void CarModeTester::setCarModeOff()
{
    CPhonePubSubProxy::Instance()->ChangePropertyValue(KPSUidTelCarMode, KTelCarMode, EPSCarModeOff);
}

#include "main.moc"
