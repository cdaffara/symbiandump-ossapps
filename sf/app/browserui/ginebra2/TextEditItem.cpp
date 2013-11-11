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

#include <QGraphicsTextItem>
#include "TextEditItem.h"
#include "ChromeSnippet.h"

namespace GVA {
  EditorWidget::EditorWidget(QGraphicsItem * parent)
    : QGraphicsTextItem(parent)
  {
    setText("");
  }
  void EditorWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem * option, QWidget* widget)
  {
    //Hack to get rid of the ugly selection ants. This should be tunable in QGraphicsTextItem!
    QStyleOptionGraphicsItem newOption = *option;
    newOption.state &= (!QStyle::State_Selected | !QStyle::State_HasFocus);
    painter->save();
    QGraphicsTextItem::paint(painter, &newOption, widget);
    painter->restore();
  }

  // The editor document signals cursor movements when the document modified (because characters
  // are being added or removed, but not when the cursor is simply being moved. Also, the
  // document's idea of cursor position is based on the character count, not the actual pixel
  // position. To implement scrolling, we need our own cursor change event that supplies the
  // pixel change in all cases.

  void EditorWidget::keyPressEvent(QKeyEvent *event)
  {
    qreal oldX = cursorX();
    QGraphicsTextItem::keyPressEvent(event);
    emit cursorXChanged(cursorX(), oldX);
  }

  void EditorWidget::setText(const QString& text, bool html)
  {
    if (html)
      setHtml(text);
    else
      setPlainText(text);
    //All this just to get the first (and only) text line of the document!
    m_textLine = document()->begin().layout()->lineForTextPosition(0);
  }

  // Use QTextLine to compute the text metrics for the current cursor position.

  qreal EditorWidget::cursorX() {
     return m_textLine.cursorToX(textCursor().position());
  }

  TextEditItem::TextEditItem(ChromeSnippet * snippet, QGraphicsItem* parent)
    : NativeChromeItem(snippet, parent),
      m_textWidth(0),
      m_scrollPos(0)
  {
    setFlags(QGraphicsItem::ItemIsMovable);
    //The viewport clips the editor when text overflows
    m_viewPort = new QGraphicsWidget(this);
    m_viewPort->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    //The actual text editor item
    m_editor = new EditorWidget(m_viewPort);
    m_cursor = m_editor->textCursor();
    connect(m_editor, SIGNAL(cursorXChanged(qreal, qreal)), this, SLOT(onCursorXChanged(qreal, qreal)));

    //Force the editor to be a single text line
    m_textOption = m_editor->document()->defaultTextOption();
    m_textOption.setWrapMode(QTextOption::NoWrap);
    m_editor->document()->setDefaultTextOption(m_textOption);

    //Not exactly well-documented, but this flag is needed to make cursor keys work
    m_editor->setTextInteractionFlags(Qt::TextEditorInteraction);

    //Non-default key handling for scrolling, etc.
    m_editor->installEventFilter(this);

    //Set text and background colors from element css
    QString cssVal = m_snippet->element().styleProperty("color", QWebElement::ComputedStyle);
    CSSToQColor(cssVal, m_textColor);
    m_editor->setDefaultTextColor(m_textColor);
    cssVal = m_snippet->element().styleProperty("background-color", QWebElement::ComputedStyle);
    CSSToQColor(cssVal, m_backgroundColor);

    //For border-related properties, we constrain all values (top, left, etc.) to be the same.
    //These can be set using the css shorthand (e.g. padding:10px), but the computed css style will be for
    //the four primitive values (padding-top, padding-left) etc, which will all be equal.
    //Hence we just use one of the computed primitive values (top) to represent the common value.

    cssVal = m_snippet->element().styleProperty("border-top-color", QWebElement::ComputedStyle);
    CSSToQColor(cssVal, m_borderColor);
    cssVal = m_snippet->element().styleProperty("padding-top", QWebElement::ComputedStyle);
    m_padding = cssVal.remove("px").toInt();
    cssVal = m_snippet->element().styleProperty("border-top-width", QWebElement::ComputedStyle);
    m_border = cssVal.remove("px").toInt();

    //Cool effect, but shadow, if any, should be set by js
    //m_shadow = new QGraphicsDropShadowEffect();
    //m_shadow->setOffset(3.0,3.0);
    //m_shadow->setBlurRadius(2.0);
    //setGraphicsEffect(m_shadow);
  }

  TextEditItem::~TextEditItem()
  {
    delete m_editor;
  }

  void TextEditItem::resizeEvent(QGraphicsSceneResizeEvent * ev)
  {
    NativeChromeItem::resizeEvent(ev);
    m_viewPortWidth = boundingRect().width()-m_padding*2;
    m_viewPort->setGeometry(m_padding,(boundingRect().height()-m_editor->boundingRect().height())/2,m_viewPortWidth, m_editor->boundingRect().height() );
    m_editor->setTextWidth(m_viewPortWidth);
    //Make a rectangular background with a cut-out for the text. The width of the surrounding
    //background is set by padding
    m_background.addRect(boundingRect());
    m_background.addRoundedRect(m_padding, m_padding, m_viewPortWidth, boundingRect().height()-m_padding*2,4,4);
 }

  //Filter key events to emit activate signal absorb up, down keys

  bool TextEditItem::eventFilter(QObject * obj, QEvent *ev)
  {
    if (obj == m_editor){
      if (ev->type() == QEvent::KeyPress){
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(ev);
    if (keyEvent->key() == Qt::Key_Select || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
          //Signal that a carriage return-like key-press happened
      emit activated();
      return true;
    }
    if (keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up)
      return true;
        //Otherwise, pass keypress to the text editor
    return false;
      }
    }
    return NativeChromeItem::eventFilter(obj, ev);
  }

  void TextEditItem::internalScroll(qreal deltaX)
  {
    if (deltaX > -m_scrollPos)
      m_editor->moveBy(-m_scrollPos,0);
    else
      m_editor->moveBy(deltaX,0);
    m_scrollPos = m_editor->pos().x();
  }

  //Handle text scrolling
  //NB: Still needs some tweaking, for example to keep the last character visibile when
  //inserting! Rewrite as state machine?

  void TextEditItem::onCursorXChanged(qreal newX, qreal oldX)
  {
    qreal oldTextWidth = m_textWidth;
    m_textWidth = m_editor->document()->size().width();
    if (oldTextWidth == 0)
    return;
    qreal textDelta = m_textWidth - oldTextWidth;
    qreal deltaX = oldX - newX;
    //Just moving the cursor, slide window as needed
    if (textDelta == 0){
      //NB: Currently slides by one character, in some browsers slides by multiple characters
      if ((newX <= -m_scrollPos)||(newX >= (m_viewPortWidth - m_scrollPos))){
    internalScroll(deltaX);
      }
    }
    //Inserting characters
    else if (textDelta > 0){
      if (newX >= (m_viewPortWidth - m_scrollPos)){
        internalScroll(deltaX);
      }
    }
    //Deleting characters.
    else {
      if (m_scrollPos < 0){
        //Delete may be a selected block, in which case the cursor movement may be
        //different from the text delta.
        internalScroll(-textDelta);
      }
    }
  }

  // Paint background and any border

  void TextEditItem::paint(QPainter* painter, const QStyleOptionGraphicsItem * option, QWidget* widget){
    NativeChromeItem::paint(painter, option,widget);
    QPainterPath path;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(m_backgroundColor);
    if (m_border > 0){
      QPen pen;
      pen.setWidth(m_border);
      pen.setBrush(m_borderColor);
      painter->setPen(pen);
    }
    painter->drawPath(m_background);
    painter->restore();
  }

  //NB: Move these slots to the containing snippet so they can be exported to JS

  QString TextEditItem::text(){
    return m_editor->toPlainText();
  }

  void TextEditItem::setText(const QString & text){
    m_editor->setText(text);
  }

}

