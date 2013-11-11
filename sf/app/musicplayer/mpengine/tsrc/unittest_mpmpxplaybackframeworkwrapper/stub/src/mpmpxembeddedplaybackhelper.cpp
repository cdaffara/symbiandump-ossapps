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
* Description: Embedded playback helper stub for testing mpmpxpbframeworkwrapper.
*
*/

#include <mpxplaybackutility.h>
#include "stub/inc/mpmpxembeddedplaybackhelper.h"
#include "mptrace.h"


/*!
 \internal
 */
CMpMpxEmbeddedPlaybackHelper* CMpMpxEmbeddedPlaybackHelper::NewL( 
    TUid aHostUid,
    MMPXPlaybackUtility* aPlaybackUtility,
    MMpMpxEmbeddedPlaybackHelperObserver* aObserver )
{
    CMpMpxEmbeddedPlaybackHelper* self =
            new ( ELeave ) CMpMpxEmbeddedPlaybackHelper( aHostUid, aPlaybackUtility, aObserver );
    return self;
}

/*!
 \internal
 Destructor
 */
CMpMpxEmbeddedPlaybackHelper::~CMpMpxEmbeddedPlaybackHelper()
{
}

/*!
 \internal
  */
void CMpMpxEmbeddedPlaybackHelper::playL( const TDesC& aFilename )
{
    TX_LOG
    iFileNameToPlay = QString::fromUtf16( aFilename.Ptr(), aFilename.Length() );
}


/*!
 \internal
 */
void CMpMpxEmbeddedPlaybackHelper::triggerError()
{
    iObserver->HandleEmbeddedPlaybackError( KErrGeneral );
}

/*!
 \internal
  */
CMpMpxEmbeddedPlaybackHelper::CMpMpxEmbeddedPlaybackHelper(
        TUid aHostUid,
        MMPXPlaybackUtility* aPlaybackUtility,
        MMpMpxEmbeddedPlaybackHelperObserver* aObserver )
    : iPlaybackUtility( aPlaybackUtility ),
      iObserver( aObserver ),
      iHostUid( aHostUid )
{
}

