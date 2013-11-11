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
* Description:  Test Harness for VideoHelixPlaybackPlugin
*
*/

// Version : %version: 28 %


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>

#include <mpxmediageneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mmf/common/mmferrors.h>
#include <mpxplaybackpluginversion2.h>
#include <mpxmessagegeneraldefs.h>

#include <accpolnamevaluerecord.h>
#include <accconfigfileparser.h>
#include <acccongenericid.h>
#include <accpolnamevaluerecord.h>

#include "dlmgr_stub.h"
#include "mpxvideoregion.h"
#include "videohelixtest.h"
#include "mpxvideoaccessoryobserver_stub.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxmediavideodefs.h"
#include "mpxvideo_debug.h"
#include "mpxvideoplayerutility_stub.h"
#include "mpxhelixplaybackplugindefs.h"

#ifdef __WINSCW__
    _LIT( KVideoTestPath, "c:\\data\\Videos\\" );
#else
    _LIT( KVideoTestPath, "f:\\testing\\data\\" );
#endif

_LIT( KOpenControllerFilename, "c:\\vhpp_openerror.txt" );
_LIT( KPrepareControllerFilename, "c:\\vhpp_prepareerror.txt" );
_LIT( KPlaybackCompleteFilename, "c:\\vhpp_playerror.txt" );
_LIT( KDlMgrFile, "c:\\vhpp_dlmgr.txt" );
_LIT( KDrmRightsFile, "c:\\vhpp_drmrights.txt" );

#define ELightsNotBlinking 1
#define ELightsBlinking 2
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//  CVHPPTestClass::RunMethodL
//  Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVHPPTestClass::RunMethodL( CStifItemParser& aItem )
{
    static TStifFunctionInfo const KFunctions[] =
    {
        //
        //  Copy this line for every implemented function.
        //  First string is the function name used in TestScripter script file.
        //  Second is the actual implementation member function.
        //
        ENTRY( "Initialize", CVHPPTestClass::InitializeL ),
        ENTRY( "InitializeHandle", CVHPPTestClass::InitializeHandleL ),
        ENTRY( "InitializeLink", CVHPPTestClass::InitializeLinkL ),
        ENTRY( "InitializeSdp", CVHPPTestClass::InitializeSdpL ),
        ENTRY( "IssueGeneralCommand", CVHPPTestClass::IssueGeneralCommandL),
        ENTRY( "IssuePlaybackCommand", CVHPPTestClass::IssuePlaybackCommandL),
        ENTRY( "IssueVideoCommand", CVHPPTestClass::IssueVideoCommandL),
        ENTRY( "IssueSeekingCommand", CVHPPTestClass::IssueSeekingCommandL),
        ENTRY( "SetProperty", CVHPPTestClass::SetPropertyL),
        ENTRY( "GetProperty", CVHPPTestClass::GetPropertyL),
        ENTRY( "GetSubPlayerNames", CVHPPTestClass::GetSubPlayerNamesL),
        ENTRY( "SelectSubPlayer", CVHPPTestClass::SelectSubPlayerL),
        ENTRY( "SubPlayerName", CVHPPTestClass::SubPlayerName),
        ENTRY( "SubPlayerIndex", CVHPPTestClass::SubPlayerIndex),
        ENTRY( "CancelRequest", CVHPPTestClass::CancelRequest),
        ENTRY( "GetMedia", CVHPPTestClass::GetMediaL),
        ENTRY( "SetPlaybackComplete", CVHPPTestClass::SetPlaybackCompleteL),
        ENTRY( "SetUnexpectedMmfEvent", CVHPPTestClass::SetUnexpectedMmfEventL),
        ENTRY( "ConnectToDownload", CVHPPTestClass::ConnectToDownloadL),
        ENTRY( "PauseDownload", CVHPPTestClass::PauseDownloadL),
        ENTRY( "ResumeDownload", CVHPPTestClass::ResumeDownloadL),
        ENTRY( "CancelDownload", CVHPPTestClass::CancelDownloadL),
        ENTRY( "RetrievePdlStatus", CVHPPTestClass::RetrievePdlStatusL),
        ENTRY( "ChangeAspectRatioL", CVHPPTestClass::ChangeAspectRatioL),

        ENTRY( "AlarmOn", CVHPPTestClass::AlarmOn),
        ENTRY( "PhoneCallRejected", CVHPPTestClass::PhoneCallRejected),
        ENTRY( "VoiceCallAccepted", CVHPPTestClass::VoiceCallAccepted),
        ENTRY( "VideoCallAccepted", CVHPPTestClass::VideoCallAccepted),
        ENTRY( "PlayduringVideoCall", CVHPPTestClass::PlayduringVideoCall),
        ENTRY( "PlayduringVoiceCall", CVHPPTestClass::PlayduringVoiceCall),

        ENTRY( "EndPhoneCall", CVHPPTestClass::EndPhoneCall),

        ENTRY( "SetTvOutConnected", CVHPPTestClass::SetTvOutConnectedL ),
        ENTRY( "SetTvOutDisconnected", CVHPPTestClass::SetTvOutDisconnectedL ),
        ENTRY( "ConnectTvOut", CVHPPTestClass::ConnectTvOutL ),
        ENTRY( "DisconnectTvOut", CVHPPTestClass::DisconnectTvOutL ),

        ENTRY( "SetDrmProtected", CVHPPTestClass::SetDrmProtectedL ),
        ENTRY( "IssueSeekedToEndCommand", CVHPPTestClass::IssueSeekedToEndCommandL),
        ENTRY( "HandleVolume", CVHPPTestClass::HandleVolumeL ),

        ENTRY ("InitializeWithPositionL", CVHPPTestClass::InitializeWithPositionL),
        ENTRY ("InitializeLinkWithPositionL", CVHPPTestClass::InitializeLinkWithPositionL),
        ENTRY ("InitializeHandleWithPositionL", CVHPPTestClass::InitializeHandleWithPositionL),
        ENTRY ( "InitializeStreamingWithSdpFileHandleL", CVHPPTestClass::InitializeStreamingWithSdpFileHandleL ),
        ENTRY ( "RetrieveFileNameAndModeL", CVHPPTestClass::RetrieveFileNameAndModeL )

        //
        //  ADD NEW ENTRIES HERE
        //
        // ENTRY( "SendPlayCommandToServer",
        //        CVHPPTestClass::SendPlayCommandToServer ),
        // [test cases entries] - Do not remove
    };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
}

void
CVHPPTestClass::WriteControllerError( const TDesC& aFilename, TInt aData )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::WriteControllerError()"),
                   _L("file = %S, data = %d"), &aFilename, aData );

    //
    //  Write to test file so the controller open fails
    //
    RFile file;

    RFs fs;
    TInt error = fs.Connect();

    TBuf8<16> tgt;
    tgt.Num( aData );

    file.Replace( fs, aFilename, EFileWrite );

    file.Write( tgt );

    file.Close();
    fs.Close();
}

void
CVHPPTestClass::CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::CreateGeneralPlaybackCommandL()"),
                   _L("aCmd = %d"), aCmd );

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId,
                                 KMPXCommandIdPlaybackGeneral );

    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, aCmd );

    iPlaybackPlugin->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

void
CVHPPTestClass::PreparePluginL()
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::PreparePluginL()"));

    //
    //  Set this class as the observer for the Plugin events
    //
    iLog->Log(_L("    SetObserver on Plugin"));
    iPlaybackPlugin->SetObserver( *this );

    //
    //  Create Playback Message
    //
    iLog->Log(_L("    Close the Plugin"));
    CreateGeneralPlaybackCommandL( EPbCmdClose );
}

TInt
CVHPPTestClass::ReadFileInitializationParameters( CStifItemParser& aItem, TDes& aFilename )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ReadFileInitializationParameters()"));

    TPtrC filename;

    //
    //  Read in the filename from the config file
    //
    TInt err = aItem.GetNextString( filename );

    if ( err == KErrNone )
    {
        //
        //  Build the full path to the file
        //
        aFilename.Append( KVideoTestPath );
        aFilename.Append( filename );

        err = ReadInitializationErrors( aItem );
    }

    return err;
}

