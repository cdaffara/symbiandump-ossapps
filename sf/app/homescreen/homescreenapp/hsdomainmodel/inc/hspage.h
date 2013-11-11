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

#ifndef HSPAGE_H
#define HSPAGE_H

#include <QPointF>
#include <HbWidget>
#include <HbIconItem>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

class HsWidgetHost;
class HsPageData;
class HsWallpaper;
class HsPageTouchArea;
class HsPageVisual;

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsPage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int pageIndex READ pageIndex)

public:
    HsPage(QObject *parent = 0);
    ~HsPage();
    int databaseId() const;
    void setDatabaseId(int id);
   // void setGeometry(const QRectF &rect);
    bool load();
    HsWallpaper *wallpaper() const;
    bool addExistingWidget(HsWidgetHost *widgetHost);
    bool removeWidget(HsWidgetHost *widgeHost);
    QList<HsWidgetHost *> newWidgets();
    bool addNewWidget(HsWidgetHost *widgetHost, const QPointF &position = QPointF());
    void layoutNewWidgets();
    void resetNewWidgets();
    bool deleteFromDatabase();
    QList<HsWidgetHost *> widgets() const;
    bool isRemovable() const;
    void setRemovable(bool removable);
    bool isDefaultPage() const;
    bool isActivePage() const;
    static HsPage *createInstance(const HsPageData &pageData);
    QPointF adjustedWidgetPosition(const QRectF &origWidgetRect);
    QRectF contentGeometry();
    QRectF contentGeometry(Qt::Orientation orientation);
    QRectF contentRect();
    QRectF contentRect(Qt::Orientation orientation);
    HsPageVisual *visual() const;

public slots:
    void showWidgets();
    void hideWidgets();
    void setOnline(bool online = true);
    void updateZValues();
    int pageIndex();

private:
    Q_DISABLE_COPY(HsPage)
  //  void setupTouchArea();
    void connectWidget(HsWidgetHost *widget);
    void disconnectWidget(HsWidgetHost *widget);

private slots:
    void onWidgetFinished();
    void onWidgetFaulted();
    void onWidgetResized();
    void onWidgetAvailable();
    void onWidgetUnavailable();
    void onOrientationChanged(Qt::Orientation orientation);
    void onPageMarginChanged(const QString &value);

private:
    int mDatabaseId;
    HsPageVisual *mPageVisual;
    HsWallpaper *mWallpaper;
    bool mRemovable;
    QList<HsWidgetHost*> mWidgets;
    QList<HsWidgetHost*> mNewWidgets;
    QList<HsWidgetHost*> mUnavailableWidgets;
    //HsPageTouchArea *mTouchArea;
    QPointF mTouchPoint;    
    qreal mPageMargin;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif
