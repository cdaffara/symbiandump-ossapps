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

#ifndef __GINEBRA_CHROMELAYOUT_H__
#define __GINEBRA_CHROMELAYOUT_H__

#include <QtGui>

class ControllableViewBase;
class QGraphicsSceneContextMenuEvent;

namespace GVA {

  class ChromeSnippet;
  class SlidingWidget;

  enum ChromeAnchor
  {
    anchorNone,
    anchorCenter,
    anchorTop,
    anchorBottom,
    anchorLeft,
    anchorRight,
    anchorTopLeft,
    anchorTopRight,
    anchorBottomLeft,
    anchorBottomRight
  };

  enum Aspect
  {
    portrait,
    landscape
  };
 

  /*!
   * \brief This class is responsible for laying out the snippets and views that constitute the UI
   */

  class ChromeLayout : public QGraphicsWidget
  {

  Q_OBJECT

  public:
    ChromeLayout(QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~ChromeLayout();
    void addView(ControllableViewBase * controllableView);
    void addSnippet(ChromeSnippet * snippet, ChromeSnippet * container);
    void anchorSnippet(ChromeSnippet * snippet); //TODO: should be private, but is needed from ChromeWidget::anchorSnippet which is caled from ChromeSnippet
    //NB: Should be deprecated (see comment in .cpp)
    void adjustAnchorOffset(ChromeSnippet * snippet, qreal delta);
    void anchorToView(ChromeSnippet* snippet, const QString & where = "top");
    void detachFromView(ChromeSnippet* snippet, const QString & where = "top");
    void anchorTogether(ChromeSnippet* first, ChromeSnippet *  second, qreal x = 0, qreal y = 0);
    void unAnchor(ChromeSnippet* first);
    QGraphicsScene * scene() { return m_scene; }
    void setScene(QGraphicsScene *scene);
    int bottomBarHeight() { return m_bottomBarHeight;}
    /// \brief Returns either "portrait" or "landscape".
    QString getDisplayMode() const;
    Q_PROPERTY(QString displayMode READ getDisplayMode)

  public slots:
    void snippetShown(ChromeSnippet * snippet);
    void snippetHiding(ChromeSnippet * snippet);
    qreal slideView(qreal delta);
    qreal shrinkView(qreal delta);
    int width(){ return (int)size().width(); }
  
  signals:
    void resizing(QSizeF newSize);
    void aspectChanged(int aspect);

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent *ev);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

  private:
    void addAnchors();
    QGraphicsScene * m_scene;
    QGraphicsAnchorLayout *m_layout;
    QGraphicsAnchorLayout *m_viewLayout;
    SlidingWidget *m_viewPort;
    QGraphicsWidget *m_topBar;
    QGraphicsWidget *m_bottomBar;
    QGraphicsWidget *m_leftBar;
    QGraphicsWidget *m_rightBar;
    Aspect m_aspect;
    int m_bottomBarHeight;
  };

} // end of namespace GVA

#endif // __GINEBRA_CHROMEWIDGET_H__
