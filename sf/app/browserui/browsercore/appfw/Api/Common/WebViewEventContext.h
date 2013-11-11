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

#ifndef WEBVIEWCONTEXT_H_
#define WEBVIEWCONTEXT_H_

#include <QPoint>
#include <QObject>

#include "BWFGlobal.h"
#include "ViewEventContext.h"

class QWebHitTestResult;


    /// The WebViewEventContext class contains information about a context menu, 
    /// or long-press, event that has occurred in a WebView.
    class BWF_EXPORT WebViewEventContext : public ViewEventContext {
        Q_OBJECT
      public:
        WebViewEventContext();
        WebViewEventContext(const QString &viewType, const QWebHitTestResult &hitTest);
        WebViewEventContext(const WebViewEventContext &o);
        ~WebViewEventContext() {}

        /// Holds the link text if the event occured over a link.  
        QString getLinkText() const { return m_linkText; }
        Q_PROPERTY(QString linkText READ getLinkText)
        
        /// Holds the link URL if the event occured over a link.  
        QString getLinkUrl() const { return m_linkUrl; }
        Q_PROPERTY(QString linkUrl READ getLinkUrl)

        /// Holds the link title if the event occured over a link.                          
        QString getLinkTitle() const { return m_linkTitle; }
        Q_PROPERTY(QString linkTitle READ getLinkTitle)

        QString getLinkElementId() const { return m_linkElementId; }
        Q_PROPERTY(QString linkElementId READ getLinkElementId)

        QString getLinkFrameName() const { return m_linkFrameName; }
        Q_PROPERTY(QString linkFrameName READ getLinkFrameName)
                
        QString getElementId() const { return m_elementId; }
        Q_PROPERTY(QString elementId READ getElementId)
                
        QString getFrameName() const { return m_frameName; }
        Q_PROPERTY(QString frameName READ getFrameName)
                
        QString getImageUrl() const { return m_imageUrl; }
        Q_PROPERTY(QString imageUrl READ getImageUrl)

        bool getIsContentSelected () const { return m_isContentSelected; }
        Q_PROPERTY(bool isContentSelected READ getIsContentSelected)
        
      public:
        QString m_linkText;
        QString m_linkUrl;
        QString m_linkTitle;
        QString m_elementId;
        QString m_frameName;
        QString m_imageUrl;
        bool m_isContentSelected;
        QString m_linkElementId;
        QString m_linkFrameName;
    };

#endif /* WEBVIEWCONTEXT_H_ */
