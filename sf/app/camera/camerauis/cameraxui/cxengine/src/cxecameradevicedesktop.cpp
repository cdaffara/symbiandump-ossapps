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

#include "cxecameradevicedesktop.h"

#include <QTimer>
#include <QDir>
#include "cxutils.h"


/*!
* Constructor
*/
CxeCameraDeviceDesktop::CxeCameraDeviceDesktop() :
        mSwitchPictureTimer(0),
        mPictureIndex(0)
{
    CX_DEBUG_ENTER_FUNCTION();
    loadPictures();
    setupTimer();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxeCameraDeviceDesktop::~CxeCameraDeviceDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();
    mSwitchPictureTimer->stop();
    delete mSwitchPictureTimer;
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Start the camera device
*/
void CxeCameraDeviceDesktop::start()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (!mSwitchPictureTimer->isActive()) {
        mSwitchPictureTimer->start();
    CX_DEBUG_EXIT_FUNCTION();
    }
}

/*!
* Stop the camera device
*/
void CxeCameraDeviceDesktop::stop()
{
    CX_DEBUG_ENTER_FUNCTION();
    mSwitchPictureTimer->stop();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get current picture
* @ Return current picture
*/
const QPixmap &CxeCameraDeviceDesktop::currentSnaphot()
{
    return mPictureList.at(mPictureIndex);
}

/*!
* Handle timeout
*/
void CxeCameraDeviceDesktop::handleTimeout()
{
    CX_DEBUG_ENTER_FUNCTION();
    mPictureIndex++;

    if (mPictureIndex >= mPictureList.count()) {
        mPictureIndex = 0;
    }
    
    emit imageChanged(mPictureList.at(mPictureIndex));
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Load pictures
*/
void CxeCameraDeviceDesktop::loadPictures()
{
    CX_DEBUG_ENTER_FUNCTION();
    QDir currentDir(".","*.jpg");
    CX_DEBUG(("Searching images from %s", qPrintable(currentDir.absolutePath())));
    CX_DEBUG(("Number of images found %d", currentDir.count()));

    if (!currentDir.count()) {
        mPictureList.append(QPixmap(360, 640));
        return;
    }

    foreach (QString entry, currentDir.entryList()){
        mPictureList.append(QPixmap(entry).scaledToHeight(360));
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Create timer
*/
void CxeCameraDeviceDesktop::setupTimer()
{
    CX_DEBUG_ENTER_FUNCTION();
    mSwitchPictureTimer = new QTimer(this);
    mSwitchPictureTimer->setInterval(500);
    mSwitchPictureTimer->setSingleShot(false);
    connect(mSwitchPictureTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    CX_DEBUG_EXIT_FUNCTION();
}