TInt
CVHPPTestClass::ReadInitializationErrors( CStifItemParser& aItem )
{
    TInt err;
    TInt initError;
    TInt openError;
    TInt prepError;

    //
    //  Read in the expected init error code
    //
    err = aItem.GetNextInt( initError );

    if ( err == KErrNone && initError != KErrNone )
    {
        AddErrorEvent( initError );
    }
    else
    {
        //
        //  Read in the expected open complete error code
        //
        err = aItem.GetNextInt( openError );

        WriteControllerError( KOpenControllerFilename, openError );

        if ( err == KErrNone && openError != KErrNone )
        {
            AddErrorEvent( openError );
        }
        else
        {
            //
            //  Open Complete does not return error
            //  Read in the expected prepare complete error code
            //
            err = aItem.GetNextInt( prepError );

            WriteControllerError( KPrepareControllerFilename, prepError );

            if ( err == KErrNone && prepError != KErrNone )
            {
                AddErrorEvent( prepError );
            }
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::InitializeL
//  Test creation of a general playback command.
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::InitializeL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeL()"));

    TInt duration;
    TInt volumeSteps;

    TInt err = aItem.GetNextInt( duration );

    if ( err == KErrNone )
    {
        //
        //  We will always get an Init Complete message out
        //
        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = EPInitialised;
        event->iData  = duration;
        event->iError = KErrNone;

        AddExpectedEvent( event );

        //
        // read number of volume steps
        //
        err = aItem.GetNextInt( volumeSteps );

        if ( err == KErrNone )
        {
            //
            // set volume steps
            //
            SetVolumeSteps( volumeSteps );

            TBuf<120> fullPath;

            err = ReadFileInitializationParameters( aItem, fullPath );

            if ( err == KErrNone )
            {
                PreparePluginL();

                //
                //  Initalize the Plugin with a file name
                //
                MPX_DEBUG(_L("Initialize the Plugin:  filename = %S"), &fullPath);
                iLog->Log(_L("Initialize the Plugin:  filename = %S"), &fullPath);

                iPlaybackPlugin->InitialiseL( fullPath );
            }
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::InitializeLinkL
//  Test creation of a general playback command.
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::InitializeLinkL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeLinkL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeLinkL()"));

    TInt duration;
    TInt volumeSteps;
    TInt err = aItem.GetNextInt( duration );

    if ( err == KErrNone )
    {
        //
        //  We will always get an Init Complete message out
        //
        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = EPInitialised;
        event->iData  = duration;
        event->iError = KErrNone;

        AddExpectedEvent( event );

        //
        // read number of volume steps
        //
        err = aItem.GetNextInt( volumeSteps );

        if ( err == KErrNone )
        {
            //
            // set volume steps
            //
            SetVolumeSteps( volumeSteps );

            TPtrC  link;

            //
            //  Read in the link from the config file
            //
            TInt err = aItem.GetNextString( link );

            if ( err == KErrNone )
            {
                TInt err = ReadInitializationErrors( aItem );

                if ( err == KErrNone )
                {
                    PreparePluginL();

                    //
                    //  Extract the streaming link from the ram file and
                    //  Initalize the Plugin with the link and an access point
                    //
                    MPX_DEBUG(_L("Initialize the Plugin:  link = %S"), &link);
                    iLog->Log(_L("Initialize the Plugin:  link = %S"), &link);

                    iPlaybackPlugin->InitStreamingL( link, KNullDesC8, 11 );
                }
            }
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::InitializeSdpL
//  Test creation of a general playback command.
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::InitializeSdpL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeSdpL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeSdpL()"));

    TBuf<120> fullPath;
    TInt      initError = KErrNone;

    TInt err = KErrNone;// = ReadInitializationParameters( aItem, fullPath, initError );

    if ( err == KErrNone )
    {
        PreparePluginL();

        MPX_DEBUG(_L("Initialize the Plugin:  filename = %S"), &fullPath);
        iLog->Log(_L("Initialize the Plugin:  filename = %S"), &fullPath);


        TRAPD( err,
               static_cast<CMPXPlaybackPluginVersion2*>(iPlaybackPlugin)->InitStreamingL
                                     ( fullPath, KNullDesC8, 11 );
              );

        MPX_DEBUG(_L("CVHPPTestClass::InitialisePluginL(%d)"), err);

        if ( err == initError )
        {
            MPX_DEBUG(_L("InitialiseL() returned proper error %d"), err);
            iLog->Log(_L("InitialiseL() returned proper error %d"), err);
            err = KErrNone;
        }
    }

    return err;
}


TInt
CVHPPTestClass::InitializeHandleL( CStifItemParser& aItem  )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeHandleL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeHandleL()"));

    TInt duration;
    TInt volumeSteps;
    TInt fileHandle32;

    TInt err = aItem.GetNextInt( fileHandle32 );

    if ( err == KErrNone )
    {

#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        //
        // set RFile as default if the 64-bit flag is not defined
        //
        fileHandle32 = ETrue;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        err = aItem.GetNextInt( duration );

        if ( err == KErrNone )
        {
            //
            //  We will always get an Init Complete message out
            //
            TCallbackEvent* event = new TCallbackEvent;

            event->iEvent = EPInitialised;
            event->iData  = duration;
            event->iError = KErrNone;

            AddExpectedEvent( event );

            //
            // read number of volume steps
            //
            err = aItem.GetNextInt( volumeSteps );

            if ( err == KErrNone )
            {
                //
                // set volume steps
                //
                SetVolumeSteps( volumeSteps );

                TBuf<120> fullPath;

                err = ReadFileInitializationParameters( aItem, fullPath );

                if ( err == KErrNone )
                {
                    PreparePluginL();

                    RFs fs;
                    TInt error = fs.Connect();

                    //
                    //  Open a file handle to the clip
                    //
                    if ( fileHandle32 )
                    {
                        RFile file;
                        User::LeaveIfError( file.Open( fs, fullPath, EFileRead ) );
                        iPlaybackPlugin->InitialiseL( file );
                        file.Close();
                    }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                    else
                    {
                        RFile64 file64;
                        User::LeaveIfError( file64.Open( fs, fullPath, EFileRead ) );
                        iPlaybackPlugin->Initialise64L( file64 );
                        file64.Close();
                    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

                    //
                    //  Initalize the Plugin with a file name
                    //
                    MPX_DEBUG(_L("Initialize the Plugin with File Handle:  filename = %S"), &fullPath);
                    iLog->Log(_L("Initialize the Plugin with File Handle:  filename = %S"), &fullPath);

                    fs.Close();
                }
            }
        }
    }

    return err;
}

TRect
CVHPPTestClass::ReadRect( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ReadRect()"));

    TRect rect;

    TInt value;

    if ( ! aItem.GetNextInt( value ) )
    {
        rect.iTl.iX = value;

        if ( ! aItem.GetNextInt( value ) )
        {
            rect.iTl.iY = value;

            if ( ! aItem.GetNextInt( value ) )
            {
                rect.iBr.iX = value;

                if ( ! aItem.GetNextInt( value ) )
                {
                    rect.iBr.iY = value;
                }
            }
        }
    }

    return rect;
}

void
CVHPPTestClass::CreateVideoPlaybackCommandL( CStifItemParser& aItem,
                                             TMPXVideoPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::CreateVideoPlaybackCommandL()"),
                   _L("aCmd = %d"), aCmd );

    //
    //  create command to pass to playback plugin
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId,
                                 KMPXMediaIdVideoPlayback );

    cmd->SetTObjectValueL<TInt>( KMPXMediaVideoPlaybackCommand, aCmd );

    switch ( aCmd )
    {
        case EPbCmdRestartDSA:
        {
            TRect rect = ReadRect( aItem );
            RRegion region( rect );

            CMPXVideoRegion* vidReg = CMPXVideoRegion::NewL();
            CleanupStack::PushL( vidReg );

            vidReg->SetRegion( region );

            vidReg->Print();

            cmd->SetCObjectValueL<CMPXVideoRegion>( KMPXMediaVideoPlaybackDSARegion, vidReg );

            CleanupStack::PopAndDestroy( vidReg );

            break;
        }
        case EPbCmdSetDisplayWindow:
        {
            TRect rect = ReadRect( aItem );
            RRegion region( rect );

            CMPXVideoRegion* vidReg = CMPXVideoRegion::NewL();
            CleanupStack::PushL( vidReg );

            vidReg->SetRegion( region);

            vidReg->Print();

            cmd->SetTObjectValueL<TRect>( KMPXMediaVideoPlaybackTRect, rect );

            cmd->SetCObjectValueL<CMPXVideoRegion>( KMPXMediaVideoPlaybackDSARegion, vidReg );

            CleanupStack::PopAndDestroy( vidReg );

            break;
        }
    }

    iPlaybackPlugin->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::IssueVideoCommandL
//  Test creation of a general playback command.
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::IssueVideoCommandL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::IssueVideoCommandL()"));
    iLog->Log(_L("CVHPPTestClass::IssueVideoCommandL()"));

    TInt vidCmd;
    TInt err = aItem.GetNextInt( vidCmd );

    if ( err == KErrNone )
    {
        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = vidCmd;
        event->iData  = 0;
        event->iError = KErrNone;

        AddExpectedEvent( event );

        CreateVideoPlaybackCommandL( aItem, (TMPXVideoPlaybackCommand)vidCmd );
    }

    return KErrNone;
}

TInt
CVHPPTestClass::IssueSeekingCommandL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::IssueSeekingCommandL()"));
    iLog->Log(_L("CVHPPTestClass::IssueSeekingCommandL()"));

    TInt vidCmd;
    TInt err = aItem.GetNextInt( vidCmd );

    //
    //  Seeking is starting, allow posiiton callbacks
    //
    iAllowPositionCallback = ETrue;

    if ( err == KErrNone )
    {
        TCallbackEvent* event = new TCallbackEvent;

        if ( vidCmd == EPbCmdStartVideoSeekingForward || vidCmd == EPbCmdStartVideoSeekingBackward )
        {
            TBool addEvent = ETrue;

            aItem.GetNextInt( addEvent );

            if ( addEvent )
            {
                event->iEvent = EPPluginSeeking;
                event->iData  = 0;
                event->iError = KErrNone;
                AddExpectedEvent( event );
            }
        }
        else if ( vidCmd == EPbCmdStopVideoSeeking )
        {
            TInt callbackEvent;
            aItem.GetNextInt( callbackEvent );

            if ( callbackEvent )
            {
                event->iEvent = callbackEvent;
                event->iData  = 0;
                event->iError = KErrNone;
                AddExpectedEvent( event );

                iAllowPositionCallback = EFalse;
            }
        }

        CreateVideoPlaybackCommandL( aItem, (TMPXVideoPlaybackCommand)vidCmd );
    }

    return KErrNone;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::IssueSeekedToEndCommandL
//  Test creation of a End of Clip command.
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::IssueSeekedToEndCommandL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::IssueSeekedToEndCommandL()"));
    iLog->Log(_L("CVHPPTestClass::IssueSeekedToEndCommandL()"));

    TCallbackEvent* event = new TCallbackEvent;

    event->iEvent = EPPlayComplete;
    event->iData  = 0;
    event->iError = KErrNone;

    AddExpectedEvent( event );

    CreateVideoPlaybackCommandL( aItem, EPbCmdEndofClipReached);

    return KErrNone;
}

TInt
CVHPPTestClass::ParsePlaybackCommand( CStifItemParser& aItem, TInt& aCmd )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ParsePlaybackCommand()"));

    TInt err = aItem.GetNextInt( aCmd );

    if ( err == KErrNone )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iData  = 0;

        err = aItem.GetNextInt( event->iError );

        if ( err == KErrNone )
        {
            if ( event->iError == KErrNotSupported )
            {
                delete event;
            }
            else
            {
                err = aItem.GetNextInt( event->iEvent );

                if ( err == KErrNone )
                {
                    if ( event->iEvent == EPBufferingStarted )
                    {
                        //
                        //  A playing command will be added also
                        //
                        AddExpectedEvent( event );

                        event = new TCallbackEvent;
                        event->iEvent = EPPlaying;
                        event->iData  = 0;
                        event->iError = KErrNone;
                    }

                    AddExpectedEvent( event );
                }
            }
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::IssueGeneralCommandL
//  Test creation of a general playback command.
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::IssueGeneralCommandL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::IssueGeneralCommandL()"));
    iLog->Log(_L("CVHPPTestClass::IssueGeneralCommandL()"));

    TInt genCmd = 0;

    TInt error = ParsePlaybackCommand( aItem, genCmd );

    if ( error == KErrNone )
    {
        CreateGeneralPlaybackCommandL( (TMPXPlaybackCommand)genCmd );
    }

    return error;
}

TInt
CVHPPTestClass::IssuePlaybackCommandL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::IssuePlaybackCommandL()"));
    iLog->Log(_L("CVHPPTestClass::IssuePlaybackCommandL()"));

    TInt genCmd;

    TInt error = ParsePlaybackCommand( aItem, genCmd );

    if ( error == KErrNone )
    {
        iPlaybackPlugin->CommandL( (TMPXPlaybackCommand)genCmd, 0 );
    }

    return error;
}

TInt
CVHPPTestClass::SetPropertyL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SetPropertyL()"));
    iLog->Log(_L("CVHPPTestClass::SetPropertyL()"));

    TInt property;
    TInt value;

    TInt trapErr = KErrNone;
    TInt err = aItem.GetNextInt( property );

    if ( err == KErrNone )
    {
        err = aItem.GetNextInt( value );

        if ( err == KErrNone )
        {
            if ( property >= EPbPropertyNum )
            {
                trapErr = KErrNotSupported;
            }
            else
            {
                TCallbackEvent* event = new TCallbackEvent;

                if ( property == EPbPropertyVolume )
                {
                    TInt addEvent;
                    err = aItem.GetNextInt( addEvent );

                    if ( err == KErrNone && addEvent )
                    {
                        event->iEvent = EPVolumeChanged;
                        event->iData  = value;
                        event->iError = KErrNone;
                        AddExpectedEvent( event );
                    }
                }
                else if ( property == EPbPropertyMute )
                {
                    event->iEvent = EPMuteChanged;
                    event->iData  = value;
                    event->iError = KErrNone;
                    AddExpectedEvent( event );
                }
                else
                {
                    event->iEvent = EPSetComplete;
                    event->iData  = property;
                    event->iError = KErrNone;
                    AddExpectedEvent( event );
                }

            }

            TRAP( err, iPlaybackPlugin->SetL( (TMPXPlaybackProperty)property, value ) );
        }
    }

    if ( err == trapErr )
    {
        MPX_DEBUG(_L("CVHPPTestClass::SetPropertyL() leaves with proper error"));
        err = KErrNone;
    }

    return err;
}

TInt
CVHPPTestClass::GetPropertyL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::GetPropertyL()"));
    iLog->Log(_L("CVHPPTestClass::GetPropertyL()"));

    TInt property;
    TInt value;

    TInt trapErr = KErrNone;

    TInt err = aItem.GetNextInt( property );

    if ( err == KErrNone )
    {
        err = aItem.GetNextInt( value );

        if ( err == KErrNone )
        {
            if ( property >= EPbPropertyNum )
            {
                trapErr = KErrNotSupported;
            }
            else
            {
                TCallbackEvent* event = new TCallbackEvent;

                event->iEvent = property;
                event->iData  = value;
                event->iError = KErrNone;

                AddExpectedEvent( event );
            }

            TRAP( err, iPlaybackPlugin->PropertyL( (TMPXPlaybackProperty)property ) );
        }
    }

    if ( err == trapErr )
    {
        MPX_DEBUG(_L("CVHPPTestClass::GetPropertyL() leaves with proper error"));
        err = KErrNone;
    }

    return err;
}

