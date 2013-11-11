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

#ifndef __WEBPAGECONTROLLER_H__
#define __WEBPAGECONTROLLER_H__

//#include <QWidget>
#include <QAction>
#include <QNetworkReply>
#include <QSslError>
#include <QWebFrame>
#include <QIcon>
#include <QEvent>
#include "browserpagefactory.h"
#include "BWFGlobal.h"
#include "messageboxproxy.h"
#include <QDir>

class QGraphicsWebView;
class LowMemoryHandler;

namespace WRT {
    class WrtBrowserContainer;
}

using namespace WRT;

// -----------------------------------------------------------

class BWF_EXPORT WebPageControllerUpdateViewPortEvent : public QEvent {
  public:
    WebPageControllerUpdateViewPortEvent() : QEvent(staticType()) {}
    static QEvent::Type staticType() {
        static int s_type = QEvent::registerEventType();
        return (QEvent::Type)s_type;
    }
};

// -----------------------------------------------------------

class WebPageControllerPrivate;

class BWF_EXPORT WebPageController : public QObject, public BrowserPageFactory
{
    Q_OBJECT
public:
   /*!
    * WebPageController Constructor
    * @param parent: parent QObject 
    */  
    WebPageController(QObject* parent = 0);
    /*!
     * WebPageController Destructor
     */ 
    ~WebPageController();

public:

    QString currentDocTitle();
    Q_PROPERTY(QString currentDocTitle READ currentDocTitle)

    QString currentDocUrl() const;
    Q_PROPERTY(QString currentDocUrl READ currentDocUrl)

    QString currentPartialUrl();
    Q_PROPERTY(QUrl currentPartialUrl READ currentPartialUrl)

    QString currentRequestedUrl() const;
    Q_PROPERTY(QString currentRequestedUrl READ currentRequestedUrl)

    QString currentPartialReqUrl();
    Q_PROPERTY(QUrl currentPartialReqUrl READ currentPartialReqUrl)

    int currentPageIndex();
    Q_PROPERTY(int currentPageIndex READ currentPageIndex)

    int contentsYPos();
    Q_PROPERTY(int contentsYPos READ contentsYPos)

    /* This indicates whether the current page is secure or not */
    int secureState();
    Q_PROPERTY(int secureState READ secureState)

    int loadState();
    Q_PROPERTY(int loadState READ loadState)

    QString  loadText();
    Q_PROPERTY(QString loadText READ loadText)

    int loadProgressValue();
    Q_PROPERTY(int loadProgressValue READ loadProgressValue)

    bool isPageLoading();
    Q_PROPERTY(bool isPageLoading READ isPageLoading)

    bool loadCanceled();
    Q_PROPERTY(bool loadCanceled READ loadCanceled)

    bool errorUrlMatches();
    Q_PROPERTY(bool errorUrlMatches READ errorUrlMatches)
    
    bool networkError();
    Q_PROPERTY(bool networkError READ networkError)

    QString networkErrorMsg();
    Q_PROPERTY(QString networkErrorMsg READ networkErrorMsg)

    QString networkErrorUrl();
    Q_PROPERTY(QString networkErrorUrl READ networkErrorUrl)
    
    QString promptMsg();
    Q_PROPERTY(QString promptMsg READ promptMsg)

    QString promptReserved();
    Q_PROPERTY(QString promptReserved READ promptReserved)

    WRT::WrtBrowserContainer* openPage();
    WRT::WrtBrowserContainer* openPageFromHistory(int index);
    void closePage(WRT::WrtBrowserContainer*);

    WRT::WrtBrowserContainer* currentPage() const;

    QList<WRT::WrtBrowserContainer*>* allPages();

    QList<QAction*> getContext();

    // persistent storage related methods
    void saveHistory(int* windowsSaved, int* activeWindowId);
    void deleteHistory();    
    
    WRT::WrtBrowserContainer* startupRestoreHistory(QWidget* parent, int index, WRT::WrtBrowserContainer* page);
    void saveNumberOfWindows(int windowsSaved);
    void saveActiveWindowId(int activeWindowId);
    int restoreNumberOfWindows();
    int historyWindowCount();
    int activeWindowId();
    
    void setLastUrl(QString url);

    QIcon pageIcon();
    
    QGraphicsWebView* webView();
    void updatePageThumbnails();
    void resizeAndUpdatePageThumbnails(QSize& s);
    QString partialUrl(const QUrl &url);
    bool removeDirectory(QDir &aDir);
    
