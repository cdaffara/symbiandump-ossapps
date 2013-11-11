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

#ifndef __WINDOWVIEW_P_H__
#define __WINDOWVIEW_P_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include <QTimer>

class QTimer;
class QWebHistory;
class WebPageController;

namespace WRT {
    class WrtBrowserContainer;
    class GraphicsFlowInterface;

    class WindowViewPrivate
    {
    public: // public actions available for this view
        QAction * m_actionForward;
        QAction * m_actionBack;
        QAction * m_actionGoBack;
        QAction * m_actionOK;
        QAction * m_actionCancel;
        QAction * m_actionAddWindow;
        QAction * m_actionDelWindow;

    public:
        WindowViewPrivate(WebPageController * mgr, QWidget* parent);
        WindowViewPrivate(WebPageController * mgr, QGraphicsWidget* parent);
        ~WindowViewPrivate();

        GraphicsFlowInterface * m_flowInterface;
        QWidget * m_widgetParent;
        QGraphicsWidget * m_graphicsWidgetParent;

        WebPageController * m_pageManager; // not owned
        QTimer* m_transTimer;

        QList<WrtBrowserContainer*>* m_pageList;
        QList<WrtBrowserContainer*> m_newPages;

        QSize m_windowViewSize;

        int m_mode;
        int m_state;

    private:
        void init();
    };
}
#endif // __WINDOWVIEW_P_H__
