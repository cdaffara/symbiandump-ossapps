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
* Description:  Implements the ECom plugin for HTI audio playback control
*                service.
*
*/



// INCLUDE FILES
#include <apgcli.h>
#include <AudioPreference.h>
#include <bautils.h>
#include <e32std.h>
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>
#include <pathinfo.h>

#include "HtiAudioServicePlugin.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const static TInt KPlayToneCmdLength        = 13;
const static TInt KPlayTone2CmdLength       = 16;
const static TInt KStopCmdLength            = 1;
const static TInt KSetVolCmdLength          = 2;
const static TInt KSetVol2CmdLength         = 5;
const static TInt KListCmdMinLength         = 4;
const static TInt KPlayDtmfCmdMinLength     = 17;
const static TInt KPlayDtmf2CmdMinLength    = 20;
const static TInt KPlayFileCmdMinLength     = 21;
const static TInt KPlayFile2CmdMinLength    = 24;
const static TInt KDurationCmdMinLength     = 6;
const static TInt KMaxVolCmdMinLength       = 6;

const static TInt KTUintSize = sizeof( TUint );

// MACROS

// LOCAL CONSTANTS AND MACROS
const static TUid KAudioServiceUid = { 0x10210CCB };

_LIT( KBackslash, "\\" );
_LIT( KRngMimeType, "application/vnd.nokia.ringing-tone" );
_LIT( KAudioMimeType, "audio/*" );

// NOTE: Max length for error description is defined
// in HtiDispatcherInterface.h (currently 118).
_LIT8( KErrorNoCmd, "ERROR: No command given" );
_LIT8( KErrorUnknownCmd, "ERROR: Unknown Audio Service command" );
_LIT8( KErrorInvalidParameters,
    "ERROR: Invalid parameter data for this command" );
