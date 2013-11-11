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

#include "GGraphicsWebView.h"
namespace GVA {

  GGraphicsWebView::GGraphicsWebView(QGraphicsItem * parent)
    : QGraphicsWebView(parent),
      m_loading(false)
  {
    connect(this, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
  }

  void GGraphicsWebView::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
  {
    //    qDebug() << "d:" << painter->device()->depth() << " w:" << painter->device()->width() << " h:" << painter->device()->height();
    if (m_loading && m_loadTime.elapsed() < 7000) {
      painter->save();
      painter->setBrush(Qt::transparent);
      painter->setPen(Qt::NoPen);
      painter->drawRect(option->exposedRect.toRect());
      painter->restore();
    } else {
      QGraphicsWebView::paint(painter, option, widget);
    }
  }

  void GGraphicsWebView::onLoadStarted()
  {
    m_loading = true;
    m_loadTime.start();
  }

  void GGraphicsWebView::onLoadFinished(bool /*ok*/)
  {
    m_loading = false;
    update();
  }

}
