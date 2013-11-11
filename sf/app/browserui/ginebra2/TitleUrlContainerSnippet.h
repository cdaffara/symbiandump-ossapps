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
#ifndef __GINEBRA_TITLEURLCONTAINERSNIPPET_H
#define __GINEBRA_TITLEURLCONTAINERSNIPPET_H

#include <QtGui>
#include "ChromeSnippet.h"
#include "NativeChromeItem.h"

namespace GVA {


class ChromeWidget;
class GUrlSearchItem;
class ActionButton;

/* \brief This is the container widget for title-url combo snippet
 *
 * 
 */
class TitleUrlContainerItem : public NativeChromeItem
{
    Q_OBJECT
    public:
        TitleUrlContainerItem(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
        virtual ~TitleUrlContainerItem();
        
        /// The URL of the web page.
        QString url() const;

    protected:
        virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
        virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

    private Q_SLOTS:
        void onChromeResize();
        void changeLayout(bool editMode);

    private:
        void setProperties();

    private:
        ChromeWidget * m_chrome;
        QGraphicsWidget * m_viewPort;
        ActionButton * m_backStepButton;
        GUrlSearchItem * m_urlTileWidget;
        QGraphicsPixmapItem * m_dividerImage;
        // painting support
        QPen m_pen;
        QLinearGradient m_grad;
};


/* \brief This is the container snippet for title-url combo
 *
 * 
 */
class TitleUrlContainerSnippet : public ChromeSnippet
{
    Q_OBJECT
    public:
        TitleUrlContainerSnippet(const QString & elementId, ChromeWidget * chrome,
                                 QGraphicsWidget * widget, const QWebElement & element);
        virtual ~TitleUrlContainerSnippet();

        static TitleUrlContainerSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
        
        /// The URL of the web page.
        QString url() const;
        Q_PROPERTY(QString url READ url)
};

} // end of namespace GVA

#endif // __GINEBRA_TITLEURLCONTAINERSNIPPET_H