_LIT8( KErrorInvalidPath, "ERROR: Invalid path" );
_LIT8( KErrorToneInitFailed, "ERROR: Tone initialization failed" );
_LIT8( KErrorTonePlayFailed, "ERROR: Tone playing failed" );
_LIT8( KErrorFileInitFailed, "ERROR: File playing initialization failed" );
_LIT8( KErrorFilePlayFailed, "ERROR: File playing failed" );
_LIT8( KErrorBusyPlaying, "ERROR: Currently busy playing" );
_LIT8( KErrorNothingPlaying, "ERROR: Nothing playing" );
_LIT8( KErrorDurationFailed, "ERROR: Duration query failed" );
_LIT8( KErrorMaxVolFailed, "ERROR: Max volume query failed" );
_LIT8( KErrorPosition, "ERROR: Invalid start or end position value" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::CHtiAudioServicePlugin
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHtiAudioServicePlugin::CHtiAudioServicePlugin():iIsBusy( EFalse ),
                                                 iIsPlaying( EFalse ),
                                                 iCommandId( 0 ),
                                                 iPlayCommandId( 0 ),
                                                 iMessage( NULL ),
                                                 iErrorCode( 0 ),
                                                 iVolume( 0 ),
                                                 iRepeats( 0 ),
                                                 iTrailingSilence( 0 ),
                                                 iDtmfLength( 0 ),
                                                 iDtmfGapLength( 0 ),
                                                 iStartPos( 0 ),
                                                 iEndPos( 0 )
    {
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::ConstructL()
    {
    }
 

// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHtiAudioServicePlugin* CHtiAudioServicePlugin::NewL()
    {
    CHtiAudioServicePlugin* self = new (ELeave) CHtiAudioServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CHtiAudioServicePlugin::~CHtiAudioServicePlugin()
    {
    delete iMessage;
    iMessage = NULL;
    delete iAudioPlayer;
    iAudioPlayer = NULL;
    delete iTonePlayer;
    iTonePlayer = NULL;
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::ProcessMessageL
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::ProcessMessageL( const TDesC8& aMessage,
                                        THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::ProcessMessageL" );
    HTI_LOG_FORMAT( "Message length = %d", aMessage.Length() );

    if ( iIsBusy )
        {
        HTI_LOG_TEXT( "Plugin is busy - leaving" );
        User::Leave( KErrInUse );
        }

    // Will be set to EFalse in the SendResponseMsg or SendErrorResponseMsg
    // methods when the response has been successfully sent and the plugin is
    // ready for next message.
    iIsBusy = ETrue;

    if ( aMessage.Length() < 1 )
        {
        User::LeaveIfError(
            SendErrorResponseMsg( KErrArgument, KErrorNoCmd ) );
        return;
        }

    iCommandId = aMessage[0];
    HTI_LOG_FORMAT( "Command = %d", iCommandId );
    TInt err = KErrNone;

    if ( iCommandId == ECmdListAudioFiles )
        {
        TRAP( err, HandleListAudioFilesCmdL( aMessage ) );
        }

    else if ( iCommandId == ECmdPlayFile || iCommandId == ECmdPlayFile2)
        {
        TRAP( err, HandlePlayFileCmdL( aMessage ) );
        }

    else if ( iCommandId == ECmdPlayTone || iCommandId == ECmdPlayTone2)
        {
        TRAP( err, HandlePlayToneCmdL( aMessage ) );
        }

    else if ( iCommandId == ECmdPlayDtmf || iCommandId == ECmdPlayDtmf2)
        {
        TRAP( err, HandlePlayDtmfCmdL( aMessage ) );
        }

    else if ( iCommandId == ECmdStop )
        {
        TRAP( err, HandleStopCmdL( aMessage ) );
        }

    else if ( iCommandId == ECmdGetDuration )
        {
        TRAP( err, HandleGetDurationCmdL( aMessage ) );
        }

    else if ( iCommandId == ECmdGetMaxVol || iCommandId == ECmdGetMaxVol2)
        {
        TRAP( err, HandleGetMaxVolCmdL( aMessage ) );
        }

    else if ( iCommandId == ECmdSetVol || iCommandId == ECmdSetVol2)
        {
        TRAP( err, HandleSetVolCmdL( aMessage ) );
        }

    else
        {
        User::LeaveIfError(
            SendErrorResponseMsg( KErrArgument, KErrorUnknownCmd ) );
        }

    if ( err != KErrNone )
        {
        User::LeaveIfError(
                SendErrorResponseMsg( err, KNullDesC8, iCommandId ) );
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::ProcessMessageL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandleListAudioFilesCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandleListAudioFilesCmdL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandleListAudioFilesCmdL" );

    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );

    // Build a list of directories to scan
    CDesCArraySeg* directories = new (ELeave) CDesCArraySeg( 5 );
    CleanupStack::PushL( directories );

    if ( aMessage.Length() == 1 )  // Add default sound directories
        {
        TFileName directory;

        // ROM
        directory.Append( PathInfo::RomRootPath() );
        directory.Append( PathInfo::SoundsPath() );
        if ( BaflUtils::PathExists( fsSession, directory ) )
            {
            directories->AppendL( directory );
            AddSubdirsRecursivelyL( directories->MdcaPoint(
                directories->Count() - 1 ), *directories, fsSession );
            }

        // Phone memory
        directory.Zero();
        directory.Append( PathInfo::PhoneMemoryRootPath() );
        directory.Append( PathInfo::SoundsPath() );
        if ( BaflUtils::PathExists( fsSession, directory ) )
            {
            directories->AppendL( directory );
            AddSubdirsRecursivelyL( directories->MdcaPoint(
                directories->Count() - 1 ), *directories, fsSession );
            }

        // Memory card
        directory.Zero();
        directory.Append( PathInfo::MemoryCardRootPath() );
        directory.Append( PathInfo::SoundsPath() );
        if ( BaflUtils::PathExists( fsSession, directory ) )
            {
            directories->AppendL( directory );
            AddSubdirsRecursivelyL( directories->MdcaPoint(
                directories->Count() - 1 ), *directories, fsSession );
            }
        }

    else  // Add given directory
        {
        if ( aMessage.Length() < KListCmdMinLength )
            {
            User::LeaveIfError( SendErrorResponseMsg(
                                    KErrArgument, KErrorInvalidParameters ) );
            CleanupStack::PopAndDestroy( 2 ); // directories, fsSession
            return;
            }

        TInt pathLength = aMessage[1];
        if ( ( aMessage.Length() - pathLength ) != KListCmdMinLength - 2 )
            {
            User::LeaveIfError( SendErrorResponseMsg(
                                    KErrArgument, KErrorInvalidParameters ) );
            CleanupStack::PopAndDestroy( 2 ); // directories, fsSession
            return;
            }

        TFileName directory;
        TInt nextOffset = ParseString( aMessage, 1, directory );
        TInt dirLength = directory.Length();
        if ( dirLength < 2 || nextOffset < 0 )
            {
            User::LeaveIfError( SendErrorResponseMsg(
                                    KErrArgument, KErrorInvalidParameters ) );
            CleanupStack::PopAndDestroy( 2 ); // directories, fsSession
            return;
            }

        HTI_LOG_DES( directory );

        if ( directory[dirLength - 1] != '\\' )
            {
            HTI_LOG_TEXT( "Adding backslash to the end" );
            directory.Append( KBackslash );
            HTI_LOG_DES( directory );
            }

        if ( BaflUtils::PathExists( fsSession, directory ) )
            {
            HTI_LOG_TEXT( "Given path exists" );
            directories->AppendL( directory );
            AddSubdirsRecursivelyL( directories->MdcaPoint(
                directories->Count() - 1 ), *directories, fsSession );
            }
        }

    // Buffer for the file list that is returned
    CBufFlat* fileListBuf = CBufFlat::NewL( 256 );
    CleanupStack::PushL( fileListBuf );
    TInt bufPos = 0;

    TInt audioFileCount = 0;
    TInt dirCount( directories->Count() );
    HTI_LOG_FORMAT( "Total directory count = %d", dirCount );

    if ( dirCount == 0 )
        {
        HTI_LOG_TEXT( "The given directory did not exist" );
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidPath ) );
        CleanupStack::PopAndDestroy( 3 ); // fileListBuf, directories, fsSession
        return;
        }

    // Loop all the directories
    for ( TInt i = 0; i < dirCount; i++ )
        {
        HTI_LOG_TEXT( "Reading dir:" );
        HTI_LOG_DES( directories->MdcaPoint( i ) );
        CDir* dir;
        TInt err = fsSession.GetDir( directories->MdcaPoint( i ),
            KEntryAttNormal, ESortNone, dir );
        if ( err )
            {
            delete dir;
            dir = NULL;
            continue; // This dir is skipped
            }
        CleanupStack::PushL( dir );

        // Loop all the entries in this directory
        TInt fileCount( dir->Count() );
        for ( TInt j = 0; j < fileCount; j++ )
            {
            TFileName filePath;
            filePath.Copy( directories->MdcaPoint( i ) );
            filePath.Append( ( *dir )[j].iName );

            // Check MIME type match
            if ( MatchMimeTypeL( filePath, KAudioMimeType ) ||
                 MatchMimeTypeL( filePath, KRngMimeType ) )
                {
                HBufC8* filePathBuf8 = HBufC8::NewLC( KMaxFileName );
                filePathBuf8->Des().Copy( filePath );
                TInt pathLength = filePathBuf8->Length();
                HTI_LOG_DES( *filePathBuf8 );
                fileListBuf->ExpandL( bufPos, pathLength + 1 );
                TBuf8<1> lengthBuf;
                lengthBuf.Append( pathLength );
                fileListBuf->Write( bufPos, lengthBuf, 1 );
                bufPos++;
                fileListBuf->Write( bufPos, filePathBuf8->Ptr(), pathLength );
                bufPos += pathLength;
                CleanupStack::PopAndDestroy(); // filePathBuf8
                audioFileCount++;
                }

            } // files loop
        CleanupStack::PopAndDestroy(); // dir
        } // directories loop

    HTI_LOG_FORMAT( "Total audio file count = %d", audioFileCount );

    // All files added - write number of files to the beginning of buffer...
    TBuf8<2> countBuf;
    countBuf.Append( (TUint8*)(&audioFileCount), 2 );
    fileListBuf->InsertL( 0, countBuf, 2 );

    // ...and send it away
    TPtr8 ptr = fileListBuf->Ptr( 0 );
    User::LeaveIfError( SendResponseMsg( ptr ) );

    CleanupStack::PopAndDestroy( 3 ); // fileListBuf, directories, fsSession

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandleListAudioFilesCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandlePlayFileCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandlePlayFileCmdL( const TDesC8&aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandlePlayFileCmdL" );

    if ( iIsPlaying )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrInUse, KErrorBusyPlaying ) );
        return;
        }

    iPlayCommandId = (TUint8) aMessage[0];

    /*
    Message bytes:
             0      = command code
             1      = path length
         2 - n      = full path to file
        next 1 or 4 = volume
        next 4      = start position
        next 4      = end position
        next 1      = repeats
        next 4      = silence between repeats
        next 1      = audio setting
    */

    if ( aMessage.Length() < KPlayFileCmdMinLength )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    // Parse parameter values from the message
    const TUint8* ptr = aMessage.Ptr();
    TInt pathLength = aMessage[1];

    if ( (iPlayCommandId == ECmdPlayFile &&
            ( aMessage.Length() - pathLength ) !=  ( KPlayFileCmdMinLength - 4 ))
            || (iPlayCommandId == ECmdPlayFile2 && 
            ( aMessage.Length() - pathLength ) !=  ( KPlayFile2CmdMinLength - 4 )))
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    TFileName filePath;
    TInt nextOffset = ParseString( aMessage, 1, filePath );
    if ( filePath.Length() < 2 || nextOffset < 0 )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }
    HTI_LOG_TEXT( "Full file path:" );
    HTI_LOG_DES( filePath );
    if(iPlayCommandId == ECmdPlayFile)
        {
        iVolume = aMessage[nextOffset];
        nextOffset++;
        }
    else
        {
        iVolume = ParseUint32( ptr + nextOffset );
        nextOffset += 4;
        }
    HTI_LOG_FORMAT( "Volume = %d", iVolume );
    iStartPos = ParseUint32( ptr + nextOffset );
    HTI_LOG_FORMAT( "Start position = %d", iStartPos );
    nextOffset += 4;
    iEndPos = ParseUint32( ptr + nextOffset );
    HTI_LOG_FORMAT( "End position = %d", iEndPos );
    nextOffset += 4;
    iRepeats = aMessage[nextOffset];
    nextOffset++;
    HTI_LOG_FORMAT( "Repeats = %d", iRepeats );
    iTrailingSilence = ParseUint32( ptr + nextOffset );
    HTI_LOG_FORMAT( "Trailing silence = %d", iTrailingSilence );
    nextOffset += 4;
    TInt audioSetting = aMessage[nextOffset];
    HTI_LOG_FORMAT( "Audio setting = %d", audioSetting );

    // Set audio settings
    if ( audioSetting > ERingTonePreview ) audioSetting = EDefault;
    SetAudioSettings( ( TAudioSetting ) audioSetting );

    // Check if file is rng ringtone, it has to be played using tone player -
    // other formats are played with audio player.

    TInt err = KErrNone;
    TBool isRng = EFalse;
    TRAP( err, isRng = MatchMimeTypeL( filePath, KRngMimeType ) );

    if ( err )
        {
        User::LeaveIfError( SendErrorResponseMsg( err, KErrorFileInitFailed ) );
        return;
        }

    if ( isRng )
        {
        HTI_LOG_TEXT( "File is RNG - creating tone player" );
        TRAP( err, iTonePlayer = CMdaAudioToneUtility::NewL(
                *this, NULL, iAudioPriority, iAudioPriorityPreference ) );
        }

    else
        {
        HTI_LOG_TEXT( "File is not RNG - creating audio player" );
        TRAP( err, iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(
                filePath, *this, iAudioPriority, iAudioPriorityPreference ) );
        // MapcInitComplete callback function will be called
        }

    if ( err )
        {
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        delete iTonePlayer;
        iTonePlayer = NULL;
        User::LeaveIfError( SendErrorResponseMsg(
                                err, KErrorFileInitFailed ) );
        }

    if ( iTonePlayer )
        {
        iTonePlayer->PrepareToPlayFileSequence( filePath );
        // MatoPrepareComplete callback function will be called
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandlePlayFileCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandlePlayToneCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandlePlayToneCmdL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandlePlayToneCmdL" );

    if ( iIsPlaying )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrInUse, KErrorBusyPlaying ) );
        return;
        }

    iPlayCommandId = (TUint8)aMessage[0];

    /*
    Message bytes(ECmdPlayTone):
            0  = command code
        1 - 2  = frequency value
        3 - 6  = duration value
            7  = volume value
            8  = repeat value
        9 - 12 = silence between repeats
            
    Message bytes(ECmdPlayTone2):
            0  = command code
        1 - 2  = frequency value
        3 - 6  = duration value
        7 - 10 = volume value
            11 = repeat value
        12- 15 = silence between repeats
    */

    if ( (iPlayCommandId == ECmdPlayTone && aMessage.Length() != KPlayToneCmdLength)
            || (iPlayCommandId == ECmdPlayTone2 && aMessage.Length() != KPlayTone2CmdLength))
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    // Parse parameter values from the message
    const TUint8* ptr = aMessage.Ptr();
    TInt nextOffset = 1;
    TInt frequency = ParseUint16( ptr + nextOffset );
    HTI_LOG_FORMAT( "Freq = %d", frequency );
    nextOffset += 2;
    TUint duration = ParseUint32( ptr + 3 );
    HTI_LOG_FORMAT( "Duration = %d", duration );
    nextOffset += 4;
    if(iPlayCommandId == ECmdPlayTone)
        {
        iVolume = aMessage[nextOffset];
        nextOffset++;
        }
    else
        {
        iVolume = ParseUint32( ptr + nextOffset );
        nextOffset += 4;
        }
    HTI_LOG_FORMAT( "Volume = %d", iVolume );
    iRepeats = aMessage[nextOffset];
    nextOffset++;
    HTI_LOG_FORMAT( "Repeats = %d", iRepeats );
    iTrailingSilence = ParseUint32( ptr + nextOffset );
    nextOffset +=4;
    HTI_LOG_FORMAT( "Silence = %d", iTrailingSilence );

    TRAPD( err, iTonePlayer = CMdaAudioToneUtility::NewL( *this ) );

    if ( err )
        {
        delete iTonePlayer;
        iTonePlayer = NULL;
        User::LeaveIfError( SendErrorResponseMsg(
                                err, KErrorToneInitFailed ) );
        }

    iTonePlayer->PrepareToPlayTone( frequency,
                              TTimeIntervalMicroSeconds( duration ) );
    // MatoPrepareComplete callback function will be called when ready to play
    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandlePlayToneCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandlePlayDtmfCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandlePlayDtmfCmdL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandlePlayDtmfCmdL" );

    if ( iIsPlaying )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrInUse, KErrorBusyPlaying ) );
        return;
        }

    iPlayCommandId = (TUint8)aMessage[0];

    /*
    Message bytes:
             0      = command code
             1      = DTMF string length
         2 - n      = dtmf string
        next 4      = tone length
        next 4      = tone gap length
        next 1 or 4 = volume
        next 1      = repeats
        next 4      = silence between repeats
    */

    if ( (iPlayCommandId == ECmdPlayDtmf && aMessage.Length() < KPlayDtmfCmdMinLength )
            || (iPlayCommandId == ECmdPlayDtmf2 && aMessage.Length() < KPlayDtmf2CmdMinLength ))
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    // Parse parameter values from the message
    const TUint8* ptr = aMessage.Ptr();
    TInt stringLength = aMessage[1];

    if ( (iPlayCommandId == ECmdPlayDtmf && ( aMessage.Length() - stringLength ) !=
         ( KPlayDtmfCmdMinLength - 1 ))
         || (iPlayCommandId == ECmdPlayDtmf2 && ( aMessage.Length() - stringLength ) !=
                 ( KPlayDtmf2CmdMinLength - 1 )))
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    TBuf<255> dtmfString;
    TInt nextOffset = ParseString( aMessage, 1, dtmfString );
    if ( dtmfString.Length() < 1 || nextOffset < 0 )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }
    HTI_LOG_TEXT( "DTMF string:" );
    HTI_LOG_DES( dtmfString );
    iDtmfLength = ParseUint32( ptr + nextOffset );
    nextOffset += 4;
    HTI_LOG_FORMAT( "DTMF length = %d", iDtmfLength );
    iDtmfGapLength = ParseUint32( ptr + nextOffset );
    nextOffset += 4;
    HTI_LOG_FORMAT( "DTMF gap length = %d", iDtmfGapLength );
    if(iPlayCommandId == ECmdPlayDtmf)
        {
        iVolume = aMessage[nextOffset];
        nextOffset++;
        }
    else
        {
        iVolume = ParseUint32( ptr + nextOffset );
        nextOffset += 4;
        }
    HTI_LOG_FORMAT( "Volume = %d", iVolume );
    iRepeats = aMessage[nextOffset];
    nextOffset++;
    HTI_LOG_FORMAT( "Repeats = %d", iRepeats );
    iTrailingSilence = ParseUint32( ptr + nextOffset );
    HTI_LOG_FORMAT( "Trailing silence = %d", iTrailingSilence );

    SetAudioSettings( EDtmfString );

    TRAPD( err, iTonePlayer = CMdaAudioToneUtility::NewL(
            *this, NULL, iAudioPriority, iAudioPriorityPreference ) );

    if ( err )
        {
        delete iTonePlayer;
        iTonePlayer = NULL;
        User::LeaveIfError( SendErrorResponseMsg( err, KErrorToneInitFailed ) );
        }

    iTonePlayer->PrepareToPlayDTMFString( dtmfString );
    // MatoPrepareComplete callback function will be called when ready to play
    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandlePlayDtmfCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandleStopCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandleStopCmdL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandleStopCmdL" );

    if ( aMessage.Length() != KStopCmdLength )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    if ( !iIsPlaying )
        {
        HTI_LOG_TEXT( "Not playing - nothing to stop" );
        // Just send "OK" reply if nothing is currently playing
        User::LeaveIfError( SendResponseMsg( _L8( "OK" ) ) );
        }

    else
        {
        if ( iAudioPlayer )
            {
            HTI_LOG_TEXT( "Stopping audio player" );
            iAudioPlayer->Stop();
            iIsPlaying = EFalse;
            delete iAudioPlayer;
            iAudioPlayer = NULL;
            // According to documentation should call MapcPlayComplete callback
            // method but it doesn't, so sending reply here.
            User::LeaveIfError( SendResponseMsg( _L8( "OK" ) ) );
            }

        else if ( iTonePlayer )
            {
            HTI_LOG_TEXT( "Stopping tone player" );
            iTonePlayer->CancelPlay();
            iIsPlaying = EFalse;
            delete iTonePlayer;
            iTonePlayer = NULL;
            // Callback method MatoPlayComplete is not called -
            // sending reply here.
            User::LeaveIfError( SendResponseMsg( _L8( "OK" ) ) );
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandleStopCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandleGetDurationCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandleGetDurationCmdL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandleGetDurationCmdL" );

    if ( iIsPlaying )
        {
         // If currently playing, no parameters allowed. Returns the duration
         // of currently playing sound.
        if ( aMessage.Length() != 1 )
            {
            User::LeaveIfError( SendErrorResponseMsg(
                                    KErrInUse, KErrorBusyPlaying ) );
            return;
            }

        if ( iAudioPlayer )
            {
            TTimeIntervalMicroSeconds durationValue =
                                iAudioPlayer->Duration();

            if ( I64HIGH( durationValue.Int64() ) > 0 )
                {
                User::LeaveIfError( SendErrorResponseMsg(
                                        KErrOverflow, KErrorDurationFailed ) );
                return;
                }
            else
                {
                TUint duration = I64LOW( durationValue.Int64() );
                TBuf8<KTUintSize> durationBuf;
                durationBuf.Append( (TUint8*)(&duration), KTUintSize );
                User::LeaveIfError( SendResponseMsg( durationBuf ) );
                return;
                }
            }

        else  // Duration supported only for audio player
            {
            User::LeaveIfError( SendErrorResponseMsg(
                                    KErrNotSupported, KErrorDurationFailed ) );
            return;
            }
        }

    /* Command must have file path parameter if not currently playing.
    Message bytes:
             0 = command code
             1 = path length
         2 - n = full path to file
    */

    if ( aMessage.Length() < KDurationCmdMinLength )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    TInt pathLength = aMessage[1];
    if ( ( aMessage.Length() - pathLength ) !=
         ( KDurationCmdMinLength - 4 ) )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    // Parse parameter values from the message
    TFileName filePath;
    TInt nextOffset = ParseString( aMessage, 1, filePath );
    if ( filePath.Length() < 2 || nextOffset < 0 )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }
    HTI_LOG_TEXT( "Full file path:" );
    HTI_LOG_DES( filePath );

    TRAPD( err, iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(
                                                        filePath, *this ) );
    if ( err )
        {
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        User::LeaveIfError( SendErrorResponseMsg( err, KErrorDurationFailed ) );
        }

    // MapcInitComplete callback function will be called
    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandleGetDurationCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandleGetMaxVolCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandleGetMaxVolCmdL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandleGetMaxVolCmdL" );

    if ( iIsPlaying )
        {
         // If currently playing, no parameters allowed. Returns the max volume
         // of currently playing sound.
        if ( aMessage.Length() != 1 )
            {
            User::LeaveIfError( SendErrorResponseMsg(
                                    KErrInUse, KErrorBusyPlaying ) );
            return;
            }

        TInt maxVol = -1;

        if ( iAudioPlayer )
            {
            maxVol = iAudioPlayer->MaxVolume();
            }

        else if ( iTonePlayer )
            {
            maxVol = iTonePlayer->MaxVolume();
            }

        HTI_LOG_FORMAT( "Max volume = %d", maxVol );

        if ( maxVol < 0 )
            {
            // Should not happen
            User::LeaveIfError( SendErrorResponseMsg(
                                    KErrGeneral, KErrorMaxVolFailed ) );
            return;
            }

        if(aMessage[0] == ECmdGetMaxVol)
            {
            if ( maxVol > 255 ) maxVol = 255;
            TBuf8<1> maxVolBuf;
            maxVolBuf.Append( maxVol );
            User::LeaveIfError( SendResponseMsg( maxVolBuf ) );
            }
        else
            {
            TBuf8<4> maxVolBuf;
            maxVolBuf.Append( (TUint8*) &maxVol, 4 );
            User::LeaveIfError( SendResponseMsg( maxVolBuf ) );
            }
        return;
        }

    /*
    Message bytes:
             0 = command code
             1 = path length
         2 - n = full path to file
    */

    if ( aMessage.Length() < KMaxVolCmdMinLength )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    TInt pathLength = aMessage[1];
    if ( ( aMessage.Length() - pathLength ) != ( KMaxVolCmdMinLength - 4 ) )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }

    // Parse parameter values from the message
    TFileName filePath;
    TInt nextOffset = ParseString( aMessage, 1, filePath );
    if ( filePath.Length() < 2 || nextOffset < 0 )
        {
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrArgument, KErrorInvalidParameters ) );
        return;
        }
    HTI_LOG_TEXT( "Full file path:" );
    HTI_LOG_DES( filePath );

    TInt err = KErrNone;
    TBool isRng = EFalse;
    TRAP( err, isRng = MatchMimeTypeL( filePath, KRngMimeType ) );

    if ( err )
        {
        User::LeaveIfError( SendErrorResponseMsg( err, KErrorMaxVolFailed ) );
        return;
        }

    if ( isRng )
        {
        HTI_LOG_TEXT( "File is RNG - creating tone player" );
        TRAP( err, iTonePlayer = CMdaAudioToneUtility::NewL( *this ) );
        }

    else
        {
        HTI_LOG_TEXT( "File is not RNG - creating audio player" );
        TRAP( err, iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(
                                                        filePath, *this ) );
        // MapcInitComplete callback function will be called
        }

    if ( err )
        {
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        delete iTonePlayer;
        iTonePlayer = NULL;
        User::LeaveIfError( SendErrorResponseMsg( err, KErrorMaxVolFailed ) );
        }

    if ( iTonePlayer )
        {
        iTonePlayer->PrepareToPlayFileSequence( filePath );
        // MatoPrepareComplete callback function will be called
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandleGetMaxVolCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::HandleSetVolCmdL()
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::HandleSetVolCmdL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::HandleSetVolCmdL" );

    if ( (aMessage[0] == ECmdSetVol && aMessage.Length() != KSetVolCmdLength )
            || (aMessage[0] == ECmdSetVol2 && aMessage.Length() != KSetVol2CmdLength ))
        {
        User::LeaveIfError( SendErrorResponseMsg( KErrArgument,
                                                  KErrorInvalidParameters ) );
        }

    if ( !iIsPlaying )
        {
        HTI_LOG_TEXT( "Nothing playing - not setting volume" );
        User::LeaveIfError( SendErrorResponseMsg(
                                KErrNotReady, KErrorNothingPlaying ) );
        }

    else if(aMessage[0] == ECmdSetVol)
        {
        TInt volume = aMessage[1]; // [0] = command code, [1] = volume value
        HTI_LOG_FORMAT( "requested volume = %d", volume );

        if ( iAudioPlayer )
            {
            HTI_LOG_TEXT( "Setting audio player volume" );
            TInt maxVol = iAudioPlayer->MaxVolume();
            HTI_LOG_FORMAT( "max volume = %d", maxVol );
            if ( volume > maxVol ) volume = maxVol;
            iAudioPlayer->SetVolume( volume );
            TBuf8<1> volBuf;
            volBuf.Append( volume );
            User::LeaveIfError( SendResponseMsg( volBuf ) );
            }
        else if ( iTonePlayer )
            {
            HTI_LOG_TEXT( "Setting tone player volume" );
            TInt maxVol = iTonePlayer->MaxVolume();
            HTI_LOG_FORMAT( "max volume = %d", maxVol );
            if ( volume > maxVol ) volume = maxVol;
            iTonePlayer->SetVolume( volume );
            TBuf8<1> volBuf;
            volBuf.Append( volume );
            User::LeaveIfError( SendResponseMsg( volBuf ) );
            }
        }
    else if(aMessage[0] == ECmdSetVol2)
        {
        TInt volume = aMessage[1] + (aMessage[2] << 8)
                + (aMessage[3] << 16) + (aMessage[4] << 24);
        HTI_LOG_FORMAT( "requested volume = %d", volume );
    
        if ( iAudioPlayer )
            {
            HTI_LOG_TEXT( "Setting audio player volume" );
            TInt maxVol = iAudioPlayer->MaxVolume();
            HTI_LOG_FORMAT( "max volume = %d", maxVol );
            if ( volume > maxVol ) volume = maxVol;
            iAudioPlayer->SetVolume( volume );
            TBuf8<4> volBuf;
            volBuf.Append( (TUint8*)&volume, 4 );
            User::LeaveIfError( SendResponseMsg( volBuf ) );
            }
        else if ( iTonePlayer )
            {
            HTI_LOG_TEXT( "Setting tone player volume" );
            TInt maxVol = iTonePlayer->MaxVolume();
            HTI_LOG_FORMAT( "max volume = %d", maxVol );
            if ( volume > maxVol ) volume = maxVol;
            iTonePlayer->SetVolume( volume );
            TBuf8<4> volBuf;
            volBuf.Append( (TUint8*)&volume, 4 );
            User::LeaveIfError( SendResponseMsg( volBuf ) );
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::HandleSetVolCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::MatoPrepareComplete()
// Tone player prepare complete
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::MatoPrepareComplete( TInt aError )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::MatoPrepareComplete" );

    if ( iCommandId == ECmdGetMaxVol || iCommandId == ECmdGetMaxVol2)
        {
        if ( aError )
            {
            SendErrorResponseMsg( aError, KErrorMaxVolFailed );
            }

        else
            {
            TInt maxVol = iTonePlayer->MaxVolume();
            HTI_LOG_FORMAT( "Max volume = %d", maxVol );
            if(iCommandId == ECmdGetMaxVol)
                {
                if ( maxVol > 255 ) maxVol = 255;
                TBuf8<1> maxVolBuf;
                maxVolBuf.Append( maxVol );
                SendResponseMsg( maxVolBuf );
                }
            else
                {
                TBuf8<4> maxVolBuf;
                maxVolBuf.Append( (TUint8*)&maxVol, 4 );
                SendResponseMsg( maxVolBuf );
                }
            }

        delete iTonePlayer;
        iTonePlayer = NULL;
        return;
        }

    if ( aError )
        {
        SendErrorResponseMsg( aError, KErrorToneInitFailed );
        delete iTonePlayer;
        iTonePlayer = NULL;
        }

    else
        {
        if ( iCommandId == ECmdPlayDtmf || iCommandId == ECmdPlayDtmf2)
            {
            iTonePlayer->SetDTMFLengths(
                TTimeIntervalMicroSeconds32( iDtmfLength ),
                TTimeIntervalMicroSeconds32( iDtmfGapLength ),
                TTimeIntervalMicroSeconds32( 0 ) );
            }

        if ( iVolume > iTonePlayer->MaxVolume() )
            {
            iVolume = iTonePlayer->MaxVolume();
            }

        iTonePlayer->SetVolume( iVolume );
        iTonePlayer->SetRepeats( iRepeats + 1,
                        TTimeIntervalMicroSeconds( iTrailingSilence ) );
        iIsPlaying = ETrue;
        iTonePlayer->Play();
        iIsBusy = EFalse;
        // MatoPlayComplete callback function will be called when playing ends.
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::MatoPrepareComplete" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::MatoPlayComplete()
// Tone play complete
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::MatoPlayComplete( TInt aError )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::MatoPlayComplete" );

    iIsPlaying = EFalse;
    iIsBusy = ETrue; // Busy dispatching the play complete message

    if ( aError )
        {
        SendErrorResponseMsg( aError, KErrorTonePlayFailed, iPlayCommandId );
        }

    else
        {
        SendResponseMsg( _L8( "OK" ), iPlayCommandId );
        }

    delete iTonePlayer;
    iTonePlayer = NULL;

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::MatoPlayComplete" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::MapcInitComplete()
// Audio player init complete
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::MapcInitComplete( TInt aError,
                                 const TTimeIntervalMicroSeconds& aDuration )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::MapcInitComplete" );

    if ( iCommandId == ECmdPlayFile || iCommandId == ECmdPlayFile2)
        {
        if ( aError )
            {
            SendErrorResponseMsg( aError, KErrorFileInitFailed );
            delete iAudioPlayer;
            iAudioPlayer = NULL;
            }

        else
            {
            if ( iEndPos < iStartPos ||
                        TTimeIntervalMicroSeconds( iStartPos ) > aDuration )
                {
                SendErrorResponseMsg( KErrArgument, KErrorPosition );
                delete iAudioPlayer;
                iAudioPlayer = NULL;
                return;
                }

            if ( iEndPos > 0 )
                {
                iAudioPlayer->SetPlayWindow(
                    TTimeIntervalMicroSeconds( iStartPos ),
                    TTimeIntervalMicroSeconds( iEndPos ) );
                }

            HTI_LOG_FORMAT( "Max volume = %d", iAudioPlayer->MaxVolume() );
            HTI_LOG_FORMAT( "Setting volume = %d", iVolume );
            if ( iVolume > iAudioPlayer->MaxVolume() )
                {
                iVolume = iAudioPlayer->MaxVolume();
                }

            iAudioPlayer->SetVolume( iVolume );
            iAudioPlayer->SetRepeats( iRepeats,
                            TTimeIntervalMicroSeconds( iTrailingSilence ) );
            iIsPlaying = ETrue;
            iAudioPlayer->Play();

            // Have to do this after play command because
            // volume setting before play seems to have no effect.
            iAudioPlayer->SetVolume( 0 );
            iAudioPlayer->SetVolume( iVolume );

            iIsBusy = EFalse;
            // MapcPlayComplete callback function is called when playing ends
            }
        }

    else if ( iCommandId == ECmdGetDuration )
        {
        if ( aError )
            {
            SendErrorResponseMsg( aError, KErrorDurationFailed );
            }

        else
            {
            if ( I64HIGH( aDuration.Int64() ) > 0 )
                {
                SendErrorResponseMsg( KErrOverflow, KErrorDurationFailed );
                }
            else
                {
                TUint duration = I64LOW( aDuration.Int64() );
                TBuf8<KTUintSize> durationBuf;
                durationBuf.Append( (TUint8*)(&duration), KTUintSize );
                SendResponseMsg( durationBuf );
                }
            }
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        }

    else if ( iCommandId == ECmdGetMaxVol || iCommandId == ECmdGetMaxVol2)
        {
        if ( aError )
            {
            SendErrorResponseMsg( aError, KErrorMaxVolFailed );
            }

        else
            {
            TInt maxVol = iAudioPlayer->MaxVolume();
            HTI_LOG_FORMAT( "Max volume = %d", maxVol );
            if(iCommandId == ECmdGetMaxVol)
                {
                if ( maxVol > 255 ) maxVol = 255;
                TBuf8<1> maxVolBuf;
                maxVolBuf.Append( maxVol );
                SendResponseMsg( maxVolBuf );
                }
            else
                {
                TBuf8<4> maxVolBuf;
                maxVolBuf.Append( (TUint8*)&maxVol, 4 );
                SendResponseMsg( maxVolBuf );
                }
            }
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::MapcInitComplete" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::MapcPlayComplete()
// Audio play complete
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::MapcPlayComplete( TInt aError )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::MapcPlayComplete" );

    iIsPlaying = EFalse;
    iIsBusy = ETrue; // Busy dispatching the play complete message

    if ( aError )
        {
        SendErrorResponseMsg( aError, KErrorFilePlayFailed, ECmdPlayFile );
        }

    else
        {
        SendResponseMsg( _L8( "OK" ), iPlayCommandId );
        }

    delete iAudioPlayer;
    iAudioPlayer = NULL;

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::MapcPlayComplete" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::NotifyMemoryChange
// Called when HTI Framework has dispatched a message forward and the amount
// of free memory in the message queue has changed.
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::NotifyMemoryChange( TInt aAvailableMemory )
    {
    if ( iIsBusy && iMessage )
        {
        if ( aAvailableMemory >= iMessage->Size() )
            {
            if ( iErrorCode == 0 )
                {
                TInt err = iDispatcher->DispatchOutgoingMessage(
                    iMessage, KAudioServiceUid );

                if ( err == KErrNone )
                    {
                    // Ownership of iMessage has been transferred
                    iMessage = NULL;
                    iIsBusy = EFalse;
                    iDispatcher->RemoveMemoryObserver( this );
                    }

                else if ( err == KErrNoMemory )
                    {
                    // Keep retrying.
                    }

                else // Give up on sending
                    {
                    delete iMessage;
                    iMessage = NULL;
                    iIsBusy = EFalse;
                    iDispatcher->RemoveMemoryObserver( this );
                    }
                }

            else
                {
                TInt err = iDispatcher->DispatchOutgoingErrorMessage(
                    iErrorCode, *iMessage, KAudioServiceUid );

                // If it was success or some other error than KErrNoMemory
                // we are done sending or trying to send this message.
                if ( err != KErrNoMemory )
                    {
                    delete iMessage;
                    iMessage = NULL;
                    iIsBusy = EFalse;
                    iDispatcher->RemoveMemoryObserver( this );
                    }

                else
                    {
                    // Keep retrying.
                    }
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::IsBusy
// -----------------------------------------------------------------------------
//
TBool CHtiAudioServicePlugin::IsBusy()
    {
    return iIsBusy;
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::SendResponseMsg
// Sends a message out to the message dispatcher.
// -----------------------------------------------------------------------------
//
TInt CHtiAudioServicePlugin::SendResponseMsg( const TDesC8& aMsg,
                                              const TUint8 aCommandId )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::SendResponseMsg" );

    iErrorCode = 0;

    if ( iDispatcher == NULL )
        {
        iIsBusy = EFalse;
        return KErrGeneral;
        }

    iDispatcher->RemoveMemoryObserver( this );

    delete iMessage;
    iMessage = NULL;
    iMessage = HBufC8::New( aMsg.Length() + 1 );

    if ( iMessage == NULL )
        {
        iIsBusy = EFalse;
        return KErrNoMemory;
        }

    TPtr8 ptr8 = iMessage->Des();
    if ( aCommandId != 0 )
        {
        ptr8.Append( aCommandId );
        }
    else
        {
        ptr8.Append( iCommandId );
        }

    ptr8.Append( aMsg );

    TInt err = KErrNone;

    err = iDispatcher->DispatchOutgoingMessage( iMessage, KAudioServiceUid );

    if ( err == KErrNoMemory )
        {
        HTI_LOG_TEXT( "Message queue memory full - waiting" );
        iIsBusy = ETrue; // Should already be true, but just in case
        iDispatcher->AddMemoryObserver( this );
        // For the caller of this method all is OK, sending is just delayed
        err = KErrNone;
        }

    else if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "Message sent to dispatcher" );
        iMessage = NULL; // Ownership of iMessage has been transferred
        iIsBusy = EFalse;
        }

    else // give up on sending
        {
        HTI_LOG_FORMAT( "Other dispatcher error %d", err );
        delete iMessage;
        iMessage = NULL;
        iIsBusy = EFalse;
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::SendResponseMsg" );
    return err;
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::SendErrorResponseMsg
// Sends an error message out to the message dispatcher.
// -----------------------------------------------------------------------------
//
TInt CHtiAudioServicePlugin::SendErrorResponseMsg( TInt aErrorCode,
                        const TDesC8& aErrorDescription,
                        const TUint8 aCommandId )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::SendErrorResponseMsg" );

    iErrorCode = aErrorCode;

    if ( iDispatcher == NULL )
        {
        iIsBusy = EFalse;
        return KErrGeneral;
        }

    iDispatcher->RemoveMemoryObserver( this );

    delete iMessage;
    iMessage = NULL;
    iMessage = HBufC8::New( aErrorDescription.Length() + 1 );

    if ( iMessage == NULL )
        {
        iIsBusy = EFalse;
        return KErrNoMemory;
        }

    TPtr8 ptr8 = iMessage->Des();
    if ( aCommandId != 0 )
        {
        ptr8.Append( aCommandId );
        }
    else
        {
        ptr8.Append( iCommandId );
        }

    ptr8.Append( aErrorDescription );

    TInt err = KErrNone;

    err = iDispatcher->DispatchOutgoingErrorMessage(
        aErrorCode, *iMessage, KAudioServiceUid );

    if ( err == KErrNoMemory )
        {
        HTI_LOG_TEXT( "Message queue memory full - waiting" );
        iIsBusy = ETrue; // Should already be true, but just in case
        iDispatcher->AddMemoryObserver( this );
        // For the caller of this method all is OK, sending is just delayed
        err = KErrNone;
        }

    else if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "Error message sent to dispatcher" );
        delete iMessage;
        iMessage = NULL;
        iIsBusy = EFalse;
        }

    else // give up on sending
        {
        HTI_LOG_FORMAT( "Other dispatcher error %d", err );
        delete iMessage;
        iMessage = NULL;
        iIsBusy = EFalse;
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::SendErrorResponseMsg" );
    return err;
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::ParseString()
// -----------------------------------------------------------------------------
//
TInt CHtiAudioServicePlugin::ParseString( const TDesC8& aRequest,
                                          TInt aOffset,
                                          TDes& aResult )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::ParseString" );

    // If offset outside the string return empty string
    if ( aOffset >= aRequest.Size() )
        {
        return aOffset;
        }

    TInt length = aRequest[aOffset];
    HTI_LOG_FORMAT( "String length = %d", length );

    // If length is zero return empty string
    if ( length < 1 )
        {
        return aOffset + 1;
        }

    if ( length > aResult.MaxLength() )
        {
        return KErrBadDescriptor;
        }

    TInt nextOffset = length + aOffset + 1;
    HTI_LOG_FORMAT( "Next offset = %d", nextOffset );
    HTI_LOG_FORMAT( "Request size = %d", aRequest.Size() );

    if ( nextOffset > aRequest.Size() )
        {
        return KErrArgument;
        }

    aResult.Copy( aRequest.Mid( aOffset + 1, length ) );

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::ParseString" );
    return nextOffset;
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::AddSubdirsRecursivelyL
// Scan all subdirectories from the given path and add them to the aArray.
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::AddSubdirsRecursivelyL( const TDesC& aPath,
                                                     CDesCArraySeg& aArray,
                                                     RFs& aFs )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::AddSubdirsRecursivelyL" );

    CDirScan* dirScan = CDirScan::NewL( aFs );
    CleanupStack::PushL( dirScan );

    TFileName* path = new (ELeave) TFileName;
    CleanupStack::PushL( path );

    CDir* directory = NULL;
    TPtrC currentPath;

    dirScan->SetScanDataL( aPath, KEntryAttMatchExclusive | KEntryAttDir, ESortNone );
    dirScan->NextL( directory );

    while ( directory )
        {
        CleanupStack::PushL( directory );
        currentPath.Set( dirScan->FullPath() );

        TInt dirCount( directory->Count() );
        for ( TInt i = 0; i < dirCount ; ++i )
            {
            path->Copy( currentPath );
            path->Append( ( *directory )[ i ].iName );
            path->Append( KBackslash );
            aArray.AppendL( *path );
            }

        CleanupStack::PopAndDestroy( directory );
        directory = NULL;

        dirScan->NextL( directory );
        }

    CleanupStack::PopAndDestroy( 2 ); // path, dirScan

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::AddSubdirsRecursivelyL" );
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::MatchMimeTypeL
// Check if the MIME type of the given file matches to the given pattern.
// -----------------------------------------------------------------------------
//
TBool CHtiAudioServicePlugin::MatchMimeTypeL( const TDesC& aFilePath,
                      const TDesC& aMimeTypeMatchPattern )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::MatchMimeTypeL" );

    RApaLsSession apaSession;
    User::LeaveIfError( apaSession.Connect() );
    CleanupClosePushL( apaSession );

    TUid dummyUid( KNullUid );
    TDataType dataType;
    User::LeaveIfError( apaSession.AppForDocument( aFilePath,
                                                   dummyUid,
                                                   dataType ) );
    CleanupStack::PopAndDestroy(); // apaSession

    if ( dataType.Des().MatchF( aMimeTypeMatchPattern ) >= 0 )
        {
        HTI_LOG_TEXT( "Match" );
        return ETrue;
        }

    HTI_LOG_TEXT( "Not match" );
    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::MatchMimeTypeL" );
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CHtiAudioServicePlugin::SetAudioSettings
// Set the audio priority and priority preference values.
// -----------------------------------------------------------------------------
//
void CHtiAudioServicePlugin::SetAudioSettings( TAudioSetting aSetting )
    {
    HTI_LOG_FUNC_IN( "CHtiAudioServicePlugin::SetAudioSettings" );
    HTI_LOG_FORMAT( "Setting values for audio setting %d", aSetting );

    switch ( aSetting )
        {
        case EGeneralMusic:
            {
            iAudioPriority = KAudioPriorityRealOnePlayer;
            iAudioPriorityPreference =
                ( TMdaPriorityPreference ) KAudioPrefRealOneLocalPlayback;
            break;
            }

        case ERingTonePreview:
            {
            iAudioPriority = KAudioPriorityRingingTonePreview;
            iAudioPriorityPreference =
                ( TMdaPriorityPreference ) KAudioPrefRingFilePreview;
            break;
            }
/*
        case EIncomingCall:
            {
            iAudioPriority = KAudioPriorityPhoneCall;
            iAudioPriorityPreference =
                ( TMdaPriorityPreference ) KAudioPrefIncomingCall;
            break;
            }
*/
        case EDtmfString:
            {

            iAudioPriority = KAudioPriorityDTMFString;
            iAudioPriorityPreference =
                ( TMdaPriorityPreference ) KAudioDTMFString;
            break;
            }

        default:
            {
            iAudioPriority = EMdaPriorityNormal;
            iAudioPriorityPreference = EMdaPriorityPreferenceTimeAndQuality;
            break;
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiAudioServicePlugin::SetAudioSettings" );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
