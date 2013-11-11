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

#include "TitleItem.h"
#include "webpagecontroller.h"
#include "Utilities.h"


namespace GVA {

  // Methods for class TitleItem

  //GTitleItem extends QGraphicsTextItem 

  GTitleItem::GTitleItem(QGraphicsItem * parent)
  : QGraphicsTextItem(parent)
  , m_maxTextLength(0)
  {
    // Disable wrapping, force text to be stored and displayed
    // as a single line.
    QTextOption textOption = document()->defaultTextOption();
    textOption.setWrapMode(QTextOption::NoWrap);
    document()->setDefaultTextOption(textOption);
    // This is needed to initialize m_textLine.
    setText("");

    WebPageController * pageController = WebPageController::getSingleton();
  }

  GTitleItem::~GTitleItem()
  {
  }

  void GTitleItem::setText(const QString & text)
  {
    //qDebug() << "GTitleItem::setText " << text;
    QString newText = text;
    if(m_maxTextLength > 0 && text.length() > m_maxTextLength ) {
        newText = newText.left(m_maxTextLength);
    }
    setPlainText(newText);
    m_textLine = document()->begin().layout()->lineForTextPosition(0);
  }

  void GTitleItem::setMaxTextLength(int length)
  {
    if (m_maxTextLength <= 0 && length > 0) {
        QString text = toPlainText();
        if( text.length() > length ) {
          setPlainText(text.left(length));
        }
    }
    m_maxTextLength = length;
  }


  qreal GTitleItem::textWidth()
  {
    return m_textLine.naturalTextWidth();
  }

  void GTitleItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
  {
    QGraphicsTextItem::mousePressEvent(event);
    QPointF pos = event->pos();
    //qDebug() << "GTitleItem: received mouse press" << pos;
    emit tapped(pos);
  }

} // namespace GVA
