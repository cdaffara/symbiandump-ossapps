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
* Description:  This class plays local video file
 *
*/

// Version : %version: 15 %


//
//  INCLUDE FILES
//
#include <apgcli.h>
#include <e32cmn.h>
#include <mpxcenrepwatcher.h>
#include <mpxcenrepobserver.h>
#include <mpxmediageneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mmf/server/mmffile.h>

#include "mpxvideohelixplayback.h"
#include "mpxvideoprivatecrkeys.h"
#include <mpxvideoplaybackdefs.h>

//
//  CONSTANTS
//
const TUid KLocalPlaybackUid = { 0x10282551 };


// ============================ MEMBER FUNCTIONS ===============================

//  ----------------------------------------------------------------------------
//    Two-phased constructor.
//  ----------------------------------------------------------------------------
//
CMPXVideoHelixPlayback* CMPXVideoHelixPlayback::NewL( TAny* /*aInitParams*/ )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::NewL()"));

    CMPXVideoHelixPlayback* p = new (ELeave) CMPXVideoHelixPlayback();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
}

//  ----------------------------------------------------------------------------
//    Symbian 2nd phase constructor can leave.
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::ConstructL()"));

    iVideoPlaybackCtlr = CMPXVideoPlaybackController::NewL( *iObs );

    User::LeaveIfError( iFs.Connect() );
    iFs.ShareProtected();
}

//  ----------------------------------------------------------------------------
//    C++ constructor
//  ----------------------------------------------------------------------------
//
CMPXVideoHelixPlayback::CMPXVideoHelixPlayback()
{
    MPX_DEBUG(_L("CMPXVideoHelixPlayback::CMPXVideoHelixPlayback()"));
}

//  ----------------------------------------------------------------------------
//    Destructor
//  ----------------------------------------------------------------------------
//
CMPXVideoHelixPlayback::~CMPXVideoHelixPlayback()
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::~CMPXVideoHelixPlayback"));

    iFs.Close();

    if ( iVideoPlaybackCtlr )
    {
        delete iVideoPlaybackCtlr;
        iVideoPlaybackCtlr = NULL;
    }
}

//  ------------------------------------------------------------------------------------------------
//    Sets the file name and file handle
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::OpenFileHandleL( const TDesC& aUri, RFile& aFile )
{
    MPX_ENTER_EXIT(
            _L("CMPXVideoHelixPlayback::OpenFileHandleL( RFile )"),
            _L("aSong %S"), &aUri );

    if ( aFile.SubSessionHandle() )
    {
        aFile.Close();
    }

    TInt err = aFile.Open( iFs, aUri, EFileRead | EFileShareReadersOrWriters );

    //
    //  Check if RFile64 is needed
    //
    if ( err == KErrTooBig )
    {
        User::Leave( KErrTooBig );
    }
    else if ( err != KErrNone )
    {
        CheckForStreamingUrlL( aUri );
    }
}

//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a file name
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::InitialiseL( const TDesC& aSong )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::InitialiseL()"),
                  _L("aSong %S"), &aSong );

    InitialiseWithPositionL( aSong );
}


//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a file name with position
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::InitialiseWithPositionL( const TDesC& aSong, TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::InitialiseL()"),
                   _L("aSong %S, aPosition %d"), &aSong, aPosition );

    RFile fileHandle;

    CleanupClosePushL( fileHandle );

    MPX_TRAPD( err, OpenFileHandleL( aSong, fileHandle ));

    if ( err == KErrNone )
    {
        iVideoPlaybackCtlr->OpenFileL( aSong, fileHandle, aPosition );
    }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else if ( err == KErrTooBig )
    {
        //
        // use RFile64 handle
        //
        RFile64 fileHandle64;
        CleanupClosePushL( fileHandle64 );

        MPX_TRAPD( err, OpenFileHandle64L( aSong, fileHandle64 ));

        if ( err != KErrNone )
        {
            // Handle error
            iVideoPlaybackCtlr->HandleError( err );
        }
        else
        {
                iVideoPlaybackCtlr->OpenFile64L( aSong, fileHandle64, aPosition );
        }

        CleanupStack::PopAndDestroy(); // fileHandle64
    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else
    {
        // Handle error
        iVideoPlaybackCtlr->HandleError( err );
    }

    CleanupStack::PopAndDestroy(); // fileHandle
}

//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a file handle
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::InitialiseL( RFile& aSong )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::InitialiseL( RFile )"));

    InitialiseWithPositionL( aSong );
}

//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a file handle with position
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::InitialiseWithPositionL( RFile& aSong, TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::InitialiseWithPositionL( RFile )"),
                   _L("aPosition %d"), aPosition );

    TFileName filename;
    aSong.FullName( filename );

    iVideoPlaybackCtlr->OpenFileL( filename, aSong, aPosition );
}


