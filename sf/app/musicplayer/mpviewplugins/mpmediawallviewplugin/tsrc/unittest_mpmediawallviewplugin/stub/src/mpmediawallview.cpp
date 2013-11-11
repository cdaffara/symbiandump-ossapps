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
* Description: Music Player Settings view Stub.
*
*/

#include "stub/inc/mpmediawallview.h"


MpMediaWallView::MpMediaWallView() :
    mInitializeViewCount(0),
    mActivateViewCount(0),
    mDeactivateViewCount(0)
{
}
    
/*!
 Destructs the media wall view.
 */
MpMediaWallView::~MpMediaWallView()
{
}

/*!
 Initializes the media wall view.
 */
void MpMediaWallView::initializeView()
{
    mInitializeViewCount++;
}

/*!
 Activates the media wall view.
 */
void MpMediaWallView::activateView()
{
    mActivateViewCount++;
}

/*!
 Deactivates the media wall view.
 */
void MpMediaWallView::deactivateView()
{
    mDeactivateViewCount++;
}

/*!
 Resets counters
 */
void MpMediaWallView::resetCounters()
{
    mInitializeViewCount = 0;
    mActivateViewCount = 0;
    mDeactivateViewCount = 0;
}

