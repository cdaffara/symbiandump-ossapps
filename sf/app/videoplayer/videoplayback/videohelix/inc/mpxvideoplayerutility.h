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
* Description:  This class is the interface between the playback plugin and RMMFController
*
*/


// Version : %version: 14 %


#ifndef __MPXVIDEOPLAYERUTILITY__
#define __MPXVIDEOPLAYERUTILITY__

//
//  INCLUDES
//
#include <mmf/common/mmfcontroller.h>
#include <e32base.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfstandardcustomcommandsimpl.h>
#else
#include <mmf/common/mmfstandardcustomcommands.h>
#endif

#include <mmf/common/mmfdrmcustomcommands.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#endif

//
//  FORWARD DECLARATIONS
//
class CMPXVideoPlaybackController;
class CFbsBitmap;

//
//  CLASS DECLARATION
//

NONSHARABLE_CLASS( CMpxVideoPlayerUtility ) : public CActive,
                                              public MMMFControllerEventMonitorObserver
{
    public:

        static CMpxVideoPlayerUtility* NewL( CMPXVideoPlaybackController* aVideoPlaybackCtrl );
        ~CMpxVideoPlayerUtility();

        void Reset();

        void OpenFileL( const TDesC& aFileName );
        void OpenFileL( const RFile& aFile );
        void OpenUrlL( const TDesC& aUrl, TInt aApId );

        TTimeIntervalMicroSeconds PositionL() const;
        TTimeIntervalMicroSeconds DurationL() const;

        TInt NumberOfMetaDataEntriesL() const;
        CMMFMetaDataEntry* MetaDataEntryL( TInt aIndex ) const;

        TBool AudioEnabledL() const;
        void SetVolumeL( TInt aVolume );
        TInt Volume() const;
        TInt MaxVolume() const;

        void VideoFrameSizeL( TSize& aSize ) const;
        TInt VideoBitRateL() const;
        TInt AudioBitRateL() const;
        TInt VideoFormatMimeType( TDes8& aMimeType ) const;
        TUint32 FourCCCode() const;
        void RefreshFrameL();

        void RestartDsa( const TRegion& aRegion );
        void AbortDsa();
        void SetDisplayWindowL( const TRect& aScreenRect,
                                const TRect& aClipRect,
                                const TRegion& aDrawingRegion );

        TInt CustomCommandSync( const TMMFMessageDestinationPckg& aDestination,
                                TInt aFunction,
                                const TDesC8& aDataTo1,
                                const TDesC8& aDataTo2 );

        void Prepare();
        void PlayL();
        void PauseL();
        void Close();
        TInt Stop();

        void SetPositionL( const TTimeIntervalMicroSeconds& aPosition );

        void SetPlayVelocityL( TInt aVelocity );

        void GetVideoLoadingProgressL( TInt& aPercentageProgress );

        void GetFrameL();

        //
        //  MMMFControllerEventMonitorObserver Implementation
        //
        void HandleEvent( const TMMFEvent& aEvent );

        CFbsBitmap& GetBitmap();

        void RunL();
        void DoCancel();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        void OpenFile64L( const RFile64& aFile );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

#ifdef SYMBIAN_BUILD_GCE

        TInt VideoSurfaceCreated();
        TInt SurfaceParametersChanged();
        void RemoveSurface();
        TInt RemoveSurfaceFromHelix( TSurfaceId aSurfaceId );

        void SendSurfaceCommandL( TInt aCmd );

        void SendSurfaceCommandL( TInt aCmd,
                                  const TSurfaceId& aSurfaceId,
                                  const TRect& aCropRect,
                                  TVideoAspectRatio aAspectRatio );

        void SendSurfaceCreatedCommand();

#endif

    private:

        void ConstructL();
        CMpxVideoPlayerUtility( CMPXVideoPlaybackController* aVideoPlaybackCtrl );
        void OpenControllerL();

    private:

        //
        //  Data
        //
        CMPXVideoPlaybackController*              iVideoPlaybackController;

        RMMFController                            iController;
        RMMFVideoControllerCustomCommands         iVideoControllerCustomCommands;
        RMMFVideoPlayControllerCustomCommands     iVideoPlayControllerCustomCommands;
        RMMFAudioPlayDeviceCustomCommands         iAudioPlayDeviceCustomCommands;
        RMMFDRMCustomCommands                     iDrmCustomCommands;
        RMMFVideoPlayControllerExtCustomCommands  iVideoPlayControllerExtCustomCommands;

#ifdef SYMBIAN_BUILD_GCE
        RMMFVideoPlaySurfaceSupportCustomCommands iVideoPlaySurfaceSupportCustomCommands;

        TSurfaceId                                iSurfaceId;
        TRect                                     iCropRect;
        TVideoAspectRatio                         iAspectRatio;
#endif

        CMMFControllerEventMonitor*               iControllerEventMonitor;

        TBool                                     iDirectScreenAccessAbort;

        CFbsBitmap*                               iPosterFrameBitmap;
};

#endif /* __MPXVIDEOPLAYERUTILITY__ */
