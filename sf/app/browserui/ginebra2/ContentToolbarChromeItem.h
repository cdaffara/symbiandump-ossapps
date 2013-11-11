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

#ifndef __GINEBRA_CONTENTTOOLBARCHROMEITEM_H
#define __GINEBRA_CONTENTTOOLBARCHROMEITEM_H

#include <QtGui>
#include "Toolbar.h"
#include "ToolbarChromeItem.h"

class QTimeLine;
class QTimer;

namespace GVA {

  class ToolbarFadeAnimator: public QObject
  {

    Q_OBJECT

    public:

      ToolbarFadeAnimator();
      ~ToolbarFadeAnimator();
      void start(bool visible);
      void stop();


    private slots:
      void valueChange(qreal step);

    Q_SIGNALS:
      void updateVisibility(qreal step);
      void finished();

    private:
      QTimeLine *m_timeLine;

  };

  class ContentToolbarChromeItem : public ToolbarChromeItem
  {
    Q_OBJECT

    public:
      ContentToolbarChromeItem(ChromeSnippet* snippet, QGraphicsItem* parent = 0);
      virtual ~ContentToolbarChromeItem();
      virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
      virtual void setSnippet(ChromeSnippet * s);
      void addLinkedChild(ChromeSnippet * s);

      void toggleMiddleSnippet();
      bool autoHideToolbar() { return  m_autoHideToolbar ;}

    protected:
      virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);
      /// Reimplemented to consume the events
      virtual void mousePressEvent( QGraphicsSceneMouseEvent * ev );
      virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * ev );



    private slots:
      void onChromeComplete();
      void stopInactivityTimer();
      void onLoadFinished(bool);
      void onLoadStarted();
      void resetTimer();
      void onInactivityTimer();
      void onSnippetMouseEvent( QEvent::Type type);

      void onAnimFinished();
      void onUpdateVisibility(qreal);
      void onMVCloseComplete();

    private:
      void addFullBackground();
      void changeState( ContentToolbarState state, bool animate = false);
      void onStateEntry(ContentToolbarState state, bool animate);
      bool mvSnippetVisible();
      void hideLinkedChildren() ;

      // State Enter and Exit functions
      void  stateEnterFull(bool);
      void  stateEnterPartial(bool animate=false);
      void  stateEnterAnimToPartial(bool animate =false);
      void  stateEnterAnimToFull(bool animate =false);

      ToolbarFadeAnimator * m_animator;
      QPainterPath* m_background;
      QTimer* m_inactivityTimer;
      QList <ChromeSnippet *> m_linkedChildren;
      qreal m_bgopacity;
      qreal m_maxOpacity;
      ContentToolbarState m_state;
      bool m_autoHideToolbar;
      ContentToolbarTimerState m_timerState;
  };

} // end of namespace GVA

#endif // __GINEBRA_CONTENTTOOLBARCHROMEITEM_H
