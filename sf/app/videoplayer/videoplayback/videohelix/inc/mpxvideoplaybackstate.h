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


// Version : %version: 22 %


#ifndef _CMPXVIDEOPLAYBACKSTATE_H_
#define _CMPXVIDEOPLAYBACKSTATE_H_

//
//  INCLUDES
//
#include <e32def.h>
#include <e32base.h>

#include "mpxhelixplaybackplugindefs.h"
#include "mpxvideo_debug.h"

//
//  FORWARD DECLARATIONS
//
class CMPXVideoPlaybackController;

//
//  CLASS DECLARATION
//

/*
 *  CMpxVideoPlaybackState class
 *
 *  State Machine keeptrack of the state of the playback controller
 *  @lib mpxvideohelixplayback.lib
 *
 */
NONSHARABLE_CLASS( CMPXVideoPlaybackState ) : public CBase
{
    public:
        //
        //  Constructors and destructor
        //

        /*
         *  Destructor
         *  Destroy the object and release all memory objects
         */
        ~CMPXVideoPlaybackState();

        //
        //  CMPXVideoPlaybackState Implementation
        //

        // Commands received from the Framework
        virtual void  HandlePlay();
        virtual void  HandlePause();
        virtual void  DoHandlePause();
        virtual void  HandlePlayPause();
        virtual void  HandleStop();
        virtual void  HandleStartSeekL( TBool aForward );
        virtual void  HandleStopSeekL();
        virtual void  HandleClose();
        virtual void  HandleSetPosterFrame();
        virtual void  HandleForeground();
        virtual void  HandleBackground();

        virtual void  HandleCustomPause();
        virtual void  HandleCustomPlay();


        virtual void  CommandHandleBackground();
        virtual void  CommandHandleForeground();

        // Events received from the MMF
        virtual void HandleOpenComplete(TInt aError);
        virtual void HandlePrepareComplete(TInt aError);
        virtual void HandleLoadingStarted();
        virtual void HandleLoadingComplete(TInt aError);
        virtual void HandlePlaybackComplete(TInt aError);
        virtual void HandleUnexpectedError( TInt aError );

        // general method
        virtual void OpenFileL( const TDesC& aMediaFile );
        virtual void OpenFileL( const RFile& aMediaFile );
        virtual void HandleVolumeChange();
        virtual void HandleSetPositionL(TInt aPosition);
        virtual TInt RetrieveFileDetailsL(const TArray<TMPXAttribute>& aAttrs, CMPXMedia* aMedia);
        void RetrieveGeneralAttributesL( CMPXMedia* aMedia, TUint attrG );
        void RetrieveVideoAttributesL( CMPXMedia* aMedia, TUint attrV );
        inline virtual TBool IsInitialized();
        virtual void HandleEndOfClip();
        virtual TInt RetrieveBufferingPercentage();

        virtual void ResolveTimeoutError( TInt aError );

        virtual void UpdateSeekableL( CMPXCommand& aCmd );

        //
        // Set aspect ratio to mmf controller
        //
        virtual void SetAspectRatioL( CMPXCommand& aCmd, TMMFScalingType aType );

        virtual void SendErrorToViewL( TInt aError );

        inline virtual TMPXVideoPlaybackState  GetState();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        virtual void OpenFile64L( const RFile64& aMediaFile );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        void IssuePlayCommand( TMPXVideoPlaybackState aState,
                               MMPXPlaybackPluginObserver::TEvent aEvent,
                               TBool aSendEvent = ETrue );

    protected:
        /*
         *  C++ default constructor
         */
        CMPXVideoPlaybackState();

        /*
         *  the second phase constructor ConstructL to safely construct things
         *  that can leave
         */
        void ConstructL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);

    protected:
        //
        //  Data
        //
        CMPXVideoPlaybackController*        iVideoPlaybackCtlr;   // not owned
        RFs                                 iFs;
};


/*******************************************************/
/*         SUB CLASS DECLARATIONS                         */
/*******************************************************/

