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
* Description:   This class plays files with the Helix Engine
*
*/


// Version : %version: 30 %


#ifndef _CMPXVIDEOPLAYBACKCONTROLLER_H_
#define _CMPXVIDEOPLAYBACKCONTROLLER_H_

//
//  INCLUDES
//
#include <mpxplaybackplugin.h>
#include <mpxcenrepobserver.h>
#include <mediarecognizer.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxcenrepwatcher.h>
#include <MMFScalingCustomCommandConstants.h>

#include "mpxvideoprivatecrkeys.h"
#include "mpxvideoplaybackstate.h"
#include "mpxvideoplaybackmode.h"
#include "mpxvideofiledetails.h"
#include "mpxhelixplaybackplugindefs.h"
#include "mpxvideoaccessoryobserver.h"
#include "mpxvideoplayerutility.h"
#include "mpxvideo_debug.h"

#include "mpxcalldetector.h"
// DATA TYPES

// FORWARD DECLARATIONS

class MMPXPlaybackPluginObserver;
class CMPXCenRepWatcher;
class CMPXPlaybackPlugin;
class CMPXVideoPlaybackState;
class CMediaRecognizer;
class CMPXVideoSeeker;
class CMpxVideoDrmHelper;
class CHWRMLight;

//
//  CLASS DECLARATION
//

/*
 *  CMPXVideoPlaybackController class
 *
 *  Plug-in basically handles Video Playback with the Helix Engine
 *  @lib mpxvideohelixplayback.lib
 *
 */
