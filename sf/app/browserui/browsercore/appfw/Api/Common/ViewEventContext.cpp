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

#include "ViewEventContext.h"


#include "ViewEventContext.h"
#include <QWebHitTestResult>
#include <QWebElement>

static const QString s_ObjectName = "viewEventContext";

ViewEventContext::ViewEventContext(const QString &viewType, const QPoint pos)
  : m_viewType(viewType), 
    m_pos(pos)
{
    setObjectName(s_ObjectName);
    m_pos.setParent(this);
}

ViewEventContext::ViewEventContext(const ViewEventContext &o)
  : m_viewType(o.getViewType()), 
    m_pos(o.getPosition())  
{
    setObjectName(o.objectName());
    m_pos.setParent(this);
}


