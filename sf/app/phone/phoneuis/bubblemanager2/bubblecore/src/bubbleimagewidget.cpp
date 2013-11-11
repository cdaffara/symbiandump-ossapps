/*!
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
* Description:  Widget to display contact image.
*               Displays default avatar, when image is not set.
*
*/

#include <QPainter>
#include <QGraphicsSceneResizeEvent>
#include <QDebug>
#include <hbstyle.h>
#include <hbiconitem.h>
#include <hbstyleloader.h>
#include <hbdeviceprofile.h>
#include "bubbleimagemanager.h"
#include "bubbleimagewidget.h"

static const qreal BubbbleImageRounding = 1.5; // units

BubbleImageWidget::BubbleImageWidget(
    BubbleImageManager& imageManager,
    QGraphicsItem* parent)
    : HbWidget(parent),
      mImageManager(imageManager),
      mDefaultAvatar(0),
      mKeepSquareShape(false)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    // create avatar
    mDefaultAvatar = new HbIconItem(this);
    style()->setItemName( mDefaultAvatar, "default_avatar" );
    mDefaultAvatar->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
    mDefaultAvatar->setAlignment(Qt::AlignCenter);

    connect(&mImageManager, SIGNAL(pixmapReady(QString)),
            this, SLOT(imageLoaded(QString)));

    HbStyleLoader::registerFilePath(
        QLatin1String(":/bubbleimagewidget.css"));
    HbStyleLoader::registerFilePath(
        QLatin1String(":/bubbleimagewidget.widgetml"));
}

BubbleImageWidget::~BubbleImageWidget()
{
}

void BubbleImageWidget::setImageName(const QString& name)
{
    mImageName = name;

    if (!mImageName.isEmpty()) {
        mDefaultAvatar->setVisible(false);
        mDefaultAvatar->setIcon(HbIcon());
        mImageManager.loadImage(mImageName);
    } else {
        mDefaultAvatar->setIconName(QLatin1String("qtg_large_avatar"));
        mDefaultAvatar->setVisible(true);
    }
}

QString BubbleImageWidget::imageName() const
{
    if (!mImageName.isEmpty()) {
        return mImageName;
    } else {
        return mDefaultAvatar->iconName();
    }
}

void BubbleImageWidget::imageLoaded(QString imageFileName)
{
    if (imageFileName==mImageName) {
        update(); // does nothing if invisible
    }
}

void BubbleImageWidget::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (mImageName.isEmpty())
        return;

    const QPixmap* pixmap =
        mImageManager.pixmap(mImageName);

    if (pixmap) {
        painter->save();

        // up or downscales images to fill image area
        QSize imageSize(pixmap->size());
        QSize sourceSize(rect().size().toSize());
        sourceSize.scale(imageSize,Qt::KeepAspectRatio);
        QRect sourceRect(QPoint(0,0),sourceSize);
        int xOff = (imageSize.width() - sourceSize.width())/2;
        int yOff = (imageSize.height() - sourceSize.height())/2;
        sourceRect.moveLeft(xOff);
        sourceRect.moveTop(yOff);
        // qDebug() << "imageSize:" << imageSize;
        // qDebug() << "sourceRect:" << sourceRect;
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->setRenderHint(QPainter::Antialiasing);

        QPainterPath clip;
        qreal rounding = BubbbleImageRounding *
                         HbDeviceProfile::profile(this).unitValue();
        clip.addRoundedRect(rect(), rounding, rounding);
        painter->setClipPath(clip);

#ifdef __WINS__
        // sourceRect crashes emulator, RC ou1cimx1#320113
        painter->drawPixmap(rect().toRect(),*pixmap);
#else        
        painter->drawPixmap(rect().toRect(),*pixmap,sourceRect);
#endif        
        painter->restore();
    }
}

QSizeF BubbleImageWidget::sizeHint(
    Qt::SizeHint which,
    const QSizeF & constraint) const
{
    Q_UNUSED(which);
    Q_UNUSED(constraint);

    if (mKeepSquareShape && which==Qt::PreferredSize ) {
        QSizeF hint(size().height(), size().height());
        if (hint.isNull()) {
            hint = QSizeF(100,100);
        }

        // qDebug() << "size hint: " << hint;
        return hint;
    } else {
        return HbWidget::sizeHint(which,constraint);
    }
}

void BubbleImageWidget::resizeEvent(QGraphicsSceneResizeEvent * event)
{
    // qDebug() << "size:" << event->newSize();
    if ( event->newSize().height()!= event->oldSize().height() &&
         event->newSize().height() > 1 ) {
        updateGeometry();
    }

    HbWidget::resizeEvent(event);
}

bool BubbleImageWidget::keepSquareShape() const
{
    return mKeepSquareShape;
}

void BubbleImageWidget::setKeepSquareShape(bool keepSquare)
{
    mKeepSquareShape = keepSquare;

    if (mKeepSquareShape) {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    } else {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    updateGeometry();
}

void BubbleImageWidget::displayEmergencyCallIcon()
{
    // this method overrides default avatar set in setImageName()
    if (mImageName.isEmpty()) {
        mDefaultAvatar->setIconName(
            QLatin1String("qtg_large_emergency_call"));
    }
}

