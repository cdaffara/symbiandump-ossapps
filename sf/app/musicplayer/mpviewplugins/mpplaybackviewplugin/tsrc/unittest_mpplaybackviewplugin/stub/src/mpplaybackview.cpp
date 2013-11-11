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
* Description: MpPlaybackView stub for testing MpPlaybackViewplugin
*
*/

#include "stub/inc/mpplaybackview.h"


/*!
 Constructs the MpPlaybackView stub
 */
MpPlaybackView::MpPlaybackView()
    :mInitializeViewCount(0),
    mActivateViewCount(0),
    mDeactivateViewCount (0)
{
}

/*!
 Destructs the MpPlaybackView stub
 */
MpPlaybackView::~MpPlaybackView()
{
}
    
/*!
 Initializes the MpPlaybackView stub
 */
void MpPlaybackView::initializeView()
{
    mInitializeViewCount++;
}

/*!
 Activates the MpPlaybackView stub
 */
void MpPlaybackView::activateView()
{
    mActivateViewCount++;
}

/*!
 Deactivates the MpPlaybackView stub
 */
void MpPlaybackView::deactivateView()
{
    mDeactivateViewCount++;
}

void MpPlaybackView::resetCounters()
{
    mInitializeViewCount = 0;
    mActivateViewCount = 0;
    mDeactivateViewCount = 0;
}

/*!
 emits the signal command
 */
void MpPlaybackView::emitCommand(int cmd)
{
    emit command(cmd);
}