TInt
CVHPPTestClass::GetSubPlayerNamesL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::GetSubPlayerNamesL()"));
    iLog->Log(_L("CVHPPTestClass::GetSubPlayerNmaesL()"));

    iPlaybackPlugin->SubPlayerNamesL();

    return KErrNone;
}

TInt
CVHPPTestClass::SelectSubPlayerL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SelectSubPlayerL()"));
    iLog->Log(_L("CVHPPTestClass::SelectSubPlayerL()"));

    TRAPD( err, iPlaybackPlugin->SelectSubPlayerL( 1 ); );

    if ( err == KErrNotSupported )
    {
        err = KErrNone;
    }

    return err;
}

TInt
CVHPPTestClass::SubPlayerName( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SubPlayerName()"));
    iLog->Log(_L("CVHPPTestClass::SubPlayerName()"));

    TInt err = KErrNone;

    TDesC name = iPlaybackPlugin->SubPlayerName();

    if ( name != KNullDesC )
    {
        err = KErrNotFound;
    }

    return err;
}

TInt
CVHPPTestClass::SubPlayerIndex( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SubPlayerIndex()"));
    iLog->Log(_L("CVHPPTestClass::SubPlayerIndex()"));

    TInt err = iPlaybackPlugin->SubPlayerIndex();

    if ( err == KErrNotFound )
    {
        err = KErrNone;
    }

    return err;
}

