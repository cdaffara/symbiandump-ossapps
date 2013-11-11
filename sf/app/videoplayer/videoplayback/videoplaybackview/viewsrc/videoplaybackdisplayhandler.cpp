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

// Version : %version:  31 %

#include <sysutil.h>
#include <s32file.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxvideoplaybackdefs.h>

#include "videocontainer.h"
#include "mpxvideoviewwrapper.h"
#include "videoplaybackdisplayhandler.h"
#include "mpxvideoregion.h"
#include "videoplaybackviewfiledetails.h"

const TInt KVIDEORESIZINGREPEATRATE = 50000;
const TReal32 KTRANSITIONEFFECTCNT = 8;

_LIT( KAspectRatioFile, "c:\\private\\200159b2\\mpxvideoplayer_aspect_ratio.dat" );


CVideoPlaybackDisplayHandler::CVideoPlaybackDisplayHandler( CMPXVideoViewWrapper* aViewWrapper )
    : iTransitionEffectCnt( 0 )
    , iViewWrapper( aViewWrapper )
    , iScaleWidth( 100.0f )
    , iScaleHeight( 100.0f )
    , iHorizontalPosition( EHorizontalAlignCenter )
    , iVerticalPosition( EVerticalAlignCenter )
    , iRotation( EVideoRotationNone )
    , iAutoScale( EAutoScaleBestFit )
{
}

CVideoPlaybackDisplayHandler::~CVideoPlaybackDisplayHandler()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::~CVideoPlaybackDisplayHandler()"));

    MPX_TRAPD( error, SaveAspectRatioL() );

    if ( iResizingTimer )
    {
        iResizingTimer->Cancel();
        delete iResizingTimer;
        iResizingTimer = NULL;
    }

    iAspectRatioArray.Close();

    if ( iVideoDisplay )
    {
        SurfaceRemoved();

        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }

	if ( iVideoContainer )
    {
        delete iVideoContainer;
        iVideoContainer = NULL;
    }
}

CVideoPlaybackDisplayHandler*
CVideoPlaybackDisplayHandler::NewL( CMPXVideoViewWrapper* aViewWrapper )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::NewL()"));

    CVideoPlaybackDisplayHandler* self =
        new(ELeave) CVideoPlaybackDisplayHandler( aViewWrapper );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
