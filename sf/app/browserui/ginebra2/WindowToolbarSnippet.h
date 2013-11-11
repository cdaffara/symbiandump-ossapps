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
#ifndef __GINEBRA_WINDOWTOOLBARSNIPPET_H
#define __GINEBRA_WINDOWTOOLBARSNIPPET_H

#include <QtGui>
#include "DualButtonToolbarSnippet.h"

namespace GVA {

/* \brief This class provides a snippet for the toolbar in WindowsView
 *
 * This class provides a snippet for the toolbar in WindowsView. It contains two
 * buttons - back and add, placed at either ends of the toolbar. The button 'actions'
 * are created in \sa WindowView . It inherits \sa Toolbar class
 * Note: only back button action is handled in this class. Add action is handled directly
 * in \sa WindowView
 */
  class WindowToolbarSnippet : public DualButtonToolbarSnippet
  {
    Q_OBJECT
  public:
    WindowToolbarSnippet(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
    virtual ~WindowToolbarSnippet();
    static WindowToolbarSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);

  private Q_SLOTS:
    /// Back action trigger handler. Switches back to webview
    void handleBackButton();
    void onAspectChanged( ); 

  private:
    /// Reimplemented
    void setAction(ChromeSnippet * s);
    void addChild(ChromeSnippet * child) ;
  };

} // end of namespace GVA

#endif // __GINEBRA_WINDOWTOOLBARSNIPPET_H
