/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  Declaration of the Ui Loader .
 *
 */
#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include "search_global.h"
#include <qobject.h>
#include <qmutex.h>
#include <hbmainwindow.h>
class HbDocumentLoader;
class HbView;
class HbListWidget;
class HbSearchPanel;
class TsTaskSettings;
class SearchMainWindow;
class HbShrinkingVkbHost;
class SearchContentInfoDbData;
class ContentInfoDbRead;
SEARCH_CLASS( SearchStateProviderTest)

class SearchUiLoader : public QObject
    {
Q_OBJECT
public:
    /**
     * Singleton construtor
     * @since S60 ?S60_version.
     */
    static SearchUiLoader* instance()
        {
        static QMutex mutex;
        if (!m_instance)
            {
            mutex.lock();
            m_instanceCounter++;
            if (!m_instance)
                {
                m_instance = new SearchUiLoader;
                }
            mutex.unlock();
            }

        return m_instance;
        }

    /**
     * Singleton destructor
     * @since S60 ?S60_version.
     */
    static void deleteinstance()
        {
        m_instanceCounter--;
        if ((m_instanceCounter <= 0) && (m_instance))
            {
            delete m_instance;
            m_instance = 0;
            }
        }

private:
    void readDB();

public slots:

    /**
     * Slot to send the application to background fake exit
     * @since S60 ?S60_version.
     */
    void slotsendtobackground();

    /**
     * Slot to open vkb 
     * @since S60 ?S60_version.
     */
    void slotbringvkb();

    void slotdataChanged();

signals:
    void dbChanged();

public:

    /**
     * Function returns the HbListWidget object 
     *  @param aUid Unique app Id.
     */
    HbView* View()
        {
        return mView;
        }

    /**
     * Function returns the HbListWidget object
     *  @param aUid Unique app Id.
     */
    HbListWidget* ListWidget()
        {
        return mListWidget;
        }

    /**
     * Function returns the HbSearchPanel object
     *  @param aUid Unique app Id.
     */
    HbSearchPanel* SearchPanel()
        {
        return mSearchPanel;
        }
    QList<SearchContentInfoDbData*> ContentInfoList()
        {
        return mContentInfoList;
        }
private:
    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    SearchUiLoader();
    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    ~SearchUiLoader();
private:

    /**
     * static instance to make single instance
     * Own.
     */
    static SearchUiLoader *m_instance;

    /**
     * counter for number for instance
     * Own.
     */
    static int m_instanceCounter;

    /**
     * Doucument loader API
     * Own.
     */
    HbDocumentLoader* mDocumentLoader;

    /**
     * Application view 
     * Own.
     */
    HbView* mView;

    /**
     * Listwidget to show the search results
     * Own.
     */
    HbListWidget* mListWidget;

    /**
     * searchpanel for query
     * Own.
     */
    HbSearchPanel* mSearchPanel;

    /**
     * to hide for task switcher
     * Own.
     */
    TsTaskSettings* mClient;

    /**
     * application main window
     */
    SearchMainWindow* mMainWindow;

    /**
     * vkbhost to resize the result screen 
     * Own.
     */
    HbShrinkingVkbHost* mVirtualKeyboard;

    /**
     * flag to validate fake exit
     * Own.
     */
    bool mBringtoForground;

    QList<SearchContentInfoDbData*> mContentInfoList;

    ContentInfoDbRead* mDb;
private:
    SEARCH_FRIEND_CLASS ( SearchStateProviderTest)
    };

class SearchMainWindow : public HbMainWindow
    {
Q_OBJECT
public:
    /**
     * Constructor.
     * @since S60 ?S60_version.
     */
    SearchMainWindow()
        {
        connect(this, SIGNAL(viewReady()), this, SLOT(slotViewReady()));
        }
    /**
     * Function capture the focusin event 
     *  @param event .
     */
    void focusInEvent(QFocusEvent * event)
        {
        Q_UNUSED(event);
        slotViewReady();
        }
public slots:
    /**
     * Slot to send the application to background fake exit
     * @since S60 ?S60_version.
     */
    void slotViewReady();
signals:
    /**
     * signal to send the notification for vkb
     * @since S60 ?S60_version.
     */
    void bringvkb();
    };
class SearchContentInfoDbData : public QObject
    {
Q_OBJECT
public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    SearchContentInfoDbData()
        {
        mSearchProgress = false;
        mSelected = false;
        }

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    ~SearchContentInfoDbData()
        {
        }
public:
    QString getCategoryName()
        {
        return mCategoryName;
        }
    QString getDisplayName()
        {
        return mDisplayName;
        }
    QString getDisplayIcon()
        {
        return mDisplayIcon;
        }
    QString getExceptionString()
        {
        return mExceptionString;
        }
    QString getBaseApp()
        {
        return mBaseApp;
        }
    QString getTranslationPath()
        {
        return mTranslationPath;
        }
    bool getActivityUri()
        {
        return mActivityUri;
        }
    int getDisplayOrder()
        {
        return mDisplayOrder;
        }
    bool getSearchProgress()
        {
        return mSearchProgress;
        }
    bool getSelected()
        {
        return mSelected;
        }

    void setCategoryName(QString aCategoryName)
        {
        mCategoryName = aCategoryName;
        }
    void setDisplayName(QString aDisplayName)
        {
        mDisplayName = aDisplayName;
        }
    void setDisplayIcon(QString aDisplayIcon)
        {
        mDisplayIcon = aDisplayIcon;
        }
    void setExceptionString(QString aExceptionString)
        {
        mExceptionString = aExceptionString;
        }
    void setBaseApp(QString aBaseApp)
        {
        mBaseApp = aBaseApp;
        }
    void setTranslationPath(QString aTranslationPath)
        {
        mTranslationPath = aTranslationPath;
        }
    void setActivityUri(QString aActivityUri)
        {
        mActivityUri = false;
        if (aActivityUri.length())
            {
            mActivityUri = true;
            }
        }
    void setDisplayOrder(int aDisplayOrder)
        {
        mDisplayOrder = aDisplayOrder;
        }
    void setSearchProgress(bool aSearchProgress)
        {
        mSearchProgress = aSearchProgress;
        }
    void setSelected(bool aSelected)
        {
        mSelected = aSelected;
        }
private:
    QString mCategoryName;
    QString mDisplayName;
    QString mDisplayIcon;
    QString mExceptionString;
    bool mActivityUri;
    QString mBaseApp;
    int mDisplayOrder;
    QString mTranslationPath;

    bool mSearchProgress;
    bool mSelected;
    };

#endif //SEARCH_CONTROLLER_H
