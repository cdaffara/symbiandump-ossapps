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

#ifndef __gva_actionbuttonsnippet_h__
#define __gva_actionbuttonsnippet_h__

#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "ActionButton.h"

namespace GVA {



  class ActionButtonSnippet : public ChromeSnippet
  {
    Q_OBJECT
    public:
       ActionButtonSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
       virtual ~ActionButtonSnippet() {;}
       static ActionButtonSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
       QAction * getDefaultAction();
       void setDefaultAction( QAction * action, bool triggerOnDown =false, bool triggerOnUp=true);
       QIcon icon();
       bool isChecked();
       void setActive( bool active );
       void setActiveOnPress( bool );

    public slots:
       void setIcon( const QString & icon );
       void setDisabledIcon( const QString & icon );
       void setActiveIcon( const QString & icon );
       void connectAction( const QString & action, const QString & view, bool onDown = false, bool onUp = true);

       void setEnabled( bool enabled );
       void updateButtonState(bool state);

  };
}

#endif // ACTIONBUTTONSNIPPET_H