TInt
CVHPPTestClass::GetMediaL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::GetMediaL()"));
    iLog->Log(_L("CVHPPTestClass::GetMediaL()"));

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL(attrs);

    // TMPXAttribute attr( KMPXMediaGeneralAll );

    attrs.Append( KMPXMediaGeneralAll );
    attrs.Append( KMPXMediaVideoAll );

    iPlaybackPlugin->MediaL( attrs.Array() );

    CleanupStack::PopAndDestroy( &attrs );

    return KErrNone;
}

TInt
CVHPPTestClass::CancelRequest( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::CancelRequest()"));
    iLog->Log(_L("CVHPPTestClass::CancelRequest()"));

    iPlaybackPlugin->CancelRequest();

    return KErrNone;
}

TInt
CVHPPTestClass::SetPlaybackCompleteL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SetPlaybackCompleteL()"));
    iLog->Log(_L("CVHPPTestClass::SetPlaybackCompleteL()"));

    TInt playError = KErrNone;

    TInt err = aItem.GetNextInt( playError );

    if ( err == KErrNone )
    {
        TInt duration;

        err = aItem.GetNextInt( duration );

        if ( err == KErrNone )
        {
            WriteControllerError( KPlaybackCompleteFilename, playError );

            TCallbackEvent* event = new TCallbackEvent;
            event->iEvent = EPSetComplete;
            event->iData  = EPbPropertyPosition;
            event->iError = KErrNone;
            AddExpectedEvent( event );

            //
            //  Trigger a playback complete by setting time to duration
            //
            iPlaybackPlugin->SetL( EPbPropertyPosition, duration );

            if ( playError )
            {
                AddErrorEvent( playError );
            }
            else
            {
                TCallbackEvent* event = new TCallbackEvent;

                event->iEvent = EPPlayComplete;
                event->iData  = 0;
                event->iError = playError;

                AddExpectedEvent( event );
            }
        }
    }

    return KErrNone;
}

TInt
CVHPPTestClass::SetUnexpectedMmfEventL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SetUnexpectedMmfEventL()"));
    iLog->Log(_L("CVHPPTestClass::SetUnexpectedMmfEventL()"));

    TInt playError = KErrNone;

    TInt err = aItem.GetNextInt( playError );

    if ( err == KErrNone )
    {
        WriteControllerError( KPlaybackCompleteFilename, playError );

        //
        //  Trigger an unexpected event by setting time to a negative value
        //
        iPlaybackPlugin->SetL( EPbPropertyPosition, -1 );

        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = EPClosed;
        event->iData  = 0;
        event->iError = playError;

        AddExpectedEvent( event );
    }

    return KErrNone;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::ConnectToDownload
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::ConnectToDownloadL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ConnectToDownloadL()"));
    iLog->Log(_L("CVHPPTestClass::ConnectToDownloadL()"));

    TInt err = KErrNone;

#ifdef USE_S60_DOWNLOAD_MANAGER

    iDlMgrTester = CDlMgrTestClass::NewL();
    iDlMgrTester->AddStifObserver( this );

    TInt dlId;
    TPtrC filename;
    TBuf<120> fullPath;

    //
    //   Read in the download id and filename
    //
    err = aItem.GetNextInt( dlId );

    if ( err == KErrNone )
    {
        err = aItem.GetNextString( filename );

        if ( err == KErrNone )
        {
            TCallbackEvent* event = new TCallbackEvent;
            event->iEvent = EConnectedToDownload;
            event->iData  = dlId;
            event->iError = KErrNone;

            AddExpectedEvent( event );

            //
            //  Build the full path to the file
            //
            fullPath.Append( KVideoTestPath );
            fullPath.Append( filename );

            CMPXCommand* cmd = CMPXCommand::NewL();
            CleanupStack::PushL( cmd );

            cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
            cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
            cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType, EPbCmdStartPd );
            cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackPDTransactionID, dlId );
            cmd->SetTextValueL( KMPXMediaVideoPlaybackFileName, fullPath );
            cmd->SetTObjectValueL<TInt>( KMPXMediaVideoMovePdlFile, ETrue );

            iPlaybackPlugin->CommandL( *cmd );

            CleanupStack::PopAndDestroy( cmd );
        }
    }

#else // USE_S60_DOWNLOAD_MANAGER

    // suppress build warning
    MPX_DEBUG(_L("CVHPPTestClass::ConnectToDownloadL() : parsing type = %d"), aItem.ParsingType());

    // Signal TestScripter to continue from waittestclass
    Signal();

#endif // USE_S60_DOWNLOAD_MANAGER

    return err;
}

// -------------------------------------------------------------------------------------------------
//   CVHPPTestClass::SendPdlCustomCommandL()
// -------------------------------------------------------------------------------------------------
//
void
CVHPPTestClass::SendPdlCustomCommandL( TMPXPlaybackPdCommand aCustomCmd )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SendPdlCustomCommandL"),
                   _L("aCustomCmd = %d"), aCustomCmd );

#ifdef USE_S60_DOWNLOAD_MANAGER

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType, aCustomCmd );

    iPlaybackPlugin->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );

#else // USE_S60_DOWNLOAD_MANAGER

    // Signal TestScripter to continue from waittestclass
    Signal();

#endif // USE_S60_DOWNLOAD_MANAGER

}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::PauseDownloadL
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::PauseDownloadL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::PauseDownloadL()"));
    iLog->Log(_L("CVHPPTestClass::PauseDownloadL()"));

#ifdef USE_S60_DOWNLOAD_MANAGER

    TCallbackEvent* event = new TCallbackEvent;

    event->iEvent = EPDownloadStateChanged;
    event->iData  = EPbDlStateDownloadPaused;
    event->iError = KErrNone;

    AddExpectedEvent( event );

    iDlMgrTester->PauseDownload();

#else // USE_S60_DOWNLOAD_MANAGER

    // Signal TestScripter to continue from waittestclass
    Signal();

#endif // USE_S60_DOWNLOAD_MANAGER

    return KErrNone;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::ResumeDownloadL
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::ResumeDownloadL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ResumeDownloadL()"));
    iLog->Log(_L("CVHPPTestClass::ResumeDownloadL()"));

    TInt err = KErrNone;

