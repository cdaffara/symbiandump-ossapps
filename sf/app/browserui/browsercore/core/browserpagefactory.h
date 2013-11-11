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

#ifndef __BROWSERPAGEFACTORY_H__
#define __BROWSERPAGEFACTORY_H__

#include "BWFGlobal.h"

class QWebPage;

namespace WRT {
    class WrtBrowserContainer;
}
using namespace WRT;
  
struct BWF_EXPORT BrowserPageFactory 
{
    virtual WRT::WrtBrowserContainer* openPage() = 0;

    static WRT::WrtBrowserContainer* openBrowserPage();
    bool m_bRestoreSession;
};

#endif // __BROWSERPAGEFACTORY_H__

