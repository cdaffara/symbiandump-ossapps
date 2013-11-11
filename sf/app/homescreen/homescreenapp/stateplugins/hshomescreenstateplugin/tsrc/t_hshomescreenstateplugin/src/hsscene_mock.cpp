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
* Description:
*
*/

#include <HbInstance>
#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hsconfiguration.h"

HsScene::~HsScene()
{
    delete mWallpaper;
    qDeleteAll(mPages);
}

int HsScene::databaseId() const
{
    return mDatabaseId;
}

bool HsScene::load()
{
    int pageCount = property("pageCount").toInt();
    if ( pageCount <= 0 ){        
        pageCount = 1;
    }
    int defaultPageIndex = property("defaultPageIndex").toInt();

    mDatabaseId = 1; //Just one scene in the test
    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::SceneWallpaper) {
        mWallpaper = new HsSceneWallpaper(this);
    }
    
    for (int i = 0; i < pageCount; ++i) {
        HsPage *page = new HsPage;
        page->load();
        mPages << page;
    }

    if(defaultPageIndex > -1){
        mActivePage = mPages[defaultPageIndex];
        mActivePage->setRemovable(false);
    }

    return true;
}

HsWallpaper *HsScene::wallpaper() const
{
    return mWallpaper;
}

QList<HsPage *> HsScene::pages() const
{
    return mPages;
}

bool HsScene::addPage(HsPage *page)
{
    if (!page) {
        return false;
    }

    if (mPages.contains(page)) {
        return true;
    }

    mPages << page;
    return true;
}

bool HsScene::removePage(HsPage *page)
{
    if (!page) {
        return false;
    }
    int index = activePageIndex();
    mPages.removeOne(page);
    if(mPages.isEmpty()) {
        mActivePage = 0;
    }
    if(page == mActivePage){
        if(index >= mPages.count()){
            index--;
        }
        setActivePageIndex(index);
    }
    return true; 
}

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

bool HsScene::setActivePageIndex(int index)
{
    if (index < 0 || mPages.count() <= index) {
        return false;
    }
    return setActivePage(mPages[index]);
}

HsPage *HsScene::activePage() const
{
    return mActivePage;
}

int HsScene::activePageIndex() const
{
    return mPages.indexOf(mActivePage);
}

void HsScene::setActiveWidget(HsWidgetHost *widget)
{
    mActiveWidget = widget;
}

HsWidgetHost *HsScene::activeWidget() const
{
    return mActiveWidget;
}

void HsScene::setOnline(bool online)
{
    mIsOnline = online;
    foreach (HsPage *page, mPages) {
        page->setOnline(online);
    }
}

bool HsScene::isOnline()const
{
    return mIsOnline;
}

/*!
    Singleton.
*/
HsScene *HsScene::instance()
{
    if (!mInstance) {
        mInstance = new HsScene;
    }
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
    Listens for application background/foreground changes.
*/
bool HsScene::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
        case QEvent::ApplicationActivate:
            mActivePage->showWidgets();
            break;
		case QEvent::ApplicationDeactivate:
            mActivePage->hideWidgets();
            break;
        default:
            break;
	}
    return QObject::eventFilter(watched, event);
}

HsScene::HsScene(QObject *parent)
  : QObject(parent),
    mDatabaseId(-1),
    mWallpaper(0),
    mActivePage(0),
    mActiveWidget(0),
    mIsOnline(true)
{
}

/*!
    Points to the scene instance.
*/
HsScene *HsScene::mInstance = 0;
