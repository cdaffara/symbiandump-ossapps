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

#ifndef HSSCENE_H
#define HSSCENE_H

#include <QObject>
#include <QScopedPointer>
#include <QSizeF>
#include <QPointF>

#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class QGestureEvent;

class HbMainWindow;

class HsWallpaper;
class HsPage;
class HsWidgetHost;
class HsSceneData;

class HSDOMAINMODEL_EXPORT HsScene : public QObject
{
    Q_OBJECT

public:
    HsScene(QObject *parent = 0);
    ~HsScene();

    int databaseId() const;

    bool load();

    HsWallpaper *wallpaper() const;

    QList<HsPage *> pages() const;

    bool addPage(HsPage *page);
    bool removePage(HsPage *page);

    bool setActivePage(HsPage *page);
    bool setActivePageIndex(int index);
    HsPage *activePage() const;
    int activePageIndex() const;

    void setActiveWidget(HsWidgetHost *widget);
    HsWidgetHost *activeWidget() const;

    void setOnline(bool online = true);
    bool isOnline() const;

    static HsScene *instance();
    static HsScene *takeInstance();
    static void setInstance(HsScene *instance);
    //static HbMainWindow *mainWindow();
    //static Qt::Orientation orientation();

signals:
    void pageTapFinished(QGestureEvent *event);
    void pageTapAndHoldFinished(QGestureEvent *event);
    void pagePanStarted(QGestureEvent *event);
    void pagePanUpdated(QGestureEvent *event);
    void pagePanFinished(QGestureEvent *event);    
    void widgetTapStarted(QPointF, HsWidgetHost *widget);
    void widgetTapAndHoldFinished(QGestureEvent *event, HsWidgetHost *widget);
    void widgetMoveUpdated(const QPointF &scenePos, HsWidgetHost *widget);
    void widgetMoveFinished(const QPointF &scenePos, HsWidgetHost *widget);
    void activePageChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Q_DISABLE_COPY(HsScene)

private:
    int mDatabaseId;
    HsWallpaper *mWallpaper;
    QList<HsPage *> mPages;
    HsPage *mActivePage;
    HsWidgetHost *mActiveWidget;
    bool mIsOnline;
    bool mIsBackground;
    bool mIsSleeping;

    static HsScene *mInstance;

    friend class HsPage;
    friend class HsPageTouchArea;
    friend class HsWidgetHostVisual;
    friend class HsWidgetTouchArea;
    friend class HsWidgetMoveTouchArea;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSSCENE_H
