
#include <hbaction.h>
#include <hbmenu.h>
#include <hbpushbutton.h>
#include <qgraphicslinearlayout.h>
#include <QDebug>
#include "etelincomingcallmonitor.h"
#include "automaticanswertest.h"


automaticanswertest::automaticanswertest(QGraphicsItem *parent)
    : HbView(parent), m_incomingVoiceCallMonitor(NULL)
{
    qDebug () << "automaticanswertest::automaticanswertest<<";
    setTitle(tr("automaticanswer"));

    // Add menu item for automatic answer
    /*   connect(
           menu()->addAction("start automatic answer"), SIGNAL(triggered()), 
           this, SLOT(answer()));*/

    createContent();
    qDebug () << "automaticanswertest::automaticanswertest>>";
}

automaticanswertest::~automaticanswertest()
{
    delete m_incomingVoiceCallMonitor;
}

void automaticanswertest::createContent()
{
    qDebug () << "automaticanswertest::createContent<<";
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    
    answering = new HbPushButton(tr("start autom. answer"), this); 
    connect(answering, SIGNAL(clicked()), this, SLOT(answer()));
    mainLayout->addItem(answering);
    stopanswering = new HbPushButton(tr("stop autom. answer"), this); 
    connect(stopanswering, SIGNAL(clicked()), this, SLOT(stopanswer()));
    mainLayout->addItem(stopanswering);
    stopanswering->setEnabled(false); 
    setLayout(mainLayout);
}

void automaticanswertest::answer()
    {
    qDebug () << "automaticanswertest::answer()<<";
    if (m_incomingVoiceCallMonitor == NULL)
        {
        // Create and start incoming voice call monitor for primary line
        m_incomingVoiceCallMonitor = new CEtelIncomingCallMonitor();
        m_incomingVoiceCallMonitor->StartMonitoring();
        qDebug () << "automaticanswertest::answer()__etelmonitor__started";
        }
    else
        {
        m_incomingVoiceCallMonitor->StartMonitoring();
        }
    stopanswering->setEnabled(true);
    answering->setEnabled(false);
    }
void automaticanswertest::stopanswer()
    {
    if (m_incomingVoiceCallMonitor)
        {
        m_incomingVoiceCallMonitor->Cancel();
        }
    stopanswering->setEnabled(false);
    answering->setEnabled(true);    
    }