#ifdef USE_S60_DOWNLOAD_MANAGER

    TInt dlSize;

    err = aItem.GetNextInt( dlSize );

    if ( err == KErrNone )
    {
        WriteControllerError( KDlMgrFile, dlSize );

        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = EPDownloadStateChanged;
        event->iError = KErrNone;

        if ( dlSize >= 100 )
        {
            event->iData = EPbDlStateDownloadCompleted;
        }
        else
        {
            event->iData  = EPbDlStateDownloading;
        }

        AddExpectedEvent( event );

        iDlMgrTester->ResumeDownload();
    }

#else // USE_S60_DOWNLOAD_MANAGER

    // suppress build warning
    MPX_DEBUG(_L("CVHPPTestClass::ResumeDownloadL() : parsing type = %d"), aItem.ParsingType());

    // Signal TestScripter to continue from waittestclass
    Signal();

#endif // USE_S60_DOWNLOAD_MANAGER

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::CancelDownloadL
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::CancelDownloadL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::CancelDownloadL()"));
    iLog->Log(_L("CVHPPTestClass::CancelDownloadL()"));

#ifdef USE_S60_DOWNLOAD_MANAGER

    TCallbackEvent* event = new TCallbackEvent;

    event->iEvent = EPDownloadStateChanged;
    event->iData  = EPbDlStateDownloadCanceled;
    event->iError = KErrNone;

    AddExpectedEvent( event );

    iDlMgrTester->CancelDownload();

#else // USE_S60_DOWNLOAD_MANAGER

    // Signal TestScripter to continue from waittestclass
    Signal();

#endif // USE_S60_DOWNLOAD_MANAGER

    return KErrNone;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::RetrievePdlStatusL
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::RetrievePdlStatusL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::RetrievePdlStatusL()"));
    iLog->Log(_L("CVHPPTestClass::RetrievePdlStatusL()"));

    TInt err = KErrNone;

#ifdef USE_S60_DOWNLOAD_MANAGER

    TInt pdlState;
    TInt expectedPdlState;
    TInt downloadedBytes;
    TInt expectedDownloadedBytes;
    TInt downloadSize;
    TInt expectedDownloadSize;

    //
    //   Read in the expected download data
    //
    err = aItem.GetNextInt( expectedPdlState );

    if ( err == KErrNone )
    {
        err = aItem.GetNextInt( expectedDownloadedBytes );

        if ( err == KErrNone )
        {
            err = aItem.GetNextInt( expectedDownloadSize );
        }
    }

    if ( err == KErrNone )
    {
        CMPXCommand* cmd( CMPXCommand::NewL() );
        CleanupStack::PushL( cmd );

        cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
        cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
        cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType,
                                                      EPbCmdGetPdStatus );

        iPlaybackPlugin->CommandL( *cmd );

        //
        //  Parse the PDL information
        //

        pdlState = cmd->ValueTObjectL<TMPXPlaybackPdDownloadState>( KMPXCommandPlaybackPDState );
        downloadedBytes = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDDownloadedBytes );
        downloadSize = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDTotalBytes );

        MPX_DEBUG(_L("    Expected Data:  PDL State = %d, current size = %d, total size = %d"),
            expectedPdlState, expectedDownloadedBytes, expectedDownloadSize );

        MPX_DEBUG(_L("    Received PDL State = %d, current size = %d, total size = %d"),
            pdlState, downloadedBytes, downloadSize );

        if ( ( expectedPdlState != pdlState ) ||
             ( expectedDownloadedBytes != downloadedBytes ) ||
             ( expectedDownloadSize != downloadSize ) )
        {
            err = KErrGeneral;
        }

        CleanupStack::PopAndDestroy( cmd );
    }

#else // USE_S60_DOWNLOAD_MANAGER

    // suppress build warning
    MPX_DEBUG(_L("CVHPPTestClass::RetrievePdlStatusL() : parsing type = %d"), aItem.ParsingType());

    // Signal TestScripter to continue from waittestclass
    Signal();

#endif // USE_S60_DOWNLOAD_MANAGER

    return err;
}

void
CVHPPTestClass::HandlePluginEvent( TEvent aEvent, TInt aData, TInt aError )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandlePluginEvent"),
                   _L("(%d, %d, %d)"), aEvent, aData, aError );

    iLog->Log(_L("CVHPPTestClass::HandlePluginEvent(%d, %d, %d)"),
        aEvent, aData, aError);

    TCallbackEvent* callback = new TCallbackEvent;

    callback->iEvent = aEvent;
    callback->iData  = aData;
    callback->iError = aError;

    ProcessEvent( callback );
}

void
CVHPPTestClass::HandlePlaybackMessage( CMPXMessage* aMsg, TInt /*aErr*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandlePlaybackMessage()"));
    iLog->Log(_L("CVHPPTestClass::HandlePlaybackMessage"));

    TMPXMessageId id( *(aMsg->Value<TMPXMessageId>(KMPXMessageGeneralId)) );

    if ( KMPXMediaIdVideoPlayback == id )
    {
        TMPXVideoPlaybackCommand message =
            ( *(aMsg->Value<TMPXVideoPlaybackCommand>(KMPXMediaVideoPlaybackCommand)) );

        switch ( message )
        {
            case EPbCmdPluginError:
            {
                TInt error( *aMsg->Value<TInt>( KMPXMediaVideoError ) );

                ProcessErrorEvent( error );
                break;
            }
            case EPbCmdTvOutEvent:
            {
                TCallbackEvent* callback = new TCallbackEvent;

                callback->iEvent = EPbCmdTvOutEvent;
                callback->iData  = *aMsg->Value<TInt>( KMPXMediaVideoTvOutConnected );
                callback->iError = KErrNone;

                ProcessEvent( callback );

                break;
            }
        }
    }
}

void
CVHPPTestClass::HandleProperty( TMPXPlaybackProperty aProperty,
                                TInt aValue,
                                TInt aError )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandleProperty()"));
    iLog->Log(_L("CVHPPTestClass::HandleProperty"));

    TCallbackEvent* callback = new TCallbackEvent;

    callback->iEvent = aProperty;
    callback->iData  = aValue;
    callback->iError = aError;

    ProcessEvent( callback );
}

void
CVHPPTestClass::HandleSubPlayerNames( TUid /*aPlayer*/,
                                      const MDesCArray* /*aSubPlayers*/,
                                      TBool /*aComplete*/,
                                      TInt /*aError*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandleSubPlayerNames()"));
    iLog->Log(_L("CVHPPTestClass::HandleSubPlayerNames"));

    Signal();
}

void
CVHPPTestClass::HandleMedia( CMPXMedia* aMedia, TInt /*aError*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandleMedia()"));
    iLog->Log(_L("CVHPPTestClass::HandleMedia"));

    //
    //  Check for an error condition
    //
    if ( aMedia->IsSupported( KMPXMediaVideoError ) )
    {
        MPX_DEBUG(_L("CVHPPTestClass::HandleMedia() Plugin has error"));

        TInt error = aMedia->ValueTObjectL<TInt>( KMPXMediaVideoError );

        ProcessErrorEvent( error );
    }
    else
    {
        TBool partialPlaybackError = EFalse;

        if ( aMedia->IsSupported( KMPXMediaVideoPartialPlayback ) )
        {
            partialPlaybackError = aMedia->ValueTObjectL<TInt>( KMPXMediaVideoPartialPlayback );
        }

        if ( partialPlaybackError )
        {
            ProcessErrorEvent( KErrMMPartialPlayback );
        }
        else
        {
            Signal();
        }
    }
}

void
CVHPPTestClass::HandlePlaybackCommandComplete( CMPXCommand* /*aCommandResult*/,
                                               TInt /*aError*/ )
{
    MPX_DEBUG(_L("CVHPPTestClass::HandlePlaybackCommandComplete"));
}

void
CVHPPTestClass::AddExpectedEvent( TCallbackEvent* aEvent )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::AddExpectedEvent()"));

    iExpectedCallbackArray->AppendL( aEvent );

    if ( iTimeoutController && ! iTimeoutController->IsActive() )
    {
        iTimeoutController->Start( TTimeIntervalMicroSeconds(15000000) );
    }
}

