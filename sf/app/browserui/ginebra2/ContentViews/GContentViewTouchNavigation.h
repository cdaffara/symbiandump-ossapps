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

#ifndef GWEBCONTENTVIEWTOUCHNAVIGATION_H
#define GWEBCONTENTVIEWTOUCHNAVIGATION_H

#include <QObject>
#include "GWebTouchNavigation.h"
#include "GWebContentViewWidget.h"
#include "ChromeLayout.h"
#include "ChromeWidget.h"

namespace GVA
{
    /*!
     * This class provides the virtual function overrides needed to make GWebTouchNavigation
     * work on a GWebContentViewWidget.
     */
    class GContentViewTouchNavigation : public GWebTouchNavigation
    {
        Q_OBJECT
    public:
        GContentViewTouchNavigation(QWebPage* webPage, GWebContentViewWidget* view);
        void setChromeWidget(ChromeWidget* chrome) { m_chrome = chrome;}

        Q_PROPERTY(qreal doubleClickEnabled READ getDoubleClickEnabled WRITE setDoubleClickEnabled)
        qreal getDoubleClickEnabled() const { return m_doubleClickEnabled; }
        void setDoubleClickEnabled(qreal value)  { m_doubleClickEnabled = value; }
    
    protected:
        GWebContentViewWidget *contentViewWidget() { return static_cast<GWebContentViewWidget*>(m_view); }

        virtual qreal slideView(qreal delta) { return m_chrome->layout()->slideView(delta); }
        virtual qreal shrinkView(qreal delta) { return m_chrome->layout()->shrinkView(delta); }
        virtual qreal viewInitialScale() { return contentViewWidget()->initialScale(); }
        virtual void setViewBlockElement(const QWebElement &el) { contentViewWidget()->setBlockElement(el); }
        virtual void setViewZoomFactor(qreal zoom) { return contentViewWidget()->setZoomFactor(zoom); }

    protected:
        ChromeWidget* m_chrome;
    
    };

}

#endif
