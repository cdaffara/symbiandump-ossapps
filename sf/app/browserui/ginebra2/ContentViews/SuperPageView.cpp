/*
 * SuperPageView.cpp
 *
 *  Created on: Jun 11, 2010
 *      Author: lewontin
 */

#include "SuperPageView.h"
#include "GWebContentView.h"

namespace GVA {

SuperPageView::SuperPageView(GWebContentView * contentView, QObject * parent, const QString &objectName)
: ControllableViewBase(parent),
  m_contentView(contentView)
{
    setObjectName(objectName);  
}

SuperPageView::~SuperPageView(){
    
}

QGraphicsWidget* SuperPageView::widget() const 
{
    //SuperPageView's widget is the content view widget
    return m_contentView->widget();
}

void SuperPageView::show(){
    //Ask the content view to show the super page
    m_contentView->showSuperPage(objectName());
    ControllableViewBase::activate();
}

QList<QAction*> SuperPageView::getContext() {
    return m_actions.values();
}

//Creates an action, visible to JavaScript as name. If the script parameter is supplied, the
//the action will invoke the supplied script in the context of the super page main frame 
//(not in the context of the caller).

void SuperPageView::addAction(const QString & name, const QString & script){
    QAction* action;    
    action = m_actions[name] =  new QAction(name, this);
    action->setObjectName(name);
    if(!script.isNull()){
        action->setData(script);
        connect(action, SIGNAL(triggered()), this, SLOT(invokeScriptAction()));
    }
}

void SuperPageView::invokeScriptAction() {
    QAction * action  = static_cast<QAction*>(sender());
    QString script = action->data().toString();
    m_contentView->webWidget()->page()->mainFrame()->evaluateJavaScript(script);
}

}
