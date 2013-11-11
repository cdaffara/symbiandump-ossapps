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

#ifndef __WRTBROWSERCONTAINER_P_H__
#define __WRTBROWSERCONTAINER_P_H__
#include "ZoomMetaData.h"

class QGraphicsWidget;
class QObject;
struct BrowserPageFactory;
struct ZoomMetaData;

namespace WRT {
    class SchemeHandler;
    class SecSession;
    class WrtBrowserContainer;
    class WrtBrowserFileChooser;
    class SecureUIController;
    class LoadController;
        
    class WrtBrowserContainerPrivate
    {
    public:
        WrtBrowserContainerPrivate(QObject* parent=0 , WrtBrowserContainer* page = NULL);
        ~WrtBrowserContainerPrivate();
        WrtBrowserContainer* m_page;
        SchemeHandler* m_schemeHandler; 
        BrowserPageFactory* m_pageFactory;
        QGraphicsWidget* m_widget;
        WRT::SecureUIController *m_secureController; //Owned
        WRT::LoadController * m_loadController; //Owned
        ZoomMetaData m_zoomData;
        WrtBrowserFileChooser * m_fileChooser; // Owned
        bool m_needUpdateThumbnail;
    };
}
#endif
