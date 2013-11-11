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

#include "SettingsToolbarSnippet.h"
#include "ToolbarChromeItem.h"
#include "ViewStack.h"
#include "GWebContentView.h"
#include "webpagecontroller.h"
#include <QDebug>

namespace GVA {

    SettingsToolbarSnippet::SettingsToolbarSnippet(const QString& elementId, ChromeWidget * chrome,
                                                   const QWebElement & element)
        : DualButtonToolbarSnippet(elementId, chrome, element),
          m_action1(0), m_action2(0)
    {      
        connect(m_chrome, SIGNAL(aspectChanged(int)) , this, SLOT(onAspectChanged()));
    }

    SettingsToolbarSnippet::~SettingsToolbarSnippet()
    {
        if(m_action1)
            delete m_action1;
        if(m_action2)
            delete m_action2;
    }
    void SettingsToolbarSnippet::onAspectChanged( ) {
       	
        updateOwnerArea();
          
    }
    SettingsToolbarSnippet * SettingsToolbarSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
    {
        SettingsToolbarSnippet * that = new SettingsToolbarSnippet( elementId, chrome, element );
        that->setChromeWidget( new ToolbarChromeItem( that ) );
        return that;
    }
    
    void SettingsToolbarSnippet::addChild(ChromeSnippet * child) {
        WebChromeContainerSnippet * s =  dynamic_cast<WebChromeContainerSnippet* >(child);
        if (!s) {
            ToolbarActions_t* t = new ToolbarActions_t();
            if (child->elementId() == "SettingsBackButton" ) {
                t->actionId = SETTINGS_VIEW_ACTION_BACK;
                t->actionName = SETTINGS_TOOLBAR_BACK;
                t->normalImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back.png";
                t->disabledImg = "";
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back_pressed.png";
            }
            else if (child->elementId() == "SettingsFeedbackButton" ) {
                t->actionId = SETTINGS_VIEW_ACTION_FEEDBACK;
                t->actionName = SETTINGS_TOOLBAR_FEEDBACK;
                t->normalImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_feedback.png";
                t->disabledImg = "";
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_feedback_pressed.png";
            }
            t->id = child->elementId();
            m_actionInfo.append(t);
        }

        WebChromeContainerSnippet::addChild(child);
    }

    void SettingsToolbarSnippet::setAction(ChromeSnippet * s) {
        ToolbarSnippet::setAction(s);

        ActionButtonSnippet * button  = static_cast<ActionButtonSnippet*> (s);
        int index = getIndex(s);

        if (index != -1 ) {
            ToolbarActions_t * t = m_actionInfo.at(index);
            QAction * action =  button->getDefaultAction();
            if (t->actionId == SETTINGS_VIEW_ACTION_BACK ) {
                if( !action ) {
                    action = new QAction(0);
                    button->setDefaultAction(action);
                    m_action1 = action;
                }
                connect(action, SIGNAL(triggered()), this, SLOT(handleBackButton()));
            }
            else if (t->actionId == SETTINGS_VIEW_ACTION_FEEDBACK) {
                if( !action ) {
                    action = new QAction(0);
                    button->setDefaultAction(action);
                    m_action2 = action;
                }
                connect(action, SIGNAL(triggered()), this, SLOT(handleFeedbackButton()));
            }
        }
    }

    void SettingsToolbarSnippet::handleBackButton() {
        GWebContentView* webView = static_cast<GWebContentView*> (chrome()->getView("WebView"));
        if(webView) {
            // Reloading here because otherwise the settings won't collapse when leaving and
            // re-entering the view. This fixes BR-3525. If there's a better way to do this in
            // the future this can be removed.
            webView->reload();
            webView->showNormalPage();
            webView->setGesturesEnabled(true);
        }

        ViewStack::getSingleton()->switchView( TOOLBAR_WEB_VIEW, TOOLBAR_SETTINGS_VIEW );
    }

    void SettingsToolbarSnippet::handleFeedbackButton() {
        QString mailTo = "oss-browser.errors@nokia.com"; 
        QString feedbackBody = "Enter your feedback here:"; 
        WebPageController::getSingleton()->feedbackMail(mailTo, feedbackBody);
    }


} // end of namespace GVA