//  CVideoPlaybackDisplayHandler::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::ConstructL()
{
    iResizingTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    LoadAspectRatioL();
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::CreateDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::CreateDisplayWindowL(
                                          RWsSession& /*aWs*/,
                                          CWsScreenDevice& aScreenDevice,
                                          RWindow& aWin,
                                          TRect aDisplayRect,
                                          VideoPlaybackViewFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::CreateDisplayWindowL()"));

    iFileDetails = aFileDetails;

    //
    // get window aspect ratio
    //   if device is in landscape mode, width > height
    //   if device is in portrait mode, width < height
    //
    TReal32 width = (TReal32) aDisplayRect.Width();
    TReal32 height = (TReal32) aDisplayRect.Height();
    iDisplayAspectRatio = (width > height)? (width / height) : (height / width);

    if ( ! iVideoContainer )
    {
        iVideoContainer = new ( ELeave ) CVideoContainer();
        iVideoContainer->ConstructL();
        iVideoContainer->SetRect( aDisplayRect );
    }

    RWindowBase *videoWindow = iVideoContainer->DrawableWindow();
    ((RWindow*)videoWindow )->SetBackgroundColor( KRgbBlack );

    videoWindow->SetOrdinalPosition( -1 );
    (&aWin)->SetOrdinalPosition( 0 );

    MPX_DEBUG(_L("VideoWindow ordinal position is: %d"), videoWindow->OrdinalPosition());
    MPX_DEBUG(_L("UiWindow ordinal position is: %d"), (&aWin)->OrdinalPosition());

    AddDisplayWindowL( aScreenDevice, *videoWindow, (RWindow*)videoWindow );
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::RemoveDisplayWindow()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::RemoveDisplayWindow()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::RemoveDisplayWindow()"));

    if ( iVideoDisplay )
    {
        SurfaceRemoved();
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }

    if ( iVideoContainer )
    {
        delete iVideoContainer;
        iVideoContainer = NULL;
    }

    iSurfaceId = TSurfaceId::CreateNullId();
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::HandleVideoDisplayMessage()"));

    TMPXVideoDisplayCommand message =
        ( *(aMessage->Value<TMPXVideoDisplayCommand>(KMPXMediaVideoDisplayCommand)) );

    MPX_DEBUG(
      _L("CVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL() message = %d"), message );

    switch ( message )
    {
        case EPbMsgVideoSurfaceCreated:
        {
            SurfaceCreatedL( aMessage );
            break;
        }
        case EPbMsgVideoSurfaceChanged:
        {
            SurfaceChangedL( aMessage );
            break;
        }
        case EPbMsgVideoSurfaceRemoved:
        {
            SurfaceRemoved();
            break;
        }
    }
}


// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackDisplayHandler::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::SetAspectRatioL()"));

    TInt aspectRatio;

    aspectRatio = SetNgaAspectRatioL( aCmd );

    //
    //  Update the aspect ratio in the array
    //
    TInt count = iAspectRatioArray.Count();

    if ( count > 0 && count > iCurrentIndexForAspectRatio )
    {
        iAspectRatioArray[iCurrentIndexForAspectRatio].scalingType = (TMMFScalingType)aspectRatio;
    }

    return aspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::CalculateAspectRatioL
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackDisplayHandler::CalculateAspectRatioL()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::CalculateAspectRatioL()"));

    TInt newAspectRatio = EMMFNatural;

    if ( iFileDetails->mVideoHeight > 0 && iFileDetails->mVideoWidth > 0 )
    {
        TMMFScalingType scalingType = EMMFNatural;

        TReal32 videoAspectRatio = (TReal32)iFileDetails->mVideoWidth /
                                   (TReal32)iFileDetails->mVideoHeight;
        //
        //  If the pixel aspect ratio is valid, use it to modify the videoAspectRatio
        //
        if ( iAspectRatio.iDenominator )
        {
            MPX_DEBUG(_L("VideoPlaybackDisplayHandler::CalculateAspectRatioL() iAspectRatio = (%d,%d)"),
                iAspectRatio.iNumerator, iAspectRatio.iDenominator );

            TReal32 par = (TReal32)iAspectRatio.iNumerator / (TReal32)iAspectRatio.iDenominator;
            videoAspectRatio *= par;
       }

        TInt cnt = iAspectRatioArray.Count();
        TInt i = 0;

        //
        //  check whether dat file has the information about (videoRatio + screenRatio)
        //
        for ( ; i < cnt ; i++ )
        {
            if ( IsAspectRatioEqual( iAspectRatioArray[i].videoRatio, videoAspectRatio ) &&
                 IsAspectRatioEqual( iAspectRatioArray[i].screenRatio, iDisplayAspectRatio ) &&
                 ( scalingType = iAspectRatioArray[i].scalingType ) > 0 )
            {
                break;
            }
        }

        //
        //  if can't find out match aspect ratio in dat file,
        //  choose the scaling type through the rule
        //      aspectRatioDiff =  videoAspectRatio - iDisplayAspectRatio
        //      aspectRatioDiff >= - 0.00001 and <= 0.00001    ==> natural
        //      aspectRatioDiff > 0.1                          ==> zoom
        //      aspectRatioDiff < - 0.3                        ==> natural
        //      aspectRatioDiff >= - 0.3 and <= 0.1            ==> stretch
        //
        //               -0.3      -0.00001       0.00001         0.1
        //     ------------------------------------------------------------
        //         Natural |   Stretch  |   Natural  |   Stretch   |  Zoom
        //

        if ( i == cnt )
        {
            TReal32 aspectRatioDiff = videoAspectRatio - iDisplayAspectRatio;

            MPX_DEBUG(_L("VideoPlaybackDisplayHandler::CalculateAspectRatioL() videoAspectRatio = d,iDisplayAspectRatio = %d)"),
                    videoAspectRatio, iDisplayAspectRatio );

            if ( IsAspectRatioEqual( videoAspectRatio, iDisplayAspectRatio )  )
            {
                scalingType = EMMFNatural;
            }
            else if ( aspectRatioDiff > 0.1 )
            {
                scalingType = EMMFZoom;
            }
            else if ( aspectRatioDiff > - 0.3 )
            {
                scalingType = EMMFStretch;
            }

            TMPXAspectRatio ratio;

            ratio.videoRatio = videoAspectRatio;
            ratio.screenRatio = iDisplayAspectRatio;
            ratio.scalingType = scalingType;

            iAspectRatioArray.Append( ratio );
        }

        iFileDetails->mAspectRatioChangeable =
                ! IsAspectRatioEqual( videoAspectRatio, iDisplayAspectRatio );

        iCurrentIndexForAspectRatio = i;

        TMPXVideoPlaybackCommand aspectRatioCmd = EPbCmdNaturalAspectRatio;

        if ( scalingType == EMMFZoom )
        {
            aspectRatioCmd = EPbCmdZoomAspectRatio;
        }
        else if ( scalingType == EMMFStretch )
        {
            aspectRatioCmd = EPbCmdStretchAspectRatio;
        }

        newAspectRatio = SetAspectRatioL( aspectRatioCmd );
    }

    return newAspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SaveAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::SaveAspectRatioL()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::SaveAspectRatioL"));

    RFs fs;
    TInt err = fs.Connect();
    CleanupClosePushL<RFs>( fs );

    TBool canSave = EFalse;

    TRAP_IGNORE( canSave = ! SysUtil::FFSSpaceBelowCriticalLevelL(
                                 &fs, sizeof(TMPXAspectRatio) * iAspectRatioArray.Count()) );

    if ( canSave )
    {
        // save list to disk
        RFileWriteStream out;

        TInt err( out.Replace( fs, KAspectRatioFile, EFileWrite ) );

        if ( err == KErrPathNotFound )
        {
            fs.MkDirAll( KAspectRatioFile );
            err = out.Create( fs, KAspectRatioFile, EFileWrite );
        }

        if ( ! err )
        {
            TInt cnt = iAspectRatioArray.Count();

            MPX_TRAP( err,
            {
                for ( TInt i = 0 ; i < cnt ; i++ )
                {
                    //Save (videoRatio + screenRatio + scalingType)
                    out.WriteReal32L( iAspectRatioArray[i].videoRatio );
                    out.WriteReal32L( iAspectRatioArray[i].screenRatio );
                    out.WriteInt8L( iAspectRatioArray[i].scalingType );
                }
            } );

            out.Close();
        }
    }

    CleanupStack::PopAndDestroy();
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::LoadAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::LoadAspectRatioL()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::LoadAspectRatioL()"));

    RFs fs;
    RFileReadStream in;

    TInt err = fs.Connect();
    CleanupClosePushL<RFs>( fs );

    if ( ! err && in.Open( fs, KAspectRatioFile, EFileRead ) == KErrNone )
    {
        TMPXAspectRatio ratio;

        MPX_TRAP( err,
        {
            for ( err = KErrNone ; err == KErrNone ; )
            {
                //
                //  Read (videoRatio + screenRatio + scalingType)
                //
                ratio.videoRatio = in.ReadReal32L();
                ratio.screenRatio = in.ReadReal32L();
                ratio.scalingType = (TMMFScalingType)in.ReadInt8L();

                iAspectRatioArray.Append( ratio );
            }
        } );

        in.Close();
    }

    CleanupStack::PopAndDestroy();
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::UpdateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::UpdateVideoRectL( TRect aClipRect, TBool transitionEffect )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::UpdateVideoRectL()"));

    if ( transitionEffect )
    {
        iTlXDiff = (TReal32)( iWindowRect.iTl.iX - aClipRect.iTl.iX ) / KTRANSITIONEFFECTCNT;
        iTlYDiff = (TReal32)( iWindowRect.iTl.iY - aClipRect.iTl.iY ) / KTRANSITIONEFFECTCNT;
        iBrXDiff = (TReal32)( iWindowRect.iBr.iX - aClipRect.iBr.iX ) / KTRANSITIONEFFECTCNT;
        iBrYDiff = (TReal32)( iWindowRect.iBr.iY - aClipRect.iBr.iY ) / KTRANSITIONEFFECTCNT;

        if ( iResizingTimer->IsActive() )
        {
            iResizingTimer->Cancel();
        }

        iResizingTimer->Start(
                0,
                KVIDEORESIZINGREPEATRATE,
                TCallBack( CVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL, this ) );
    }
    else
    {
        SetVideoRectL( aClipRect );

        iWindowRect = aClipRect;

        iViewWrapper->UpdateVideoRectDone();
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL()
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL( TAny* aPtr )
{
    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL()"));

    static_cast<CVideoPlaybackDisplayHandler*>(aPtr)->CalculateVideoRectL();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::CalculateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::CalculateVideoRectL()
{
    iTransitionEffectCnt++;

    TRect windowRect( (TInt)( (TReal32)iWindowRect.iTl.iX - iTlXDiff * (TReal32)iTransitionEffectCnt ),
                      (TInt)( (TReal32)iWindowRect.iTl.iY - iTlYDiff * (TReal32)iTransitionEffectCnt ),
                      (TInt)( (TReal32)iWindowRect.iBr.iX - iBrXDiff * (TReal32)iTransitionEffectCnt ),
                      (TInt)( (TReal32)iWindowRect.iBr.iY - iBrYDiff * (TReal32)iTransitionEffectCnt ) );

    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::CalculateVideoRectL() %d %d %d %d"),
            windowRect.iTl.iX, windowRect.iTl.iY, windowRect.iBr.iX, windowRect.iBr.iY );

    SetVideoRectL( windowRect );

    if ( iTransitionEffectCnt >= KTRANSITIONEFFECTCNT )
    {
        iTransitionEffectCnt = 0;
        iWindowRect = windowRect;

        if ( iResizingTimer->IsActive() )
        {
            iResizingTimer->Cancel();
        }

        MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::CalculateVideoRectL() Done"));

        iViewWrapper->UpdateVideoRectDone();
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SetVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::SetVideoRectL( TRect aRect )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::SetVideoRectL()"));

    if ( iVideoDisplay )
    {
        TRect temp = aRect;

        MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::SetVideoRectL() origianl rect %d %d %d %d"),
                temp.iTl.iX, temp.iTl.iY, temp.iBr.iX, temp.iBr.iY );

        //
        // Need to transform based on rotation clockwise
        // since the input rect is based on orbit orientation(which can be landscape or potrait)
        // and the video surface is always in device orientation(can't be changed by anything).
        // If iRotation is EVideoRotationNone,
        // we don't need to transfrom the rect since video and ui surfaces are in same orietation.
        // but if iRotation is not EVideoRotationNone,
        // we need to transform the rect based on clockwise.
        //
        switch( iRotation )
        {
            case EVideoRotationClockwise90:
            {
                int screenWidth = iWindowBase->Size().iWidth;

                temp = TRect( screenWidth - aRect.iBr.iY,
                              aRect.iTl.iX,
                              screenWidth - aRect.iTl.iY,
                              aRect.iBr.iX );
                break;
            }
        }

        MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::SetVideoRectL() transformed rect %d %d %d %d"),
                temp.iTl.iX, temp.iTl.iY, temp.iBr.iX, temp.iBr.iY );

        iVideoDisplay->SetVideoExtentL( *iWindowBase, temp, iCropRect );
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::AddDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                                         RWindowBase& aWindowBase,
                                                         RWindow* aWin )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::AddDisplayWindowL()"));

    iWindowBase = &aWindowBase;

    TInt displayId = aScreenDevice.GetScreenNumber();

    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::AddDisplayWindowL() displayId %d"), displayId);

    CMediaClientVideoDisplay* tempDisplay = iVideoDisplay;

    iVideoDisplay = CMediaClientVideoDisplay::NewL( displayId );

    delete tempDisplay;

    TRect cropRect = TRect( aWin->Size() );

    //
    // If RWindow is still in potrait, rotate surface to play a video in landscape
    //
    if ( cropRect.Width() < cropRect.Height() )
    {
        iRotation = EVideoRotationClockwise90;
    }

    iWindowRect = cropRect;

    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::AddDisplayWindowL() cropRect (%d, %d), (%d, %d)"),
        cropRect.iTl.iX, cropRect.iTl.iY, cropRect.iBr.iX, cropRect.iBr.iY);

    MPX_TRAPD( dispError,
    iVideoDisplay->AddDisplayWindowL( iWindowBase,
                                      cropRect,
                                      cropRect,
                                      cropRect,
                                      iScaleWidth,
                                      iScaleHeight,
                                      iRotation,
                                      iAutoScale,
                                      iHorizontalPosition,
                                      iVerticalPosition,
                                      aWin );
    );

    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::AddDisplayWindowL() Display Added"));

    //
    //  Check if surface was created before window was ready
    //
    if ( iSurfaceCached )
    {
        iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );

        iSurfaceCached = EFalse;

        //
        // Let ControlsController know that we get the surface.
        //
        iViewWrapper->SurfacedAttached( true );
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SurfaceCreatedL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::SurfaceCreatedL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::SurfaceCreatedL()"));

    TSurfaceId oldSurfaceId = iSurfaceId;

    //
    //  Extract the surface parameters from the message
    //
    iSurfaceId = aMessage->ValueTObjectL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId );
    iCropRect = aMessage->ValueTObjectL<TRect>( KMPXMediaVideoDisplayCropRect );
    iAspectRatio = aMessage->ValueTObjectL<TVideoAspectRatio>( KMPXMediaVideoDisplayAspectRatio );

    iViewWrapper->SetDefaultAspectRatio( CalculateAspectRatioL() );

    if ( iVideoDisplay )
    {
        //
        //  Remove old surface if one exists
        //
        if ( ! oldSurfaceId.IsNull() )
        {
            iVideoDisplay->RemoveSurface();
        }

        //
        //  Add new surface
        //
        iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );

        //
        // Let ControlsController know that we get the surface.
        //
        iViewWrapper->SurfacedAttached( true );
    }
    else
    {
        //
        //  Video display has not been created yet, save surface information to create
        //  the surface when the display is created
        //
        iSurfaceCached = ETrue;
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SurfaceChangedL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::SurfaceChangedL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::SurfaceChangedL()"));

    //
    //  Extract the surface parameters from the message
    //
    iSurfaceId = aMessage->ValueTObjectL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId );
    iCropRect = aMessage->ValueTObjectL<TRect>( KMPXMediaVideoDisplayCropRect );
    iAspectRatio = aMessage->ValueTObjectL<TVideoAspectRatio>( KMPXMediaVideoDisplayAspectRatio );

    iViewWrapper->SetDefaultAspectRatio( CalculateAspectRatioL() );

    if ( iVideoDisplay )
    {
        //
        //  Add new surface
        //
        iVideoDisplay->SurfaceParametersChanged( iSurfaceId, iCropRect, iAspectRatio );

        iVideoDisplay->RedrawWindows( iCropRect );
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SurfaceRemoved()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::SurfaceRemoved()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::SurfaceRemoved()"));

    //
    // Let ControlsController know that we get the surface.
    //
    iViewWrapper->SurfacedAttached( false );

    if ( iVideoDisplay )
    {
        iVideoDisplay->RemoveSurface();
    }

    iSurfaceId = TSurfaceId::CreateNullId();
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SetNgaAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackDisplayHandler::SetNgaAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::SetNgaAspectRatioL()"));

    TInt aspectRatio = EMMFNatural;

    switch ( aCmd )
    {
        case EPbCmdNaturalAspectRatio:
        {
            iAutoScale = EAutoScaleBestFit;
            aspectRatio = EMMFNatural;
            break;
        }
        case EPbCmdZoomAspectRatio:
        {
            iAutoScale = EAutoScaleClip;
            aspectRatio = EMMFZoom;
            break;
        }
        case EPbCmdStretchAspectRatio:
        {
            iAutoScale = EAutoScaleStretch;
            aspectRatio = EMMFStretch;
            break;
        }
    }

    if ( iVideoDisplay && ! iSurfaceId.IsNull() )
    {
        iVideoDisplay->SetAutoScaleL( iAutoScale,
                                      iHorizontalPosition,
                                      iVerticalPosition,
                                      iCropRect );
    }

    return aspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::IsAspectRatioEqual()
// -------------------------------------------------------------------------------------------------
//
TBool CVideoPlaybackDisplayHandler::IsAspectRatioEqual( TReal32 aRatio1, TReal32 aRatio2 )
{
    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::IsAspectRatioEqual() ar1 = %f ar2 = %f)"),
            aRatio1, aRatio2 );

    TBool valuesEqual = EFalse;
    TReal32 arDiff = aRatio1 - aRatio2;

    if ( arDiff < 0.00001 && arDiff > -0.00001 )
    {
        valuesEqual = ETrue;
    }

    MPX_DEBUG(_L("CVideoPlaybackDisplayHandler::IsAspectRatioEqual(%d)"), valuesEqual);

    return valuesEqual;
}

// End of File