NONSHARABLE_CLASS( CMPXVideoPlaybackController )
    : public CBase
    , public MMPXCenRepObserver
    , public MMPXCallDetectorObserver
{
    public:
        //
        //  Constructors and destructor
        //
        static CMPXVideoPlaybackController* NewL( MMPXPlaybackPluginObserver& aObs );
        /*
         *  Destructor
         *  Destroy the object and release all memory objects
         */
        ~CMPXVideoPlaybackController();

        // Maybe we can replace this with a SendMedia CustomCommand
        // at a later stage
        void SendMediaL( const TArray<TMPXAttribute>& aAttrs );
        void HandleCommandL( CMPXCommand& aCmd );
        void HandleCommandL( TMPXPlaybackCommand aCmd, TInt aData );
        void DoHandleCommandL( TInt aCmd );
        void HandleMMFEvent( const TMMFEvent& aEvent );
        void SetPropertyL( TMPXPlaybackProperty aProperty , TInt aValue );
        void GetPropertyL( TMPXPlaybackProperty aProperty ) const;

        //
        //  MMPXCenRepObserver Implementation
        //
        void HandleSettingChange( const TUid& aRepositoryUid,
                                  TUint32 aSettingId );

        void OpenFileL( const TDesC& aMediaFile, RFile& aFile, TInt aPosition, TInt aAccessPointId = KUseDefaultIap );

        void HandleGeneralPlaybackCommandL( CMPXCommand& aCmd );

        void HandleCustomPlaybackCommandL( CMPXCommand& aCmd );

        void HandlePdlCommandL( CMPXCommand& aCmd );

        /* closes the playback controller */
        void CloseController();

        /* Creates state instances needed before initialization & sets default state */
        void CreatePreInitStatesL();

        /* Creates state instances needed after initialization */
        void CreatePostInitStatesL();

        inline void SetMPXObserver(MMPXPlaybackPluginObserver& aObs);

        void ReadFileDetailsL();

        void SetPlaybackModeL();

        TBool IsKeyLocked();
        TBool IsAlarm();
        TBool IsPhoneCall();
        TBool IsActivePhoneCall();
        TBool IsVideoCall();
        TBool IsVoiceCall();

        TBool IsViewActivated();

        void HandleTvOutEventL( TBool aConnected );

        void HandleError(TInt error);
        void SetVolumeSteps( TInt aVolumeSteps );
        void HandleVolumeL( TBool aIncrease );

        void SendHideControlsEventL();
        
        void HandleFrameReady(TInt aError);

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        void OpenFile64L( const TDesC& aMediaFile, RFile64& aFile, TInt aPosition, TInt aAccessPointId = KUseDefaultIap );

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API


    private:
        void CallDetectedL();

        void SetDisplayWindowL( CMPXCommand& aCmd );
        void RestartDSA( CMPXCommand& aCmd );
        inline void AbortDSA();

        void InitVolumeWatchersL();

        void ChangeState(TMPXVideoPlaybackState aChangeToState);

        void SetVolumeMMFL();
        void SetVolumeCenRepL( TInt aVolume );
        void SetMuteCenRepL( TBool aMute );

        TMPXVideoMode GetModeToSet();
        void DetermineMediaTypeL();

        void ResetMemberVariables();

        void StartLightsControl();
        void CancelLightsControl();

        void EnableDisplayBacklight();
        void DisableDisplayBacklight();

        static TInt HandleBackLightTimeout( TAny* aPtr );
        void DoHandleBackLightTimeout();
        void StartBackLightTimer();
        void CancelBackLightTimer();

        void InitUserActivityTimerL();
        void RestartUserActivityTimer();
        void CancelUserActivityTimer();
        static TInt HandleUserActivityTimeout( TAny* aPtr );
        void DoHandleUserActivityTimeout();

        TTimeIntervalMicroSeconds32 InitDisplayTimerL();
        void RestartDisplayTimer();
        void CancelDisplayTimer();
        static TInt HandleDisplayTimeout( TAny* aPtr );
        void DoHandleDisplayTimeout();

        CHWRMLight* GetLightsL();
        void ReleaseLights();

        void SendTvOutEventL( TBool aConnected );

    protected:

        /*
         *  C++ default constructor
         */
        CMPXVideoPlaybackController();

        /*
         *  the second phase constructor ConstructL to safely construct things
         *  that can leave
         */
        void ConstructL( MMPXPlaybackPluginObserver& aObs );

    protected:
        //
        //  Data
        //
        CMpxVideoPlayerUtility*                iPlayer;

        CMPXVideoAccessoryObserver*            iAccessoryMonitor;

        MMPXPlaybackPluginObserver*            iMPXPluginObs; // Not owned
        CMPXCenRepWatcher*                     iVolumeWatcher;
        CMPXCenRepWatcher*                     iMuteWatcher;

        HBufC*                                 iClipName;
        HBufC*                                 iRecognizedMimeType;  // mime type returned by the recognizer
        RFile                                  iFileHandle;
        CMPXVideoFileDetails*                  iFileDetails;
        TBool                                  iAppInForeground;
        CMediaRecognizer::TMediaType           iMediaType;
        TInt                                   iAccessPointId;
        TBool                                  iAllowAutoPlay;
        TBool                                  iHelixLoadingStarted;
        TInt                                   iPBPluginError;
        TInt                                   iVolumeNormalizer;

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        RFile64                                iFileHandle64;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API


        // State
        CMPXVideoPlaybackState*                 iState;
        CMPXVideoPlaybackState*                 iNotIntialisedState;
        CMPXVideoPlaybackState*                 iInitialisingState;
        CMPXVideoPlaybackState*                 iInitialisedState;
        CMPXVideoPlaybackState*                 iPlayingState;
        CMPXVideoPlaybackState*                 iBufferingState;
        CMPXVideoPlaybackState*                 iPausedState;
        CMPXVideoPlaybackState*                 iSeekingState;
        CMPXVideoPlaybackState*                 iStoppedState;

        // Mode
        CMPXVideoPlaybackMode*                  iPlaybackMode;

        // Video seeker
        CMPXVideoSeeker*                        iVideoSeeker;

        // Timer for calling User::ResetInactivityTime() periodicallly while playing.
        // This keeps backligth on and screensaver off.
        CPeriodic*                              iBackLightTimer;

        // Timer to turn display lights off when playing to TV-out
        CPeriodic*                              iDisplayTimer;
        TTimeIntervalMicroSeconds32             iDisplayTimerTimeout;
        // Timer monitoring user activity when TV-out is connected
        CPeriodic*                              iUserActivityTimer;

        // Call detector
        CMPXCallDetector*                       iCallDetector; 

        TBool                                   iSeekable;

        CMpxVideoDrmHelper*                     iDrmHelper;
        CHWRMLight*                             iLight;
        TInt                                    iLightStatus;
        TInt                                    iSavedPosition;
        TBool                                   iViewActivated;

    public:     // Friend classes

        // states are friends of the controller
        friend class CMPXVideoPlaybackState;
        friend class CMPXNotInitialisedState;
        friend class CMPXInitialisingState;
        friend class CMPXInitialisedState;
        friend class CMPXPlayingState;
        friend class CMPXBufferingState;
        friend class CMPXPausedState;
        friend class CMPXSeekingState;
        friend class CMPXStoppedState;

        // modes are friends of the controller
        friend class CMPXVideoPlaybackMode;
        friend class CMPXLocalPlaybackMode;
        friend class CMPXStreamingPlaybackMode;
        friend class CMPXLiveStreamingPlaybackMode;
        friend class CMPXProgressiveDLPlaybackMode;

        friend class CMPXVideoSeeker;
#ifdef USE_S60_DOWNLOAD_MANAGER 
        friend class CMPXVideoDlMgrIf;
#endif // USE_S60_DOWNLOAD_MANAGER 
        friend class CMPXVideoAccessoryObserver;
        friend class CMpxVideoPlayerUtility;
        friend class CMPXVideoPosterFrameSetter;
};

// INLINE METHODS

inline
void CMPXVideoPlaybackController::SetMPXObserver(MMPXPlaybackPluginObserver& aObs)
{
    iMPXPluginObs = &aObs;
}

inline
void CMPXVideoPlaybackController::AbortDSA()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::AbortDSA()"));

    iPlayer->AbortDsa();
}

#endif //_CMPXVIDEOPLAYBACKCONTROLLER_H_
