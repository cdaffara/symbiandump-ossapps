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



#ifndef HISTORYVIEWCONTEXT_H_
#define HISTORYVIEWCONTEXT_H_

#include <QPoint>
#include <QObject>

#include "BWFGlobal.h"
#include "ViewEventContext.h"


namespace WRT
{

    /// The HistoryViewEventContext class contains information about a context menu, 
    /// or long-press, event that has occurred in a HistoryView.
    class BWF_EXPORT HistoryViewEventContext : public ViewEventContext {
        Q_OBJECT
      public:
        HistoryViewEventContext();
        HistoryViewEventContext(const QString &viewType, 
                                const QPoint pos, 
                                int index, 
                                const QString &title);
        
        HistoryViewEventContext(const HistoryViewEventContext &o);
        ~HistoryViewEventContext() {}

        QString getItemTitle() const { return m_itemTitle; }
        Q_PROPERTY(QString itemTitle READ getItemTitle)

        int getItemIndex() const { return m_itemIndex; }
        Q_PROPERTY(int itemIndex READ getItemIndex)
        
        // May also need a flag to indicate whether the event occured over the center item,
        // over no item, or over a side item.
        
      public:
        QString m_itemTitle;
        int m_itemIndex;
    };

}

#endif /* HISTORYVIEWCONTEXT_H_ */
