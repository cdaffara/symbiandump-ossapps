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

#include <QVariantHash>
#include <QGraphicsLinearLayout>

#include <HbIconItem>

#include "hswallpaper.h"
#include "hsscene.h"
#include "hspage.h"

/*!

*/

/*!

*/
HsWallpaper::HsWallpaper(QGraphicsItem *parent)
  : HbWidget(parent),
    mIsDefaultImage(false),
    mIconItem(0)
{
    setFlag(ItemHasNoContents);
    setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

    mIconItem = new HbIconItem();
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(mIconItem);
    setLayout(layout);
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
        emit imageSet();
    } else {
        emit imageSetFailed();
    }
}

void HsWallpaper::setImages(const QString &portraitFileName, const QString &landscapeFileName)
{
    Q_UNUSED(portraitFileName)
    Q_UNUSED(landscapeFileName)
    emit imageSet();
}

/*!

*/
void HsWallpaper::setDefaultImage()
{
}

/*!

*/
void HsWallpaper::remove()
{
    mPortraitImagePath.clear();
    mLandscapeImagePath.clear();
}

/*!

*/
bool HsWallpaper::setExistingImage()
{
    return true;
}

QString HsWallpaper::rootDirectory() const
{
    return QString();
}

/*!

*/
void HsWallpaper::onLoaderFinished()
{
    emit imageSet();
}

/*!

*/
void HsWallpaper::onLoaderFailed()
{
    emit imageSetFailed();
}

/*!

*/
void HsWallpaper::updateIconItem(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
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
    Q_UNUSED(scene);
}

/*!

*/
QString HsSceneWallpaper::wallpaperDirectory() const
{
    return QString();
}

/*!

*/
QVariantHash HsSceneWallpaper::createTargets(const QString &sourcePath)
{
    Q_UNUSED(sourcePath);
    return QVariantHash();
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
}

/*!

*/
QString HsPageWallpaper::wallpaperDirectory() const
{
    return QString();
}

/*!

*/
QVariantHash HsPageWallpaper::createTargets(const QString &sourcePath)
{
    Q_UNUSED(sourcePath);
    return QVariantHash();
}

