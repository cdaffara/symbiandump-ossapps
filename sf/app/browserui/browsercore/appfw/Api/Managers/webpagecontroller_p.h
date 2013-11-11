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

#ifndef __WEBPAGECONTROLLER_P_H__
#define __WEBPAGECONTROLLER_P_H__

#include <QWidget>
#include <QAction>
#include "BWFGlobal.h"

#define WEBPAGE_ZOOM_RANGE_MIN 25
#define WEBPAGE_ZOOM_RANGE_MAX 300
#define WEBPAGE_ZOOM_PAGE_STEP 20

namespace WRT {
    class WrtBrowserContainer;
    class SecContext;
}

class WebPageController;

class WebPageControllerPrivate
{
public: // public actions available for this view
    QAction * m_actionReload;
    QAction * m_actionStop;
    QAction * m_actionBack;
    QAction * m_actionForward;

public:
    WebPageControllerPrivate(WebPageController* qq);
    ~WebPageControllerPrivate();

    WebPageController * const q;

    QObject* m_widgetParent;
    QList<WRT::WrtBrowserContainer*> m_allPages;
    int m_currentPage;
    WRT::SecContext * m_secContext;
    QString m_historyDir;
    QObject* m_actionsParent;    
    bool donotsaveFlag;
};
#endif // __WEBPAGECONTROLLER_P_H__
