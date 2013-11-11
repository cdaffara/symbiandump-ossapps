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
#include "HistoryView_p.h"
#include "HistoryFlowView.h"
#include "PictureFlow.h"


#include <QWebHistory>
#include <QWebFrame>
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"

namespace WRT {

ControllableView* HistoryFlowView::createNew(QWidget *parent)
{
    return new HistoryFlowView(WebPageController::getSingleton(),parent, Type());
}

ControllableViewBase* HistoryFlowView::createNew(QGraphicsWidget *parent)
{
    return new HistoryFlowView(WebPageController::getSingleton(),parent, Type());
}

/*!
 * \class HistoryFlowView
 *
 * \brief presents the history of the current page in the "Flow" style
 *
 * This class implements the HistoryView base class
 */

/*!
  Basic HistoryFlowView constructor requires a PageManager to manage the pages
  and a parent QWidget
  @param mgr : WebPageController handle for this class
  @param parent : Widget parent for this class
  @see  WebPageController
*/
HistoryFlowView::HistoryFlowView(WebPageController* mgr, QWidget* parent, const QString& aType) :
    HistoryView(mgr,parent)
,   m_type(aType)
{
    m_jsObject = new HistoryViewJSObject(this, 0, type());
}

/*!
  Basic HistoryFlowView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
  Note: This functionality is not yet tested
  @param mgr : WebPageController handle for this class
  @param parent : Graphics Widget parent for this class
  @see  WebPageController
  
*/
HistoryFlowView::HistoryFlowView(WebPageController* mgr, QGraphicsWidget* parent,const QString& aType) :
    HistoryView(mgr,parent)
,   m_type(aType)
{
m_jsObject = new HistoryViewJSObject(this, 0, type());
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void HistoryFlowView::activate()
{
    if (!d->m_flowInterface) {
      d->m_flowInterface = new GraphicsPictureFlow(d->m_widgetParent);
      emit instantiated(this);
    }
    HistoryView::activate();
}

/*! 
  set the history view's center index
  @param i   : history index to be set
*/
void HistoryFlowView::setCenterIndex(int i)
{    
    HistoryView::setCenterIndex(i);
    GraphicsPictureFlow * pf = static_cast<GraphicsPictureFlow*>(d->m_flowInterface);
    pf->showSlide(i);
}

/*!
  get the history view's current slide image
  @return QImage
*/

QImage HistoryFlowView::getCurrentSlide()
{
    GraphicsPictureFlow * pf = static_cast<GraphicsPictureFlow*>(d->m_flowInterface);
    qDebug()<<pf->centerIndex();
    return pf->slide(pf->centerIndex());
}

} // namespace WRT