void
CVHPPTestClass::AddErrorEvent( TInt aError )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::AddErrorEvent"),
                   _L("(%d)"), aError );

    iExpectedError = aError;

    if ( iTimeoutController && ! iTimeoutController->IsActive() )
    {
        iTimeoutController->Start( TTimeIntervalMicroSeconds(15000000) );
    }
}

void
CVHPPTestClass::HandleTimeout( TInt aError )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandleTimeout"),
                   _L("(%d)"), aError );

    if ( aError == KErrNone )
    {
        iLog->Log(_L("    Callback timed out"));
        Signal( KErrTimedOut );
    }
    else
    {
        iLog->Log(_L("    Callback timeout error %d"), aError);
        Signal( aError );
    }
}

void
CVHPPTestClass::ProcessEvent( TCallbackEvent* aCallback )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ProcessEvent"));

    if ( iExpectedCallbackArray->Count() > 0 )
    {
        TCallbackEvent* expectedCallback = (*iExpectedCallbackArray)[0];

        MPX_DEBUG(_L("CVHPPTestClass::ProcessEvent(%d,%d,%d) Expected(%d,%d,%d)"),
            aCallback->iEvent, aCallback->iData, aCallback->iError,
            expectedCallback->iEvent, expectedCallback->iData, expectedCallback->iError);

        if ( expectedCallback->iEvent == aCallback->iEvent )
        {
            if ( expectedCallback->iData == aCallback->iData &&
                 expectedCallback->iError == aCallback->iError )
            {
                MPX_DEBUG(_L("Callback received with proper error code"));
                iLog->Log(_L("Callback received with proper error code"));

                //
                //  Pop the event from the array
                //
                iExpectedCallbackArray->Delete( 0 );

                if ( iExpectedCallbackArray->Count() == 0 )
                {
                    iTimeoutController->Cancel();
                }

                Signal();
            }
            else
            {
                iTimeoutController->Cancel();
                MPX_DEBUG(_L("Callback received with wrong error code"));
                iLog->Log(_L("Callback received with wrong error code"));
                Signal( KErrGeneral );
            }
        }
    }
    else
    {
        if ( ! ( iAllowPositionCallback && aCallback->iEvent == EPPositionChanged ) )
        {
            //
            //  Unexpected event
            //
            _LIT( KPanic, "Unexpected Event" );
            User::Panic( KPanic, -33 );
        }
    }

    delete aCallback;
}

void
CVHPPTestClass::ProcessErrorEvent( TInt aError )
{
    MPX_DEBUG(_L("CVHPPTestClass::ProcessErrorEvent(%d) ExpectedError(%d)"),
        aError, iExpectedError );

    iLog->Log(_L("    Error received (%d)"), aError );

    if ( iExpectedError == aError )
    {
        iTimeoutController->Cancel();

        MPX_DEBUG(_L("Error received with proper error code"));
        iLog->Log(_L("Error received with proper error code"));
        Signal();

        iExpectedError = KErrNone;
    }
    else
    {
        iTimeoutController->Cancel();

        MPX_DEBUG(_L("Unexpected error received"));
        iLog->Log(_L("Unexpected error received"));
        Signal( KErrAbort );

        iExpectedError = KErrNone;
    }
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::ChangeAspectRatioL
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::ChangeAspectRatioL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ChangeAspectRatioL()"));
    iLog->Log(_L("CVHPPTestClass::ChangeAspectRatioL()"));

    TInt aspectRatioCmd = 0;
    TInt expectedResult = 0;
    TInt err = aItem.GetNextInt( aspectRatioCmd );

    if ( err == KErrNone )
    {
        err = aItem.GetNextInt( expectedResult );

        if ( err == KErrNone )
        {
            //
            //  create command to pass to playback plugin
            //
            CMPXCommand* cmd = CMPXCommand::NewL();
            CleanupStack::PushL( cmd );

            cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
            cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );

            cmd->SetTObjectValueL<TInt>( KMPXMediaVideoPlaybackCommand,
                                         (TMPXVideoPlaybackCommand)aspectRatioCmd );

            iPlaybackPlugin->CommandL( *cmd );

            TInt result = cmd->ValueTObjectL<TInt>( KMPXMediaVideoAspectRatio );

            if ( expectedResult != result )
            {
                err = KErrGeneral;
            }

            CleanupStack::PopAndDestroy( cmd );
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::AlarmOn
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::AlarmOn( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::AlarmOn()"));
    iLog->Log(_L("CVHPPTestClass::AlarmOn()"));

    TInt backgroundCmd = 0;
    TInt err = aItem.GetNextInt( backgroundCmd );

    if ( err == KErrNone )
    {
        //callback event
        TCallbackEvent* event = new TCallbackEvent;
        event->iError = 0;
        event->iData  = 0;
        event->iEvent = EPPaused;
        AddExpectedEvent( event );

        // set alarm status
        RProperty::Set( KPSUidCoreApplicationUIs, KLightsAlarmLightActive, ELightsBlinking );

        //
        //  create background Cmd to pass to playback plugin
        //
        CMPXCommand* cmdPause = CMPXCommand::NewL();
        CleanupStack::PushL( cmdPause );
        cmdPause->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
        cmdPause->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
        cmdPause->SetTObjectValueL<TInt>( KMPXMediaVideoPlaybackCommand, backgroundCmd );
        cmdPause->SetTObjectValueL<TBool>( KMPXMediaVideoAppForeground, ETrue );
        iPlaybackPlugin->CommandL( *cmdPause );
        CleanupStack::PopAndDestroy( cmdPause );

        //reset alarm
        RProperty::Set( KPSUidCoreApplicationUIs, KLightsAlarmLightActive, ELightsNotBlinking );
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::PhoneCallRejected
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::PhoneCallRejected( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::PhoneCallRejected()"));
    iLog->Log(_L("CVHPPTestClass::PhoneCallRejected()"));

    TInt background = 0;
    TInt err = aItem.GetNextInt( background );
    TCallbackEvent* event = new TCallbackEvent;

    if ( err == KErrNone )
    {
        //callback event
        TCallbackEvent* event = new TCallbackEvent;
        event->iError = 0;
        event->iData  = 0;
        event->iEvent = EPPaused;
        AddExpectedEvent( event );

        //set phone call as ringing
        RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallState, EPSCTsyCallStateRinging);

        //pause
        CreateBackgroundCommand(background);

        err = aItem.GetNextInt( background );

        if ( err == KErrNone )
        {
            CreateBackgroundCommand(background);
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::VoiceCallAccepted
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::VoiceCallAccepted( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::VoiceCallAccepted()"));
    iLog->Log(_L("CVHPPTestClass::VoiceCallAccepted()"));

    TInt background = 0;
    TInt err = aItem.GetNextInt( background );

    if ( err == KErrNone )
    {
        //callback event
        TCallbackEvent* event = new TCallbackEvent;
        event->iError = 0;
        event->iData  = 0;
        event->iEvent = EPPaused;
        AddExpectedEvent( event );

        //set phone call as ringing
        RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallState, EPSCTsyCallStateRinging);

        //pause
        CreateBackgroundCommand(background);

        err = aItem.GetNextInt( background );

        if ( err == KErrNone )
        {
            //set phone call as Connected
            RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallState, EPSCTsyCallStateConnected);

            CreateBackgroundCommand(background);
         }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::VideoCallAccepted
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::VideoCallAccepted( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::VideoCallAccepted()"));
    iLog->Log(_L("CVHPPTestClass::VideoCallAccepted()"));

    TInt background = 0;
    TInt err = aItem.GetNextInt( background );

    if ( err == KErrNone )
    {
        //callback event
        TCallbackEvent* event = new TCallbackEvent;
        event->iError = 0;
        event->iData  = 0;
        event->iEvent = EPPaused;
        AddExpectedEvent( event );

        //set phone call as ringing
        RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallState, EPSCTsyCallStateRinging);

        //pause
        CreateBackgroundCommand(background);

        err = aItem.GetNextInt( background );

        if ( err == KErrNone )
        {
            //set phone call as Connected
            RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallType, EPSCTsyCallTypeH324Multimedia);

           CreateBackgroundCommand(background);
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::PlayduringVideoCall
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::PlayduringVideoCall( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::PlayduringVideoCall()"));
    iLog->Log(_L("CVHPPTestClass::PlayduringVideoCall()"));

    TInt err = 0;

    //set phone call as Connected
    err = RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallType, EPSCTsyCallTypeH324Multimedia);

    AddErrorEvent(KMPXVideoCallOngoingError);

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::PlayduringVoiceCall
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::PlayduringVoiceCall( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::PlayduringVoiceCall()"));
    iLog->Log(_L("CVHPPTestClass::PlayduringVoiceCall()"));

    TInt err = 0;

    //callback event
    TCallbackEvent* event = new TCallbackEvent;
    event->iError = 0;
    event->iData  = 0;
    event->iEvent = EPPaused;
    AddExpectedEvent( event );

    //set phone call as Connected
    err = RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallState, EPSCTsyCallStateConnected);

    AddErrorEvent(KMPXVideoPlayOver2GDuringVoiceCallError);

    return err;
}

TInt
CVHPPTestClass::EndPhoneCall()
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::EndPhoneCall()"));
    iLog->Log(_L("CVHPPTestClass::EndPhoneCall()"));

    //Disconnect phone call
    RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallState, EPSCTsyCallStateNone);
    RProperty::Set(KPSUidCtsyCallInformation, KCTsyCallType, EPSCTsyCallTypeNone);

    return KErrNone;
}

TInt
CVHPPTestClass::ConnectTvOutL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::ConnectTvOutL()"));
    iLog->Log(_L("CVHPPTestClass::ConnectTvOutL()"));

    TBool playable = EFalse;

    TInt err = aItem.GetNextInt( playable );

    if ( err == KErrNone )
    {
        iAccObserver->SetTvOutPlaybackAllowed( playable );

        //
        //  Add event for callback
        //
        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = EPbCmdTvOutEvent;
        event->iData  = ETrue;
        event->iError = KErrNone;

        AddExpectedEvent( event );
    }

    iAccObserver->UpdateTvOutStatusL( ETrue );

    return err;
}

TInt
CVHPPTestClass::DisconnectTvOutL()
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::DisconnectTvOutL()"));
    iLog->Log(_L("CVHPPTestClass::DisconnectTvOutL()"));

    //
    //  Add event for callback
    //
    TCallbackEvent* event = new TCallbackEvent;

    event->iEvent = EPbCmdTvOutEvent;
    event->iData  = EFalse;
    event->iError = KErrNone;

    AddExpectedEvent( event );

    event = new TCallbackEvent;
    event->iEvent = EPPaused;
    event->iData  = 0;
    event->iError = KErrNone;

    AddExpectedEvent( event );

    iAccObserver->UpdateTvOutStatusL( EFalse );

    return KErrNone;
}

void
CVHPPTestClass::CreateBackgroundCommand( TBool aBackground )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::CreateBackgroundCommand()"),
                   _L("aBackground = %d"), aBackground );

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
    cmd->SetTObjectValueL<TInt>( KMPXMediaVideoPlaybackCommand, aBackground );
    cmd->SetTObjectValueL<TBool>( KMPXMediaVideoAppForeground, ETrue );
    iPlaybackPlugin->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
}

