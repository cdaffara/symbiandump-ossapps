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

#ifndef __GINEBRA_CHROMEVIEW_H__
#define __GINEBRA_CHROMEVIEW_H__
#include <QtGui>

#ifdef ORBIT_UI
#include <hbmainwindow.h>
#define ChromeGraphics HbMainWindow
#else
#define ChromeGraphics QGraphicsView
#endif // ORBIT_UI

namespace GVA {

class ChromeWidget;

class ChromeView : public ChromeGraphics
{
    Q_OBJECT
 public:
  ChromeView(QGraphicsScene *graphicsScene, ChromeWidget * chrome, QWidget * parent = 0 );
  ~ChromeView();
  void resizeEvent(QResizeEvent * ev);
  bool event(QEvent* event);
  void scrollContentsBy(int dx, int dy);
  //protected:
  //bool eventFilter(QObject * obj, QEvent * ev);

 private:
  QGraphicsWidget * m_topWidget;
  ChromeWidget * m_chrome; //TODO: get rid of this
};
}
#endif //__GINEBRA_CHROMEVIEW_H__
