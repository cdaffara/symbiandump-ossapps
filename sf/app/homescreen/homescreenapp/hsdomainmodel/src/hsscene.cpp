/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <QApplication>

#include <HbInstance>
#include <HbMainWindow>
#include <hbevent.h>

#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hsdatabase.h"
#include "hswallpaper.h"
#include "hsconfiguration.h"


/*!
    \class HsScene
    \ingroup group_hsdomainmodel
    \brief Represents a scene in the framework.
    HsScene can have a wallpaper.
*/

/*!
    Destructor.
*/
HsScene::~HsScene()
{
    delete mWallpaper;
    qDeleteAll(mPages);
}

/*!
    Return database id.
*/
int HsScene::databaseId() const
{
    return mDatabaseId;
}

/*!
    Load scene from database.
*/
bool HsScene::load()
{
    HsDatabase *db = HsDatabase::instance();

    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        return false;
    }

    mDatabaseId = sceneData.id;

    QList<HsPageData> pageDatas;
    if (!db->pages(pageDatas) || pageDatas.empty()) {
        return false;
    }
    
    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::SceneWallpaper) {
        mWallpaper = new HsSceneWallpaper(this);
    }

    foreach (HsPageData pageData, pageDatas) {
        HsPage *page = new HsPage;
        page->setDatabaseId(pageData.id);
        if (page->load()) {
            mPages.append(page);
        } else {
            delete page;
            continue;
        }
        if (pageData.id == HSCONFIGURATION_GET(defaultPageId)) {
            mActivePage = page;
            mActivePage->setRemovable(false);
        }
    }
    
	return true;
}

/*!
    Return wallpaper.
*/
HsWallpaper *HsScene::wallpaper() const
{
    return mWallpaper;
}

/*!
   Return pages array.
*/
QList<HsPage *> HsScene::pages() const
{
    return mPages;
}

/*!
    Add page \a page.
*/
bool HsScene::addPage(HsPage *page)
{
    if (!page) {
        return false;
    }

    if (mPages.contains(page)) {
        return true;
    }

    HsDatabase *db = HsDatabase::instance();

    HsPageData data;
    data.id = page->databaseId();
    if (!db->page(data)) {
        return false;
    }

    int addPosition = data.indexPosition;

    db->transaction();

    for (int i = data.indexPosition; i < mPages.count(); ++i) {
        data.id = mPages.at(i)->databaseId();
        data.indexPosition = i + 1;
        if (!db->updatePage(data)) {
            db->rollback();
            return false;
        }
    }

    db->commit();

    mPages.insert(addPosition, page);
    return true;
}

/*!
    Removes page \a page.
*/
bool HsScene::removePage(HsPage *page)
{
    if (!page) {
        return false;
    }

    if (!mPages.contains(page)) {
        return true;
    }

    int nextIndex = mPages.indexOf(page) + 1;

    HsDatabase *db = HsDatabase::instance();
    db->transaction();
    // update page indexes
    HsPageData data;
    int pageCount(mPages.count());
    for (;nextIndex < pageCount; ++nextIndex) {
        data.id = mPages.at(nextIndex)->databaseId();
        data.indexPosition = nextIndex - 1;
        if (!db->updatePage(data)) {
            db->rollback();
            return false;
        }
    }
    // page will remove itself from database
    if (!page->deleteFromDatabase()) {
        db->rollback();
        return false;
    }

    db->commit();
    // update internal list
    int index = mPages.indexOf(page);
    bool lastPage(mPages.last() == page);
    mPages.removeOne(page);
    if (mPages.isEmpty()) {
        mActivePage = NULL;
    }else if (mActivePage == page) {
        if (lastPage) {
            index--;
        }
        setActivePageIndex(index);
    }
    
    return true;
}

/*!
    Set active page \a page.
*/
bool HsScene::setActivePage(HsPage *page)
{
    if (!page) {
        return false;
    }

    if (!mPages.contains(page)) {
        return false;
    }

    if (page == mActivePage) {
        return true;
    }

    mActivePage = page;
    emit activePageChanged();

    foreach (HsPage *p, mPages) {
        if (p == mActivePage) {
            p->showWidgets();
        } else {
            p->hideWidgets();
        }
    }

    return true;
}

/*!
    Set active page \a index.
*/
bool HsScene::setActivePageIndex(int index)
{
    if (index < 0 || mPages.count() <= index) {
        return false;
    }
    return setActivePage(mPages[index]);
}

/*!
    Return active page.
*/
HsPage *HsScene::activePage() const
{
    return mActivePage;
}

/*!
    Return active page index.
*/
int HsScene::activePageIndex() const
{
    return mPages.indexOf(mActivePage);
}

/*!
    Set active widget \a widget.
*/
void HsScene::setActiveWidget(HsWidgetHost *widget)
{
    mActiveWidget = widget;
}

/*!
   Return active widget.
*/
HsWidgetHost *HsScene::activeWidget() const
{
    return mActiveWidget;
}
/*!
    Toggle application online state. Defaults
    to true.
*/
void HsScene::setOnline(bool online)
{
    mIsOnline = online;
    foreach (HsPage *page, mPages) {
        page->setOnline(online);
    }
}

/*!
    Return current online setting.
*/
bool HsScene::isOnline()const
{
    return mIsOnline;
}

/*!
    Singleton.
*/
HsScene *HsScene::instance()
{
    return mInstance;
}

HsScene *HsScene::takeInstance()
{
    HsScene *instance = mInstance;
    mInstance = 0;
    return instance;
}
 
void HsScene::setInstance(HsScene *instance)
{
    if (mInstance != instance) {
        delete mInstance;
        mInstance = instance; 
    }    
}

/*!
    Return main window.
*/
/*HbMainWindow *HsGui::instance()
{
    return hbInstance->allMainWindows().first();
}
*/
/*!
    Return current orientation.
*/
/*Qt::Orientation HsGui::instance()->orientation()
{
    return mainWindow()->orientation();
}*/

/*!
    Listens for application background/foreground changes.
*/
bool HsScene::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ApplicationDeactivate && !mIsBackground) {
        qDebug() << "QEvent::ApplicationDeactivate: calling hide for active page widgets";
        mActivePage->hideWidgets(); 
        mIsBackground = true;
    } else if (event->type() == QEvent::ApplicationActivate && mIsBackground) {
        if(!mIsSleeping) {
            qDebug() << "QEvent::ApplicationActivate: not sleeping, calling show for active page widgets";
            mActivePage->showWidgets();
        }
        mIsBackground = false;
    } else if (event->type() == HbEvent::SleepModeEnter && !mIsSleeping) {
        qDebug() << "HbEvent::SleepModeEnter: calling hide for active page widgets";
        mActivePage->hideWidgets();
        mIsSleeping = true;
    } else if (event->type() == HbEvent::SleepModeExit && mIsSleeping) {
        if(!mIsBackground) {
           qDebug() << "HbEvent::SleepModeExit: in foreground, calling show for active page widgets";
           mActivePage->showWidgets();
        }
        mIsSleeping = false;
    }    

    return QObject::eventFilter(watched, event);
}

/*!
    Constructor
*/
HsScene::HsScene(QObject *parent)
  : QObject(parent),
    mDatabaseId(-1),
    mWallpaper(0),
    mActivePage(0),
    mActiveWidget(0),
    mIsOnline(true),
    mIsBackground(false),
    mIsSleeping(false)
{
}

/*!
    Points to the scene instance.
*/
HsScene *HsScene::mInstance = 0;
