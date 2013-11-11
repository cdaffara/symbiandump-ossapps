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

#include <QDir>
#include <QFile>
#include <QVariantHash>
#include <QGraphicsLinearLayout>

#include <HbMainWindow>
#include <HbIconItem>

#include "hswallpaper.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswallpaperloader.h"
#include "hsconfiguration.h"
#include "hsgui.h"


/*!
    \class HsWallpaper
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!

*/
HsWallpaper::HsWallpaper(QGraphicsItem *parent)
  : HbWidget(parent),
    mId(-1),
    mIsDefaultImage(false),
    mIconItem(0),
    mLoader(0)
{
    setFlag(ItemHasNoContents);
    setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

    mLoader = new HsWallpaperLoader(this);
    connect(mLoader, SIGNAL(finished()), SLOT(onLoaderFinished()), Qt::UniqueConnection);
    connect(mLoader, SIGNAL(failed()), SLOT(onLoaderFailed()), Qt::UniqueConnection);

    mIconItem = new HbIconItem();
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(mIconItem);
    setLayout(layout);

    connect(HsGui::instance(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(updateIconItem(Qt::Orientation)));
}

/*!

*/
HsWallpaper::~HsWallpaper()
{
}

/*!

*/
void HsWallpaper::setImage(const QString &path)
{
    if (!path.isEmpty()) {
        if(mLoader->isRunning()) {
            mLoader->cancel();
        }
    
        mLoader->setSourcePath(path);
        mLoader->setTargets(createTargets(path));
        mLoader->start();
    } else {
        emit imageSetFailed();
    }
}

/*!

*/
void HsWallpaper::setImages(const QString &portraitFileName, const QString &landscapeFileName)
{
    if (portraitFileName.isEmpty() || 
        landscapeFileName.isEmpty()) {
        emit imageSetFailed();
        return;
    }
    
    // Qt doesn't support writing GIFs, so let's save those 
    // and everything else but JPGs as PNGs
    QString suffix("png");
    if (QFileInfo(portraitFileName).suffix().toUpper() == "JPG" ) {
        suffix = "jpg";
    }
    QString portraitTargetFileName = wallpaperDirectory()
        + QString("%1_portrait.").arg(mId) + suffix;
    QString landscapeTargetFileName = wallpaperDirectory()
        + QString("%1_landscape.").arg(mId) + suffix;
    
    if (mIsDefaultImage) {
        mIsDefaultImage = false;
    } else {
        QFile::remove(mPortraitImagePath);
        QFile::remove(mLandscapeImagePath);
    }
    
    QFile::copy(portraitFileName, portraitTargetFileName);
    QFile::copy(landscapeFileName, landscapeTargetFileName);
    QFile::remove(portraitFileName);
    QFile::remove(landscapeFileName);

    setExistingImage();
    emit imageSet();
}

/*!

*/
void HsWallpaper::setDefaultImage()
{
    if (mIsDefaultImage) {
        return;
    }

    QDir dir(wallpaperDirectory());
    QStringList filters;
    filters << "default_portrait.*";
    filters << "default_landscape.*";
    QFileInfoList infos = dir.entryInfoList(
        filters, QDir::Files, QDir::Name | QDir::IgnoreCase);

    Q_ASSERT(infos.count() == 2);

    mIsDefaultImage = true;
    mLandscapeImagePath = infos.first().absoluteFilePath();
    mPortraitImagePath = infos.last().absoluteFilePath();

    updateIconItem(HsGui::instance()->orientation());
}

/*!

*/
void HsWallpaper::remove()
{
    if (mIsDefaultImage) {
        mIsDefaultImage = false;
    } else {
        QFile::remove(mPortraitImagePath);
        QFile::remove(mLandscapeImagePath);
    }
    mPortraitImagePath.clear();
    mLandscapeImagePath.clear();
}

/*!

*/
bool HsWallpaper::setExistingImage()
{
    QDir dir(wallpaperDirectory());
    QStringList filters;
    filters << QString("%1_portrait.*").arg(mId);
    filters << QString("%1_landscape.*").arg(mId);
    QFileInfoList infos = dir.entryInfoList(
        filters, QDir::Files, QDir::Name | QDir::IgnoreCase);

    Q_ASSERT(infos.isEmpty() || infos.count() == 2);

    if (infos.isEmpty()) {
        return false;
    }

    mIsDefaultImage = false;
    mLandscapeImagePath = infos.first().absoluteFilePath();
    mPortraitImagePath = infos.last().absoluteFilePath();

    updateIconItem(HsGui::instance()->orientation());
    return true;
}

QString HsWallpaper::rootDirectory() const
{
#ifdef Q_OS_SYMBIAN
    QString directory("c:/private/20022f35/wallpapers/");
#else
    QString directory(QDir::currentPath() + "/private/20022f35/wallpapers/");
#endif

#ifdef HSDOMAINMODEL_TEST
    directory = QDir::currentPath() + "/wallpapers/";
#endif
    return directory;
}

/*!

*/
void HsWallpaper::onLoaderFinished()
{
    if (mIsDefaultImage) {
        mIsDefaultImage = false;
    } else {
        QFile::remove(mPortraitImagePath);
        QFile::remove(mLandscapeImagePath);
    }
    foreach (QString path, mLoader->targets().keys()) {
        QString newName = path; newName.remove("temp");
        QFile::rename(path, newName);
    }
    setExistingImage();
    emit imageSet();
}

/*!

*/
void HsWallpaper::onLoaderFailed()
{
    foreach (QString path, mLoader->targets().keys()) {
        QFile::remove(path);
    }
    emit imageSetFailed();
}

/*!

*/
void HsWallpaper::updateIconItem(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) {
        mIconItem->setIcon(HbIcon(QIcon(mPortraitImagePath)));
    } else {
        mIconItem->setIcon(HbIcon(QIcon(mLandscapeImagePath)));
    }
}

