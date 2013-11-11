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
 * Description: Widget for displaying attachment media objects.
 *
 */

#include "univiewerattachmentwidget.h"

// SYSTEM INCLUDES
#include <HbIconItem>
#include <HbTextItem>
#include <HbFrameItem>
#include <HbMenu>
#include <HbEffect>
#include <HbTapGesture>
#include <HbInstantFeedback>

#include <QFileInfo>
#include <QTimer>

// USER INCLUDES
#include "msgservicelaunchutil.h"
#include "unidatamodelplugininterface.h"

// LOCAL CONSTANTS
#define LOC_OPEN hbTrId("txt_common_menu_open")
#define LOC_SAVE_TO_CONTACTS hbTrId("txt_messaging_menu_save_to_contacts")

const QString BG_FRAME_NORMAL("qtg_fr_list_normal");
const QString BG_FRAME_PRESSED("qtg_fr_list_pressed");
const QString ATTACHMENT_ICON("qtg_small_attachment");
const QString CORRUPTED_ATTACH_ICON("qtg_small_corrupted");
const QString VCARD_MIMETYPE("text/X-vCard");

const int KILOBYTE = 1024;

//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::UniViewerAttachmentWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAttachmentWidget::UniViewerAttachmentWidget(QGraphicsItem *parent) :
    HbWidget(parent), mMediaIcon(0), mName(0), mInfo(0), mFrameItem(0), mViewerUtils(0)
{
    this->grabGesture(Qt::TapGesture);

    setProperty("state", "normal");

    mMediaIcon = new HbIconItem(this);
    HbStyle::setItemName(mMediaIcon, "mediaIcon");

    mName = new HbTextItem(this);
    HbStyle::setItemName(mName, "text-1");

    mInfo = new HbTextItem(this);
    HbStyle::setItemName(mInfo, "text-2");

    mFrameItem = new HbFrameItem(BG_FRAME_NORMAL, HbFrameDrawer::NinePieces, this);
    HbStyle::setItemName(mFrameItem, "bgFrame");

    HbEffect::add("attachmentWidget", "listviewitem_press", "pressed");
    HbEffect::add("attachmentWidget", "listviewitem_release", "released");
}

//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::~UniViewerAttachmentWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAttachmentWidget::~UniViewerAttachmentWidget()
{
}

//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerAttachmentWidget::populate(UniMessageInfo *info)
{
    mMimeType = info->mimetype();
    mMediaPath = info->path();

    QString attachIcon;
    if (info->isProtected()) {
        attachIcon = ATTACHMENT_ICON;
    }
    else if (info->isCorrupted()) {
        attachIcon = CORRUPTED_ATTACH_ICON;
    }
    else {
        attachIcon = ATTACHMENT_ICON;
    }

    mMediaIcon->setIconName(attachIcon);
    QFileInfo fileInfo(mMediaPath);
    mName->setText(fileInfo.fileName());

    QString sizeString('B');
    int fileSize = fileInfo.size();
    if (fileSize > KILOBYTE) {
        // Convert to Kilobytes.
        fileSize /= KILOBYTE;
        sizeString = "Kb";
    }
    QString fileDetails = "(" + QString::number(fileSize) + sizeString + ")";
    mInfo->setText(fileDetails);
}

//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::resizeEvent
// @see header file
//----------------------------------------------------------------------------
void UniViewerAttachmentWidget::gestureEvent(QGestureEvent *event)
{
    HbTapGesture *tapGesture = qobject_cast<HbTapGesture*> (event->gesture(Qt::TapGesture));
    if (tapGesture) {
        switch (tapGesture->state()) {
        case Qt::GestureStarted:
        {
            // Trigger haptic feedback.
            HbInstantFeedback::play(HbFeedback::Basic);
            setPressed(true);
            break;
        }
        case Qt::GestureUpdated:
        {
            if (HbTapGesture::TapAndHold == tapGesture->tapStyleHint()) {
                // Handle longtap.
                setPressed(false);
                handleLongTap(tapGesture->scenePosition());
            }
            break;
        }
        case Qt::GestureFinished:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            if (HbTapGesture::Tap == tapGesture->tapStyleHint()) {
                // Handle short tap.
                setPressed(false);
                handleShortTap();
            }
            break;
        }
        case Qt::GestureCanceled:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            setPressed(false);
            break;
        }
        }
    }
    else {
        HbWidget::gestureEvent(event);
    }
}

//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::handleOpen
// @see header file
//----------------------------------------------------------------------------
void UniViewerAttachmentWidget::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);
    
    if (!mViewerUtils) {
        mViewerUtils = new MsgServiceLaunchUtil(this);
    }
    mViewerUtils->launchContentViewer(mMimeType, mMediaPath);
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}



//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::handleShortTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerAttachmentWidget::handleShortTap()
{
    emit shortTap(mMediaPath);

    // Open the media.
    handleOpen();
}

//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::handleLongTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerAttachmentWidget::handleLongTap(const QPointF &position)
{
    emit longTap(position);

    // Display context sensitive menu.
    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setPreferredPos(position);

    if (mMimeType.contains(VCARD_MIMETYPE, Qt::CaseInsensitive)) {
        // For vcard opening & saving is same.
        menu->addAction(LOC_SAVE_TO_CONTACTS, this, SLOT(handleOpen()));
    }
    else {
        menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
        
    }

    menu->show();
}

//----------------------------------------------------------------------------
// UniViewerAttachmentWidget::setPressed
// @see header file
//----------------------------------------------------------------------------
void UniViewerAttachmentWidget::setPressed(bool pressed)
{
    if (pressed) {
        setProperty("state", "pressed");
        mFrameItem->frameDrawer().setFrameGraphicsName(BG_FRAME_PRESSED);
        HbEffect::cancel(mFrameItem, "released");
        HbEffect::start(mFrameItem, "attachmentWidget", "pressed");

    }
    else {
        setProperty("state", "normal");
        mFrameItem->frameDrawer().setFrameGraphicsName(BG_FRAME_NORMAL);
        HbEffect::cancel(mFrameItem, "pressed");
        HbEffect::start(mFrameItem, "attachmentWidget", "released");
    }
}

//---------------------------------------------------------------
// UniViewerAttachmentWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void UniViewerAttachmentWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
