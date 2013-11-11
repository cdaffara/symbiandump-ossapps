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
*           Messaging Image Info class.
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>

#include <imageconversion.h> // CImageDecoder
#include <icl/imagedata.h>   // TFrameInfo

#include "MsgImageInfo.h"
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
// CMsgImageInfo::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CMsgImageInfo* CMsgImageInfo::NewL( RFile& aFile, TDataType& aMimeType )
    {
    CMsgImageInfo* self = new ( ELeave ) CMsgImageInfo( aMimeType );
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgImageInfo::CMsgImageInfo
//
// Constructor.
// ---------------------------------------------------------
//
CMsgImageInfo::CMsgImageInfo( TDataType& aMimeType ) :
    CMsgMediaInfo( aMimeType, EMsgMediaImage ),
    iDelay( 0 ),
    iFrames( 0 ),
    iBitsPerPixel( 0 ),
    iOverallSizeInPixels( 0, 0 ),
    iFrameFlags( 0 )
    {
    }


// ---------------------------------------------------------
// CMsgImageInfo::CMsgImageInfo
//
// Destructor.
// ---------------------------------------------------------
//
CMsgImageInfo::~CMsgImageInfo()
    {
    }

// ---------------------------------------------------------
// CMsgImageInfo::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgImageInfo::ConstructL( RFile& aFile )
    {
    CMsgMediaInfo::ConstructL( aFile );
    }

// -----------------------------------------------------------------------------
// CMsgImageInfo::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CMsgImageInfo::DoCancel()
    {
    CMsgMediaInfo::DoCancel();
    }
        
// -----------------------------------------------------------------------------
// CMsgImageInfo::RunL
// 
// -----------------------------------------------------------------------------
//
void CMsgImageInfo::RunL()
    {
    iParseError = iStatus.Int();
    if ( !iParseError )
        {
        FreezeRights();
        ResolveImageInfoL( iFile );
        iParsed = ETrue;
        }
    CompleteObserver();
    }

// ---------------------------------------------------------
// CMsgImageInfo::ResolveImageInfoL
//
// ---------------------------------------------------------
//
void CMsgImageInfo::ResolveImageInfoL( RFile& aFile )
    {
    CImageDecoder* imageDecoder
        = CImageDecoder::FileNewL( aFile, ContentAccess::EPeek );
    
    // If we survive from this we have legal imagefile. Thus no need to further checking.
    
    // Information of first (or only) frame
    TFrameInfo frameInfo = imageDecoder->FrameInfo();

    iFrames = imageDecoder->FrameCount();

    iBitsPerPixel = frameInfo.iBitsPerPixel;
    iOverallSizeInPixels = frameInfo.iOverallSizeInPixels;

    //iFrameFlags = frameInfo.iFlags;
    if ( frameInfo.iFlags & TFrameInfo::EFullyScaleable )
        {
        iFrameFlags |= EFullyScalable;
        }
    if ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible )
        {
        iFrameFlags |= ETransparent;
        }

    if ( iFrames > 1 )
        {
        iDelay = I64INT( frameInfo.iDelay.Int64() / 1000 );
        for ( TInt frame = 0;  frame < iFrames ; frame++ )
            {
            frameInfo = imageDecoder->FrameInfo( frame );
            TInt delay = I64INT( frameInfo.iDelay.Int64() / 1000 );
            iDuration += delay;
            }
        }
    else
        {
        iDelay = 0;
        iDuration = 0;
        }
    delete imageDecoder;
    }
