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
* Description:   Implementation of video playback display handler
*
*/

// Version : %version:  14 %


#ifndef __VIDEOPLAYBACKDISPLAYHANDLER_H__
#define __VIDEOPLAYBACKDISPLAYHANDLER_H__

// INCLUDES

#include <mpxmessage2.h>
#include <MMFScalingCustomCommandConstants.h>
#include <mpxvideoplaybackdefs.h>
#include <mediaclientvideodisplay.h>


//
//  CLASS DECLARATION
//
class CVideoContainer;
class CMPXVideoViewWrapper;
class VideoPlaybackViewFileDetails;

/*
 *  CVideoPlaybackDisplayHandler
 *
 */
class CVideoPlaybackDisplayHandler : public CBase
{
    //
    //  To save user's preference for scaling type in video ratio + screen ratio
    //
    typedef struct
    {
        TReal32         videoRatio;
        TReal32         screenRatio;
        TMMFScalingType scalingType;
    } TMPXAspectRatio ;

    public:

        ~CVideoPlaybackDisplayHandler();

        static CVideoPlaybackDisplayHandler* NewL( CMPXVideoViewWrapper* aViewWrapper );

        void CreateDisplayWindowL( RWsSession& aWs,
                                   CWsScreenDevice& aScreenDevice,
                                   RWindow& aWin,
                                   TRect aDisplayRect,
                                   VideoPlaybackViewFileDetails* aFileDetails );

        void RemoveDisplayWindow();

        void HandleVideoDisplayMessageL( CMPXMessage* aMessage );

        TInt SetAspectRatioL( TMPXVideoPlaybackCommand aCmd );

        TInt CalculateAspectRatioL();

        void UpdateVideoRectL( TRect aRect, TBool transitionEffect );

    private:

        CVideoPlaybackDisplayHandler( CMPXVideoViewWrapper* aViewWrapper );

        void ConstructL();

        void LoadAspectRatioL();

        void SaveAspectRatioL();

        void SetVideoRectL( TRect aClipRect );

        void CalculateVideoRectL();

        static TInt UpdateVideoRectTimeOutL( TAny* aPtr );

    private:
        void AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                RWindowBase& aWindowBase,
                                RWindow* aWin );

        void SurfaceCreatedL( CMPXMessage* aMessage );
        void SurfaceChangedL( CMPXMessage* aMessage );
        void SurfaceRemoved();
        TInt SetNgaAspectRatioL( TMPXVideoPlaybackCommand aCmd );
        TBool IsAspectRatioEqual( TReal32 aRatio1, TReal32 aRatio2 );

    private:
        RArray<TMPXAspectRatio>             iAspectRatioArray;
        TInt                                iCurrentIndexForAspectRatio;
        TReal32                             iDisplayAspectRatio;

        TRect                               iWindowRect;

        TReal32                             iTlXDiff;
        TReal32                             iTlYDiff;
        TReal32                             iBrXDiff;
        TReal32                             iBrYDiff;

        TInt                                iTransitionEffectCnt;

        CPeriodic*                          iResizingTimer;
        CMPXVideoViewWrapper*               iViewWrapper;

        CMediaClientVideoDisplay*           iVideoDisplay;

        RWindowBase*                        iWindowBase;
        TBool                               iSurfaceCached;
        TSurfaceId                          iSurfaceId;
        TRect                               iCropRect;
        TVideoAspectRatio                   iAspectRatio;
        TReal32                             iScaleWidth;
        TReal32                             iScaleHeight;
        TInt                                iHorizontalPosition;
        TInt                                iVerticalPosition;
        TVideoRotation                      iRotation;
        TAutoScaleType                      iAutoScale;
        CVideoContainer*                    iVideoContainer;
        VideoPlaybackViewFileDetails*       iFileDetails;
};

#endif // __VIDEOPLAYBACKDISPLAYHANDLER_H__
