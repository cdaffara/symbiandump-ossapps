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

#include  <QWebFrame>
#include "scriptobjects.h"
#include "actionjsobject.h"
#include "controllableview.h"
#include "controllableviewjsobject.h"
#include <QGraphicsWidget>


ControllableViewJSObject::ControllableViewJSObject(ControllableView *contentView, QWebFrame *chromeFrame, const QString &objectName)
  : m_contentView(contentView),
    m_actionsParent(0)
{
    setObjectName(objectName);
    setChromeFrame(chromeFrame);
    updateActions();
}

ControllableViewJSObject::~ControllableViewJSObject() {
    if(m_actionsParent) {
        // Also deletes the actions since they are children of m_actionsParent.
        delete m_actionsParent;
    }
}


void ControllableViewJSObject::updateActions() {   // slot
    if(m_actionsParent) {
        delete m_actionsParent;
        m_actionsParent = 0;
    }
    if(m_contentView && !m_contentView->getContext().isEmpty()) {
        m_actionsParent = new QObject(this);
        m_actionsParent->setObjectName("actions");
        foreach(QAction *action, m_contentView->getContext()) {
            ActionJSObject *jso = new ActionJSObject(m_actionsParent, action);
        }
    }
}

void ControllableViewJSObject::setChromeFrame(QWebFrame *frame) {
    m_chromeFrame = frame;
    if(m_chromeFrame)
        m_chromeFrame->addToJavaScriptWindowObject(objectName(), this);
}

QObject *ControllableViewJSObject::getGeometry() const {
    ScriptRect *r = new ScriptRect(m_contentView->widget()->geometry().toRect());
    if(m_chromeFrame)
        m_chromeFrame->addToJavaScriptWindowObject(objectName() + "_rect", r, QScriptEngine::ScriptOwnership);
    return r;
}

void ControllableViewJSObject::setGeometry(int x, int y, int w, int h) {
    m_contentView->widget()->setGeometry(x, y, w, h);
}

QObject *ControllableViewJSObject::getPosition() const {
    ScriptPoint *p = new ScriptPoint(m_contentView->widget()->pos().toPoint());
    if(m_chromeFrame)
        m_chromeFrame->addToJavaScriptWindowObject(objectName() + "_point", p);
    return p;
}

void ControllableViewJSObject::setPosition(int x, int y) {
    m_contentView->widget()->setPos(x, y);
}

QString ControllableViewJSObject::getType() const { 
    return m_contentView ? m_contentView->type() : QString::null;
}

void ControllableViewJSObject::activate() {
    m_contentView->activate();
}

void ControllableViewJSObject::deactivate() {
    m_contentView->deactivate();
}

void ControllableViewJSObject::show() { 
    m_contentView->show(); 
}

void ControllableViewJSObject::hide() { 
    m_contentView->hide(); 
}

QString ControllableViewJSObject::getTitle() const {
    return m_contentView ? m_contentView->title() : QString::null;
}


