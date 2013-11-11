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

#ifndef CHROMEWIDGETJSOBJECT_H_
#define CHROMEWIDGETJSOBJECT_H_

#include <QObject>
#include <QUrl>

#include "ChromeLayout.h"
#include "ChromeWidget.h"
#include "ChromeEffect.h"

// Comment out this define to disable logging via javascript.
#define ENABLE_LOG

namespace GVA {

/*!
 * \defgroup JavascriptAPI Javascript API objects.
 * All classes in this group provide Javascript API access to the underlying
 * C++ classes in the browser via their public slots, signals and properties.
 */

/*! \ingroup JavascriptAPI
 * \brief Javascript API wrapper for ChromeWidget.
 *
 * Javascript object name: "chrome".
 *
 * Example javascript code to connect the chromeComplete signal:
 * \code
 * chrome.chromeComplete.connect(function() {
 *    app.debug("chrome complete");
 *    }
 * );
 * \endcode
 * \sa ChromeWidget
 */
class ChromeWidgetJSObject : public QObject {
  Q_OBJECT
public:
  ChromeWidgetJSObject(QObject *parent, ChromeWidget *chromeWidget);

public slots:
  int width() { return m_chromeWidget->layout()->size().width(); }
  void alert(const QString & msg) { m_chromeWidget->alert(msg); }
  qreal slideView(qreal delta) { return m_chromeWidget->layout()->slideView(delta); }
  void chromeInitialized() { m_chromeWidget->chromeInitialized(); }
  void reloadChrome() { m_chromeWidget->reloadChrome(); }
  void loadUrlToCurrentPage(const QString & url) { m_chromeWidget->loadUrlToCurrentPage(url); }
  void updateChromeLayout() { m_chromeWidget->updateChromeLayout(); }

#ifdef ENABLE_LOG
  /// Write string \a msg to log file "c:\Data\GinebraLog.txt".
  void log(const QString msg);
#endif

signals:
  void chromeComplete();
  void aspectChanged(int aspect);
  void prepareForGeometryChange();
  //NB: This should be symbian ifdef'd but that would require symbian-specific chrome
  void symbianCarriageReturn();
  void popupShown(const QString &id);
  void popupHidden(const QString &id);

public:
  /// Returns a list of all existing snippets.
  QObjectList getSnippets() { return m_chromeWidget->getSnippets(); }
  Q_PROPERTY(QObjectList snippets READ getSnippets)

  /// \brief Returns either "portrait" or "landscape".
  QString getDisplayMode() const { return m_chromeWidget->layout()->getDisplayMode(); }
  Q_PROPERTY(QString displayMode READ getDisplayMode)

  QObject* getDisplaySize() const { return m_chromeWidget->getDisplaySize(); }
  Q_PROPERTY(QObject* displaySize READ getDisplaySize)

  /// Returns the path to the chrome directory.  Ex: "chrome/bedrockchrome".
  QString getBaseDirectory() const { return m_chromeWidget->getBaseDirectory(); }
  Q_PROPERTY(QString baseDirectory READ getBaseDirectory)

  /*! The color applied to disabled chrome snippets and views.
   * \sa disabledOpacity
   */
  Q_PROPERTY(QString disabledColor READ disabledColor WRITE setDisabledColor)
  QString disabledColor() const { return ChromeEffect::disabledColor.name(); }
  void setDisabledColor(const QString &value) { ChromeEffect::disabledColor.setNamedColor(value); }

  /*! The opacity of the color applied to disabled chrome snippets and views.
   * \sa disabledColor
   */
  Q_PROPERTY(qreal disabledOpacity READ disabledOpacity WRITE setDisabledOpacity)
  qreal disabledOpacity() const { return ChromeEffect::disabledOpacity; }
  void setDisabledOpacity(const qreal &value) { ChromeEffect::disabledOpacity = value; }

protected:
#ifdef ENABLE_LOG
  QString getLogPath();
  void initLogFile();
#endif

private:
  ChromeWidget *m_chromeWidget;
};

}

#endif /* CHROMEWIDGETJSOBJECT_H_ */
