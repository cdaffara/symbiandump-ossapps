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
#include "actionjsobject.h"

#include <QDebug>

ActionJSObject::ActionJSObject(QObject *parent, QAction *action) 
    : QObject(parent),
      m_action(action)
{
    connect(m_action, SIGNAL(triggered(bool)), this, SIGNAL(triggered(bool)));
    connect(m_action, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    connect(m_action, SIGNAL(changed()), this, SIGNAL(changed()));
    
    // Copy object name of action.
    setObjectName(action->objectName());
}

void ActionJSObject::initClass() {   // static
    qMetaTypeId<QObjectList>();
    qRegisterMetaType<QObjectList>("QObjectList");

    qMetaTypeId<ActionJSObject*>();
    qRegisterMetaType<ActionJSObject>("ActionJSObject*");
}

void ActionJSObject::trigger() { 
    if(m_action) {
        qDebug() << "ActionJSObject::trigger: " << m_action->text(); 
        m_action->trigger(); 
    }
}
