/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*           Messaging Audio info class
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>

#include <mdaaudiosampleplayer.h> // MMdaAudioPlayerCallback & CMdaAudioPlayerUtility

#include "MsgAudioInfo.h"
#include "MsgMediaInfo.h"
#include "MsgMedia.hrh"


// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgAudioInfo::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CMsgAudioInfo* CMsgAudioInfo::NewL( RFile& aFile, 
                                   TDataType& aMimeType )
    {
    CMsgAudioInfo* self = new ( ELeave ) CMsgAudioInfo( aMimeType );
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgAudioInfo::CMsgAudioInfo
//
// Constructor.
// ---------------------------------------------------------
//
CMsgAudioInfo::CMsgAudioInfo( TDataType& aMimeType ) :
    CMsgMediaInfo( aMimeType, EMsgMediaAudio )
    {
    }


// ---------------------------------------------------------
// CMsgAudioInfo::CMsgAudioInfo
//
// Destructor.
// ---------------------------------------------------------
//
CMsgAudioInfo::~CMsgAudioInfo()
    {
    delete iAudioUtil;
    }

// ---------------------------------------------------------
// CMsgAudioInfo::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgAudioInfo::ConstructL( RFile& aFile )
    {
    CMsgMediaInfo::ConstructL( aFile );
    }

// ---------------------------------------------------------
// CMsgAudioInfo::ResolveAudioInfoL
//
// ---------------------------------------------------------
//
void CMsgAudioInfo::ResolveAudioInfoL( RFile& aFile )
    {
    if ( !iAudioUtil )
        {
        iAudioUtil = CMdaAudioPlayerUtility::NewL( *this );
        }
    iAudioUtil->OpenFileL( aFile );
    }

// ---------------------------------------------------------
// Callbacks from CMdaAudioPlayerUtility
//
// ---------------------------------------------------------
//
void CMsgAudioInfo::MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration )
    {
    if ( !aError )
        {
        iDuration = I64INT( aDuration.Int64() / 1000 );
        iState = EAudioStateReady;
        iParsed = ETrue;
        }
    CompleteSelf( aError );
    }

// -----------------------------------------------------------------------------
// CMsgAudioInfo::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CMsgAudioInfo::DoCancel()
    {
    CMsgMediaInfo::DoCancel();
    delete iAudioUtil;
    iAudioUtil = NULL;
    iState = EAudioStateIdle;
    }
        
// -----------------------------------------------------------------------------
// CMsgAudioInfo::RunL
// 
// -----------------------------------------------------------------------------
//
void CMsgAudioInfo::RunL()
    {
    iParseError = iStatus.Int();
    if ( iParseError )
        {
        iState = EAudioStateReady;
        }
    switch ( iState )
        {
        case EAudioStateIdle:
            {
            FreezeRights();
            ResolveAudioInfoL( iFile );
            break;
            }
        case EAudioStateReady:
        default:
            {
            delete iAudioUtil;
            iAudioUtil = NULL;
            CompleteObserver();
            iState = EAudioStateIdle;
            break;
            }
        }
    }

// ---------------------------------------------------------
// Callbacks from CMdaAudioPlayerUtility
//
// ---------------------------------------------------------
//
void CMsgAudioInfo::MapcPlayComplete( TInt /*aError*/ )
    {
    // Empty
    }
