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
*           Messaging Text info class
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>
#include <f32file.h>

#include <msgtextutils.h>

#include "MsgTextInfo.h"
#include "MsgMediaInfo.h"


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
// CMsgTextInfo::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CMsgTextInfo* CMsgTextInfo::NewL(
        RFile& aFile, 
        TDataType& aMimeType,
        RFs& aFs )
    {
    CMsgTextInfo* self = new ( ELeave ) CMsgTextInfo( aMimeType, aFs );
    CleanupStack::PushL( self );
    if ( aFile.SubSessionHandle() != KNullHandle )
        {
        self->ConstructL( aFile );
        }
    else
        {
        self->ConstructEmpty();
        }
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgTextInfo::CMsgTextInfo
//
// Constructor.
// ---------------------------------------------------------
//
CMsgTextInfo::CMsgTextInfo( TDataType& aMimeType, RFs& aFs ) :
        CMsgMediaInfo( aMimeType, EMsgMediaText ),
        iFs( aFs )
    {
    }


// ---------------------------------------------------------
// CMsgTextInfo::CMsgTextInfo
//
// Destructor.
// ---------------------------------------------------------
//
CMsgTextInfo::~CMsgTextInfo()
    {
    }

// ---------------------------------------------------------
// CMsgTextInfo::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgTextInfo::ConstructL( RFile& aFile )
    {
    CMsgMediaInfo::ConstructL( aFile );
    }

// ---------------------------------------------------------
// CMsgTextInfo::ConstructEmpty
// ---------------------------------------------------------
//
void CMsgTextInfo::ConstructEmpty()
    {
    iMediaType = EMsgMediaText;
    iParsed = ETrue;
    }

// -----------------------------------------------------------------------------
// CMsgTextInfo::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CMsgTextInfo::DoCancel()
    {
    CMsgMediaInfo::DoCancel();
    }
        
// -----------------------------------------------------------------------------
// CMsgTextInfo::RunL
// 
// -----------------------------------------------------------------------------
//
void CMsgTextInfo::RunL()
    {
    iParseError = iStatus.Int();
    if ( !iParseError )
        {
        if ( !iCharSetRecognized )
            {
            iCharSet = CMsgTextUtils::RecognizeCharSetL( iFs, iFile );    
            iCharSetRecognized = ETrue;
            }
        iParsed = ETrue;
        }
    CompleteObserver();
    }

//  End of File  
