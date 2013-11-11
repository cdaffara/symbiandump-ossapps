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

#ifndef _GINEBRA_CHROME_DOM_H_
#define _GINEBRA_CHROME_DOM_H_

#include <QObject>
#include <QtCore/qvariant.h>
#include <QList>
#include <QHash>
#include <QWebElement>
#include "CachedHandler.h"

class QWebPage;
class QGraphicsItem;

namespace GVA {

class ChromeSnippet;
class ChromeWidget;
class ChromeRenderer;

class ChromeDOM : public QObject //TBD: Need QObject here?
{
  Q_OBJECT

public:
  ChromeDOM(QWebPage * page, ChromeWidget * chrome);
  ~ChromeDOM();
  QVariant evalInChromeContext(QString js);
  QWebElement getElementById(const QString &id);
  QSize getElementSize(const QString &id);
  QRect getElementRect(const QString &id);
  QString getElementAttribute(const QString &id, const QString &attribute);
  QList <QWebElement> getInitialElements();
  ChromeSnippet * getSnippet(const QString &docElementId, QGraphicsItem* parent = 0);
  //QString getCacheableScript();
  QList<CachedHandler> getCachedHandlers(const QString &elementId, const QRectF & ownerArea);
  int height() { return m_height; }
private:
  QWebElement findChromeParent(QWebElement element);
  ChromeSnippet *nativeSnippetForClassName(const QString & className, const QString elementId, QWebElement element);
  QWebPage * m_page;
  ChromeRenderer * m_renderer;
  ChromeWidget * m_chrome;
  int m_height;
  int m_bytes;
};

} // end of namespace GVA

#endif
