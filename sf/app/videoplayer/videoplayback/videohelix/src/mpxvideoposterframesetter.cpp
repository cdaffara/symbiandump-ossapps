/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: poster frame setter
 *
*/

// Version : %version: ou1cpsw#2 %


// INCLUDE FILES
#include <fbs.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideoposterframesetter.h"
#include "mpxvideoplaybackcontroller.h"
#include "mpxvideo_debug.h"

// ============================ MEMBER FUNCTIONS ===================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::CMPXVideoPosterFrameSetter
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPosterFrameSetter::CMPXVideoPosterFrameSetter( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
    : iVideoPlaybackCtlr( aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPosterFrameSetter::CMPXVideoPosterFrameSetter()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPosterFrameSetter::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPosterFrameSetter::ConstructL()"));
    
    iTNManager = CThumbnailManager::NewL( *this );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::NewL
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPosterFrameSetter* CMPXVideoPosterFrameSetter::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPosterFrameSetter::NewL()"));

    CMPXVideoPosterFrameSetter* self = new( ELeave ) CMPXVideoPosterFrameSetter( aVideoPlaybackCtlr );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::~CMPXVideoPosterFrameSetter
// Destructor
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPosterFrameSetter::~CMPXVideoPosterFrameSetter()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPosterFrameSetter::~CMPXVideoPosterFrameSetter()"));

    if ( iTNManager )
    {
        delete iTNManager;
        iTNManager = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::RequestPosterFrameL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPosterFrameSetter::RequestPosterFrame()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPosterFrameSetter::RequestPosterFrame()"));
    
    if ( ! iVideoPlaybackCtlr->iFileDetails->iDrmProtected )
    {
        TRAP_IGNORE( iVideoPlaybackCtlr->iPlayer->GetFrameL() );    
    }
    else
    {
        MPX_TRAPD(err, 
                  iVideoPlaybackCtlr->iState->SendErrorToViewL(KMPXVideoSetPosterFrameNotSupported));     
    }   
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::HandlePosterFrameReady
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPosterFrameSetter::HandlePosterFrameReady(TInt aError)
{
    MPX_ENTER_EXIT(_L("CMPXVideoPosterFrameSetter::HandlePosterFrameReady()"));
    
    if ( aError == KErrNone )
    {     
        CThumbnailObjectSource* tnSource( 0 );
        TRAP_IGNORE
        (
            tnSource = CThumbnailObjectSource::NewL( &iVideoPlaybackCtlr->iPlayer->GetBitmap(),
                                                      iVideoPlaybackCtlr->iClipName->Des() );
            
            CleanupStack::PushL( tnSource );
            iRequestId = iTNManager->SetThumbnailL( *tnSource );
            CleanupStack::PopAndDestroy( tnSource );
        );    
    }        
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::ThumbnailPreviewReady
//
// callback method from MThumbnailManagerObserver
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPosterFrameSetter::ThumbnailPreviewReady( MThumbnailData& /*aThumbnail*/,
                                                        TThumbnailRequestId /*aId*/ )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPosterFrameSetter::ThumbnailPreviewReady()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPosterFrameSetter::ThumbnailReady
//
// callback method from MThumbnailManagerObserver
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPosterFrameSetter::ThumbnailReady( TInt aError, 
                                                 MThumbnailData& /*aThumbnail*/,
                                                 TThumbnailRequestId /*aId*/ )
{
    MPX_DEBUG(_L("CMPXVideoPosterFrameSetter::ThumbnailReady() aError = %d"), aError );
    
    if ( aError == KErrNone )
    {
        MPX_TRAPD( err, 
                   iVideoPlaybackCtlr->iState->SendErrorToViewL( KMPXVideoSetPosterFrameSuccess ) );    
    }
    else
    {
        MPX_TRAPD( err, 
                   iVideoPlaybackCtlr->iState->SendErrorToViewL( KMPXVideoSetPosterFrameFailure ) );    
    }
}

//  End of File
