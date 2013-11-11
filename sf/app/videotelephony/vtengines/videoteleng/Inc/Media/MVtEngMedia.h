/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Media interface
*
*/



#ifndef MVTENGMEDIA_H
#define MVTENGMEDIA_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVtEngLocalVideoDisplay;
class CFbsBitmap;

// CLASS DECLARATION

/**
*  Media interface for defining data types for media transport and
*  presentation of it locally as well methods for obtaining current
*  configuration.
*
*  @lib videoteleng.lib
*  @since Series 60 2.6
*/
class MVtEngMedia
    {
    public:  // New functions.

        enum TMediaType
            {
            EMediaAudio = (1 << 0 ),
            EMediaVideo = ( 1 << 1 ),
            EMediaAudioPreparing = ( 1 << 2 ),
            EMediaVideoPreparing = ( 1 << 3 ),
            // Status of audio channel, applicable only for outgoing audio.
            EMediaAudioChannelOpen = ( 1 << 4 ),
            // Status of video channel, applicable only for outgoing video.
            EMediaVideoChannelOpen = ( 1 << 5 )
            };

        /**
        * ESourceCapsPrimaryCamera
        *   primary camera can be selected.
        * ESourceCapsSecondaryCamera
        *   secondary camera can be selected.
        * ESourceCapsStillImage
        *   still image can be selected.
        */
        enum TSourceCapsFlags
            {
            ESourceCapsPrimaryCamera   = ( 1 << 0 ),
            ESourceCapsSecondaryCamera = ( 1 << 1 ),
            ESourceCapsStillImage      = ( 1 << 2 )
            };

        /**
        * EMediaOutgoing
        *  outbound media
        * EMediaIncoming
        *  inbound media
        */
        enum TMediaDirection
            {
            EMediaOutgoing,
            EMediaIncoming
            };

        /**
        * EMediaNone
        *  No media source selected.
        * EMediaCamera
        *  Camera selected
        * EMediaCameraPri
        *  Primary camera selected.
        *    Camera is inwards
        * EMediaCameraSec
        *  Secondary camera selected.
        *    Camera is outwards
        * EMediaStillImage
        *  Still image as source.
        * EMediaVideoStream
        *  Video stream selected.
        */
        enum TMediaSource
            {
            EMediaNone,
            EMediaCamera,
            EMediaCameraPri,
            EMediaCameraSec,
            EMediaStillImage,
            EMediaVideoStream,
            EMediaShare
            };

        /**
        * EPrimaryCamera
        *  Primary camera is inwards faced.
        * ESecondaryCamera
        *  Camera is facing outwards.
        */
        enum TCameraId
            {
            EPrimaryCamera,
            ESecondaryCamera
            };

        /**
        * Camera information (zoom mainly).
        */
        struct TCameraInfo
            {
            TInt    iMinZoom;
            TInt    iMaxZoom;
            TInt    iMaxDigitalZoom;
            TInt    iMajorZoomStepCount;
            };

        /**
        *
        * Video quality enumeration.
        *
        * EVideoQualityUndefined video quality has not been set
        * yet.
        * EVideoQualityDetail means slow frame rate, good image
        * quality..
        * EVideoQualityNormal smoother video with somewhat lower
        * image quality than EVideoQualityDetail.
        * EVideoQualityMotion the smoothest video with the worst
        * image quality.
        */
        enum TVideoQuality
            {
            EVideoQualityUndefined,
            EVideoQualityDetail,
            EVideoQualityNormal,
            EVideoQualityMotion
            };

        /**
        * Enumerates object sharing state.
        *
        * ENotAbleToShare Is not able to share any objects (engine in wrong
        * state)
        * ENotSharing Is not sharing anything
        * EInitializingShareImage Initializing still image sharing.
        * ESharingImage Is sharing image
        * ESharingVideoClip Is sharing video clip
        */
        enum TShareObjectState
            {
            ENotAbleToShare,
            ENotSharing,
            EInitializingShareImage,
            EInitializingShareVideoClip,
            ESharingImage,
            ESharingVideoClip
            };

        /**
        * Defines layout (screen orientation) used by
        * Video Telephone application per active camera.
        * Each camera has one of the following values:
        *
        * EOrientationLandscape Layout is always landscape.
        * EOrientationPortrait Layout is always portrait.
        * EOrientationObeyLayoutSwitch Obey layout in specific device mode.
        */
        enum TCameraOrientation
          	{
            EOrientationLandscape,
            EOrientationPortrait,
            EOrientationObeyLayoutSwitch
          	};

        /**
        * Command parameters for Image sharing.
        */
        typedef TFileName TShareObjectImageParameters;

        /**
        * Command parameters for Video Clip sharing.
        */
        typedef TFileName TShareObjectVideoClipParameters;

        /**
        * Parameters for Prepare Camera.
        */
        struct TPrepareCameraParams
            {
            TMediaSource iMediaSource;
            TBool iInitialize;
            };

        /**
        * Returns state of transmission local video/audio to remote end.
        *
        * @param aMediaDirection
        * @param aActiveMedia returns active media as bitmap (see TMediaType).
        * @return Symbian OS error code.
        */
        virtual TInt GetMediaState(
            const TMediaDirection aMediaDirection,
            TInt& aActiveMedia ) const = 0;

        /**
        * Returns active source for video transmission and
        * local display.
        * @param aSource source in return
        * @return Symbian OS error code
        */
        virtual TInt GetSource( TMediaSource& aSource ) const = 0;

        /**
        * Returns source capabilities.
        * @param combination of capability flags
        * @return Symbian OS error code
        */
        virtual TInt GetSourcesCaps( TInt& aCaps ) const = 0;

        /**
        * Returns currently active camera.
        * @param returns current camera id.
        * @return Symbian OS error code.
        * KErrNotReady if the engine is not ready or it's in wrong state,
        * KErrInUse if camera is used by other application.
        * Camera must be initialized, by async KVtEngPrepareCamera command or
        * KVtEngSetSource command with camera parameter, in order to get
        * KErrInUse indication.
        */
        virtual TInt GetCurrentCameraId( TCameraId& aId ) const = 0;

        /**
        * Returns camera information.
        * @param aId camera to query information for.
        * @param aInfo returns camera information
        * @return Symbian OS error code. If camera is not
        * available returns KErrNotFound.
        */
        virtual TInt GetCameraInfo(
            const TCameraId aId,
            TCameraInfo& aInfo ) const = 0;

        /**
        * Gets current zoom step
        * @param aCurrentStep current zoom step
        * @return Symbian OS error code.
        */
        virtual TInt GetCurrentZoomStep( TInt& aCurrentStep ) = 0;

        /**
        * Gets maximum zoom value that may be passed to camera.
        * @param aMaxZoomStep maximum zoom step.
        * @return Symbian OS error code.
        */
        virtual TInt GetMaxZoomStep( TInt& aMaxZoomStep ) = 0;

        /**
        * Gets number of major zoom level leaps.
        * Less than or equal to maximum digital zoom steps.
        * @param aCount number of leaps
        * @return Symbian OS error code.
        */
        virtual TInt GetMajorZoomStepCount( TInt& aCount ) = 0;

        /**
        * Maps major zoom leap step to corresponding step index.
        * @param aIndex major zoom step.
        * @param aStep zoom step
        */
        virtual void GetMajorZoomStep( TInt aIndex, TInt& aStep ) = 0;

        /**
        * Gets freeze support status.
        * @param aFreezeSupported ETrue if freeze is supported, EFalse otherwise.
        * @return KErrNotReady if provider is not ready yet.
        * @since S60 3.1
        */
        virtual TInt GetFreezeSupported( TBool& aFreezeSupported ) const = 0;

        /**
        * Gets freeze current status.
        * @param aFreezeSupported ETrue if active provider is frozen, EFalse otherwise.
        * @return KErrNotReady if provider is not ready yet.
        * @since S60 3.1
        */
        virtual TInt GetFreezeState( TBool& aFrozen ) const = 0;

        /**
        * Returns state of DSA drawing, whether is active or not.
        * @param aMediaDirection in or out direction
        * @param aState returns state of Direct Screen Access
        * @return Symbian OS error code
        */
        virtual TInt GetDsaState(
            const TMediaDirection aMediaDirection,
            TBool& aActive ) const = 0;

        /**
        * Returns state of VideoCall, whether is connected or not.
        * @return state
        */
        virtual TBool VideoCallConnected () const = 0;
         
        /**
        * Returns ETrue if rendering has been started.
        *
        * @param aDirection specifies the rendering direction to check
        * @return ETrue if rendering has been started.
        */
        virtual TBool RenderingStarted( const TMediaDirection
            aDirection ) const = 0;

        /**
        * Gets current local video quality setting.
        * @param aVideoQuality Current local video quality settings.
        * @since S60 3.1
        */
        virtual void GetVideoQuality( TVideoQuality& aVideoQuality ) const = 0;

        /**
        * Gets current object sharing state.
        * @param aObjectSharingState On return contains current object sharing
        * state. @see MVtEngMedia::TShareObjectState.
        * @since S60 3.1
        */
        virtual void GetObjectSharingState(
            TShareObjectState& aObjectSharingState ) const = 0;

        /**
        * Gets camera orientations for primary and secondary camera.
        * @see MVtEngMedia::TCameraOrientation
        * @param aPrimaryCameraOrientation priamry camera orientation.
        * @param aSecondaryCameraOrientation secondary camera orientation.
        */
        virtual void GetCameraOrientations(
            TCameraOrientation& aPrimaryCameraOrientation,
            TCameraOrientation& aSecondaryCameraOrientation ) const = 0;
    };

#endif      // MVTENGMEDIA_H

// End of File
