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

#include "WebChromeContainerSnippet.h"
#include "ChromeItem.h"
#include "WebChromeItem.h"
#include "ChromeWidget.h"
#include "ChromeRenderer.h"
#include "ChromeSnippet.h"

#include <QDebug>

namespace GVA {

  WebChromeContainerSnippet::WebChromeContainerSnippet(const QString & elementId, ChromeWidget * chrome, const QWebElement & element)
    : ChromeSnippet(elementId, chrome, 0, element)
      ,m_layoutHeight(0)
  {

    m_layoutWidth = chrome->layout()->size().width();

    //QGraphicsWidget * item = static_cast<QGraphicsWidget*> (widget());
    //NB: maybe size should be fixed only in one direction?
    //item->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    //NB: add a method for setting owner area
    //item->setPreferredSize(m_ownerArea.width(), m_ownerArea.height());
    //Also resize in case item is not part of anchor layout
    //item->resize(item->preferredSize());

   //NB: Linear layout efaults to horizontal: handle vertical layouts too.
    m_layout = new QGraphicsLinearLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);

    //Add a stretch element at the beginning.
    m_layout->addStretch();
    //item->setLayout(m_layout);
    //When chrome is resized sizes for snippets may change
    QObject::connect(m_chrome->renderer(), SIGNAL(chromeResized()), this, SLOT(updateOwnerArea()));
    QObject::connect(m_chrome, SIGNAL(prepareForSizeChange(QSize)), this, SLOT(updateSize(QSize)));

  }

  WebChromeContainerSnippet::~WebChromeContainerSnippet()
  {
    // delete m_layout;
  }

  void WebChromeContainerSnippet::positionChildren()
  {
    ; //Do nothing since the layout positions children automatically.
  }
  
  void WebChromeContainerSnippet::setChromeWidget(QGraphicsWidget * widget){
      widget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
      widget->setLayout(m_layout);
      ChromeSnippet::setChromeWidget(widget);
  }
  
  void WebChromeContainerSnippet::addChild(ChromeSnippet * child)
  {
    //Prevent layout from stretching the child widgets. NB: Revisit this to make configurable from chrome?
    child->widget()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    //child->widget()->setParentItem(this->widget()); // Shouldn't be needed, right?
    m_layout->addItem(child->widget());
    //Add a stretch after each element so the layout looks like this: |stretch|item|stretch|item . . . . stretch|item|
    m_layout->addStretch();

    qreal childHeight = child->widget()->size().height();
    if (childHeight > m_layoutHeight){
      m_layoutHeight = childHeight;

      updateSizes();
    }
    emit childAdded(child);

    m_layout->activate();
  }

  void WebChromeContainerSnippet:: updateOwnerArea()
  {
    updateSizes();
    QObject::disconnect(m_chrome->renderer(), SIGNAL(chromeResized()), this, SLOT(updateOwnerArea()));

  }

  void WebChromeContainerSnippet::setLayoutHeight(int height){
    if (m_layoutHeight != height){
      m_layoutHeight = height;
      updateSizes();
    }
  }

  void WebChromeContainerSnippet::setLayoutWidth(qreal width, bool update){
    if (m_layoutWidth != width){
      m_layoutWidth = width;
      if (update ) {
        updateSizes();
      }
    }
  }

  void WebChromeContainerSnippet::updateSize(QSize size ) {

    Q_UNUSED(size);
    // m_layoutWidth should have been set by now through derived classes. We don't want to set it
    // here as that would overwrite any width set before. For example, width of the middle snippet
    // in toolbar is set by the main toolbar.
    updateSizes();
  }

  void WebChromeContainerSnippet::updateSizes() {
      
    QGraphicsWidget * item = static_cast<QGraphicsWidget*> (widget());
    //Resize the item
    item->setPreferredSize(m_layoutWidth, m_layoutHeight);
    //Also resize in case item is not part of anchor layout
    item->resize(item->preferredSize());

  }

} // endof namespace GVA
