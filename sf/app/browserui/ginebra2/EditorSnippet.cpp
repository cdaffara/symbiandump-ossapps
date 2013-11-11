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

#include "EditorSnippet.h"

namespace GVA {

  EditorSnippet::EditorSnippet( const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element )
    : ChromeSnippet( elementId, chrome, widget, element )
  {
    connectAll();
  }

  void EditorSnippet::setChromeWidget(QGraphicsWidget * widget){
    ChromeSnippet::setChromeWidget(widget);
    connectAll();
  }

  EditorSnippet * EditorSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
  {
      EditorSnippet* that = new EditorSnippet(elementId, chrome, 0, element);
      that->setChromeWidget( new TextEditItem( that, chrome ) );
      return that;
  }
  
  void EditorSnippet::connectAll(){
    if(m_widget){
      GTextEditor * editor = static_cast<TextEditItem*>(m_widget)->editor();
      connect(editor, SIGNAL(textMayChanged()), this, SIGNAL(textChanged()));
      connect(editor, SIGNAL(activated()), this, SIGNAL(activated()));
      connect(editor, SIGNAL(focusChanged(bool)), this, SLOT(onFocusChanged(bool)));
      connect(editor, SIGNAL(tapped(QPointF&)), this, SLOT(onTapped(QPointF&)));
    }
  }

  void EditorSnippet::onFocusChanged(bool in)
  {
    if(in)
      emit gainedFocus();
    else
      emit lostFocus();
  }

  void EditorSnippet::onTapped(QPointF& /*pos*/){
    emit gainedFocus();
  }

  void EditorSnippet::setText( const QString & text )
  {
    static_cast<TextEditItem*>(m_widget)->setText(text);
  }

  QString EditorSnippet::text()
  {
    return static_cast<TextEditItem*>(m_widget)->text();
  }

  void EditorSnippet::setCursorPosition(int pos)
  {
    static_cast<TextEditItem*>(m_widget)->setCursorPosition(pos);
  }

  int EditorSnippet::charCount(){
    return static_cast<TextEditItem*>(m_widget)->characterCount();
  }

  void EditorSnippet::selectAll(){
    return static_cast<TextEditItem*>(m_widget)->selectAll();
  }

  void EditorSnippet::unselect(){
    return static_cast<TextEditItem*>(m_widget)->unselect();
  }
  
  int EditorSnippet::getTextOptions(){
    return (int) static_cast<TextEditItem*>(m_widget)->getTextOptions(); 
  }
  
  void EditorSnippet::setTextOptions(int flag){
    return static_cast<TextEditItem*>(m_widget)->setTextOptions(flag);
  }

  void EditorSnippet::setMaxTextLength(int length){
    return static_cast<TextEditItem*>(m_widget)->setMaxTextLength(length);
  }
}
