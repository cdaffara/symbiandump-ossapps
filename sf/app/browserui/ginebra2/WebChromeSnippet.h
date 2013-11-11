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

#ifndef __GINEBRA_WEBCHROMESNIPPET_H__
#define __GINEBRA_WEBCHROMESNIPPET_H__

#include <QtGui>
#include "ChromeSnippet.h"
#include <QWebElement>

namespace GVA {

  class ChromeRenderer;
  class ChromeWidget;
  class WebChromeItem;

  class WebChromeSnippet : public ChromeSnippet
  {
    Q_OBJECT
  public:
    WebChromeSnippet(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
    virtual ~WebChromeSnippet();
    WebChromeItem* item();
  public slots:
    void grabFocus();
    void updateOwnerArea();

  };

} // end of namespace GVA

#endif // __GINEBRA_WEBCHROMESNIPPET_H__
