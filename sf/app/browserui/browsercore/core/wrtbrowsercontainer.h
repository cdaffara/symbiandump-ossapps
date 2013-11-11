/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WRTBROWSERCONTAINER_H__
#define __WRTBROWSERCONTAINER_H__

#include "brtglobal.h"

#include <QWebPage>
#include "wrtbrowsercontainer_p.h"
#include "wrtBrowserDefs.h"
#include "ZoomMetaData.h"


#include <QWidget>
#include <QNetworkProxy>

struct BrowserPageFactory;
struct ZoomMetaData;
class QWebFrame;
class QWebHistoryItem;
class QNetworkReply;
class QAuthenticator;
class QNetworkProxy;
class QNetworkAccessManager;

namespace WRT {

class WrtController;
class WrtBrowserContainerPrivate;
class SchemeHandler;
class SecureUIController;
class LoadController;

class WRT_BROWSER_EXPORT WrtBrowserFileChooser
{
public:
    virtual ~WrtBrowserFileChooser();
    virtual QString chooseFile(QWebFrame * parentFrame, const QString & suggestedFile) = 0;
};

class WRT_BROWSER_EXPORT WrtBrowserContainer : public QWebPage
{
    Q_OBJECT
public:
    static WrtBrowserContainer* createPageWithWidgetParent(QObject* parent=0,WrtBrowserContainer* page=0);
    explicit WrtBrowserContainer(QObject* parent = 0);
    virtual ~WrtBrowserContainer();
    
    virtual WrtBrowserContainer* createWindow(QWebPage::WebWindowType);

    QGraphicsWidget* webWidget() const;
    void setWebWidget(QGraphicsWidget* view);
    SchemeHandler* schemeHandler() const;

    QImage pageThumbnail(qreal scaleX, qreal scaley);

    void setPageFactory(BrowserPageFactory* f);

    QString pageTitle();
    
    int secureState();
    WRT::LoadController * loadController( ) {return d->m_loadController;}

    ZoomMetaData pageZoomMetaData() ;
    void setPageZoomMetaData( ZoomMetaData zoomData );
    
    /* Indicates whether this is a blank window with no page loaded*/
    bool emptyWindow();
    bool restoreSession();

    void setFileChooser(WrtBrowserFileChooser * chooser);
    
    void setUpdateThumbnail(bool update) { d->m_needUpdateThumbnail = update; }
    bool needUpdateThumbnail() { return d->m_needUpdateThumbnail; }

protected:
    virtual QString chooseFile(QWebFrame * parentFrame, const QString & suggestedFile);
    virtual QString userAgentForUrl(const QUrl& url) const;


Q_SIGNALS:

    void createNewWindow(WrtBrowserContainer* page);
    void pageScrollPositionZero();

    void secureStateChange(int);

public slots:
    void savePageDataToHistoryItem(QWebFrame*, QWebHistoryItem* item);
    void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *);
    void slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
    
private slots:
    void pageSecureState(int);

private:
    WrtBrowserContainerPrivate* d;
};

}
#endif