TInt
CVHPPTestClass::SetTvOutConnectedL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SetTvOutConnectedL()"));
    iLog->Log(_L("CVHPPTestClass::SetTvOutConnectedL()"));

    TInt allowed = ETrue;

    TInt err = aItem.GetNextInt( allowed );

    if ( err == KErrNone )
    {
        iAccObserver->SetTvOutConnected( ETrue );
        iAccObserver->SetTvOutPlaybackAllowed( allowed );
    }

    return err;
}

TInt
CVHPPTestClass::SetTvOutDisconnectedL()
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SetTvOutDisconnectedL()"));
    iLog->Log(_L("CVHPPTestClass::SetTvOutDisconnectedL()"));

    iAccObserver->SetTvOutConnected( EFalse );
    iAccObserver->SetTvOutPlaybackAllowed( ETrue );

    return KErrNone;
}

void
CVHPPTestClass::HandleUtilityEvent( TCallbackEvent* aEvent )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandleUtilityEvent"),
                   _L("(%d, %d, %d)"), aEvent->iEvent, aEvent->iData, aEvent->iError );

    ProcessEvent( aEvent );
}

void
CVHPPTestClass::CleanupTempFiles()
{
    RFs fs;
    TInt error = fs.Connect();

    error = fs.Delete( KOpenControllerFilename );
    error = fs.Delete( KPrepareControllerFilename );
    error = fs.Delete( KPlaybackCompleteFilename );
    error = fs.Delete( KDlMgrFile );
    error = fs.Delete( KDrmRightsFile );

    fs.Close();
}

TInt
CVHPPTestClass::SetDrmProtectedL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SetDrmProtectedL()"));

    TBool drmError = EFalse;

    TInt err = aItem.GetNextInt( drmError );

    if ( err == KErrNone )
    {
        WriteControllerError( KDrmRightsFile, drmError );
    }

    return err;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::SetVolumeSteps
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::SetVolumeSteps( TInt aVolumeSteps )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::SetVolumeSteps()"));
    iLog->Log(_L("CVHPPTestClass::SetVolumeSteps()"));

    iPlayerUtility->SetVolumeSteps( aVolumeSteps );

    return KErrNone;
}

// -----------------------------------------------------------------------------
//  CVHPPTestClass::HandleVolumeL
// -----------------------------------------------------------------------------
//
TInt
CVHPPTestClass::HandleVolumeL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::HandleVolumeL()"));
    iLog->Log(_L("CVHPPTestClass::HandleVolumeL()"));

    TInt command;
    TInt value;

    TInt err = aItem.GetNextInt( command );

    if ( err == KErrNone )
    {
        err = aItem.GetNextInt( value );

        if ( err == KErrNone )
        {
            //
            // assign callback event values
            //
            TCallbackEvent* event = new TCallbackEvent;
            event->iEvent = EPVolumeChanged;
            event->iData  = value;
            event->iError = KErrNone;

            //
            // save expected result to be compared later
            //
            AddExpectedEvent( event );

            //
            // send command to playback plugin
            //
            CreateVideoPlaybackCommandL( aItem, (TMPXVideoPlaybackCommand)command );
        }
    }

    if ( err == KErrNone )
    {
        MPX_DEBUG(_L("CVHPPTestClass::HandleVolumeL() leaves with proper error"));
    }

    return err;
}


TInt
CVHPPTestClass::InitializeWithPositionL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeL()"));

    TInt duration;
    TInt volumeSteps;
    TInt position;

    TInt err = aItem.GetNextInt( duration );

    if ( err == KErrNone )
    {
        //
        //  We will always get an Init Complete message out
        //
        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = EPInitialised;
        event->iData  = duration;
        event->iError = KErrNone;

        AddExpectedEvent( event );

        //
        // read number of volume steps
        //
        err = aItem.GetNextInt( volumeSteps );

        if ( err == KErrNone )
        {
            //
            // set volume steps
            //
            SetVolumeSteps( volumeSteps );

            err = aItem.GetNextInt( position );

            if (err == KErrNone )
            {
                TBuf<120> fullPath;

                err = ReadFileInitializationParameters( aItem, fullPath );

                if ( err == KErrNone )
                {
                    PreparePluginL();

                    //
                    //  Initalize the Plugin with a file name
                    //
                    MPX_DEBUG(_L("Initialize the Plugin:  filename = %S, position = %d"), &fullPath, position);
                    iLog->Log(_L("Initialize the Plugin:  filename = %S, position = %d"), &fullPath, position);

                    iPlaybackPlugin->InitialiseWithPositionL( fullPath, position );
                }

            }
        }
    }

    return err;
}


