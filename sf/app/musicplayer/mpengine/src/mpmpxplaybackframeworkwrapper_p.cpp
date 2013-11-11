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
* Description: Framework wraper for Playback - Private.
*
*/

#include <mpxplaybackutility.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxplaybackframeworkdefs.h>
#include <hbglobal.h>
#include <xqsharablefile.h>
#include <mpxcollectionplaylist.h>

#include "mpmpxplaybackframeworkwrapper_p.h"
#include "mpmpxplaybackframeworkwrapper.h"
#include "mpplaybackdata.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpxaudioeffectengine.h"
#include "mpsongdata.h"
#include "mpxcollectionpath.h"

_LIT(KMPXPnRealAudioMimeType, "audio/x-pn-realaudio");
_LIT(KMPXRealAudioMimeType, "audio/x-realaudio");
_LIT(KMPXRnRealAudioMimeType, "audio/vnd.rn-realaudio");
/*!
    \class MpMpxPlaybackFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxPlaybackFrameworkWrapperPrivate::MpMpxPlaybackFrameworkWrapperPrivate( MpMpxPlaybackFrameworkWrapper *qq)
    : q_ptr(qq),
      iPlaybackUtility(0),
      iEmbeddedPlaybackHelper(0),
      iPlaybackData(0),
      iDetailsRequest(false)
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxPlaybackFrameworkWrapperPrivate::~MpMpxPlaybackFrameworkWrapperPrivate()
{
    TX_ENTRY
    if ( iPlaybackUtility ) {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL(*this) );
        iPlaybackUtility->Close();
    }
    delete iEmbeddedPlaybackHelper;
    delete iPlaybackData;
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::init( quint32 clientSecureId, MpSongData *songData )
{
    TX_ENTRY
    iHostUid = TUid::Uid( clientSecureId );
    iSongData = songData;
    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::play( QString aFilename )
{
    TRAPD( err, DoPlayL(aFilename) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::play( const XQSharableFile& file )
{
    TRAPD( err, DoPlayL( file ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::play()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL(EPbCmdPlay) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::pause()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL(EPbCmdPause) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::playPause()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdPlayPause));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::stop()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdStop));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::skipForward()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdNext));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::startSeekForward()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdStartSeekForward));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::stopSeeking()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdStopSeeking));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::skipBackward()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdPrevious));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::startSeekBackward()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdStartSeekBackward));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::setPosition( int value )
{
    TX_ENTRY_ARGS("value =" << value);
    TRAPD(err, iPlaybackUtility->SetL(EPbPropertyPosition, value));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::setShuffle( bool mode )
{
    TX_ENTRY_ARGS("mode =" << mode);
    TRAPD(err, iPlaybackUtility->SetL(EPbPropertyRandomMode, mode));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::setRepeat( bool mode )
{
    TX_ENTRY_ARGS("mode =" << mode);
    TRAPD(err, iPlaybackUtility->SetL(EPbPropertyRepeatMode, mode ? EPbRepeatAll : EPbRepeatOff));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::setBalance( int value )
{
    TX_ENTRY_ARGS("value =" << value);
    TRAPD( err, iPlaybackUtility->SetL( EPbPropertyBalance, value ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::applyAudioEffects()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL( EPbApplyEffect, KAudioEffectsID ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::applyEqualizer()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL( EPbApplyEffect, KEqualizerID ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::getMaxVolume()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->PropertyL( *this, EPbPropertyMaxVolume ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::getVolume()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->PropertyL( *this, EPbPropertyVolume ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::increaseVolume()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL( EPbCmdIncreaseVolume ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::decreaseVolume()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL( EPbCmdDecreaseVolume ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::setVolume( int value )
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL( EPbCmdSetVolume, value ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::getMuteState()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->PropertyL( *this, EPbPropertyMute ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::mute()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL( EPbCmdMuteVolume ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::unmute()
{
    TX_ENTRY
    TRAPD( err, iPlaybackUtility->CommandL( EPbCmdUnMuteVolume ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \ Closes current playback source.
 */
