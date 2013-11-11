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
 * Description: This widget displays the pixmap content in viewer.
 *
 */

#include "univiewerpixmapwidget.h"

// SYSTEM INCLUDES
#include <HbTapGesture>
#include <HbWidget>
#include <HbInstantFeedback>
#include <HbMenu>
#include <QPixmap>
#include <QTimer>
#include <thumbnailmanager_qt.h>

// USER INCLUDES
#include "msgservicelaunchutil.h"
#include "unidatamodelplugininterface.h"

// LOCAL CONSTANTS
#define LOC_OPEN    hbTrId("txt_common_menu_open")


static const char PIXMAP_ICON[] = "qtg_small_image";
static const char CORRUPTED_PIXMAP_ICON[] = "qtg_large_corrupted";
static const char VIDEO_MIMETYPE[] = "video";

static const int WIDTH_RATIO = 4;
static const int HEIGHT_RATIO = 3;

//---------------------------------------------------------------
// UniViewerPixmapWidget::UniViewerPixmapWidget
// @see header file
//---------------------------------------------------------------
UniViewerPixmapWidget::UniViewerPixmapWidget(QGraphicsItem *parent) :
    HbIconItem(parent), mInfo(0), mViewerUtils(0), mThumbnailManager(0)
{
    this->grabGesture(Qt::TapGesture);
    init();
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::~UniViewerPixmapWidget
// @see header file
//---------------------------------------------------------------
UniViewerPixmapWidget::~UniViewerPixmapWidget()
{
    if (mInfo) {
        delete mInfo;
        mInfo = NULL;
    }
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::populate
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::populate(UniMessageInfo *info)
{
    mMimeType = info->mimetype();
    mPixmapPath = info->path();

    /**
     * Create a copy of info for video content.
     * mInfo will be deleted in the destructor.
     */
    if (mMimeType.contains(VIDEO_MIMETYPE)) {
        mInfo = new UniMessageInfo(*info);
    }

    if (info->isProtected()) {
        if (mMimeType.contains(VIDEO_MIMETYPE)) {
            emit thumbnailFound(false, mInfo);
        }
        else {
            this->setIconName(PIXMAP_ICON);
        }
    }
    else if (info->isCorrupted()) {
        if (mMimeType.contains(VIDEO_MIMETYPE)) {
            emit thumbnailFound(false, mInfo);
        }
        else {
            this->setIconName(CORRUPTED_PIXMAP_ICON);
        }
    }
    else {
        if (mMimeType.contains(VIDEO_MIMETYPE)) {
            mThumbnailManager->getThumbnail(mPixmapPath);
            this->ungrabGesture(Qt::TapGesture);
        }
        else {
            QPixmap pixmap(mPixmapPath);
            this->setIcon(HbIcon(pixmap));
        }
    }
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::gestureEvent
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::gestureEvent(QGestureEvent *event)
{
    HbTapGesture *tapGesture = qobject_cast<HbTapGesture*> (event->gesture(Qt::TapGesture));
    if (tapGesture) {
        switch (tapGesture->state()) {
        case Qt::GestureStarted:
        {
            // Trigger haptic feedback.
            HbInstantFeedback::play(HbFeedback::Basic);
            break;
        }
        case Qt::GestureUpdated:
        {
            if (HbTapGesture::TapAndHold == tapGesture->tapStyleHint()) {
                // Handle longtap.
                handleLongTap(tapGesture->scenePosition());
            }
            break;
        }
        case Qt::GestureFinished:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            if (HbTapGesture::Tap == tapGesture->tapStyleHint()) {
                // Handle short tap
                handleShortTap();
            }
            break;
        }
        case Qt::GestureCanceled:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            break;
        }
        }
    }
    else {
        HbIconItem::gestureEvent(event);
    }
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::handleOpen
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);

    if (!mViewerUtils) {
        mViewerUtils = new MsgServiceLaunchUtil(this);
    }
    mViewerUtils->launchContentViewer(mMimeType, mPixmapPath);

    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}



//---------------------------------------------------------------
// UniViewerPixmapWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::thumbnailReady
// @see header
//---------------------------------------------------------------
void UniViewerPixmapWidget::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data)
    Q_UNUSED(id)
    this->grabGesture(Qt::TapGesture);
    if (error) {
        emit thumbnailFound(false, mInfo);
    }
    else {
        this->setIcon(HbIcon(pixmap));
        this->hide();
        emit thumbnailFound(true, NULL);
    }
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::init
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::init()
{
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::CropToAspectRatio);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(getThumbnailSize());

    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)), this,
        SLOT(thumbnailReady(QPixmap, void*, int, int)));
}

//----------------------------------------------------------------------------
// UniViewerPixmapWidget::handleShortTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerPixmapWidget::handleShortTap()
{
    emit shortTap(mPixmapPath);

    // Open the media.
    handleOpen();
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::handleLongTap
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::handleLongTap(const QPointF &position)
{
    emit longTap(position);

    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
    
    menu->setPreferredPos(position);
    menu->show();
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::getThumbnailSize
// @see header file
//---------------------------------------------------------------
QSize UniViewerPixmapWidget::getThumbnailSize()
{
    QSize thumbnailSize(1, 1);
    HbWidget *parent = qobject_cast<HbWidget *>(this->parentWidget());

    if (parent) {
        qreal thumbnailWidth = 0.0;
        qreal thumbnailHeight = 0.0;
        parent->style()->parameter("hb-param-screen-short-edge", thumbnailWidth);
        thumbnailHeight = (thumbnailWidth * HEIGHT_RATIO) / WIDTH_RATIO;
        thumbnailSize.setHeight(qRound(thumbnailHeight));
        thumbnailSize.setWidth(qRound(thumbnailWidth));
    }
    return thumbnailSize;
}

// EOF
