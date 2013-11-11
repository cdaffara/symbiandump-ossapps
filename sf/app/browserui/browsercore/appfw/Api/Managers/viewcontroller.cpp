/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <QDebug>

#include "viewcontroller.h"
#include "controllableview.h"
#include "controllableviewjsobject.h"

ControllableView *ViewController::createView(const QString &name, QWidget *parent) {
    qDebug() << "ViewController::createView: " << name;
    ControllableView *result = createObject(name, parent ? parent : m_viewParentWidget);
    if(result) {
        result->setObjectName(name);
        addView(result);
        return result;
    }
    else {
        qWarning() << "ViewController::createView: " << "unknown ControllableView: " << name;
    }
    return 0;
}

ViewController::~ViewController() {
    while(!m_views.empty()){
        ControllableView *tmpView = m_views.takeFirst();
        //qDebug() << "ViewController::~ViewController: " << tmpView << " " << tmpView->type();
        delete tmpView;
    }
}

void ViewController::addView(ControllableView *view,bool destroyOld) {
    ControllableView *tmpView = getView(view->type());
    if(tmpView == view) return;
    ControllableView * cView = currentView();
    if(tmpView) removeView(tmpView,destroyOld);
    m_views.append(view);

    m_currentViewIndex = -1; // reset index because it can be not valid
    if(cView) setCurrent(cView);
    // Check if the old current view was replaced by a new one and set new to current
    if(cView && !currentView()) setCurrent(view);

    if(view->jsObject())
        view->jsObject()->setParent(this);  // for javascript access.
}

void ViewController::removeView(ControllableView *view,bool destroyView) {
    int index = m_views.indexOf(view);
    if(index != -1) {
        if(m_currentViewIndex == index) m_currentViewIndex = -1;
        m_views.removeAt(index);
        if(destroyView) delete view;
    }
    else {
        qWarning() << "ViewController::removeView: view not found. " << view->objectName();
    }
}

/**
 \brief Set the current view to <var>view</var>.

 Deactivates and hides the current view, and then activates and shows the new view.
 */
void ViewController::setCurrent(ControllableView *view) {
    // TO DO: Add transition animations.
    
    int index = m_views.indexOf(view);
    if(index != -1) {
        if(index != m_currentViewIndex || !currentView()->isActive()) {
            emit currentViewChanging();
            
            if(currentView() && currentView()->isActive()) {
                hideCurrentView();
                currentView()->deactivate();
            }

            m_currentViewIndex = index;

            if(!currentView()->isActive()) currentView()->activate();
            showCurrentView();

            emit currentViewChanged();
        }
    }
    else {
        qWarning() << "ViewController::setCurrent: view not found. " << view->objectName();
    }
}

ControllableView *ViewController::currentView() const {
    return m_currentViewIndex >=0 ? m_views.at(m_currentViewIndex) : 0;
}

void ViewController::showCurrentView() {
    ControllableView *view = currentView();
    if(view) {
        qDebug() << "ViewController::showCurrentView: " + view->objectName();
        if(m_scene)
             view->initGraphics(m_scene, -2);
        view->show();
    }
}

void ViewController::hideCurrentView() {
    ControllableView *view = currentView();
    if(view) {
        qDebug() << "ViewController::hideCurrentView: " + view->objectName();
        view->hide();
    }
}

void ViewController::setChromeFrame(QWebFrame *frame) {
    foreach(ControllableView *view, m_views)
        view->setChromeFrame(frame);
}

ControllableView *ViewController::getView(const QString &type) const
{
    foreach(ControllableView *view, m_views)
        if(type.compare(view->type(), Qt::CaseInsensitive) == 0)
            return view;

    return 0;
}

void ViewController::showContent(const QString &type) {
    qDebug() << "ViewController::showContent" << type;
    // Only one view of each type for now...
    ControllableView *view = getView(type);
    if(view) {
            setCurrent(view);
            return;
        }

    // No existing view found, create a new one.

    ControllableView *newView = createView(type,m_viewParentWidget);
    if(newView) {
        setCurrent(newView);
    }
    else {
        qDebug() << "ViewController::showContent: unknown view type: " << type;
    }
}

void ViewController::freezeView() {
    ControllableView *view = currentView();
    if(view) {
        view->freeze();
    }
}

void ViewController::unfreezeView() {
    ControllableView *view = currentView();
    if(view) {
        view->unfreeze(false);
    }
}

QObject *ViewController::getCurrentView() const { 
    if(currentView())
        return static_cast<QObject *>(currentView()->jsObject()); 
    else
        return 0;
}

