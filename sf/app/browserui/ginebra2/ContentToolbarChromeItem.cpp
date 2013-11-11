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
* This class extends ToolbarChromeItem. It paints the toolbar background
* and controls toolbar visibility in the content view.
*
*/

#include "ContentToolbarChromeItem.h"
#include "ChromeItem.h"
#include "WebChromeContainerSnippet.h"
#include "ContentToolbarSnippet.h"
#include "GWebContentView.h"
#include "mostvisitedsnippet.h"
#include "webpagecontroller.h"
#include "ViewController.h"
#include "ChromeEffect.h"


#include <QTimeLine>
#include <QDebug>

#define TOOLBAR_MARGIN 4
#define TOOLBAR_ANIMATION_DURATION 200
#define TOOLBAR_INACTIVITY_DURATION 5000

namespace GVA {

  ToolbarFadeAnimator::ToolbarFadeAnimator(): m_timeLine(NULL)
  {
  }


  ToolbarFadeAnimator::~ToolbarFadeAnimator() {

    if (m_timeLine)
      delete m_timeLine;
  }

  void ToolbarFadeAnimator::start(bool visible) {
    //qDebug() << __PRETTY_FUNCTION__ << visible;
    if (!m_timeLine) {
      m_timeLine = new QTimeLine(TOOLBAR_ANIMATION_DURATION);
      connect(m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(valueChange(qreal)));
      connect(m_timeLine, SIGNAL(finished()), this, SIGNAL(finished()));
    }
    else {
      m_timeLine->stop();
    }
    if (!visible) {
      m_timeLine->setDirection(QTimeLine::Forward);

    }
    else {
      m_timeLine->setDirection(QTimeLine::Backward);

    }
    m_timeLine->start();

  }

  void ToolbarFadeAnimator::stop() {

    //qDebug() << __PRETTY_FUNCTION__ ;
    m_timeLine->stop();
  }

  void ToolbarFadeAnimator::valueChange(qreal step) {
    emit  updateVisibility(step);
  }