/*!

*/
HsSceneWallpaper::HsSceneWallpaper(HsScene *scene, QGraphicsItem *parent)
  : HsWallpaper(parent),
    mScene(0)
{
    setScene(scene);
}

/*!

*/
HsSceneWallpaper::~HsSceneWallpaper()
{
}

/*!

*/
void HsSceneWallpaper::setScene(HsScene *scene)
{
    if (!scene) {
        return;
    }

    mScene = scene;
    mId = scene->databaseId();
    if (!setExistingImage()) {
        setDefaultImage();
    }
}

/*!

*/
QString HsSceneWallpaper::wallpaperDirectory() const
{
    return QDir::toNativeSeparators(rootDirectory() + "scene/");
}

/*!

*/
QVariantHash HsSceneWallpaper::createTargets(const QString &sourcePath)
{
    QVariantHash targets;

    QString targetSuffix("png");

    // Qt doesn't support writing GIFs, so let's save those and everything else but JPGs as PNGs
    if (QFileInfo(sourcePath).suffix().toUpper() == "JPG" ) {
        targetSuffix = QString("jpg");
    }

    QString path = wallpaperDirectory()
                   + QString("temp%1_").arg(mId)
                   + QString("%1.")
                   + targetSuffix;

    targets.insert(path.arg("portrait"), QSize((2 * 360) + HSCONFIGURATION_GET(bounceEffect), 640));
    targets.insert(path.arg("landscape"), QSize((2 * 640) + HSCONFIGURATION_GET(bounceEffect), 360));
    return targets;
}

/*!
 PAGE
*/
HsPageWallpaper::HsPageWallpaper(HsPage *page, QGraphicsItem *parent)
  : HsWallpaper(parent),
    mPage(0)
{
    setPage(page);
}

/*!

*/
HsPageWallpaper::~HsPageWallpaper()
{
}

/*!

*/
void HsPageWallpaper::setPage(HsPage *page)
{
    if (!page) {
        return;
    }

    mPage = page;
    mId = page->databaseId();
    if (!setExistingImage()) {
        setDefaultImage();
    }
}

/*!

*/
QString HsPageWallpaper::wallpaperDirectory() const
{
    return QDir::toNativeSeparators(rootDirectory() + "page/");
}

/*!

*/
QVariantHash HsPageWallpaper::createTargets(const QString &sourcePath)
{
    QVariantHash targets;

    QString targetSuffix("png");

    // Qt doesn't support writing GIFs, so let's save those and everything else but JPGs as PNGs
    if (QFileInfo(sourcePath).suffix().toUpper() == "JPG" ) {
        targetSuffix = QString("jpg");
    }

    QString path = wallpaperDirectory()
                   + QString("temp%1_").arg(mId)
                   + QString("%1.")
                   + targetSuffix;

    targets.insert(path.arg("portrait"), QSize(360, 640));
    targets.insert(path.arg("landscape"), QSize(640, 360));
    return targets;
}