////////////////////////////////////////////////////////
///     Declaration of CMPXNotIntialisedState
////////////////////////////////////////////////////////
class CMPXNotInitialisedState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXNotInitialisedState();

        // general method
        virtual void HandleVolumeChange();
        virtual TInt RetrieveFileDetailsL(const TArray<TMPXAttribute>& aAttrs, CMPXMedia* aMedia);
        virtual void HandleClose();
        virtual void SetAspectRatioL( CMPXCommand& aCmd, TMMFScalingType aType );
        inline virtual TBool IsInitialized();
        virtual void HandleUnexpectedError( TInt aError );
        virtual void HandleBackground();
        virtual void HandleForeground();
        virtual void HandlePlay();
        virtual void UpdateSeekableL( CMPXCommand& aCmd );

        inline virtual TMPXVideoPlaybackState GetState();
};


////////////////////////////////////////////////////////
///     Declaration of CMPXIntialisingState
////////////////////////////////////////////////////////
class CMPXInitialisingState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXInitialisingState();

        virtual void OpenFileL( const TDesC& aMediaFile );
        virtual void OpenFileL( const RFile& aMediaFile );

        // Methods where video plabkack behavior varies for this state
        virtual void HandleForeground();
        virtual void HandleBackground();

        // Events received from the MMF
        virtual void HandleOpenComplete(TInt aError);
        virtual void HandlePrepareComplete(TInt aError);

        // general method
        virtual void HandleVolumeChange();
        virtual TInt RetrieveFileDetailsL(const TArray<TMPXAttribute>& aAttrs, CMPXMedia* aMedia);
        virtual void SetAspectRatioL( CMPXCommand& aCmd, TMMFScalingType aType );
        inline virtual TBool IsInitialized();
        virtual void HandleUnexpectedError( TInt aError );

        inline virtual TMPXVideoPlaybackState GetState();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        virtual void OpenFile64L( const RFile64& aMediaFile );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

};

////////////////////////////////////////////////////////
///     Declaration of CMPXIntialisedState
////////////////////////////////////////////////////////
class CMPXInitialisedState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXInitialisedState();

        // Methods where video plabkack behavior varies for this state

        // Commands received from the Framework
        virtual void HandlePlay();
        virtual void HandleStop();
        virtual void HandleForeground();
        virtual void HandleBackground();
        virtual void HandlePause();

        // general method
        virtual void HandleSetPositionL(TInt aPosition);

        inline virtual TMPXVideoPlaybackState GetState();
};

////////////////////////////////////////////////////////
///     Declaration of CMPXPlayingState
////////////////////////////////////////////////////////
class CMPXPlayingState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXPlayingState();

        // Methods where video plabkack behavior varies for this state

        // Commands received from the Framework
        virtual void HandlePause();
        virtual void HandleStop();
        virtual void HandleStartSeekL( TBool aForward );
        virtual void HandleSetPosterFrame();
        virtual void HandlePlayPause();
        virtual void HandleBackground();

        virtual void HandleCustomPause();

        // Events received from the MMF
        virtual void HandleLoadingStarted();

        // general method
        virtual void HandleSetPositionL(TInt aPosition);
        virtual void ResolveTimeoutError( TInt aError );

        inline virtual TMPXVideoPlaybackState GetState();
};


////////////////////////////////////////////////////////
///     Declaration of CMPXBufferingState
////////////////////////////////////////////////////////
class CMPXBufferingState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXBufferingState();

        // Methods where video plabkack behavior varies for this state

        // Commands received from the Framework
        //virtual void  HandleStop();

        virtual void HandleForeground();
        virtual void HandleBackground();
        virtual void HandleStop();
        virtual void HandlePause();
        virtual TInt RetrieveBufferingPercentage();

        // Events received from the MMF
        virtual void HandleLoadingStarted();
        virtual void HandleLoadingComplete(TInt aError);

        // Handle end of the clip situation, where we might get
        // playbackcomplete before loadingcomplete

        // general method
        virtual void ResolveTimeoutError( TInt aError );

        inline virtual TMPXVideoPlaybackState GetState();
};

