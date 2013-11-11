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
 * Description: This widget displays the pixmap content.
 *
 */

#include "msgunieditorpixmapwidget.h"

// SYSTEM INCLUDES
#include <HbTapGesture>
#include <HbWidget>
#include <HbInstantFeedback>
#include <HbMenu>
#include <QPixmap>
#include <QTimer>

// USER INCLUDES
#include "msgunieditorutils.h"

// LOCAL CONSTANTS
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_REMOVE  hbTrId("txt_common_menu_remove")

const QString IMAGE_MIMETYPE("image");

//---------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::MsgUnifiedEditorPixmapWidget
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorPixmapWidget::MsgUnifiedEditorPixmapWidget(QGraphicsItem *parent) :
HbIconItem(parent),
mEditorUtils(0)
{
    this->grabGesture(Qt::TapGesture);
}

//---------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::~MsgUnifiedEditorPixmapWidget
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorPixmapWidget::~MsgUnifiedEditorPixmapWidget()
{
}

//---------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::setPixmap
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorPixmapWidget::populate(const QString &pixmapPath)
{
    mPixmapPath = pixmapPath;
    QPixmap pixmap(mPixmapPath);
    this->setIcon(HbIcon(pixmap));
}

//---------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::gestureEvent
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorPixmapWidget::gestureEvent(QGestureEvent *event)
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
// MsgUnifiedEditorPixmapWidget::handleOpen
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorPixmapWidget::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);
    
    if (!mEditorUtils) {
        mEditorUtils = new MsgUnifiedEditorUtils(this);
    }
    mEditorUtils->launchContentViewer(IMAGE_MIMETYPE, mPixmapPath);
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}

//---------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::handleSave
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorPixmapWidget::handleRemove()
{
    emit remove();
}

//----------------------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::handleShortTap
// @see header file
//----------------------------------------------------------------------------
void MsgUnifiedEditorPixmapWidget::handleShortTap()
{
    emit shortTap(mPixmapPath);

    // Open the media.
    handleOpen();
}

//---------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::handleLongTap
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorPixmapWidget::handleLongTap(const QPointF &position)
{
    emit longTap(position);

    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setDismissPolicy(HbPopup::TapAnywhere);
    
    menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
    menu->addAction(LOC_REMOVE, this, SLOT(handleRemove()));
    
    menu->setPreferredPos(position);
    menu->show();
}

//---------------------------------------------------------------
// MsgUnifiedEditorPixmapWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorPixmapWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
