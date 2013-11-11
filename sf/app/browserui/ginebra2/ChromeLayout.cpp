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

#include <QList>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>

#include "ChromeSnippet.h"
#include "WebChromeContainerSnippet.h"
#include "ChromeLayout.h"
#include "SlidingWidget.h"
#include "ViewController.h"
#include "ObjectCharm.h"
#include "bedrockprovisioning.h"

namespace GVA {

// -----------------------------

  ChromeLayout::ChromeLayout(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags),
      m_topBar(0),
      m_bottomBar(0),
      m_leftBar(0),
      m_rightBar(0),
      m_aspect(portrait),
      m_bottomBarHeight(0)
  {
    m_layout = new QGraphicsAnchorLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    setLayout(m_layout);
    addAnchors();
    m_viewPort = new SlidingWidget(this);
    m_layout->addAnchor(m_viewPort, Qt::AnchorTop, m_topBar, Qt::AnchorBottom);
    m_layout->addAnchor(m_viewPort, Qt::AnchorBottom, m_bottomBar, Qt::AnchorTop);
    m_layout->addAnchor(m_viewPort, Qt::AnchorLeft, m_leftBar, Qt::AnchorRight);
    m_layout->addAnchor(m_viewPort, Qt::AnchorRight, m_rightBar, Qt::AnchorLeft);
  }
  
  ChromeLayout::~ChromeLayout()
  {
    delete m_bottomBar;
    delete m_topBar;
    delete m_leftBar;
    delete m_rightBar;
    delete m_viewPort;
  }
  
  void ChromeLayout::addAnchors(){
    if (!m_bottomBar){
      m_bottomBar = new QGraphicsWidget(this);
      m_bottomBar->setPreferredHeight(0);
      m_bottomBar->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
      m_layout->addAnchor(m_bottomBar, Qt::AnchorBottom, m_layout, Qt::AnchorBottom);
      m_layout->addAnchors(m_bottomBar, m_layout, Qt::Horizontal);
    }
    if (!m_topBar){
      m_topBar = new QGraphicsWidget(this);
      m_topBar->setPreferredHeight(0);
      m_topBar->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
      m_layout->addAnchor(m_topBar, Qt::AnchorTop, m_layout, Qt::AnchorTop);
      m_layout->addAnchors(m_topBar, m_layout, Qt::Horizontal);
    }
    if (!m_leftBar){
      m_leftBar = new QGraphicsWidget(this);
      m_leftBar->setPreferredWidth(0);
      m_leftBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
      m_layout->addAnchor(m_leftBar, Qt::AnchorLeft, m_layout, Qt::AnchorLeft);
      m_layout->addAnchors(m_leftBar, m_layout, Qt::Vertical);
    }
    if (!m_rightBar){
      m_rightBar = new QGraphicsWidget(this);
      m_rightBar->setPreferredWidth(0);
      m_rightBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
      m_layout->addAnchor(m_rightBar, Qt::AnchorRight, m_layout, Qt::AnchorRight);
      m_layout->addAnchors(m_rightBar, m_layout, Qt::Vertical);
    }
  }
  
  void ChromeLayout::resizeEvent(QGraphicsSceneResizeEvent *ev)
  {
    emit resizing(ev->newSize());
    int aspect = m_aspect;
    m_aspect = (ev->newSize().width() > ev->newSize().height())?landscape:portrait;
    if (m_aspect != aspect) {
      QString mode = (m_aspect == landscape ? "Landscape" : "Portrait");
      emit aspectChanged(m_aspect);
    }
    QGraphicsWidget::resizeEvent(ev);
  }
  
  void ChromeLayout::addView(ControllableViewBase * controllableView){
    if (controllableView->widget()) {
      m_viewPort->setWindow(controllableView->widget());
      controllableView->widget()->setFocusPolicy(Qt::ClickFocus); //NB: Is this needed? Does it break anything?
      controllableView->widget()->setZValue(-1.0); //Make sure we are behind any center anchored snippets
    }
  }
  
