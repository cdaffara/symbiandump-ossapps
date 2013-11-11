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
#ifndef __g_graphicswebview_h__
#define __g_graphicswebview_h__
#include <QtGui>
#include <QGraphicsWebView>

namespace GVA {
  class GGraphicsWebView : public QGraphicsWebView
  {
    Q_OBJECT
  public:
    GGraphicsWebView(QGraphicsItem * parent = 0);
    void paint(QPainter * paint, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
  private slots:
    void onLoadStarted();
    void onLoadFinished(bool ok);
  private:
    QTime m_loadTime;
    bool m_loading;
  };
}
#endif