////////////////////////////////////////////////////////
///     Declaration of CMPXPausedState
////////////////////////////////////////////////////////
class CMPXPausedState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );
        virtual ~CMPXPausedState();

        // Methods where video plabkack behavior varies for this state

        // Commands received from the Framework
        virtual void HandlePlay();
        virtual void HandleStop();
        virtual void HandleStartSeekL( TBool aForward );
        virtual void HandlePlayPause();
        virtual void HandleSetPosterFrame();
        virtual void HandleForeground();
        virtual void HandleCustomPlay();
        virtual void HandleUnexpectedError( TInt aError );

        // general method
        virtual void HandleSetPositionL( TInt aPosition );

        inline virtual TMPXVideoPlaybackState GetState();
};

////////////////////////////////////////////////////////
///     Declaration of CMPXSeekingState
////////////////////////////////////////////////////////
class CMPXSeekingState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXSeekingState();

        // Methods where video plabkack behavior varies for this state

        // Commands received from the Framework
        virtual void HandleStopSeekL();
        virtual void HandleBackground();
        virtual void HandlePause();
        virtual void HandlePlay();
        virtual void HandleSetPositionL( TInt aPosition );

        virtual void HandleStop();

        inline virtual TMPXVideoPlaybackState GetState();
};


////////////////////////////////////////////////////////
///     Declaration of CMPXStoppedState
////////////////////////////////////////////////////////
class CMPXStoppedState : public CMPXVideoPlaybackState
{
    public:
        static CMPXVideoPlaybackState* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXStoppedState();

        // Methods where video plabkack behavior varies for this state

        // Commands received from the Framework
        virtual void HandlePlay();
        virtual void HandleClose();

        // general method
        virtual void HandleSetPositionL(TInt aPosition);
        virtual void ResolveTimeoutError( TInt aError );
        virtual void HandleUnexpectedError( TInt aError );

        inline virtual TMPXVideoPlaybackState GetState();
};

// INLINE METHODS

inline
TMPXVideoPlaybackState CMPXVideoPlaybackState::GetState()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::GetState()"));

    // sub-classes to return their respective value
    return EMPXNumberOfStates;
}

inline
TMPXVideoPlaybackState CMPXNotInitialisedState::GetState()
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::GetState()"));

    return EMPXVideoNotInitialized;
}

inline
TMPXVideoPlaybackState CMPXInitialisingState::GetState()
{
    MPX_DEBUG(_L("CMPXInitialisingState::GetState()"));

    return EMPXVideoInitializing;
}

inline
TMPXVideoPlaybackState CMPXInitialisedState::GetState()
{
    MPX_DEBUG(_L("CMPXInitialisedState::GetState()"));

    return EMPXVideoInitialized;
}

inline
TMPXVideoPlaybackState CMPXPlayingState::GetState()
{
    MPX_DEBUG(_L("CMPXPlayingState::GetState()"));

    return EMPXVideoPlaying;
}

inline
TMPXVideoPlaybackState CMPXPausedState::GetState()
{
    MPX_DEBUG(_L("CMPXPausedState::GetState()"));

    return EMPXVideoPaused;
}

inline
TMPXVideoPlaybackState CMPXStoppedState::GetState()
{
    MPX_DEBUG(_L("CMPXStoppedState::GetState()"));
    return EMPXVideoStopped;
}

inline
TMPXVideoPlaybackState CMPXBufferingState::GetState()
{
    MPX_DEBUG(_L("CMPXBufferingState::GetState()"));

    return EMPXVideoBuffering;
}

inline
TMPXVideoPlaybackState CMPXSeekingState::GetState()
{
    MPX_DEBUG(_L("CMPXSeekingState::GetState()"));

    return EMPXVideoSeeking;
}

inline
TBool CMPXVideoPlaybackState::IsInitialized()
{
    return ETrue;
}

inline
TBool CMPXNotInitialisedState::IsInitialized()
{
    return EFalse;
}

inline
TBool CMPXInitialisingState::IsInitialized()
{
    return EFalse;
}



#endif  //_CMPXVIDEOPLAYBACKSTATE_H_
