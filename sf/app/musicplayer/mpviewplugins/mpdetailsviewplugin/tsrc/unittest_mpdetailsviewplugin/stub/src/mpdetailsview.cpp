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
* Description: MpDetailsView stub for testing MpDetailsViewplugin
*
*/

#include "stub/inc/mpdetailsview.h"


/*!
 Constructs the MpDetailsView stub
 */
MpDetailsView::MpDetailsView()
    :mInitializeViewCount( 0 ),
    mActivateViewCount( 0 ),
    mDeactivateViewCount( 0 )
{
}

/*!
 Destructs the MpDetailsView stub
 */
MpDetailsView::~MpDetailsView()
{
}
    
/*!
 Initializes the MpDetailsView stub
 */
void MpDetailsView::initializeView()
{
    mInitializeViewCount++;
}

/*!
 Activates the MpDetailsView stub
 */
void MpDetailsView::activateView()
{
    mActivateViewCount++;
}

/*!
 Deactivates the MpDetailsView stub
 */
void MpDetailsView::deactivateView()
{
    mDeactivateViewCount++;
}

/*!
 Reset counters
 */
void MpDetailsView::resetCounters()
{
    mInitializeViewCount = 0;
    mActivateViewCount = 0;
    mDeactivateViewCount = 0;
}

/*!
 emits the signal command
 */
void MpDetailsView::emitCommand( int cmd )
{
    emit command( cmd );
}
