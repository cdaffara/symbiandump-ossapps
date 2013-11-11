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
#ifndef __GINEBRA_TOOLBARSNIPPET_H
#define __GINEBRA_TOOLBARSNIPPET_H


#include <QtGui>
#include "ActionButtonSnippet.h"
#include "Toolbar.h"
#include "WebChromeContainerSnippet.h"

namespace GVA {


/* \brief This is the base class for all toolbar snippets.
 * 
 * This is the base class for all toolbar snippets. It maintains a list of all actions associated
 * with the toolbar (populated by the specific derived class). It connects all the actioins
 * to the associated view. It also sets the images for the different states of the button for each
 * of the buttons. 
 */

  class ToolbarSnippet : public WebChromeContainerSnippet
  {
    Q_OBJECT
  public:
    ToolbarSnippet(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
    virtual ~ToolbarSnippet();

  private Q_SLOTS:
    virtual void onChromeComplete();

  protected:
    virtual void setAction(ChromeSnippet * s);

    /// Reimplemented from WebChromeContainerSnippet
    virtual void updateOwnerArea();
    virtual void updateSize(QSize);

    /// Utilities
    int getIndex(ChromeSnippet * s );
    int getIndex(int actionId );
    ActionButtonSnippet * getActionButtonSnippet( int  actionId);

    /// Details of all actions in the toolbar
    QList<ToolbarActions_t*>  m_actionInfo;
    /// View the toolbar is associated with
    QString m_type;

  };

} // end of namespace GVA

#endif // __GINEBRA_TOOLBARSNIPPET_H
