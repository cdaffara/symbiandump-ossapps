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
#include "ProgressBarItem.h"
#include "GWebContentView.h"
#include "ContentViewDelegate.h"
#include "ChromeSnippet.h"
#include "webpagecontroller.h"

#include <QDebug>

namespace GVA {

  ProgressBarItem::ProgressBarItem(ChromeSnippet * snippet, QGraphicsItem* parent)
    : NativeChromeItem(snippet, parent),
      m_progress(0)
  {
    //Set indicator color from element css
    QString cssColor = m_snippet->element().styleProperty("color", QWebElement::ComputedStyle);
    CSSToQColor(cssColor, m_color);

    connectAll();
  }

  ProgressBarItem:: ~ProgressBarItem()
  {

  }

  void ProgressBarItem::onProgress(int progress)
  {
      setProgress(((qreal)progress)/100);
  }

  void ProgressBarItem::setProgress(qreal progress)
  {
    //QApplication::processEvents(QEventLoop::ExcludeSocketNotifiers);
    //qDebug() << "ProgressBarItem::onProgress: " << progress;
    if (m_progress != progress){
      m_progress = progress;
      update();
    }
  }

  void ProgressBarItem::onStart() {
    onProgress(0);
  }

  void ProgressBarItem::onFinished(bool ok) {
    Q_UNUSED(ok)
    onProgress(0);
  }

  void ProgressBarItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
  {
    Q_UNUSED(opt)
    Q_UNUSED(widget)

    //painter->fillRect(QRectF(0,0, geometry().width(), geometry().height()), Qt::blue);
    if (m_progress > 0) {
        QSizeF s = size();
        qreal minWidth = s.width()/10;
        painter->fillRect(QRectF(0,0, minWidth + (s.width() - minWidth)*m_progress, s.height()), m_color);
    }
    NativeChromeItem::paint(painter, opt, widget);
  }

  void ProgressBarItem::connectAll() {
    WebPageController* pageController = WebPageController::getSingleton();
    if (pageController){
      connect(pageController, SIGNAL(loadStarted()), this, SLOT(onStart()));
      connect(pageController, SIGNAL(loadProgress(int)), this, SLOT(onProgress(int)));
      connect(pageController, SIGNAL(loadFinished(bool)), this, SLOT(onFinished(bool)));
    }
  }

} // end of namespace GVA