    void setEditMode(bool);
    bool editMode();
    Q_PROPERTY(bool editMode READ editMode)

private:
    void checkAndUpdatePageThumbnails();
    WRT::WrtBrowserContainer* openPage(QObject* parent, WRT::WrtBrowserContainer* page=0);
    void releaseMemory();

public: // public actions available for this view
    QAction * getActionReload();
    QAction * getActionStop();
    QAction * getActionBack();
    QAction * getActionForward();    

public slots:
    int pageCount();
    QString getLastUrl();
    void deleteCookies();
    void deleteCache();
    void deleteDataFiles();
    void savePopupSettings(int);
    bool getPopupSettings();
    void saveSaverestoreSettings(int);
    bool getSaverestoreSettings();
    void clearHistoryInMemory();
    QString getTextEncoding();
    void setTextEncoding(const QString & encoding );
    
    void setSettingsLoaded(int value);
    int  getSettingsLoaded();
        
    void currentReload();
    void currentStop();
    void currentBack();
    void currentForward();
    void currentLoad(const QString &url);
    void currentLoad(const QUrl & url);
    void currentSetFromHistory(int historyIndex);
    void gotoCurrentItem();
    void setCurrentPage(WRT::WrtBrowserContainer*);
    void LoadInNewWindow(const QString & url);
         	    
    static WebPageController* getSingleton();

    void urlTextChanged(QString );
    void loadInitialUrlFromOtherApp(QString url);
    void loadFromHistory();
    void loadLocalFile();
    QString guessUrlFromString(const QString &s);
    QString removeScheme(const QString &s);
    QString searchUrl(const QString &s);
	QObjectList fetchSuggestions(const QString &s);
    
    void updateHistory();
    
    void share(const QString &url); 
    void feedbackMail(const QString &mailAddress, const QString &mailBody); 

private slots:
    void updateStatePageLoading();
    void updateStatePageLoadComplete(bool);
    void updateActions(bool pageIsLoading=false);
    void unsupportedContentArrived(QNetworkReply *);
    void createWindow(WrtBrowserContainer* page);
    void updateJSActions();
    void urlChanged(const QUrl &url);
    void secureStateChange(int);
    void processNetworkErrorHappened(const QString & msg); 
    void processNetworkErrorUrl(const QUrl & url);
    void handleLowMemory();
    void handleOutOfMemory();

    void onLoadFinished(bool);
    void onDatabaseQuotaExceeded (QWebFrame *,QString);  
    void onLoadFinishedForBackgroundWindow(bool);
signals:
    void creatingPage( WRT::WrtBrowserContainer* newPage);
    void pageCreated( WRT::WrtBrowserContainer* newPage);
    void pageDeleted( WRT::WrtBrowserContainer* oldPage);
    void pageChanged( WRT::WrtBrowserContainer* oldPage, WRT::WrtBrowserContainer* newPage );

    void titleChanged(const QString &);
    void loadStarted();
    void initialLayoutCompleted();
    void loadProgress( const int progress );
    void loadFinished( const bool ok );
    void loadFinishedForBackgroundWindow(const bool ok, WRT::WrtBrowserContainer *page);
    void databaseQuotaExceeded (QWebFrame *,QString);  

    void currentPageIconChanged();
    void currentPageUrlChanged( const QUrl & url);
    void partialUrlChanged(QString url);
    void unsupportedContent(QNetworkReply *);

    void networkRequestError(QNetworkReply *reply);
    void sslErrors(QNetworkReply *, const QList<QSslError> &);
    void showMessageBox(WRT::MessageBoxProxy*);

    void linkClicked( const QString & url );
    void selectionChanged();

    void pageScrollPositionZero();
    void pageScrollRequested(int, int, const QRect & );

    void showSecureIcon();
    void hideSecureIcon();

    // All signals for urlsearch 
    void pageLoadStarted();
    void pageLoadFailed(); 
    void pageLoadProgress( const int progress );
    void pageLoadFinished( const bool ok );
    void pageUrlChanged( const QString str );
    void pageIconChanged();
	
	  // Signal for network status 
    void networkErrorHappened(const QString & msg );
    
    // Signals for low and out of memory
    void lowMemory();
    void outOfMemory();
 
private:

    bool m_networkError; 
    QString m_networkErrorMsg;
    QString m_networkErrorUrl;  
    bool m_bErrorUrlMatches;
    QString m_promptMsg;
    QString m_promptReserved;  
    LowMemoryHandler *m_memoryHandler;
    WebPageControllerPrivate * const d;
    int m_settingsLoaded;
#ifdef ENABLE_TESTS
    friend class WebPageControllerTest;
#endif
};
#endif // __WEBPAGECONTROLLER_H__