//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a file name
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::InitStreamingL( const TDesC& aUri,
                                             const TDesC8& /*aType*/,
                                             TInt aAccessPoint,
                                             TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::InitStreamingL()"),
                   _L("aUri %S, aAccessPoint %d, aPosition %d"), &aUri, aAccessPoint, aPosition );

    RFile fileHandle;

    CleanupClosePushL( fileHandle );

    MPX_TRAPD( err, OpenFileHandleL( aUri, fileHandle ));

    if ( err != KErrNone )
    {
        // Handle error
        iVideoPlaybackCtlr->HandleError( err );
    }
    else
    {
        iVideoPlaybackCtlr->OpenFileL( aUri, fileHandle, aPosition, aAccessPoint );
    }

    CleanupStack::PopAndDestroy();
}

//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a file handle
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::InitStreamingL( RFile& aFile, TInt aAccessPoint, TInt aPosition  )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::InitStreamingL( RFile )"),
                   _L("aAccessPoint = %d, aPosition = %d"), aAccessPoint, aPosition );

    TFileName filename;
    aFile.FullName( filename );

    iVideoPlaybackCtlr->OpenFileL( filename, aFile, aPosition, aAccessPoint );
}

//  ----------------------------------------------------------------------------
//    Executes a command on the selected song
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::CommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::CommandL() - CMPXCommand"));

    iVideoPlaybackCtlr->HandleCommandL( aCmd );
}


//  ----------------------------------------------------------------------------
//    Executes a command on the selected song
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::CommandL(TMPXPlaybackCommand aCmd, TInt aData)
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::CommandL() - TMPXPlaybackCommand"));

    iVideoPlaybackCtlr->HandleCommandL(aCmd, aData);
}


//  ----------------------------------------------------------------------------
//    Sets a property of the plugin
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::SetL( TMPXPlaybackProperty aProperty, TInt aValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::SetL()"));

    // Route the SetL() to VideoPlaybackController
    iVideoPlaybackCtlr->SetPropertyL(aProperty, aValue);
}

//  ----------------------------------------------------------------------------
//    Gets a property of the plugin (async)
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::PropertyL( TMPXPlaybackProperty aProperty ) const
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::PropertyL()"));

    // Route the PropertyL() to VideoPlaybackController
    iVideoPlaybackCtlr->GetPropertyL( aProperty );
}

//  ----------------------------------------------------------------------------
//    Gets a list of sub players, UPnP only
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::SubPlayerNamesL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::SubPlayerNamesL()"));
    iObs->HandleSubPlayerNames( KLocalPlaybackUid, NULL, ETrue, KErrNone );
}

//  ----------------------------------------------------------------------------
//    Select a sub player
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::SelectSubPlayerL( TInt /*aIndex*/ )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::SelectSubPlayerL()"));
    User::Leave( KErrNotSupported );
}

//  ----------------------------------------------------------------------------
//    Gets media properties
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::MediaL( const TArray<TMPXAttribute>& aAttrs )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::MediaL()"));
    iVideoPlaybackCtlr->SendMediaL( aAttrs );
}

//  ----------------------------------------------------------------------------
//    Cancel request
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::CancelRequest()
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::CancelRequest()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoHelixPlayback::CheckForStreamingUrlL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::CheckForStreamingUrlL( const TDesC& aUri )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::CMPXVideoHelixPlayback::CheckForStreamingUrlL()"));

    CMediaRecognizer* recognizer = CMediaRecognizer::NewL();
    CleanupStack::PushL( recognizer );

    if ( ! recognizer->IsValidStreamingPrefix( aUri ) )
    {
        User::LeaveIfError( KErrNotFound );
    }

    CleanupStack::PopAndDestroy( recognizer );
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    Sets the file name and 64-bit file handle
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::OpenFileHandle64L( const TDesC& aUri, RFile64& aFile )
{
    MPX_ENTER_EXIT(
            _L("CMPXVideoHelixPlayback::OpenFileHandle64L( RFile64 )"),
            _L("aSong %S"), &aUri );

    if ( aFile.SubSessionHandle() )
    {
        aFile.Close();
    }

    TInt err = aFile.Open( iFs, aUri, EFileRead | EFileShareReadersOrWriters );

    if ( err != KErrNone )
    {
        CheckForStreamingUrlL( aUri );
    }
}

//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a 64-bit file handle
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::Initialise64L( RFile64& aSong, TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::Initialise64L( RFile64 )"),
                   _L("aPosition %d"), aPosition );
    TFileName filename;
    aSong.FullName( filename );

    iVideoPlaybackCtlr->OpenFile64L( filename, aSong, aPosition );
}

//  ----------------------------------------------------------------------------
//    Initializes a clip for playback from a 64-bit file handle
//  ----------------------------------------------------------------------------
//
void CMPXVideoHelixPlayback::InitStreaming64L( RFile64& aFile, TInt aAccessPoint, TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::InitStreaming64L( RFile64 )"),
                   _L("aAccessPoint = %d, aPosition %d"), aAccessPoint, aPosition );

    TFileName filename;
    aFile.FullName( filename );

    iVideoPlaybackCtlr->OpenFile64L( filename, aFile, aPosition, aAccessPoint );
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

// End of file
