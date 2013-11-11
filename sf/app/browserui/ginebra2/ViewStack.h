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

#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include <QObject>
#include "wrtbrowsercontainer.h"

namespace GVA {

class ViewController;
class ChromeView;
class ChromeWidget;

/*! \ingroup JavascriptAPI
 * \brief This class is responsible for switching the main content window from one view to another.
 *
 * Javascript object name: "ViewStack".
 *
 * Example javascript code to switch the current content view from the Web view to the Bookmarks view:
 * \code
 * ViewStack.switchView("BookmarkHistoryView", "WebView");
 * \endcode
 */
class ViewStack : public QObject
{
    Q_OBJECT

public:
    static ViewStack* getSingleton();
    ViewController *getViewController() { return m_viewController;}
    void setViewController(ViewController *value) { m_viewController = value; }
    void setChromeWidget(ChromeWidget *value) { m_chrome = value; }
    
public slots:
    void switchView(const QString &to, const QString &from);
    void creatingPage(WRT::WrtBrowserContainer*);

private:
    void fromWindowView(const QString &to);
    void fromWebView(const QString &to);
    void fromBookmarkHistoryView(const QString &to);
    void fromBookmarkTreeView(const QString &to);
    void fromSettingsView(const QString &to);
    void toWindowView();
    void toWebView();
    void toBookmarkHistoryView();
    void toBookmarkView();
    void toSettingsView();

private slots:
    void loadHistoryItem(int item);
    void goBackFromWindowView();
    void goBackFromRecentUrlView();
    void goBackFromBookmarkView();
    void showContentView(int progress);
    void goBackFromSettingsView();


signals:
    void activateBookmark();
    void activateWindowView();
    void deActivateWindowView();

    void pageChanged(int);

private:
    ViewController *m_viewController; // Not owned
    ChromeWidget *m_chrome; // Not owned
};

}  // end of namespace

#endif // VIEWSTACK_H
