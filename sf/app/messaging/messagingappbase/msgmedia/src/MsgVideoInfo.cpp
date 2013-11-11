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
* Description: MsgVideoInfo implementation
*
*/




// INCLUDE FILES
#include <videoplayer.h>
#include <eikenv.h>

#include <hxmetadatakeys.h>
#include <hxmetadatautil.h>

#include "MsgVideoInfo.h"
#include "MsgMediaInfo.h"
#include "MsgMedia.hrh"
#include "MsgMimeTypes.h"

#ifdef __MSGMEDIA_DEBUG
#include "MsgMediaLogging.h"
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgVideoInfo::NewL
// -----------------------------------------------------------------------------
EXPORT_C CMsgVideoInfo * CMsgVideoInfo::NewL( RFile& aFile, 
                                             TDataType& aMimeType )
    {
    CMsgVideoInfo* self = new ( ELeave ) CMsgVideoInfo( aMimeType );
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgVideoInfo::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgVideoInfo::ConstructL( RFile& aFile )
    {
    CMsgMediaInfo::ConstructL( aFile );
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CMsgVideoInfo::DoCancel()
    {
    CMsgMediaInfo::DoCancel();
    iState = EVideoStateIdle;
    }
        
// -----------------------------------------------------------------------------
// CMsgVideoInfo::RunL
// 
// -----------------------------------------------------------------------------
//
void CMsgVideoInfo::RunL()
    {
    iParseError = iStatus.Int();
    if ( iParseError )
        {
        iState = EVideoStateReady;
        }
    switch ( iState )
        {
        case EVideoStateIdle:
            {
            FreezeRights();
            ResolveVideoInfoL( iFile );
            break;
            }
        case EVideoStateReady:
        default:
            {
            CompleteObserver();
            iState = EVideoStateIdle;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::ResolveVideoInfoL
// -----------------------------------------------------------------------------
void CMsgVideoInfo::ResolveVideoInfoL( RFile& aFile )
    {

    CHXMetaDataUtility* dataUtil = CHXMetaDataUtility::NewL();
    
    CleanupStack::PushL( dataUtil );

    dataUtil->OpenFileL( aFile );

    ReadInformationL( *dataUtil );
    
    CleanupStack::PopAndDestroy( dataUtil );
    
    iState = EVideoStateReady;
    iParsed = ETrue;
    
    CompleteSelf( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::CMsgVideoInfo
// -----------------------------------------------------------------------------
CMsgVideoInfo::CMsgVideoInfo(  TDataType& aMimeType ):
    CMsgMediaInfo( aMimeType, EMsgMediaVideo ),
    iBitRate( 0 ),
    iAudioBitRate( 0 ),
    iOverallSizeInPixels( 0, 0 ),
    iFramesPerSecond( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::Duration
// -----------------------------------------------------------------------------
CMsgVideoInfo::~CMsgVideoInfo()
    {
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::ReadInformationL
// -----------------------------------------------------------------------------
void CMsgVideoInfo::ReadInformationL( CHXMetaDataUtility& dataUtil )
    {
    TUint count = 0;
    dataUtil.GetMetaDataCount( count );
    HXMetaDataKeys::EHXMetaDataId id;
    HBufC* des = NULL;       
    for( TInt i = 0; i<count; i++ )
        {
        dataUtil.GetMetaDataAt( i, id, des );
        // don’t delete des - it is not owned by the client
        switch ( id )
            {
            case HXMetaDataKeys::EHXDuration:
                {
                TLex input ( des->Des() );
                input.Val( iDuration );
                break;
                }
            case HXMetaDataKeys::EHXClipBitRate:
            case HXMetaDataKeys::EHXVideoBitRate:
                {
                TLex input ( des->Des() );
                input.Val( iBitRate );
                break;
                }
            case HXMetaDataKeys::EHXFrameSize:
                {
                const TChar separator = 'x';// as in e.g."177x144"
                TInt separatorLocation = des->Locate( separator );
                TLex input (des->Left( separatorLocation ));

                input.Val( iOverallSizeInPixels.iWidth );
                input = des->Right(des->Length() - separatorLocation - 1);
                input.Val( iOverallSizeInPixels.iHeight );
                break;
                }
            case HXMetaDataKeys::EHXFramesPerSecond:
                {
                TLex input ( des->Des() );
                input.Val( iFramesPerSecond );
                break;
                }
            case HXMetaDataKeys::EHXAudioBitRate:
                {
                TLex input ( des->Des() );
                input.Val( iAudioBitRate );
                break;
                }
            default:
                break;
            }
        }
    }


//  End of File
