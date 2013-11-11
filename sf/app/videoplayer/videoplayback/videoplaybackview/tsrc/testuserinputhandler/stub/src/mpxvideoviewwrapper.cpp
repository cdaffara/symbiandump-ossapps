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
* Description:   Implementation of Video base playback view
*
*/

// Version : %version: 5 %



//  Include Files

#include "mpxvideoviewwrapper.h"
#include "videobaseplaybackview.h"
#include "videoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"

//  Member Functions

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::CMPXVideoViewWrapper()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper::CMPXVideoViewWrapper( VideoBasePlaybackView* aView )
    : iView( aView )    
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::NewL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper* CMPXVideoViewWrapper::NewL( VideoBasePlaybackView* aView )
{
    CMPXVideoViewWrapper* self = new (ELeave) CMPXVideoViewWrapper( aView );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::ConstructL()
{
    iMediaRequested = EFalse;        
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::~CMPXVideoViewWrapper()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper::~CMPXVideoViewWrapper()
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::IsLive()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsLive()
{
    return EFalse;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::IsPlaylist()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsPlaylist()
{
    return EFalse;    
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::CreateGeneralPlaybackCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::CreateGeneralPlaybackCommandL( int aCmd )
{
    Q_UNUSED( aCmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleCommandL( TInt aCommand )
{
    switch ( aCommand )
    {
        case EMPXPbvCmdNextListItem:
        {
            SetFileDetails( EFalse );            
            break;    
        }
        
        case EMPXPbvCmdPreviousListItem:
        {
            SetFileDetails( ETrue );  
            break;    
        }
    }

}

// -------------------------------------------------------------------------------------------------
// Request for the media object
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::RequestMediaL()
{
    iMediaRequested = ETrue;
    
    SetFileDetails( ETrue );
    
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::ActivateClosePlayerActiveObject
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::ActivateClosePlayerActiveObject()
{
    iClosePlayerAO = ETrue;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::IsMultiItemPlaylist()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsMultiItemPlaylist()
{
    return iFileDetails->mMultiItemPlaylist;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::SetFileDetails()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::SetFileDetails(TBool aDefault)
{
    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }
    
    iFileDetails = new VideoPlaybackViewFileDetails(); 
    
    
    _LIT(KTestMimeType, "video/3gp");        
    const QString qMimeType( (QChar*)KTestMimeType().Ptr(), KTestMimeType().Length() );
    iFileDetails->mMimeType = qMimeType;   
    
    
    _LIT(KTestTitle, "Test Video Title");
    const QString qTitle( (QChar*)KTestTitle().Ptr(), KTestTitle().Length() );
    iFileDetails->mTitle = qTitle;    
    
    
    _LIT(KTestArtist, "TestArtist");
    const QString qArtist( (QChar*)KTestArtist().Ptr(), KTestArtist().Length() );
    iFileDetails->mArtist = qArtist;
    
    if ( aDefault )
    {
        _LIT(KTestClipName, "testClip.3gp");
        const QString qClipname( (QChar*)KTestClipName().Ptr(), KTestClipName().Length() );
        iFileDetails->mClipName = qClipname;        
    }
    else
    {
        _LIT(KTestClipName, "nextClip.3gp");
        const QString qClipname( (QChar*)KTestClipName().Ptr(), KTestClipName().Length() );
        iFileDetails->mClipName = qClipname;        
    }
    
    
    iFileDetails->mPlaybackMode = EMPXVideoLocal;
    iFileDetails->mSeekable = true;
    iFileDetails->mPausableStream = true;
    iFileDetails->mAudioEnabled = true;
    iFileDetails->mVideoEnabled = true;
    iFileDetails->mPartialPlayback = false;
    iFileDetails->mRNFormat = false;
    iFileDetails->mDuration = 100;
    iFileDetails->mTvOutConnected  = false;
    iFileDetails->mDrmProtected = false;
    iFileDetails->mMultiItemPlaylist = false;
    iFileDetails->mVideoHeight = 320;
    iFileDetails->mVideoWidth  = 240;
    iFileDetails->mBitRate = 16000;         
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::IssueVideoAppForegroundCmd()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::IssueVideoAppForegroundCmdL( TBool aForeground )
{
    iForeground = aForeground;
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::UpdateVideoRectDone()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::UpdateVideoRectDone()
{
}
// EOF
