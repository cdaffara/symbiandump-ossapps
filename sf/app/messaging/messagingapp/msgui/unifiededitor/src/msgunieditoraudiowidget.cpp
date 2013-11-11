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
#include "msgunieditoraudiowidget.h"

// SYSTEM INCLUDES
#include <QFileInfo>
#include <QTimer>
#include <HbMenu>

// USER INCLUDES
#include "msgmediautil.h"
#include "msgunieditorutils.h"

// LOCAL CONSTANTS
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_REMOVE  hbTrId("txt_common_menu_remove")

const QString AUDIO_ICON("qtg_mono_audio");
const QString AUDIO_MIMETYPE("audio");

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::MsgUniFiedEditorAudioWidget
// @see header file
//----------------------------------------------------------------------------
MsgUniFiedEditorAudioWidget::MsgUniFiedEditorAudioWidget(QGraphicsItem *parent) :
HbPushButton(parent), mEditorUtils(0), mValidMediaDuration(true)
{
    connect(this, SIGNAL(clicked()), this, SLOT(handleShortTap()));
    connect(this, SIGNAL(longPress(QPointF)), this, SLOT(handleLongTap(QPointF)));
}

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::~MsgUniFiedEditorAudioWidget
// @see header file
//----------------------------------------------------------------------------
MsgUniFiedEditorAudioWidget::~MsgUniFiedEditorAudioWidget()
{
}

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::~MsgUniFiedEditorAudioWidget
// @see header file
//----------------------------------------------------------------------------
void MsgUniFiedEditorAudioWidget::populate(const QString &filePath)
{
    mMediaPath = filePath;

    this->setIcon(HbIcon(AUDIO_ICON));
    QFileInfo fileInfo(mMediaPath);
    this->setText(fileInfo.baseName());    
    MsgMediaUtil mediaUtil;
    QString mediaDuration(mediaUtil.mediaDuration(mMediaPath));
    if (mediaDuration.isEmpty()) {
        mValidMediaDuration = false;
    }
    else {
        mValidMediaDuration = true;
        this->setAdditionalText(mediaDuration);
    }
    this->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
}

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::setStretched
// @see header file
//----------------------------------------------------------------------------
void MsgUniFiedEditorAudioWidget::setStretched(bool stretched)
{
    if (mValidMediaDuration) {
        HbPushButton::setStretched(stretched);
    }
    else {
        HbPushButton::setStretched(true);
    }
}

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::handleShortTap
// @see header file
//----------------------------------------------------------------------------
void MsgUniFiedEditorAudioWidget::handleShortTap()
{
    emit shortTap(mMediaPath);

    // Open the media.
    handleOpen();
}

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::handleLongTap
// @see header file
//----------------------------------------------------------------------------
void MsgUniFiedEditorAudioWidget::handleLongTap(const QPointF &position)
{
    emit longTap(position);

    // Display context sensitive menu.
    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setDismissPolicy(HbPopup::TapAnywhere);
    
    menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
    menu->addAction(LOC_REMOVE, this, SLOT(handleRemove()));
    menu->setPreferredPos(position);
    menu->show();
}

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::handleOpen
// @see header file
//----------------------------------------------------------------------------
void MsgUniFiedEditorAudioWidget::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);
    
    if (!mEditorUtils) 
    {
        mEditorUtils = new MsgUnifiedEditorUtils(this);
    }
    mEditorUtils->launchContentViewer(AUDIO_MIMETYPE, mMediaPath);
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}

//----------------------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::handleRemove
// @see header file
//----------------------------------------------------------------------------
void MsgUniFiedEditorAudioWidget::handleRemove()
{
    emit remove();
}

//---------------------------------------------------------------
// MsgUniFiedEditorAudioWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void MsgUniFiedEditorAudioWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
