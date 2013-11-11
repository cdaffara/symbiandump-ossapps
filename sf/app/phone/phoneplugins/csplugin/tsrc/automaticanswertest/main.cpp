
#include "automaticanswertest.h"

#include <QDebug>
#include <QtGui>
#include <QGraphicsWidget>
#include <hbapplication.h>
#include <hbmainwindow.h>

int main(int argc, char *argv[])
{
    qDebug () << "AUTOMATICANSWER__main()<<";
    HbApplication app (argc, argv);    
    HbMainWindow mainWindow;
    automaticanswertest* testview = new automaticanswertest ();
    mainWindow.addView(testview);
    mainWindow.setCurrentView(testview);
    mainWindow.show();
    return app.exec();
}
