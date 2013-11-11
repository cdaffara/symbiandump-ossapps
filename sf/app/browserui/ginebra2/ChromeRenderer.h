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

#ifndef __GINEBRA_CHROMERENDERER_H__
#define __GINEBRA_CHROMERENDERER_H__

#include <QObject>
#include <QWebPage>
#include <QWebFrame>

namespace GVA {

  class WebChromeItem;

  class ChromeRenderer : public QObject
  {
    Q_OBJECT
  public:
    ChromeRenderer(QWebPage * chromePage, QObject * parent = 0);
    virtual ~ChromeRenderer();
    QWebPage * page() { return m_page; }
    void setPage(QWebPage * page) {m_page = page;}
    QWebFrame * frame() { if (m_page) return m_page->mainFrame(); return 0;}
    void resize(QSizeF newSize);
    void addRenderItem(WebChromeItem * item) {m_renderList.append(item);}
    void clearRenderList() {if(!m_renderList.isEmpty()) m_renderList.clear();}
    void updateChromeLayout() { emit chromeResized(); }
  public slots:
    void repaintRequested(const QRect& dirtyRect);
  signals:
    void chromeRepainted(const QRectF& rect = QRectF());
    void chromeResized();
  private:
    QWebPage * m_page;
    QList<WebChromeItem*> m_renderList;
  };

} // end of namespace GVA

#endif // __GINEBRA_CHROMERENDERER_H__