  void ChromeLayout::anchorToView(ChromeSnippet* snippet, const QString& where)
  {
    Q_UNUSED(where)
      m_viewPort->attachItem(snippet->widget());
  }
  
  void ChromeLayout::detachFromView(ChromeSnippet* snippet, const QString& where)
  {
    Q_UNUSED(where)
      m_viewPort->detachItem(snippet->widget());
  }
  
  void ChromeLayout::anchorTogether(ChromeSnippet * first, ChromeSnippet * second, qreal x, qreal y)
  {
      first->widget()->setParentItem(second->widget());
      first->widget()->setPos(x,y);
  }
  
  void ChromeLayout::unAnchor(ChromeSnippet* snippet)
  {
    snippet->widget()->setParentItem(0);
    snippet->widget()->setParentItem(this);
  }
  
  qreal ChromeLayout::shrinkView(qreal delta)
  {
    m_viewPort->setShrinkMax(bottomBarHeight());
    return m_viewPort->shrink(delta);
  }
  
  qreal ChromeLayout::slideView(qreal delta)
  {
    return m_viewPort->slide(delta);
  }
  
  //Check to see if snippet has an anchor and lay it out accordingly.
  //Otherwise snippets determine their own positions from html
  
  void ChromeLayout::anchorSnippet(ChromeSnippet * snippet)
  {
    if (snippet->anchor() == anchorBottom){
      if (snippet->hidesContent()){
	m_layout->addAnchor(snippet->widget(), Qt::AnchorBottom, m_bottomBar, Qt::AnchorTop);
	m_layout->addAnchor(snippet->widget(), Qt::AnchorHorizontalCenter, m_bottomBar, Qt::AnchorHorizontalCenter);
      } else {
	snippet->widget()->setParentItem(m_bottomBar);
	snippet->widget()->setY(-snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorTop){
      if (snippet->hidesContent()){
	m_layout->addAnchor(snippet->widget(),Qt::AnchorTop, m_topBar, Qt::AnchorBottom);
	m_layout->addAnchor(snippet->widget(), Qt::AnchorHorizontalCenter, m_topBar, Qt::AnchorHorizontalCenter);
      }
      else {
	snippet->widget()->setParentItem(m_topBar);
	snippet->widget()->setY(snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorLeft){
      if (snippet->hidesContent())
	m_layout->addAnchor(snippet->widget(),Qt::AnchorLeft, m_leftBar, Qt::AnchorRight);
      else {
	snippet->widget()->setParentItem(m_leftBar);
	snippet->widget()->setX(snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorRight){
      if (snippet->hidesContent())
	m_layout->addAnchor(snippet->widget(),Qt::AnchorRight, m_rightBar, Qt::AnchorLeft);
      else {
	snippet->widget()->setParentItem(m_rightBar);
	snippet->widget()->setX(-snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorCenter) {
      snippet->widget()->setParentItem(m_viewPort);
      snippet->widget()->setZValue(0.0);
      QGraphicsAnchorLayout * vl = static_cast<QGraphicsAnchorLayout*>(m_viewPort->layout());
      vl->addAnchor(snippet->widget(), Qt::AnchorVerticalCenter, vl, Qt::AnchorVerticalCenter);
      vl->addAnchor(snippet->widget(), Qt::AnchorHorizontalCenter, vl, Qt::AnchorHorizontalCenter);
    }
    else if (snippet->anchor() == anchorTopLeft){
      m_layout->addCornerAnchors(snippet->widget(), Qt::TopLeftCorner, m_layout, Qt::TopLeftCorner);
    }
    else if (snippet->anchor() == anchorTopRight)
      m_layout->addCornerAnchors(snippet->widget(), Qt::TopRightCorner, m_layout, Qt::TopRightCorner);
    else if (snippet->anchor() == anchorBottomLeft)
      m_layout->addCornerAnchors(snippet->widget(), Qt::BottomLeftCorner, m_layout, Qt::BottomLeftCorner);
    else if (snippet->anchor() == anchorBottomRight)
      m_layout->addCornerAnchors(snippet->widget(), Qt::BottomRightCorner, m_layout, Qt::BottomRightCorner);
  }
  
  void ChromeLayout:: addSnippet(ChromeSnippet * snippet, ChromeSnippet * container )
  {
    snippet->widget()->setParentItem(this);
    if (container){
	container->addChild(snippet);
    }
    anchorSnippet(snippet);
  }
  
  // Call after a snippet changes to visible state
    
  void ChromeLayout::snippetShown(ChromeSnippet * snippet)
  {
    if (snippet->hidesContent()) {
      if (snippet->anchor() == anchorBottom) {
	/* snippet->widget()->size().height() is zero for  WebChromeContainerSnippet
	   when default visible container snippet is shown, get ownerArea height instead */
	WebChromeContainerSnippet * s = dynamic_cast<WebChromeContainerSnippet *>(snippet);
	if (!s) {
	  m_bottomBarHeight = snippet->widget()->size().height();
	}
	else {
	  m_bottomBarHeight = s->ownerArea().height();
	}
      }
      return;
    }
    if (snippet->anchor() == anchorBottom)
      m_bottomBar->setPreferredHeight(m_bottomBar->preferredHeight() + snippet->widget()->preferredHeight());
    else if (snippet->anchor() == anchorTop)
      m_topBar->setPreferredHeight(m_topBar->preferredHeight() + snippet->widget()->preferredHeight());
    else if (snippet->anchor() == anchorLeft){
      m_leftBar->setPreferredWidth(m_leftBar->preferredWidth() + snippet->widget()->preferredWidth());
    }
    else if (snippet->anchor() == anchorRight){
      m_rightBar->setPreferredWidth(m_rightBar->preferredWidth() + snippet->widget()->preferredWidth());
    }
  }
  
  // Call before a snippet changes to invisible state
    
  void ChromeLayout::snippetHiding(ChromeSnippet * snippet)
  {
    if (snippet->hidesContent())
      return;
    if (snippet->anchor() == anchorBottom)
      m_bottomBar->setPreferredHeight(m_bottomBar->preferredHeight() - snippet->widget()->preferredHeight());
    else if (snippet->anchor() == anchorTop)
      m_topBar->setPreferredHeight(m_topBar->preferredHeight() - snippet->widget()->preferredHeight());
    else if (snippet->anchor() == anchorLeft){
      m_leftBar->setPreferredWidth(m_leftBar->preferredWidth() - snippet->widget()->preferredWidth());
    }
    else if (snippet->anchor() == anchorRight){
      m_rightBar->setPreferredWidth(m_rightBar->preferredWidth() - snippet->widget()->preferredWidth());
    }
  }
  
  QString ChromeLayout::getDisplayMode() const {
    return (m_aspect==portrait)?"portrait":"landscape";
  }
  
  void ChromeLayout::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
  {
    event->accept();
  }

  //NB: Deprecate. This really shouldn't be necessary: anchor bars should be implemented as an expanding
  //widget class.

  void ChromeLayout::adjustAnchorOffset(ChromeSnippet * snippet, qreal delta)
  {
    if (snippet->anchor() == anchorBottom)
      m_bottomBar->setPreferredHeight(m_bottomBar->preferredHeight() + delta);
    else if (snippet->anchor() == anchorTop)
      m_topBar->setPreferredHeight(m_topBar->preferredHeight() + delta);
    else if (snippet->anchor() == anchorLeft){
      m_leftBar->setPreferredWidth(m_leftBar->preferredWidth() + delta);
    }
    else if (snippet->anchor() == anchorRight){
      m_rightBar->setPreferredWidth(m_rightBar->preferredWidth() + delta);
    }
  }

  void ChromeLayout::setScene(QGraphicsScene *scene)
  {
    m_scene = scene;
    //This is the root of the scene hierarchy
    m_scene->addItem(this);

    // For testing, turn on red dot that appears in response to various mouse events.
    if(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("EnableTouchCircle", false)) {
        new TouchCircleCharm(m_scene, this);
    }

  }
  
} // endof namespace GVA
