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

#include "ChromeRenderer.h"
#include "WebChromeItem.h"
#include <QDebug>

namespace GVA {

ChromeRenderer::ChromeRenderer(QWebPage * chromePage, QObject * parent)
  : QObject(parent),
    m_page(0)

{
  setPage(chromePage);
  QPalette viewPalette = page()->palette();
  viewPalette.setBrush(QPalette::Base, Qt::transparent);
  //viewPalette.setColor(QPalette::Window, Qt::transparent);
  page()->setPalette(viewPalette);
  connect(page(), SIGNAL(repaintRequested(const QRect &)), this, SLOT(repaintRequested(const QRect &)));
}

ChromeRenderer::~ChromeRenderer()
{

}

void ChromeRenderer::resize(QSizeF newSize)
{
  page()->setPreferredContentsSize(newSize.toSize());
  page()->setViewportSize(page()->mainFrame()->contentsSize());
  emit chromeResized();
}

void ChromeRenderer::repaintRequested(const QRect& dirtyRect)
{
  //qDebug() << "ChromeRenderer repaintRequested: " << dirtyRect;
  WebChromeItem * item;    
  if(!m_renderList.isEmpty()){
      foreach(item, m_renderList){
          if (item->elementRect().intersects(dirtyRect) && !item->isPainting()){
              item->update();
          }
      }
      emit chromeRepainted(dirtyRect);
  }
}

} // end of namespace GVA
