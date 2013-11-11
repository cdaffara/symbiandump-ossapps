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
 * This class extends ToolbarSnippet class to hold the
 * windows view toolbar buttons
 *
 */

#include "WindowToolbarSnippet.h"
#include "ToolbarChromeItem.h"
#include "ViewStack.h"
#include <QDebug>

namespace GVA {

    WindowToolbarSnippet::WindowToolbarSnippet(const QString& elementId, ChromeWidget * chrome,
                                               const QWebElement & element)
        : DualButtonToolbarSnippet(elementId, chrome, element)
    {
        m_type = TOOLBAR_WINDOWS_VIEW;
        connect(m_chrome, SIGNAL(aspectChanged(int)) , this, SLOT(onAspectChanged()));
    }

    WindowToolbarSnippet::~WindowToolbarSnippet()
    {
    }
    
    void WindowToolbarSnippet::onAspectChanged( ) {
        	 
        updateOwnerArea();      
    }

    WindowToolbarSnippet * WindowToolbarSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
    {
        WindowToolbarSnippet * that = new WindowToolbarSnippet(elementId, chrome, element);
        that->setChromeWidget(new ToolbarChromeItem(that));
        return that;
    }
    
    void WindowToolbarSnippet::addChild(ChromeSnippet * child) {

        WebChromeContainerSnippet * s =  dynamic_cast<WebChromeContainerSnippet* >(child);
        if (!s) {
            ToolbarActions_t* t = new ToolbarActions_t();
            if (child->elementId() == "WinBackButton" ) {
                t->actionId = WINDOW_VIEW_ACTION_BACK;
                t->actionName = WINDOW_TOOLBAR_BACK;
                t->normalImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back.png";
                t->disabledImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back_disabled.png";
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back_pressed.png";

            }
            else if (child->elementId() == "WinAddWindow" ) {
                t->actionId = WINDOW_VIEW_ACTION_ADD;
                t->actionName = WINDOW_TOOLBAR_ADD;
                t->normalImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_add.png";
                t->disabledImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_add_disabled.png";
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_add_pressed.png";
            }
            t->id = child->elementId();
            m_actionInfo.append(t);
        }

        WebChromeContainerSnippet::addChild(child);
    }


    void WindowToolbarSnippet::setAction(ChromeSnippet * s) {

        //qDebug() << "setAction: " << s->elementId();
        ToolbarSnippet::setAction(s);

        ActionButtonSnippet * button  = static_cast<ActionButtonSnippet*> (s);
        int index = getIndex(s);

        if (index != -1 ) {
            ToolbarActions_t * t = m_actionInfo.at(index);
            if (t->actionId == WINDOW_VIEW_ACTION_BACK ) {

                QAction * action =  button->getDefaultAction();
                connect(action, SIGNAL(triggered()), this, SLOT(handleBackButton()));
            }
        }
    }

    void WindowToolbarSnippet::handleBackButton() {

        ViewStack::getSingleton()->switchView( TOOLBAR_WEB_VIEW, TOOLBAR_WINDOWS_VIEW);

    }

} // end of namespace GVA


