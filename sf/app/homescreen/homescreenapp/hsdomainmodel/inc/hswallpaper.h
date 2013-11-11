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

#ifndef HSWALLPAPER_H
#define HSWALLPAPER_H

#include <HbWidget>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

class HbIconItem;
class HsWallpaperLoader;

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsWallpaper : public HbWidget
{
    Q_OBJECT

public:
    HsWallpaper(QGraphicsItem *parent = 0);
    ~HsWallpaper();

signals:
    void imageSet();
    void imageSetFailed();

public slots:
    void setImage(const QString &path);
    void setImages(const QString &portraitFileName, const QString &landscapeFileName);
    void setDefaultImage();
    void remove();

protected:
    virtual QString wallpaperDirectory() const = 0;
    virtual QVariantHash createTargets(const QString &sourcePath) = 0;

    bool setExistingImage();
    QString rootDirectory() const;

private:
    Q_DISABLE_COPY(HsWallpaper)

private slots:
    void onLoaderFinished();
    void onLoaderFailed();
    void updateIconItem(Qt::Orientation orientation);

protected:
    int mId;

private:
    bool mIsDefaultImage;
    QString mPortraitImagePath;
    QString mLandscapeImagePath;
    HbIconItem *mIconItem;
    HsWallpaperLoader *mLoader;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

class HsScene;
class HSDOMAINMODEL_EXPORT HsSceneWallpaper : public HsWallpaper
{
    Q_OBJECT

public:
    HsSceneWallpaper(HsScene *scene, QGraphicsItem *parent = 0);
    ~HsSceneWallpaper();

    HsScene *scene() const { return mScene; }
    void setScene(HsScene *scene);

protected:
    QString wallpaperDirectory() const;
    QVariantHash createTargets(const QString &sourcePath);

private:
    Q_DISABLE_COPY(HsSceneWallpaper)

private:
    HsScene *mScene;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

class HsPage;
class HSDOMAINMODEL_EXPORT HsPageWallpaper : public HsWallpaper
{
    Q_OBJECT

public:
    HsPageWallpaper(HsPage *page, QGraphicsItem *parent = 0);
    ~HsPageWallpaper();
    
    HsPage *page() const { return mPage; }
    void setPage(HsPage *page);

protected:
    QString wallpaperDirectory() const;
    QVariantHash createTargets(const QString &sourcePath);

private:
    Q_DISABLE_COPY(HsPageWallpaper)

private:
    HsPage *mPage;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWALLPAPER_H