void MpMpxPlaybackFrameworkWrapperPrivate::closeCurrentPlayback()
{
    TX_ENTRY
    TRAPD( err, DoCloseCurrentPlaybackL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
MpPlaybackData *MpMpxPlaybackFrameworkWrapperPrivate::playbackData()
{
    return iPlaybackData;
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::retrieveSongDetails()
{
    TX_ENTRY
    TRAPD( err, DoRetrieveSongDetailsL(true) );
    if ( err == KErrNone ) {
        iDetailsRequest = true;
    }
    else {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::forceStop()
{
    TX_ENTRY
    TRAPD( err, ForceStopL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::HandlePlaybackMessage( CMPXMessage *aMessage, TInt aError )
{
    TX_ENTRY_ARGS("aError=" << aError);
    if ( aError == KErrNone && aMessage ) {
        TRAP_IGNORE( DoHandlePlaybackMessageL(*aMessage) );
    }
    else{
        TRAP_IGNORE( DoHandlePlaybackErrorL(aError) );
	}
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
{
    TX_ENTRY_ARGS("aProperty=" << aProperty << "aValue=" << aValue << "aError=" << aError)
    if ( KErrNone == aError ) {
        switch ( aProperty  ) {
            case EPbPropertyPosition:
                TX_LOG_ARGS("EPbPropertyPosition")
                iPlaybackData->setPosition(aValue);
                break;
            case EPbPropertyDuration:
                TX_LOG_ARGS("EPbPropertyDuration")
                iPlaybackData->setDuration(aValue);
                break;
            case EPbPropertyVolume:
                TX_LOG_ARGS("EPbPropertyVolume")
                q_ptr->emit volumePropertyChanged( MpCommon::Volume, aValue );
                break;
            case EPbPropertyMaxVolume:
                TX_LOG_ARGS("EPbPropertyMaxVolume")
                q_ptr->emit volumePropertyChanged( MpCommon::MaxVolume, aValue );
                break;
            case EPbPropertyMute:
                TX_LOG_ARGS("EPbPropertyMute")
                q_ptr->emit volumePropertyChanged( MpCommon::MuteState, aValue );
                break;
            default:
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::HandleSubPlayerNamesL(
    TUid aPlayer,
    const MDesCArray *aSubPlayers,
    TBool aComplete,
    TInt aError )
{
    Q_UNUSED(aPlayer);
    Q_UNUSED(aSubPlayers);
    Q_UNUSED(aComplete);
    Q_UNUSED(aError);
    TX_LOG
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
{
    TX_ENTRY
    if ( KErrNone != aError ){
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
        TX_EXIT
        return;
    }

    if ( iDetailsRequest ) {
        iDetailsRequest = false;
        iSongData->setMpxMedia( aMedia );
    }
    else {
        bool changed = false;
        if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) ) {
            changed |= iPlaybackData->setTitle(
                QString::fromUtf16(
                    aMedia.ValueText( KMPXMediaGeneralTitle ).Ptr(),
                    aMedia.ValueText( KMPXMediaGeneralTitle ).Length() ) );
        }
        else {
            changed |= iPlaybackData->setTitle(
                QString::fromUtf16(
                    aMedia.ValueText( KMPXMediaGeneralUri ).Ptr(),
                    aMedia.ValueText( KMPXMediaGeneralUri ).Length() ) );
        }

        if ( aMedia.IsSupported( KMPXMediaMusicArtist ) ) {
            changed |= iPlaybackData->setArtist(
                QString::fromUtf16(
                    aMedia.ValueText( KMPXMediaMusicArtist ).Ptr(),
                    aMedia.ValueText( KMPXMediaMusicArtist ).Length() ) );
        }
        else {
                changed |= iPlaybackData->setArtist(QString());
        }
        if ( aMedia.IsSupported( KMPXMediaMusicAlbum ) ) {
            changed |= iPlaybackData->setAlbum(
                QString::fromUtf16(
                    aMedia.ValueText( KMPXMediaMusicAlbum ).Ptr(),
                    aMedia.ValueText( KMPXMediaMusicAlbum ).Length() ) );
        }
        else {
                changed |= iPlaybackData->setAlbum(QString());
        }
        if (aMedia.IsSupported(TMPXAttribute(KMPXMediaGeneralMimeType))) {

                const TDesC& mimeType = aMedia.ValueText ( KMPXMediaGeneralMimeType );

                bool realAudioMode =
                        ( mimeType.Compare( KMPXPnRealAudioMimeType ) == 0 ) ||
                        ( mimeType.Compare( KMPXRealAudioMimeType ) == 0) ||
                        ( mimeType.Compare( KMPXRnRealAudioMimeType ) == 0 );

                changed |= iPlaybackData->setRealAudio( realAudioMode );
        }

        if ( aMedia.IsSupported( KMPXMediaGeneralUri) ) {
            iPlaybackData->setUri(
                QString::fromUtf16(
                    aMedia.ValueText( KMPXMediaGeneralUri ).Ptr(),
                    aMedia.ValueText( KMPXMediaGeneralUri ).Length() ) );
        }
        if (aMedia.IsSupported(TMPXAttribute(KMPXMediaMusicAlbumArtFileName))) {
            iPlaybackData->setAlbumArtUri(
                QString::fromUtf16(
                    aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Ptr(),
                    aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Length()));
        }
        else {
            iPlaybackData->setAlbumArtUri(
                QString::fromUtf16(
                    aMedia.ValueText( KMPXMediaGeneralUri ).Ptr(),
                    aMedia.ValueText( KMPXMediaGeneralUri ).Length() ) );
        }
        if ( aMedia.IsSupported( KMPXMediaMusicAlbumId) ) {
        changed |= iPlaybackData->setAlbumId(
                aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaMusicAlbumId ) );
        }

        if ( aMedia.IsSupported( KMPXMediaGeneralId ) ) {
        changed |= iPlaybackData->setId(
                    aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralId ) );
        }

        // This is required to propagate the playback info to UI at once.
        iPlaybackData->commitPlaybackInfo( changed );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::HandleEmbeddedPlaybackError( TInt aError )
{
    TX_ENTRY_ARGS( "aError=" << aError );
    Q_UNUSED( aError );
    emit q_ptr->corruptedStop( true );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( iHostUid );
    iPlaybackUtility->AddObserverL( *this );
    iPlaybackData = new MpPlaybackData();

    UpdateStateL();
    if ( iPlaybackUtility->Source() ) {
        DoRetrieveSongDetailsL(false);
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::DoPlayL( QString aFilename )
{
    TX_ENTRY_ARGS( "Filename: " << aFilename );
    if ( !aFilename.isEmpty() ) {
        const TDesC& playTitle = TPtrC(reinterpret_cast<const TText*>( aFilename.constData() ));
        if ( !iEmbeddedPlaybackHelper ) {
            iEmbeddedPlaybackHelper = 
                    CMpMpxEmbeddedPlaybackHelper::NewL( iHostUid, iPlaybackUtility, this );
        }
        iEmbeddedPlaybackHelper->playL( playTitle );
    }
    else {
        HandleEmbeddedPlaybackError( KErrArgument );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::DoPlayL( const XQSharableFile& file )
{
    TX_ENTRY
    DoPlayL( file.fileName() );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
{
    TX_ENTRY
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TX_LOG_ARGS("event=" << event << ", type=" << type << ", data=" << data);

        switch ( event ) {
            case TMPXPlaybackMessage::EStateChanged:
                TX_LOG_ARGS("TMPXPlaybackMessage::EStateChanged")
                UpdateStateL();
                break;
            case TMPXPlaybackMessage::EInitializeComplete:
                TX_LOG_ARGS("TMPXPlaybackMessage::EInitializeComplete")
                if ( data ) {
                    // Play the track
                    CMPXCommand *cmd( CMPXCommand::NewL() );
                    CleanupStack::PushL( cmd );
                    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId,
                            KMPXCommandIdPlaybackGeneral );
                    cmd->SetTObjectValueL<TInt>(
                            KMPXCommandPlaybackGeneralType,
                            EPbCmdPlay );
                    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync,
                            ETrue );
                    cmd->SetTObjectValueL<TBool>(
                            KMPXCommandPlaybackGeneralNoBuffer,
                            ETrue );
                    iPlaybackUtility->CommandL( *cmd );
                    CleanupStack::PopAndDestroy( cmd );
                }
                break;
            case TMPXPlaybackMessage::EPropertyChanged:
                TX_LOG_ARGS("TMPXPlaybackMessage::EPropertyChanged")
                TMPXPlaybackProperty property;
                property = static_cast<TMPXPlaybackProperty>( type );
                HandlePropertyL( property, data, KErrNone );
                break;
            case TMPXPlaybackMessage::EMediaChanged:
                TX_LOG_ARGS("TMPXPlaybackMessage::EMediaChanged")
                DoRetrieveSongDetailsL(false);
                break;
            case TMPXPlaybackMessage::EPlaylistUpdated:
                TX_LOG_ARGS( "EPlaylistUpdated" )
                // coverity[fallthrough]
            case TMPXPlaybackMessage::EActivePlayerChanged:
                TX_LOG_ARGS( "EActivePlayerChanged or fall through from EPlaylistUpdated" )
                UpdateStateL();
                DoRetrieveSongDetailsL(false);
                break;
            default:
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::DoHandlePlaybackErrorL( const TInt aError )
{
    TX_ENTRY
    switch ( aError ) {
        case KErrNotSupported:
        case KErrCorrupt:{
            MMPXSource* source( iPlaybackUtility->Source() );
            if ( source ){
                CMPXCollectionPlaylist* playlist( source->PlaylistL() );
                if ( playlist ){
                    MpPlaybackData* pData = playbackData();
                    pData->setCorrupted( playlist->Path().IdOfIndex( playlist->Index() ).iId2 );
                           
                    if ( playlist->Index() == ( playlist->Count()-1 ) ){
                        //reach the end of list, pop up corrupt notification
                        emit q_ptr->corruptedStop(true);
                    }
                    else{
                        //corrupted song; there is more song in the playlist
                        emit q_ptr->corruptedStop(false);
                    }
                }
                else{
                    // No playlist;single file playback.
                    emit q_ptr->corruptedStop(true);
                }
            }
            break;
        }
        default:
            break;
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::ForceStopL()
{
    TX_ENTRY
    if ( iPlaybackData->playbackState() != MpPlaybackData::Stopped ) {
        // Stop the track
        CMPXCommand *cmd( CMPXCommand::NewL() );
        CleanupStack::PushL( cmd );
        cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
        cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdStop );
        cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
        cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
        iPlaybackUtility->CommandL( *cmd );
        CleanupStack::PopAndDestroy( cmd );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::UpdateStateL()
{
    TX_ENTRY
    if ( !iPlaybackUtility->Source() ) {
        TX_LOG_ARGS("There is no source")
        //this to prevent mutiple calls to state change.
        if ( iPlaybackData->playbackState() != MpPlaybackData::NotPlaying ) {
            iPlaybackData->setPlaybackState( MpPlaybackData::NotPlaying );
            iPlaybackData->resetData();
        }
    }
    else {
        switch ( iPlaybackUtility->StateL() ) {
            case EPbStatePlaying:
                TX_LOG_ARGS("EPbStatePlaying")
                iPlaybackData->setPlaybackState( MpPlaybackData::Playing );
                break;
            case EPbStatePaused:
                TX_LOG_ARGS("EPbStatePaused")
                iPlaybackData->setPlaybackState( MpPlaybackData::Paused );
                break;
            case EPbStateStopped:
                TX_LOG_ARGS("EPbStateStopped")
                iPlaybackData->setPlaybackState( MpPlaybackData::Stopped );
                break;
            default:
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::DoRetrieveSongDetailsL( bool detailsRequest )
{
    TX_ENTRY
    MMPXSource *mediaSrc = iPlaybackUtility->Source();
    User::LeaveIfNull( mediaSrc );
    RArray<TMPXAttribute> requestedAttr;
    CleanupClosePushL( requestedAttr );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralId ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralTitle ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicArtist ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbum ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumId ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralUri ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralMimeType ) );
    if ( detailsRequest ) {
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumTrack ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicComposer ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicYear ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicGenre ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralDuration ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaAudioBitrate ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaAudioSamplerate ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralCopyright ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicURL ) );
        requestedAttr.AppendL( TMPXAttribute( KMPXMediaDrmProtected ) );
    }
    mediaSrc->MediaL( requestedAttr.Array(), *this );
    CleanupStack::PopAndDestroy( &requestedAttr );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPlaybackFrameworkWrapperPrivate::DoCloseCurrentPlaybackL()
{
    TX_ENTRY
    if( iPlaybackUtility->Source() ) {
        iPlaybackUtility->CommandL( EPbCmdClose );
    }
    TX_EXIT
}