TInt
CVHPPTestClass::InitializeLinkWithPositionL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeLinkL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeLinkL()"));

    TInt duration;
    TInt volumeSteps;
    TInt position;

    TInt err = aItem.GetNextInt( duration );

    if ( err == KErrNone )
    {
        //
        //  We will always get an Init Complete message out
        //
        TCallbackEvent* event = new TCallbackEvent;

        event->iEvent = EPInitialised;
        event->iData  = duration;
        event->iError = KErrNone;

        AddExpectedEvent( event );

        //
        // read number of volume steps
        //
        err = aItem.GetNextInt( volumeSteps );

        if ( err == KErrNone )
        {
            //
            // set volume steps
            //
            SetVolumeSteps( volumeSteps );

            err = aItem.GetNextInt( position );

            if ( err == KErrNone )
            {
                TPtrC  link;

                //
                //  Read in the link from the config file
                //
                TInt err = aItem.GetNextString( link );

                if ( err == KErrNone )
                {
                    TInt err = ReadInitializationErrors( aItem );

                    if ( err == KErrNone )
                    {
                        PreparePluginL();

                        //
                        //  Extract the streaming link from the ram file and
                        //  Initalize the Plugin with the link and an access point
                        //
                        MPX_DEBUG(_L("Initialize the Plugin:  link = %S, position = %d"), &link, position);
                        iLog->Log(_L("Initialize the Plugin:  link = %S, position = %d"), &link, position);

                        iPlaybackPlugin->InitStreamingL( link, KNullDesC8, 11, position );
                    }
                }
            }
        }
    }

    return err;
}


TInt
CVHPPTestClass::InitializeHandleWithPositionL( CStifItemParser& aItem  )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeHandleL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeHandleL()"));

    TInt duration;
    TInt volumeSteps;
    TInt position;
    TInt fileHandle32;

    TInt err = aItem.GetNextInt( fileHandle32 );

    if ( err == KErrNone )
    {

#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        //
        // set RFile as default if the 64-bit flag is not defined
        //
        fileHandle32 = ETrue;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        err = aItem.GetNextInt( duration );

        if ( err == KErrNone )
        {
            //
            //  We will always get an Init Complete message out
            //
            TCallbackEvent* event = new TCallbackEvent;

            event->iEvent = EPInitialised;
            event->iData  = duration;
            event->iError = KErrNone;

            AddExpectedEvent( event );

            //
            // read number of volume steps
            //
            err = aItem.GetNextInt( volumeSteps );

            if ( err == KErrNone )
            {
                //
                // set volume steps
                //
                SetVolumeSteps( volumeSteps );

                err = aItem.GetNextInt( position );

                if (err == KErrNone )
                {
                    TBuf<120> fullPath;

                    err = ReadFileInitializationParameters( aItem, fullPath );

                    if ( err == KErrNone )
                    {
                        PreparePluginL();

                        RFs fs;
                        TInt error = fs.Connect();

                        //
                        //  Open a file handle to the clip
                        //
                        if ( fileHandle32 )
                        {
                            RFile file;
                            file.Open( fs, fullPath, EFileRead );
                            iPlaybackPlugin->InitialiseWithPositionL( file, position );
                            file.Close();
                        }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                        else
                        {
                            RFile64 file64;
                            file64.Open( fs, fullPath, EFileRead );
                            iPlaybackPlugin->Initialise64L( file64, position );
                            file64.Close();
                        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

                        //
                        //  Initalize the Plugin with a file name
                        //
                        MPX_DEBUG(_L("Initialize the Plugin with File Handle:  filename = %S, position = %d"), &fullPath, position);
                        iLog->Log(_L("Initialize the Plugin with File Handle:  filename = %S, position = %d"), &fullPath, position);

                        fs.Close();
                    }
                }

            }
        }
    }

    return err;
}

TInt
CVHPPTestClass::InitializeStreamingWithSdpFileHandleL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CVHPPTestClass::InitializeStreamingWithSdpFileHandleL()"));
    iLog->Log(_L("CVHPPTestClass::InitializeStreamingWithSdpFileHandleL()"));

    TInt duration;
    TInt volumeSteps;
    TInt fileHandle32;

    TInt err = aItem.GetNextInt( fileHandle32 );

    if ( err == KErrNone )
    {

#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        //
        // set RFile as default if the 64-bit flag is not defined
        //
        fileHandle32 = ETrue;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        err = aItem.GetNextInt( duration );

        if ( err == KErrNone )
        {
            //
            //  We will always get an Init Complete message out
            //
            TCallbackEvent* event = new TCallbackEvent;

            event->iEvent = EPInitialised;
            event->iData  = duration;
            event->iError = KErrNone;

            AddExpectedEvent( event );

            //
            // read number of volume steps
            //
            err = aItem.GetNextInt( volumeSteps );

            if ( err == KErrNone )
            {
                //
                // set volume steps
                //
                SetVolumeSteps( volumeSteps );

                TBuf<120> fullPath;
                err = ReadFileInitializationParameters( aItem, fullPath );

                if ( err == KErrNone )
                {
                    PreparePluginL();

                    MPX_DEBUG( _L("Initialize the Plugin:  link = %S"), &fullPath );
                    iLog->Log( _L("Initialize the Plugin:  link = %S"), &fullPath );

                    //
                    //  Extract the streaming link from the ram file and
                    //  Initalize the Plugin with the file handle and an access point
                    //
                    RFs fs;
                    TInt error = fs.Connect();

                    if ( fileHandle32 )
                    {
                        RFile file;
                        error = file.Open( fs, fullPath, EFileRead | EFileShareAny );

                        MPX_DEBUG( _L("Initialize the Plugin:  file open error = %d"),
                                error );

                        User::LeaveIfError( error );

                        iPlaybackPlugin->InitStreamingL( file, 11 );
                        file.Close();
                    }
    #ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                    else
                    {
                        RFile64 file64;
                        error = file64.Open( fs, fullPath, EFileRead | EFileShareAny  );

                        MPX_DEBUG( _L("Initialize the Plugin:  file open error = %d"),
                                error );

                        User::LeaveIfError( error );

                        iPlaybackPlugin->InitStreaming64L( file64, 11 );
                        file64.Close();
                    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

                    fs.Close();
                }
            }
        }
    }
    return err;
}

TInt
CVHPPTestClass::RetrieveFileNameAndModeL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT( _L("CVHPPTestClass::RetrieveFileNameAndModeL()") );
    iLog->Log( _L("CVHPPTestClass::RetrieveFileNameAndModeL()") );

    TBuf<120>    fullPath;
    TPtrC        fileName;
    TInt        err;

    err = aItem.GetNextString( fileName );

    if ( err == KErrNone )
    {
        //
        //  Build the full path to the file
        //
        fullPath.Append( KVideoTestPath );
        fullPath.Append( fileName );

        TInt mode;
        err = aItem.GetNextInt( mode);

        if ( err == KErrNone )
        {
             CMPXCommand* cmd = CMPXCommand::NewL();
            CleanupStack::PushL( cmd );

            cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
            cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId,
                                         KMPXMediaIdVideoPlayback );
            cmd->SetTObjectValueL<TInt>( KMPXMediaVideoPlaybackCommand, EPbCmdInitView );

            iPlaybackPlugin->CommandL( *cmd );

            TPtrC clipName( cmd->ValueText( KMPXMediaVideoPlaybackFileName ) );
            TMPXVideoMode  playbackMode = (TMPXVideoMode) cmd->ValueTObjectL<TInt>( KMPXMediaVideoMode );

            MPX_DEBUG( _L("    Expected Data:  filename = %S, playbackmode= %d" ),
                 &fullPath, mode );

            MPX_DEBUG( _L("    Retrieved Data: filename = %S, playbackmode= %d"),
                 &clipName, playbackMode );

            if ( fullPath.Compare( clipName) != 0 || mode != playbackMode )
            {
                err = KErrGeneral;
                MPX_DEBUG( _L("    err = %d"), err );
            }

            CleanupStack::PopAndDestroy( cmd );
        }
    }

    return err;
}

//  EOF
