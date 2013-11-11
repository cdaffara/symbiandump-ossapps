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

#include "HistoryViewEventContext.h"
#include <QWebHitTestResult>
#include <QWebElement>

namespace WRT
{

static const QString s_ObjectName = "historyViewEventContext";

HistoryViewEventContext::HistoryViewEventContext()
{
    setObjectName(s_ObjectName);
}

HistoryViewEventContext::HistoryViewEventContext(const QString &viewType, 
                                                 const QPoint pos, 
                                                 int index, 
                                                 const QString &title) 
  : ViewEventContext(viewType, pos),
    m_itemTitle(title),
    m_itemIndex(index) 
{
    setObjectName(s_ObjectName);
}

HistoryViewEventContext::HistoryViewEventContext(const HistoryViewEventContext &o)
  : ViewEventContext(o.getViewType(), o.getPosition()),
    m_itemTitle(o.getItemTitle()),
    m_itemIndex(o.getItemIndex())
{
    setObjectName(o.objectName());
}


}
