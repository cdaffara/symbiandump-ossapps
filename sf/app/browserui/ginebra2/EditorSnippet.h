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

#ifndef __gva_editorsnippet_h__
#define __gva_editorsnippet_h__

#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "EditorWidget.h"

namespace GVA {


  class EditorSnippet : public ChromeSnippet
  {
    Q_OBJECT
    public:
       EditorSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
       virtual ~EditorSnippet() {;}
       static EditorSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
       virtual void setChromeWidget(QGraphicsWidget * widget);
       Q_PROPERTY( QString text READ text WRITE setText)
       void setText( const QString & text );
       QString text();
    public slots:
       int charCount();
       void setCursorPosition(int pos);
       void selectAll();
       void unselect();
       void onFocusChanged(bool in);
       void onTapped(QPointF& pos);
       int getTextOptions();
       // Calling this function will overwrite the existing options
       void setTextOptions(int flag);
       void setMaxTextLength(int length);
    signals:
       void activated();
       void lostFocus();
       void gainedFocus();
       void textChanged();
    private:
       void connectAll();
  };
}

#endif
