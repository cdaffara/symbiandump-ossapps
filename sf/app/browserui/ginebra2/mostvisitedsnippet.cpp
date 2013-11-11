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
#include "mostvisitedsnippet.h"
#include "mostvisitedpageview.h"

namespace GVA {

MostVisitedSnippet::MostVisitedSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element) :
        ChromeSnippet(elementId, chrome, widget, element)
{
    m_chrome = chrome;
}

MostVisitedSnippet * MostVisitedSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
{
    MostVisitedSnippet* that = new MostVisitedSnippet(elementId, chrome, 0, element);
    that->setChromeWidget( new MostVisitedPagesWidget(that, chrome) );
    that->widget()->hide(); //TODO: Shouldn't be needed?
    return that;
}

void MostVisitedSnippet::toggleVisibility(bool animate)
{
    MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());
    if (!mostVisitedPagesWidget)
        return;

    if (!isVisible()) {
         mostVisitedPagesWidget->open();
         updateMVGeometry();
         mostVisitedPagesWidget->show();
         ChromeSnippet::toggleVisibility(animate);

    } else {
        mostVisitedPagesWidget->close();
    }
}


void MostVisitedSnippet::setChromeWidget(QGraphicsWidget * widget)
{
    connect(m_chrome , SIGNAL(aspectChanged(int)), this, SLOT(displayModeChanged(int)));
    ChromeSnippet::setChromeWidget(widget);
    MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget);
    connect(mostVisitedPagesWidget, SIGNAL(closeComplete()), this, SIGNAL(mostVisitedSnippetCloseComplete()));
}

void MostVisitedSnippet::updateMVGeometry()
{
    MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());
    qreal toolBarHeight;

    ChromeSnippet* visibleSnippet= m_chrome->getSnippet("WebViewToolbarId");
    if (visibleSnippet)
        toolBarHeight = visibleSnippet->widget()->rect().height();

    //Resizing the widget to exclude the toolbar area so that it can handle events on toolbar as external event
    mostVisitedPagesWidget->resize(QSize(m_chrome->layout()->size().width(),(m_chrome->layout()->size().height()- toolBarHeight - KSpaceToolbarMVP)));
    mostVisitedPagesWidget->updatePos(QPointF(0, 0), toolBarHeight);
}

void MostVisitedSnippet::displayModeChanged(int newMode)
{
    if (isVisible()) {
        MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());

        if (!mostVisitedPagesWidget)
            return;

        updateMVGeometry();

        QString  displayMode = (newMode == landscape) ? "Landscape" : "Portrait";
        mostVisitedPagesWidget->displayModeChanged(displayMode);
    }
}

void MostVisitedSnippet::hide(bool animate)
 {
     MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());

     if (!mostVisitedPagesWidget)
       return;

     ChromeSnippet::hide(animate);
     mostVisitedPagesWidget->closeAnimationCompleted();
 }

void MostVisitedSnippet::close()
{
     MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());

     if (mostVisitedPagesWidget) {
       mostVisitedPagesWidget->close();
     }
}

MostVisitedSnippet::~MostVisitedSnippet()
{
    disconnect(m_chrome , SIGNAL(aspectChanged(int)), this, SLOT(displayModeChanged(int)));
}


}
