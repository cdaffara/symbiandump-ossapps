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


#include <QDebug>

#include "webpagecontroller.h"
#include "WindowView_p.h"
#include "WindowFlowView.h"
#include "FilmstripFlow.h"

#include <QWebHistory>
#include <QWebFrame>
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"

namespace WRT {

ControllableView* WindowFlowView::createNew(QWidget* parent) {

    return new WindowFlowView( WebPageController::getSingleton(), parent, Type()); 

}

ControllableViewBase* WindowFlowView::createNew(QGraphicsWidget* parent) {

    return new WindowFlowView( WebPageController::getSingleton(), parent, Type());

}


/*!
 * \class WindowFlowView
 *
 * \brief presents the history of the current page in the "Flow" style
 *
 * This class implements the WindowView base class
 */

/*!
  Basic WindowFlowView constructor requires a PageManager to manage the pages
  and a parent QWidget

  @param mgr : WebPageController handle for this class
  @param parent : Widget parent for this class
*/
WindowFlowView::WindowFlowView(WebPageController* mgr, QWidget* parent,const QString& aType) :
    WindowView(mgr,parent)
,   m_type(aType)
{
    m_jsObject = new WindowViewJSObject(this, 0, type());
}

/*!
  Basic WindowFlowView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
  Note: This functionality is not yet tested
  @param mgr : WebPageController handle for this class
  @param parent : Graphics Widget parent for this class
  @see  WebPageController
*/
WindowFlowView::WindowFlowView(WebPageController* mgr, QGraphicsWidget* parent,const QString& aType) :
    WindowView(mgr,parent)
,   m_type(aType)
{
    m_jsObject = new WindowViewJSObject(this, 0, type());
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void WindowFlowView::activate()
{
    if (!d->m_flowInterface ) {
        d->m_flowInterface = new GraphicsFilmstripFlow(d->m_widgetParent);
        emit instantiated(this);
    }
    WindowView::activate();
}


} // namespace WRT
