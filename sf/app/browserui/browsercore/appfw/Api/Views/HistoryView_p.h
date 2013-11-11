/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __HISTORYVIEW_P_H__
#define __HISTORYVIEW_P_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>

class QWebHistory;
class WebPageController;

namespace WRT {
    class WrtBrowserContainer;
    class GraphicsFlowInterface;

    class HistoryViewPrivate
    {
    public: // public actions available for this view
        QAction * m_actionForward;
        QAction * m_actionBack;
        QAction * m_actionOK;
        QAction * m_actionCancel;

    public:
        HistoryViewPrivate(WebPageController * mgr, QWidget* parent);
        HistoryViewPrivate(WebPageController * mgr, QGraphicsWidget* parent);
        ~HistoryViewPrivate();

        GraphicsFlowInterface * m_flowInterface;
        QWidget * m_widgetParent;
        QGraphicsWidget * m_graphicsWidgetParent;

        WebPageController * m_pageManager; // not owned
        WrtBrowserContainer * m_activePage; // not owned
        int m_historyIndex; 

        bool m_isActive;
    private:
        void init();
    };
}
#endif // __HISTORYVIEW_P_H__
