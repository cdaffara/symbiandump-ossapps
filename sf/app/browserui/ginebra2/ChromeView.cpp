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

#include <QtGui>
#include <QWebPage>

#ifdef ORBIT_UI
#include <hbview.h>
#include <hbaction.h>
#endif

#include "ChromeView.h"
#include "ChromeWidget.h" //TODO: get rid of this, refer directly to layout
#include "ChromeLayout.h" 
#ifndef NO_QSTM_GESTURE
#include "qstmgestureevent.h"
#endif

#ifdef ENABLE_PERF_TRACE
#include "wrtperftracer.h"
#endif

namespace GVA {

ChromeView::ChromeView(QGraphicsScene *graphicsScene, ChromeWidget * chrome, QWidget * parent)
#ifdef ORBIT_UI
  : HbMainWindow(parent),
#else
  : QGraphicsView(graphicsScene, parent),
#endif // ORBIT_UI
    m_chrome(chrome),
    m_topWidget(chrome->layout())
{
#ifdef ORBIT_UI
  addView(chrome->layout());
#endif // ORRBIT_UI

  // Initialize the ChromeWidget with the scene created in the ChromeView
  chrome->layout()->setScene(scene());

  //setGeometry(chrome->geometry().toRect());
  setObjectName("ChromeView");
  //When content view is external widget, make the background transparent
  //setStyleSheet("QGraphicsView#ChromeView {margin:0; border: 0; padding:0; background:transparent}");
  setStyleSheet("QGraphicsView#ChromeView {margin:0; border: 0; padding:0; background:#fff}");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

#ifdef BEDROCK_TILED_BACKING_STORE
  setFrameShape(QFrame::NoFrame);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
#endif  
	
  //NB: maybe not needed?
  setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
  //installEventFilter(this);
  //chrome->page()->setView(this);
#ifndef NO_QSTM_GESTURE
  ungrabGesture(Qt::PanGesture);
  ungrabGesture(Qt::TapGesture);
  ungrabGesture(Qt::TapAndHoldGesture);
  ungrabGesture(Qt::PinchGesture);
  ungrabGesture(Qt::SwipeGesture);
#endif

#ifdef ORBIT_UI
  /* Hide the platform title bar */
  HbView * view = currentView();  
  view->setTitleBarVisible(false);
#endif

}

ChromeView::~ChromeView()
{
}

void ChromeView::resizeEvent(QResizeEvent * ev)
{
    //Resize the chrome to match the view and scene rectangle size
    if (m_topWidget) {

      // On calling setGeometry on QGraphicsWidget, the layout resizes if
      // first called before resize on children happens. In order to avoid painting
      // the children in their old positions, first let children change size
      // ChromeWidget * w = static_cast<ChromeWidget*>(m_topWidget);
      // w->sizeChange(ev->size());
      //TODO: move sizeChange to ChromeLayout, remove m_chrome member !!!!!
      m_chrome->sizeChange(ev->size());
      m_topWidget->setGeometry(0,0, ev->size().width(), ev->size().height());
    }


#ifdef ORBIT_UI
    HbMainWindow::resizeEvent(ev);
#else
     QGraphicsView::resizeEvent(ev);
#endif 
	
#ifdef BEDROCK_TILED_BACKING_STORE
    if (scene()) {
        QRectF rect(QPointF(0, 0), size());
        scene()->setSceneRect(rect);
    }
#endif	
}

//Never scroll the chrome
//NB: this shouldn't be needed, but some events from
//the chrome are causing scrolling. Need to track this
//down further.

void ChromeView::scrollContentsBy(int dx, int dy)
{
    Q_UNUSED(dx)
    Q_UNUSED(dy)
  // qDebug() << "View scroll";
  //  QGraphicsView::scrollContentsBy(dx, dy);
}

  //Eat key events not otherwise consumed.
  /*  bool ChromeView::eventFilter(QObject * obj, QEvent * ev)
  {
   if (ev->type() == QEvent::KeyPress){
        int key = static_cast<QKeyEvent*>(ev)->key();
        if (key == Qt::Key_Down || key == Qt::Key_Up || key ==Qt::Key_Left){
         return true;
       }
    }
    return QObject::eventFilter(obj,ev);

    }*/

bool ChromeView::event(QEvent* event)
{
#ifndef NO_QSTM_GESTURE
      if (event->type() == QEvent::Gesture) {
          QStm_Gesture* gesture = getQStmGesture(event);
          if (gesture) {
              QPoint pos = mapFromGlobal(gesture->position());
              QGraphicsScene* gs = scene();
              QGraphicsItem* gi = gs->itemAt(QPointF(pos));

              if (gi) {
                  gs->sendEvent(gi, event);
              }
              return true;
          }
      }
#endif
    return QGraphicsView::event(event);
}



} // namespace GVA
