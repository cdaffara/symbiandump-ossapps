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

// INCLUDES
#include "debugtraces.h"
#include <HbTextItem>
#include <HbIconItem>
#include <QFileInfo>
#include <QFont>
#include <HbFrameDrawer>
#include <HbMenu>
#include <MsgMimeTypes.h>
#include <HbFrameItem>
#include <HbInstantFeedback>
#include <HbTapGesture>
#include <QGraphicsSceneMouseEvent>
#include <HbWidgetFeedback>
#include <HbEffect>
#include <QTimer>

// USER INCLUDES
#include "msgcontactsutil.h"
#include "msgunieditorattachment.h"
#include "UniEditorGenUtils.h"
#include <xqconversions.h>
#include "msgunieditorutils.h"

// Constants
#define BYTES_TO_KBYTES_FACTOR 1024
#define BG_FRAME "qtg_fr_groupbox"

//Localized Constants for item specific menu
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_REMOVE  hbTrId("txt_common_menu_remove")

const QString LIST_ITEM_BG_FRAME_NORMAL ("qtg_fr_list_normal");
const QString LIST_ITEM_BG_FRAME_PRESSED("qtg_fr_list_pressed");

const QString ATTACHMENT_ICON("qtg_small_attachment");

MsgUnifiedEditorAttachment::MsgUnifiedEditorAttachment( const QString& attachmentpath,
                                                        const int filesize,
                                                        QGraphicsItem *parent ) :
HbWidget(parent),
mPath(attachmentpath),
mSize(filesize),
mAttachmentIcon(0),
mAttachmentName(0),
mMaxSmsSize(KFirstNormalSmsLength),
mEditorUtils(0)
{
    this->grabGesture(Qt::TapGesture);
    setProperty("state", "normal");
    
    //back ground
    mBackGround = new HbFrameItem(this);
    mBackGround->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_NORMAL);
    mBackGround->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    this->setBackgroundItem(mBackGround);        

    mAttachmentIcon = new HbIconItem(ATTACHMENT_ICON, this);
    HbStyle::setItemName(mAttachmentIcon,"attachmentIcon");

    int at_size = 0;
    TMsgMediaType mediaType = EMsgMediaUnknown;
    
            
    UniEditorGenUtils* genUtils = q_check_ptr(new UniEditorGenUtils);
    
    TRAP_IGNORE(genUtils->getFileInfoL(mPath,at_size,
        mMimeType,mediaType));
    TRAP_IGNORE(mMaxSmsSize = genUtils->MaxSmsMsgSizeL()); 
    delete genUtils;
    QFileInfo fileinfo(attachmentpath);
    QString filename = fileinfo.fileName();
    mAttachmentName = new HbTextItem(filename,this);
    HbStyle::setItemName(mAttachmentName,"attachmentName");
    mAttachmentName->setElideMode(Qt::ElideRight);

    // for sms, pure size should be shown
    // for mms, additional mimeheader size must be included
    qreal displaySize = mSize;
    if(!isMultimediaContent())
    {
        displaySize = fileinfo.size();
    }
    int sizeInKb = displaySize/BYTES_TO_KBYTES_FACTOR;
    QString fileDetails;
    // if size exceeds 1kb, then show kb or else only bytes
    if(sizeInKb >= 1)
    {
        fileDetails = QString().append(QString("(%1 Kb)").arg(sizeInKb));
    }
    else
    {
        fileDetails = QString().append(QString("(%1 B)").arg(displaySize));
    }

    mAttachmentDetails = new HbTextItem(fileDetails, this);
    HbStyle::setItemName(mAttachmentDetails,"attachmentDetails");
    mAttachmentDetails->setElideMode(Qt::ElideNone);
    
    HbEffect::add("attachmentWidget", "listviewitem_press", "pressed");
    HbEffect::add("attachmentWidget", "listviewitem_release", "released");
}

MsgUnifiedEditorAttachment::~MsgUnifiedEditorAttachment()
{
}

const QString& MsgUnifiedEditorAttachment::path()
{
    return mPath;
}

qreal MsgUnifiedEditorAttachment::size()
{
    return mSize;
}

const QString& MsgUnifiedEditorAttachment::mimeType()
{
    return mMimeType;
}

void MsgUnifiedEditorAttachment::handleLongTap(QPointF position)
{
    HbMenu* menu = new HbMenu;
    
    menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
    menu->addAction(LOC_REMOVE, this, SLOT(handleRemove()));

    menu->setDismissPolicy(HbPopup::TapAnywhere);
    menu->setAttribute(Qt::WA_DeleteOnClose, true);
    menu->setPreferredPos(position);
    menu->show();
}

void MsgUnifiedEditorAttachment::handleRemove()
{
    emit deleteMe(this);
}

void MsgUnifiedEditorAttachment::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);
    
    //open corresponding viewer app

    if (!mEditorUtils) 
    {
        mEditorUtils = new MsgUnifiedEditorUtils(this);
    }
    mEditorUtils->launchContentViewer(mMimeType, mPath);
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}

bool MsgUnifiedEditorAttachment::isMultimediaContent()
{
    bool ret = true;
    QString vcard = XQConversions::s60Desc8ToQString(KMsgMimeVCard());
    QString vcal = XQConversions::s60Desc8ToQString(KMsgMimeVCal());
    QString ical = XQConversions::s60Desc8ToQString(KMsgMimeICal());
    if( !QString::compare(mMimeType, vcard, Qt::CaseInsensitive) ||
        !QString::compare(mMimeType, vcal, Qt::CaseInsensitive) ||
        !QString::compare(mMimeType, ical, Qt::CaseInsensitive) )
    {
        QFileInfo fileinfo(mPath);
        int fSize = fileinfo.size();
        
        // if filesize is within sms size-limit, then
        // it is not mm content, else it is mm attachment
        if(fSize <= mMaxSmsSize)
        {
            ret = false;
        }
    }
    return ret;
}

void MsgUnifiedEditorAttachment::gestureEvent(QGestureEvent *event)
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

void MsgUnifiedEditorAttachment::handleShortTap()
{
    handleOpen();
}

void MsgUnifiedEditorAttachment::setPressed(bool pressed)
{
    if (pressed) 
    {
        setProperty("state", "pressed");
        mBackGround->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_PRESSED);
        HbEffect::cancel(mBackGround, "released");
        HbEffect::start(mBackGround, "attachmentWidget", "pressed");

    }
    else 
    {
        setProperty("state", "normal");
        mBackGround->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_NORMAL);
        HbEffect::cancel(mBackGround, "pressed");
        HbEffect::start(mBackGround, "attachmentWidget", "released");
    }    
}

void MsgUnifiedEditorAttachment::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
