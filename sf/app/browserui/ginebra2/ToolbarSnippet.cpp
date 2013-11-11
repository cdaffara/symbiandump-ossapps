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
* This class extends WebChromeContainerSnippet class to hold the
* toolbar buttons
*
*/

#include "ToolbarSnippet.h"
#include "ActionButtonSnippet.h"

#include <QDebug>


namespace GVA {

  ToolbarSnippet::ToolbarSnippet(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
                       : WebChromeContainerSnippet(elementId, chrome, element)
  {

      connect(m_chrome,  SIGNAL(chromeComplete()), this, SLOT(onChromeComplete()));

  }

  ToolbarSnippet::~ToolbarSnippet()
  {
      foreach(ToolbarActions_t * t, m_actionInfo) {
          delete t;
      }

  }

  void ToolbarSnippet::updateSize(QSize sz) {
      //qDebug() << "ToolbarSnippet::updateSize" << sz.width() ;
      setLayoutWidth(sz.width());
      WebChromeContainerSnippet::updateSize(sz);
  }

  void ToolbarSnippet::updateOwnerArea() {

    setLayoutWidth(m_chrome->layout()->size().width());
    WebChromeContainerSnippet::updateOwnerArea();

  }

  void ToolbarSnippet::onChromeComplete() {

      foreach(ToolbarActions_t * t, m_actionInfo) {
          setAction(m_chrome->getSnippet(t->id));
      }

  }

  void ToolbarSnippet::setAction(ChromeSnippet * s) {

      //qDebug() << "setAction: " << s->elementId() << m_actionInfo.size();
     
      ActionButtonSnippet * button  = static_cast<ActionButtonSnippet*> (s);
      int index = getIndex(s);

      if (index != -1) {
          ToolbarActions_t * t = m_actionInfo.at(index);
          button->connectAction(t->actionName, m_type);

          // Set the button icons if it has not been set for any state (we can do this through Javascript)
          //qDebug() << "setAction " << s->elementId() << button->icon().isNull();
          if (button->icon().isNull() ) {
              button->setIcon(t->normalImg);
              button->setDisabledIcon(t->disabledImg);
              button->setActiveIcon(t->activeImg);
          }

      }
  }

  int ToolbarSnippet::getIndex( ChromeSnippet *  s) {
      int index = -1;
      for (int i = 0; i < m_actionInfo.size() ; i++ ) {

          ToolbarActions_t * t = m_actionInfo.at(i);
          if (t->id == s->elementId()) {
              index = i;
              break;
          }

      }
      return index;

  }

  int ToolbarSnippet::getIndex( int actionId) {
      int index = -1;
      for (int i = 0; i < m_actionInfo.size() ; i++ ) {

          ToolbarActions_t * t = m_actionInfo.at(i);
          if (t->actionId == actionId ) {
              index = i;
              break;
          }

      }
      return index;


  }

  ActionButtonSnippet * ToolbarSnippet::getActionButtonSnippet( int  actionId) {

      int index = getIndex(actionId);
      ToolbarActions_t * t = m_actionInfo.at(index);
      ActionButtonSnippet * button  = static_cast<ActionButtonSnippet*> ( m_chrome->getSnippet(t->id));
      return button;

  }


} // end of namespace GVA