  ContentToolbarChromeItem::ContentToolbarChromeItem(ChromeSnippet* snippet, QGraphicsItem* parent)
      : ToolbarChromeItem(snippet, parent),
      m_background(NULL),
      m_state(CONTENT_TOOLBAR_STATE_FULL),
      m_autoHideToolbar(true),
      m_timerState(CONTENT_TOOLBAR_TIMER_STATE_ALLOW)
  {

    m_inactivityTimer = new QTimer(this);
    connect(m_inactivityTimer, SIGNAL(timeout()), this, SLOT(onInactivityTimer()));

    m_animator = new ToolbarFadeAnimator();
    connect(m_animator, SIGNAL(updateVisibility(qreal)), this, SLOT(onUpdateVisibility(qreal)));
    connect(m_animator, SIGNAL(finished()), this, SLOT(onAnimFinished()));
    
    m_maxOpacity = m_bgopacity = opacity();   
    if (m_autoHideToolbar ) {
       connect(m_snippet->chrome(), SIGNAL(chromeComplete()), this, SLOT(onChromeComplete()));
    }
    
    setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);

  }

  ContentToolbarChromeItem::~ContentToolbarChromeItem()
  {
    if (m_background )
        delete m_background;
    delete m_inactivityTimer;

    delete m_animator;

  }

  void ContentToolbarChromeItem::resizeEvent(QGraphicsSceneResizeEvent * ev)
  {
    //qDebug() << __PRETTY_FUNCTION__ << boundingRect();
    ToolbarChromeItem::resizeEvent(ev);
    addFullBackground();

  }

  void ContentToolbarChromeItem::mousePressEvent(QGraphicsSceneMouseEvent * ev)
  {
      // If we are not in full state, ignore the event. Once igonre, none of the
      // other mouse events are received until the next mouse press
      if (m_state == CONTENT_TOOLBAR_STATE_PARTIAL ) {
          ev->ignore();
      }
      else {
          ChromeSnippet * mv = m_snippet->chrome()->getSnippet("MostVisitedViewId");
          // Let mostvisited snippet handle the key press if it is visible
          if (mv && mv->isVisible() ){
              ev->ignore();
          }
      }
  }

  void ContentToolbarChromeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * ev)
  {
      Q_UNUSED(ev);
      // Do nothing - prevent the event from trickling down
      
  }

  void ContentToolbarChromeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
  {
    Q_UNUSED(opt)
    Q_UNUSED(widget)

//    qDebug() << __PRETTY_FUNCTION__ << m_state;
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(pen());
    painter->setOpacity(m_bgopacity);

    switch (m_state) {
        case CONTENT_TOOLBAR_STATE_PARTIAL:
          ToolbarChromeItem::paint(painter, opt, widget);
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
        case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
          ToolbarChromeItem::paint(painter, opt, widget);
        case CONTENT_TOOLBAR_STATE_FULL:
          // fill path with color
          painter->fillPath(*m_background,QBrush(grad()));
          painter->drawPath(*m_background);
          if(m_state == CONTENT_TOOLBAR_STATE_FULL && !isEnabled()) {
              // Disabled, apply whitewash.
              ChromeEffect::paintDisabledRect(painter, opt->exposedRect);
          }
          break;
        default:
          qDebug() << "ContentToolbarChromeItem::paint invalid state" ;
          break;
    }
    // restore painter
    painter->restore();

  }


  void ContentToolbarChromeItem::setSnippet(ChromeSnippet* snippet) {
    ToolbarChromeItem::setSnippet(snippet);
    m_maxOpacity = m_bgopacity = opacity();
    
    if (m_autoHideToolbar ) {
        connect(snippet->chrome(), SIGNAL(chromeComplete()), this, SLOT(onChromeComplete()));
    }

  }

  void ContentToolbarChromeItem::onChromeComplete() {

    GWebContentView* webView  = static_cast<GWebContentView*> (m_snippet->chrome()->getView("WebView"));
    //qDebug() << __PRETTY_FUNCTION__ << webView;
    if (webView ) {
        connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
        connect(webView, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
#ifdef BEDROCK_TILED_BACKING_STORE
        connect(webView, SIGNAL(contextEvent(::WebViewEventContext *)), this, SLOT(resetTimer()));
#else
        connect(webView->widget(), SIGNAL(contextEvent(::WebViewEventContext *)), this, SLOT(resetTimer()));
#endif
    }

  }

  void ContentToolbarChromeItem::onLoadStarted() {

      m_timerState = CONTENT_TOOLBAR_TIMER_STATE_ALLOW;
      stopInactivityTimer();
  }


  void ContentToolbarChromeItem::onLoadFinished(bool ok) {

    Q_UNUSED(ok);
    //qDebug() << __PRETTY_FUNCTION__ << m_state << "Timer Allowed" << m_timerState;

    if (m_autoHideToolbar  && m_timerState == CONTENT_TOOLBAR_TIMER_STATE_ALLOW) {
        ControllableViewBase* curView = m_snippet->chrome()->viewController()->currentView();
        if (curView && curView->type() == "webView") {
            GWebContentView * gView = qobject_cast<GWebContentView*> (curView);
            bool isSuperPage = gView ? gView->currentPageIsSuperPage() : false;

        // Start inactivity timer if full toolbar is visible 
        if (!isSuperPage && m_state ==  CONTENT_TOOLBAR_STATE_FULL ) 
            m_inactivityTimer->start(TOOLBAR_INACTIVITY_DURATION);
        }
    }

  }
  void ContentToolbarChromeItem::resetTimer() {
      
      m_timerState = CONTENT_TOOLBAR_TIMER_STATE_NONE;
      stopInactivityTimer();
  }

  void ContentToolbarChromeItem::stopInactivityTimer() {

    //qDebug() << __PRETTY_FUNCTION__<< m_state << " Timer Active: " << m_inactivityTimer->isActive(); 
    // Stop inactivity timer 
    if (m_inactivityTimer->isActive() )
        m_inactivityTimer->stop();

  }

  void ContentToolbarChromeItem::onInactivityTimer() {

    //qDebug() << __PRETTY_FUNCTION__ << m_state;

    //We are here because inactivity timer timed out. So we have to be in full toolbar state with no
    // popups. So change fade to Partial state after stopping inactivity timer
    m_inactivityTimer->stop();
    changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL, true);
  }

  void ContentToolbarChromeItem::onSnippetMouseEvent( QEvent::Type type) {

    //qDebug() << __PRETTY_FUNCTION__ << type ;

    if (type == QEvent::MouseButtonPress || type ==  QEvent::GraphicsSceneMousePress) {
        resetTimer();
    } 
  }

  void ContentToolbarChromeItem::addLinkedChild(ChromeSnippet * s) {
      m_linkedChildren.append(s);
  }

  void ContentToolbarChromeItem::toggleMiddleSnippet() {
      //qDebug() << "TOGGLE MIDDLE : " << m_state;
      switch (m_state) {
         case CONTENT_TOOLBAR_STATE_PARTIAL:
            changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_FULL);
            break;
          case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
            // Stop animation and change to full immediately
            m_animator->stop();
            changeState(CONTENT_TOOLBAR_STATE_FULL, false);
            break;
          case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
            // Do nothing here - will reset inactivity timer when
            // animation completes
            break;
          case CONTENT_TOOLBAR_STATE_FULL:
            // if MV is active then wait for it to hide before changing the toolbar state
            if (mvSnippetVisible()) {
                changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL, false);
            }
            else {
                changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL, true);
            }
            break;
          default:
            break;
        }
  }

  void ContentToolbarChromeItem::onMVCloseComplete() {

//    qDebug() << __PRETTY_FUNCTION__;
    ChromeSnippet * mv = m_snippet->chrome()->getSnippet("MostVisitedViewId");
    disconnect(mv, SIGNAL(mostVisitedSnippetCloseComplete()) , this, SLOT(onMVCloseComplete()));
    // MostVisitedSnippet animation complete, so let's do toolbar animation
    if (m_state == CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL)
      changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL, true);

  }

  void ContentToolbarChromeItem::onUpdateVisibility(qreal step) {

    //qDebug() << __PRETTY_FUNCTION__ << step << m_bgopacity << opacity();
    // Value we get is 1.0 based, adjust it based on our max opacity
    qreal value = step - (1.0 - m_maxOpacity);
    value =  (value > 0)? value: 0.0;

    if (m_bgopacity != value ) {
      m_bgopacity = value;
      setOpacity(m_maxOpacity - value);
      ContentToolbarSnippet * s = static_cast<ContentToolbarSnippet*>(m_snippet);
      s->middleSnippet()->widget()->setOpacity(value);
      update();
    }
  }

  void ContentToolbarChromeItem::onAnimFinished() {

    ContentToolbarState state = CONTENT_TOOLBAR_STATE_INVALID;
    bool animate = false;
    //qDebug() << __PRETTY_FUNCTION__ << m_state;
    switch (m_state) {
        case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
          state = CONTENT_TOOLBAR_STATE_PARTIAL; 
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
          state = CONTENT_TOOLBAR_STATE_FULL; 
          animate = true;
          break;
        default:
          break;

    }
  
    ContentToolbarSnippet * s = static_cast<ContentToolbarSnippet*>(m_snippet);
    s->handleToolbarStateChange(state);
    changeState(state, animate);
    //qDebug() << __PRETTY_FUNCTION__ << m_state;

  }
  void ContentToolbarChromeItem::addFullBackground() {

    //qDebug() << __PRETTY_FUNCTION__ ;
    qreal roundness((boundingRect().height() -TOOLBAR_MARGIN)/2);
    QRectF r(1, 1, boundingRect().width()-TOOLBAR_MARGIN, boundingRect().height()-TOOLBAR_MARGIN);

    if (m_background ) {
        delete m_background;
        m_background = NULL;
    }
    m_background = new QPainterPath();
    m_background->addRoundedRect(r, roundness, roundness);
  }

  void ContentToolbarChromeItem::stateEnterFull(bool animate) {

    //qDebug() <<__PRETTY_FUNCTION__ ;
    ContentToolbarSnippet * s = static_cast<ContentToolbarSnippet*>(m_snippet);

    // Show the middle snippet and reset the opacity if we are here directly with no aniamtion
    if (!animate) {
      m_bgopacity = 0.75;
      s->middleSnippet()->show();
    }

    m_state = CONTENT_TOOLBAR_STATE_FULL;
    s->middleSnippet()->widget()->setOpacity(1.0);
    // TODO: specify the rect to be updated to avoid full repaint
    update();
  }

  void ContentToolbarChromeItem::stateEnterPartial(bool animate) {

    //qDebug() <<__PRETTY_FUNCTION__ ;
    Q_UNUSED(animate);
    ContentToolbarSnippet * s = static_cast<ContentToolbarSnippet*>(m_snippet);

    // Explicity hide the linked snippets so that toggle button javascript gets the right
    // signals that it is expecting
    hideLinkedChildren();

    s->middleSnippet()->hide();
    m_state = CONTENT_TOOLBAR_STATE_PARTIAL;

  }

  void ContentToolbarChromeItem::stateEnterAnimToFull(bool animate) {

    //qDebug() <<__PRETTY_FUNCTION__ ;
    Q_UNUSED(animate);
    ContentToolbarSnippet * s = static_cast<ContentToolbarSnippet*>(m_snippet);

    m_state = CONTENT_TOOLBAR_STATE_ANIM_TO_FULL;
    s->middleSnippet()->show();
    m_animator->start(false);

  }

  void ContentToolbarChromeItem::stateEnterAnimToPartial(bool animate) {
    m_state = CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL;

    if (animate ) {
      m_animator->start(true);
    }
    else {

      MostVisitedSnippet * mv = static_cast<MostVisitedSnippet *>(m_snippet->chrome()->getSnippet("MostVisitedViewId"));
      connect(mv, SIGNAL(mostVisitedSnippetCloseComplete()) , this, SLOT(onMVCloseComplete()));
      mv->close();
    }

  }

  void ContentToolbarChromeItem::changeState( ContentToolbarState state, bool animate){

    onStateEntry(state, animate);
  }

  void ContentToolbarChromeItem::onStateEntry(ContentToolbarState state, bool animate){

   //qDebug() << __PRETTY_FUNCTION__ ;
   switch (state) {
        case CONTENT_TOOLBAR_STATE_PARTIAL:
          stateEnterPartial(animate);
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
          stateEnterAnimToFull(animate);
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
          stateEnterAnimToPartial(animate);
          break;
        case CONTENT_TOOLBAR_STATE_FULL:
          stateEnterFull(animate);
          break;
        default:
          qDebug() << "ContentToolbarChromeItem::onStateEntry -  invalid state" ;
          break;
    }
  }

  bool ContentToolbarChromeItem::mvSnippetVisible() {

    ChromeSnippet * mv = m_snippet->chrome()->getSnippet("MostVisitedViewId");
    bool result = false;

    if (mv && mv->isVisible() ) {
      result = true;
    }
    return result;
  }

  void ContentToolbarChromeItem::hideLinkedChildren() {

    for (int i=0; i < m_linkedChildren.count() ; i++) {

      m_linkedChildren.at(i)->hide();
    }
  }
} // end of namespace GVA


