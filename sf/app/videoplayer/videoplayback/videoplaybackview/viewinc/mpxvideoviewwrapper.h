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
* Description:  MPX Video base playback view
*
*/

// Version : %version:  da1mmcf#29 %



// This file defines the API for .dll

#ifndef __MPXVIDEOVIEWWRAPPER_H__
#define __MPXVIDEOVIEWWRAPPER_H__

//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf
#include <mpxplaybackobserver.h>
#include <mpxvideoplaybackdefs.h>
#include <mpxviewactivationobserver.h>
#include <mpxcollectionobserver.h>

#include "videoplaybackcontrol.hrh"

enum TMPXMediaRequestStatus
{
    MediaNotRequested,
    MediaRequested,
    MediaDelivered
};

//  Forward Declarations
class MMPXPlaybackUtility;
class MMPXCollectionUtility;
class VideoBasePlaybackView;
class CVideoPlaybackDisplayHandler;
class VideoPlaybackViewFileDetails;
class CVideoPlaybackUserInputHandler;
class VideoPlaybackControlsController;

//  Class Definitions

class CMPXVideoViewWrapper : public CBase,
                             public MMPXPlaybackObserver,
                             public MMPXViewActivationObserver,
                             public MMPXPlaybackCallback,
                             public MMPXCollectionObserver
{
    public:
        static CMPXVideoViewWrapper* NewL( VideoBasePlaybackView* aView );
        virtual ~CMPXVideoViewWrapper();

    private:
        CMPXVideoViewWrapper( VideoBasePlaybackView* aView );
        void ConstructL();

    public:
        virtual void HandleCommandL( TInt aCommand );

        void RetrieveFileNameAndModeL( CMPXCommand* aCmd );

        /*
         *  From MMPXViewActivationObserver
         *  Handle view activation.
         *
         *  @param aCurrentViewType Current view type Uid.
         *  @param aPreviousViewType Previous view type Uid.
         */
        inline void HandleViewActivation( const TUid& /*aCurrentViewType*/,
                                          const TUid& /*aPreviousViewType*/ ) {}

        /**
        * From MMPXPlaybackObserver
        * Handle playback message
        *
        * @param aMessage Playback Message
        * @param aErr system error code.
        */
        virtual void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

        /**
        *  From MMPXPlaybackCallback
        *  Handle playback property
        *
        *  @param aProperty the property
        *  @param aValue the value of the property
        *  @param aError error code
        */
    	void HandlePropertyL( TMPXPlaybackProperty aProperty,
                              TInt aValue,
                              TInt aError );

        /**
        *  Method is called continously until aComplete=ETrue, signifying that
        *  it is done and there will be no more callbacks
        *  Only new items are passed each time
        *
        *  @param aPlayer UID of the subplayer
        *  @param aSubPlayers a list of sub players
        *  @param aComplete ETrue no more sub players. EFalse more subplayer
        *                   expected
        *  @param aError error code
        */
        inline void HandleSubPlayerNamesL( TUid /*aPlayer*/,
                                           const MDesCArray* /*aSubPlayers*/,
                                           TBool /*aComplete*/,
                                           TInt /*aError*/ )
        {}

        /**
        *  Call back of media request
        *
        *  @param aMedia media
        *  @param aError error code
        */
        void HandleMediaL( const CMPXMedia& aProperties, TInt aError );


        /**
        *  Handle completion of a asynchronous command
        *  @param aCommandResult result of the command, NULL if error
        *  @param aError error code
        */
        void HandlePlaybackCommandComplete( CMPXCommand* /*aCommandResult*/,
                                            TInt /*aError*/);
        /*
         *  From base class MMPXCollectionMediaObserver
         *  (via MMPXCollectionObserver)
         *  Handle extended media properties
         *
         *  @param aMedia media
         *  @param aError error code
         */
        inline void HandleCollectionMediaL( const CMPXMedia& /*aMedia*/, TInt /*aError*/ ) {}

        /*
         *  From base class MMPXCollectionObserver
         */
        inline void HandleCollectionMessage( CMPXMessage* /*aMsg*/, TInt /*aErr*/ ) {}

        inline void HandleOpenL( const CMPXMedia& /*aEntries*/,
                                 TInt /*aIndex*/,
                                 TBool /*aComplete*/,
                                 TInt /*aError*/ ) {}

        inline void HandleOpenL( const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ ) {}

        /**
        * Set property
        */
        void SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue );
        virtual void RetrievePdlInformationL();

        TBool IsLive();

        TBool IsPlaylist();

        void IssueVideoAppForegroundCmdL( TBool aViewForeground, TBool aAppForegournd );

        void RequestMediaL();

        void CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd, TBool aDoSync = ETrue );

        void ActivateClosePlayerActiveObject();

        void CreateControlsL();

        TBool IsMultiItemPlaylist();

        void UpdateVideoRect( TInt aX, TInt aY, TInt aWidth, TInt aHeight, TBool transitionEffect );

        void UpdateVideoRectDone();

        void HandlePluginError( TInt aError );

        void ClosePlaybackViewL();

        void HandleBufferingStateL();

        /*
         *  Provides the static function for the callback to close the player
         *  Called by CIdle iIdle
         *  @since 3.2
         *  @param aPtr Pointer to callback class
         *  @return KErrNone
         */
        static TInt ClosePlayer( TAny* aPtr );

        /*
         *  Called to stop and exit the player
         *  @since 3.2
         *  @return void
         */
        void DoClosePlayer();

        void HandleVideoPlaybackMessage( CMPXMessage* aMessage );

        TBool IsResumingPlaybackAfterTermination();

        void SurfacedAttached( TBool aAttached );

        void ActivateL();

        void Deactivate();

        int VolumeSteps();

        void SetDefaultAspectRatio( TInt aspectRatio );

    private:

        /**
        *  Handle playback message
        *
        *  @param aMsg playback message
        */
        virtual void DoHandlePlaybackMessageL( CMPXMessage* aMessage );

        /**
        *  Handle playback error message
        *
        *  @param aErr system error code.
        */
        void DoHandleErrorPlaybackMessageL( TInt aError );

        void DisplayFileDetailsDialogL();

        /**
        *  Handle media properties.
        *
        *  @param aMedia media properties
        *  @param aError error code
        */
        virtual void DoHandleMediaL( const CMPXMedia& aMedia, TInt aError );

        void CreateVideoSpecificCmdL( TMPXVideoPlaybackCommand aCmd );

        void ParseMetaDataL( const CMPXMedia& aMedia );

        void DoHandleStateChangeL( TInt aNewState );

        void HandleGeneralPlaybackMessageL( CMPXMessage* aMessage );

        void SetAspectRatioL( TMPXVideoPlaybackCommand aCmd );

        void HandleVolumeCmdL( TMPXPlaybackCommand aCmd );

        void HandleShortPressBackwardL();

        void IssuePlayCommandL();

        void RequestPlaybackMediaL();

        void RequestCollectionMediaL();

        TBool IsInMemoryPlugin();

        void UpdatePbPluginMediaL( TBool aSeek );

        TInt GetMediaId();

        void InitializeFileDetails();

    protected: // data
        MMPXPlaybackUtility*                 iPlaybackUtility;
        MMPXCollectionUtility*               iCollectionUtility;
        TMPXPlaybackState                    iPlaybackState;

        VideoPlaybackViewFileDetails*        iFileDetails;
        CIdle*                               iCloseAO;

        VideoBasePlaybackView*               iView;
        CVideoPlaybackDisplayHandler*        iDisplayHandler;
        CVideoPlaybackUserInputHandler*      iUserInputHandler;
        VideoPlaybackControlsController*     iControlsController;

        TMPXMediaRequestStatus               iMediaRequestStatus;
        TBool                                iCollectionMediaRequested;
        TBool                                iPlaylistView;
        int                                  iPlayPosition;

    public:
        friend class VideoBasePlaybackView;
};

#endif  // __MPXVIDEOVIEWWRAPPER_H__

// EOF
