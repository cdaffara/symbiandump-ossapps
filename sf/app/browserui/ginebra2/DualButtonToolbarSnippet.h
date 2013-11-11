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
#ifndef __GINEBRA_DUALBUTTONTOOLBARSNIPPET_H
#define __GINEBRA_DUALBUTTONTOOLBARSNIPPET_H

#include <QtGui>
#include "ToolbarSnippet.h"

namespace GVA {

/* \brief This class provides a base-class for two-button toolbar snippets
 *
 * The buttons are places at either ends of the toolbar
 */
  class DualButtonToolbarSnippet : public ToolbarSnippet
  {
    Q_OBJECT
  public:
    DualButtonToolbarSnippet(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
    virtual ~DualButtonToolbarSnippet();

    /// Reimplemented 
    void updateOwnerArea();
    void updateSize(QSize );

  private:
    /// Must be Reimplemented
    virtual void setAction(ChromeSnippet * s) = 0;
    virtual void addChild(ChromeSnippet * child) = 0;

    /// Sets the spacing of the first child to position the other button at the other
    /// end of the screen
    void setChildSpacing(int width) ;
  };

} // end of namespace GVA

#endif // __GINEBRA_DUALBUTTONTOOLBARSNIPPET_H
