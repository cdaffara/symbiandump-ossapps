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
 * Description: This widget is used to display audio content in univiewer.
 *
 */
#include "univieweraudiowidget.h"

// SYSTEM INCLUDES
#include <QFileInfo>
#include <QTimer>
#include <HbMenu>

// USER INCLUDES
#include "msgmediautil.h"
#include "msgservicelaunchutil.h"
#include "unidatamodelplugininterface.h"

// LOCAL CONSTANTS
#define LOC_OPEN    hbTrId("txt_common_menu_open")


static const char VIDEO_MIMETYPE[] = "video";
static const char AUDIO_ICON[] = "qtg_mono_audio";
static const char VIDEO_ICON[] = "qtg_mono_video";
static const char CORRUPTED_AUDIO_ICON[] = "qtg_mono_corrupted";

//----------------------------------------------------------------------------
// UniViewerAudioWidget::UniViewerAudioWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAudioWidget::UniViewerAudioWidget(QGraphicsItem *parent) :
    HbPushButton(parent), mViewerUtils(0), mValidMediaDuration(true)
{
    connect(this, SIGNAL(clicked()), this, SLOT(handleShortTap()));
    connect(this, SIGNAL(longPress(QPointF)), this, SLOT(handleLongTap(QPointF)));
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::~UniViewerAudioWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAudioWidget::~UniViewerAudioWidget()
{
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::~UniViewerAudioWidget
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::populate(UniMessageInfo *info)
{
    mMimeType = info->mimetype();
    mMediaPath = info->path();

    HbIcon audioIcon;

    if (info->isProtected()) {
        if (mMimeType.contains(VIDEO_MIMETYPE)) {
            audioIcon.setIconName(VIDEO_ICON);
        }
        else {
            audioIcon.setIconName(AUDIO_ICON);
        }
    }
    else if (info->isCorrupted()) {
        audioIcon.setIconName(CORRUPTED_AUDIO_ICON);
    }
    else {
        if (mMimeType.contains(VIDEO_MIMETYPE)) {
            audioIcon.setIconName(VIDEO_ICON);
        }
        else {
            audioIcon.setIconName(AUDIO_ICON);
        }
    }

    this->setIcon(audioIcon);
    QFileInfo fileInfo(mMediaPath);
    this->setText(fileInfo.baseName());
    this->setTextAlignment(Qt::AlignLeft);
    MsgMediaUtil mediaUtil;
    QString mediaDuration(mediaUtil.mediaDuration(mMediaPath));
    if (mediaDuration.isEmpty()) {
        mValidMediaDuration = false;
    }
    else {
        mValidMediaDuration = true;
        this->setAdditionalText(mediaDuration);
    }
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::setStretched
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::setStretched(bool stretched)
{
    if (mValidMediaDuration) {
        HbPushButton::setStretched(stretched);
    }
    else {
        HbPushButton::setStretched(true);
    }
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::handleShortTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::handleShortTap()
{
    emit shortTap(mMediaPath);

    // Open the media.
    handleOpen();
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::handleLongTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::handleLongTap(const QPointF &position)
{
    emit longTap(position);

    // Display context sensitive menu.
    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
    menu->setPreferredPos(position);
    menu->show();
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::handleOpen
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);
    
    if (!mViewerUtils) {
        mViewerUtils = new MsgServiceLaunchUtil(this);
    }
    mViewerUtils->launchContentViewer(mMimeType, mMediaPath);
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}



//---------------------------------------------------------------
// UniViewerAudioWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void UniViewerAudioWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
