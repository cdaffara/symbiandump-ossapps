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

#ifndef PAGESNIPPET_H_
#define PAGESNIPPET_H_

#include "ChromeSnippet.h"

class QGraphicsWebView;

namespace GVA {

class PageItem;
class ExternalEventCharm;

/*! \ingroup JavascriptAPI
 * \brief This class provides a snippet that contains a QWebView.
 *
 * This class provides a snippet that contains a QWebView, allowing entire web pages to loaded
 * and displayed in the chrome.  Note that this snippet has no real physical representation in the
 * chrome page (chrome.html), ie. the DIV that defines it is empty.  Use the \ref url and
 * \ref html properties to set the contents of the page.
 *
 * Note: use these snippets sparingly since they use a significant amount of memory when
 * instantiated!  If possible, share them and/or de-instantiate them whenever possible by calling
 * \ref cleanUp.
 * \sa PageItem
 */
class PageSnippet : public ChromeSnippet {
  Q_OBJECT
public:
  PageSnippet(const QString & elementId, ChromeWidget * chrome,
              QGraphicsWidget * widget, const QWebElement & element);
  ~PageSnippet();

  static PageSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);

  // ChromeSnippet overrides:
  virtual void setVisible(bool visiblity, bool animate = true);
  virtual void setChromeWidget(QGraphicsWidget * widget);

  /// The URL of the web page.
  QString url() const;
  void setUrl(const QString &url);
  Q_PROPERTY(QString url READ url WRITE setUrl)

  /// The contents of the web page in HTML.
  void setHtml(const QString &value);
  QString html() const;
  Q_PROPERTY(QString html WRITE setHtml READ html)
  
  bool hasFocus();
  Q_PROPERTY(bool hasFocus READ hasFocus)

public slots:
  void setGeometry(int x, int y, int width, int height);
  void setSize(int width, int height);

  /// Evaluates a JavaScript expression in the context of the web page (not the chrome's context!).
  QVariant evaluateJavaScript(const QString &expression);

  /// Instantiates the web view, web page, etc.  This is called automatically when the
  /// window is shown.
  void instantiate();

  /// Destroys the web view, web page, etc. to free up memory.
  void cleanUp();

private:
  PageItem *pageItem();
  PageItem const *constPageItem() const;

  ExternalEventCharm *m_externalEventCharm;
};

}

#endif /* PAGESNIPPET_H_ */
