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

#ifndef MPMPXPLAYBACKFRAMEWORKWRAPPER_P_H
#define MPMPXPLAYBACKFRAMEWORKWRAPPER_P_H

#include <e32base.h>
#include <mpxplaybackobserver.h>
#include "mpmpxembeddedplaybackhelper.h"
#include "mpcommondefs.h"


class MMPXPlaybackUtility;
class MpMpxPlaybackFrameworkWrapper;
class QStringList;
class CMPXMedia;
class MpPlaybackData;
class XQSharableFile;
class MpSongData;

class MpMpxPlaybackFrameworkWrapperPrivate : public MMPXPlaybackObserver,
                                             public MMPXPlaybackCallback,
                                             public MMpMpxEmbeddedPlaybackHelperObserver
{
public:

    explicit MpMpxPlaybackFrameworkWrapperPrivate( MpMpxPlaybackFrameworkWrapper *wrapper );
    virtual ~MpMpxPlaybackFrameworkWrapperPrivate();

    void init( quint32 clientSecureId, MpSongData *songData );

    void play( QString aFilename );
    void play( const XQSharableFile& file );
    void play();
    void pause();
    void playPause();
    void stop();
    void skipForward();
    void startSeekForward();
    void stopSeeking();
    void skipBackward();
    void startSeekBackward();
    void setPosition( int value );
    void setShuffle( bool mode );
    void setRepeat( bool mode );
    void setBalance( int balance );
    void applyAudioEffects();
    void applyEqualizer();
    void getMaxVolume();
    void getVolume();
    void increaseVolume();
    void decreaseVolume();
    void setVolume( int value );
    void getMuteState();
    void mute();
    void unmute();
    void closeCurrentPlayback();

    MpPlaybackData *playbackData();

    void retrieveSongDetails();
    void forceStop();
    
private:

    // From MMPXPlaybackObserver
    void HandlePlaybackMessage( CMPXMessage *aMessage, TInt aError );

    // From MMPXPlaybackCallback
    void HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );
    void HandleSubPlayerNamesL( TUid aPlayer, const MDesCArray *aSubPlayers,
                                TBool aComplete, TInt aError );
    void HandleMediaL( const CMPXMedia& aProperties, TInt aError );

    // From MMpMpxEmbeddedPlaybackHelperObserver
    void HandleEmbeddedPlaybackError( TInt aError );

    void DoInitL();
    void DoPlayL( QString aFilename );
    void DoPlayL( const XQSharableFile& file );
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );
    void DoHandlePlaybackErrorL( const TInt aError );
    void ForceStopL();
    void UpdateStateL();
    void DoRetrieveSongDetailsL( bool detailsRequest );
    void DoCloseCurrentPlaybackL();

private:

    MpMpxPlaybackFrameworkWrapper       *q_ptr;
    MMPXPlaybackUtility                 *iPlaybackUtility;         // Own
    CMpMpxEmbeddedPlaybackHelper        *iEmbeddedPlaybackHelper;  // Own
    MpPlaybackData                      *iPlaybackData;            // Own
    TUid                                iHostUid;

    MpSongData                          *iSongData;                // Not owned
    bool                                iDetailsRequest;

};

#endif  // MPMPXPLAYBACKFRAMEWORKWRAPPER_P_H
