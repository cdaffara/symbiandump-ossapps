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

#include "ActionButtonSnippet.h"
#include "controllableviewimpl.h"

namespace GVA {

  ActionButtonSnippet::ActionButtonSnippet( const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element )
    : ChromeSnippet( elementId, chrome, widget, element )
  {

  }

  ActionButtonSnippet * ActionButtonSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
  {
      ActionButtonSnippet* that = new ActionButtonSnippet(elementId, chrome, 0, element);
      that->setChromeWidget( new ActionButton( that ) );
      return that;
  }

  QAction * ActionButtonSnippet::getDefaultAction()
  {
    return (static_cast<ActionButton*>(m_widget)->defaultAction());
  }

  //void ActionButtonSnippet::setDefaultAction( QAction * action, QEvent::Type triggerOn )
  void ActionButtonSnippet::setDefaultAction( QAction * action, bool triggerOnDown, bool triggerOnUp)
  {
    static_cast<ActionButton*>(m_widget)->setAction(action,triggerOnDown, triggerOnUp);
  }

  QIcon  ActionButtonSnippet::icon( )
  {
    return static_cast<ActionButton*>(m_widget)->icon();
  }

  void ActionButtonSnippet::setIcon( const QString & icon )
  {
    static_cast<ActionButton*>(m_widget)->addIcon(icon);
  }

  void ActionButtonSnippet::setDisabledIcon( const QString & icon )
  {
    static_cast<ActionButton*>(m_widget)->addIcon(icon, QIcon::Disabled);

  }

  void ActionButtonSnippet::setActiveIcon( const QString & icon )
  {
    static_cast<ActionButton*>(m_widget)->addIcon(icon, QIcon::Active);
  }

  // Scriptable method to directly connect an action button to a view action

  void ActionButtonSnippet::connectAction( const QString & action, const QString & view, bool onDown, bool  onUp)
  {
    ControllableViewBase *viewBase = m_chrome->getView( view );

    if (viewBase){
      QAction * viewAction = viewBase->getAction(action);
      if (viewAction)
        static_cast<ActionButton*>(m_widget)->setAction(viewAction, onDown, onUp);
      return;
    }
  }

  bool ActionButtonSnippet::isChecked( )
  {
    return (static_cast<ActionButton*>(m_widget)->isChecked());
  }

  void ActionButtonSnippet::setEnabled( bool enabled )
  {
    static_cast<ActionButton*>(m_widget)->setEnabled(enabled);
  }

  void ActionButtonSnippet::setActive( bool enabled )
  {
    static_cast<ActionButton*>(m_widget)->setActive(enabled);
  }

  void ActionButtonSnippet::setActiveOnPress( bool active )
  {
    static_cast<ActionButton*>(m_widget)->setActiveOnPress(active);
  }

  void ActionButtonSnippet::updateButtonState(bool state) {
      if (state ) {
          static_cast<ActionButton*>(m_widget)->onShown();
      }
      else {
          static_cast<ActionButton*>(m_widget)->onHidden();
      }
  }

}
