/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "cxedummycamera.h"

#if defined(CXE_USE_DUMMY_CAMERA) || defined(__WINSCW__)

#include <bitdev.h>

#include "cxutils.h"

// Disable warnings about unused arguments
#pragma warn_unusedarg off

namespace
{
    static const TSize IMAGE_CAPTURE_SIZES[] =
        {
        TSize(4000,3000),
        TSize(4000,2248),
        TSize(3264,2448),
        TSize(3264,1832),
        TSize(2592,1944),
        TSize(2592,1456),
        TSize(2048,1536),
        TSize(1600,1200),
        TSize(1280,960),
        TSize(1024,768),
        TSize(640,480)
        };
    static const int IMAGE_CAPTURE_SIZE_COUNT = sizeof(IMAGE_CAPTURE_SIZES)/sizeof(TSize);

    _LIT( PANICDUMMYCAMERA, "DummyCamera" );
}

CxeDummyBuffer::CxeDummyBuffer() : iData(_L8("ABC"))
    {
    CX_DEBUG_IN_FUNCTION();
    }

CxeDummyBuffer::~CxeDummyBuffer()
    {
    CX_DEBUG_ENTER_FUNCTION();
    delete iBitmap;
    CX_DEBUG_EXIT_FUNCTION();
    }



CxeDummySnapshot::CxeDummySnapshot()
    {
    CX_DEBUG(("snap this = 0x%08x", this));
    CX_DEBUG_IN_FUNCTION();
    }

CxeDummySnapshot::~CxeDummySnapshot()
    {
    CX_DEBUG_ENTER_FUNCTION();
    delete iBuffer;
    CX_DEBUG_EXIT_FUNCTION();
    }




CxeDummyCamera* CxeDummyCamera::NewL(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority, TInt aCameraVersion)
{
    return new (ELeave) CxeDummyCamera( aObserver, aCameraIndex, aPriority, aCameraVersion );
}


CxeDummyCamera::CxeDummyCamera(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority, TInt aCameraVersion)
: iObserver( aObserver ), iAdvancedSettings(0), iSnapshot(0), iStillImage(0), iImageProcessing(0)
{
    iCommandTimer = CPeriodic::NewL(0);

    iInfo.iMaxZoom = 5;
    iInfo.iMaxDigitalZoom = 5;
    iInfo.iMaxDigitalZoomFactor = 0;
    iInfo.iMaxZoomFactor = 0;
    iInfo.iMinZoomFactor = 0;
    iInfo.iMinZoom = 0;

    iInfo.iNumImageSizesSupported = IMAGE_CAPTURE_SIZE_COUNT;
}

CxeDummyCamera::~CxeDummyCamera()
    {
    delete iCommandTimer;
    delete iAdvancedSettings;
    delete iSnapshot;
    delete iStillImage;
    delete iImageProcessing;
    }

void CxeDummyCamera::doCommand( TCxeDummyCommand aCmd )
{
    TInt status = iCommandBuf.Insert( aCmd, 0 );
    if ( status != KErrNone )
        {
        User::Panic(PANICDUMMYCAMERA, 1);
        }
    if ( !iCommandTimer->IsActive() )
        {
        iCommandTimer->Start(100, 100, TCallBack(CxeDummyCamera::callBack, this));
        }
}

TInt CxeDummyCamera::callBack( TAny* aParam )
{
    CxeDummyCamera* self = (CxeDummyCamera*)aParam;
    return self->doHandleCallback();
}

void CxeDummyCamera::CaptureImage()
{
    doCommand( EProvideStillImage );

    if ( iSnapshot && iSnapshot->IsSnapshotActive() )
        {
        doCommand( EProvideSnapshot );
        }
}

void CxeDummyCamera::EnumerateCaptureSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const
{
    if(aSizeIndex < IMAGE_CAPTURE_SIZE_COUNT)
        {
        aSize = IMAGE_CAPTURE_SIZES[aSizeIndex];
        }
    else
        {
        aSize = TSize(0,0);
        }
}

TInt CxeDummyCamera::doHandleCallback()
{
    TCxeDummyCommand cmd = iCommandBuf[ 0 ];
    iCommandBuf.Remove( 0 );

    switch ( cmd )
        {
        case EReserve:
            {
            iState = EStReserved;
            TECAMEvent e( KUidECamEventReserveComplete, KErrNone );
            iObserver.HandleEvent( e );
            break;
            }
        case EPowerOn:
            {
            iState = EStPowerOn;
            TECAMEvent e( KUidECamEventPowerOnComplete, KErrNone );
            iObserver.HandleEvent( e );
            break;
            }
        case EProvideStillImage:
            {
            if ( !iStillImage )
                {
                iStillImage = new (ELeave) CxeDummyBuffer();
                }
            iObserver.ImageBufferReady(*iStillImage, KErrNone);
            break;
            }
        case EProvideSnapshot:
            {
            TECAMEvent e( KUidECamEventCameraSnapshot, KErrNone );
            iObserver.HandleEvent( e );
            break;
            }
        }

    if ( !iCommandBuf.Count() )
        {
        iCommandTimer->Cancel();
        }

    return KErrNone;
}

void CxeDummyCamera::Reserve()
{
    CX_DEBUG_ENTER_FUNCTION();
    iState = EStReserving;
    doCommand( EReserve );
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeDummyCamera::Release()
{
    CX_DEBUG_ENTER_FUNCTION();
    iState = EStReleased;
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeDummyCamera::PowerOn()
{
    iState = EStPoweringOn;
    doCommand( EPowerOn );
}

void CxeDummyCamera::PowerOff()
{
    iState = EStReserved;
}

TAny* CxeDummyCamera::CustomInterface(TUid aInterface)
    {
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeDummyCamera::CustomInterface: request for interface uid %x", aInterface));

    if ( aInterface == KECamMCameraAdvancedSettingsUid )
        {
        CX_DEBUG(("Advanced settings custom interface requested"));
        if ( !iAdvancedSettings )
            {
            iAdvancedSettings = new ( ELeave ) CxeDummyAdvancedSettings(this);
            }
        CX_DEBUG_EXIT_FUNCTION();
        return static_cast<MCameraAdvancedSettings*>( iAdvancedSettings );
        }
    else if (aInterface == KECamMCameraAdvancedSettings3Uid)
        {
        CX_DEBUG(("Advanced settings 3 custom interface requested"));
        if (!iAdvancedSettings)
            {
            iAdvancedSettings = new (ELeave) CxeDummyAdvancedSettings(this);
            }
        CX_DEBUG_EXIT_FUNCTION();
        return static_cast<MCameraAdvancedSettings3*> (iAdvancedSettings);
        }
    else if ( aInterface == KECamMCameraSnapshotUid )
        {
        CX_DEBUG(("Snapshot custom interface requested"));
        if ( !iSnapshot )
            {
            iSnapshot = new ( ELeave ) CxeDummySnapshot();
            }
        CX_DEBUG_EXIT_FUNCTION();
        return static_cast<MCameraSnapshot*>( iSnapshot );
        }
    else if ( aInterface == KECamMCameraDirectViewFinderUid )
        {
        CX_DEBUG(("Direct viewfinder custom interface requested"));
        if( !iDirectViewfinder )
            {
            iDirectViewfinder = new ( ELeave ) CxeDummyDirectViewfinder();
            }
        return static_cast<MCameraDirectViewFinder*>( iDirectViewfinder );
        }
    else if ( aInterface == KECamMCameraImageProcessingUid )
        {
        CX_DEBUG(("Image processing custom interface requested"));
        if (!iImageProcessing )
            {
            iImageProcessing = new ( ELeave ) CxeDummyImageProcessing;
            }
        return static_cast<MCameraImageProcessing*>( iImageProcessing );
        }
    CX_DEBUG_EXIT_FUNCTION();
    return NULL;
    }


void CxeDummyBuffer::CreateBitmapL( const TSize& aSize )
    {
    CX_DEBUG_ENTER_FUNCTION();

    delete iBitmap;
    iBitmap = NULL;

    iBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iBitmap->Create( aSize, EColor16MU ) );
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( iBitmap );
    CleanupStack::PushL( device );
    CFbsBitGc* gc;
    User::LeaveIfError( device->CreateContext(gc) );
    CleanupStack::PushL(gc);

    const TInt gridsize = 16;

    gc->SetPenStyle( CGraphicsContext::ENullPen );
    gc->SetBrushStyle( CGraphicsContext::ESolidBrush );

    for ( TInt x = 0; x < aSize.iWidth/16; x++ )
        {
        for ( TInt y = 0; y < aSize.iWidth/16; y++ )
            {
            gc->SetBrushColor( ( ( x + y ) & 1 ) ? 0x00AAAAAA : 0x00888888 );
            gc->DrawRect( TRect( TPoint( x * 16, y * 16 ), TSize( gridsize, gridsize ) ) );
            }
        }

    CleanupStack::PopAndDestroy(gc);
    CleanupStack::PopAndDestroy(device);

    CX_DEBUG_EXIT_FUNCTION();
    }

void CxeDummySnapshot::PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio)
{
    CX_DEBUG_ENTER_FUNCTION();
    PrepareSnapshotL( aFormat, aSize, aMaintainAspectRatio );
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeDummySnapshot::PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio)
{
    CX_DEBUG_ENTER_FUNCTION();
    if ( !iBuffer )
        {
        iBuffer = new ( ELeave ) CxeDummyBuffer();
        }
    iBuffer->CreateBitmapL( aSize );
    CX_DEBUG_EXIT_FUNCTION();
}


// CxeDummyDirectViewfinder

CxeDummyDirectViewfinder::CxeDummyDirectViewfinder()
    : iState(CCamera::CCameraDirectViewFinder::EViewFinderInActive)
    {
    CX_DEBUG_IN_FUNCTION();
    }

CxeDummyDirectViewfinder::~CxeDummyDirectViewfinder()
    {
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
    }

void CxeDummyDirectViewfinder::Release()
    {
    iState = CCamera::CCameraDirectViewFinder::EViewFinderInActive;
    }

void CxeDummyDirectViewfinder::PauseViewFinderDirectL()
    {
    if( iState != CCamera::CCameraDirectViewFinder::EViewFinderActive )
        {
        User::Leave( KErrGeneral );
        }
    iState = CCamera::CCameraDirectViewFinder::EViewFinderPause;
    }

void CxeDummyDirectViewfinder::ResumeViewFinderDirectL()
    {
    if( iState != CCamera::CCameraDirectViewFinder::EViewFinderPause )
        {
        User::Leave( KErrGeneral );
        }
    iState = CCamera::CCameraDirectViewFinder::EViewFinderActive;
    }

CCamera::CCameraDirectViewFinder::TViewFinderState CxeDummyDirectViewfinder::ViewFinderState() const
    {
    return iState;
    }


// CxeDummyAdvancedSettings

CxeDummyAdvancedSettings::CxeDummyAdvancedSettings(CCamera* aCamera)
    : iCamera( aCamera )
{
    QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION(iNotificationTimer = CPeriodic::NewL(0));
    iZoomValue = 0;
}

CxeDummyAdvancedSettings::~CxeDummyAdvancedSettings()
{
    delete iNotificationTimer;
    iNotificationTimer = NULL;

    iClientNofications.Reset();
    iClientNofications.Close();
}

void CxeDummyAdvancedSettings::GetDigitalZoomStepsForStillL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aSizeIndex,
                CCamera::TFormat aFormat, TBool& aIsInfluencePossible) const
{

    aDigitalZoomSteps.Reset();
    aDigitalZoomSteps.AppendL(0);
    aDigitalZoomSteps.AppendL(1);
    aDigitalZoomSteps.AppendL(2);
    aDigitalZoomSteps.AppendL(3);
    aDigitalZoomSteps.AppendL(4);
    aDigitalZoomSteps.AppendL(5);

}

void CxeDummyAdvancedSettings::GetDigitalZoomStepsL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo) const
{
    aDigitalZoomSteps.Reset();
    aDigitalZoomSteps.AppendL(0);
    aDigitalZoomSteps.AppendL(1);
    aDigitalZoomSteps.AppendL(2);
    aDigitalZoomSteps.AppendL(3);
    aDigitalZoomSteps.AppendL(4);
    aDigitalZoomSteps.AppendL(5);
}


TInt CxeDummyAdvancedSettings::SupportedFocusModes() const
{
    return CCamera::CCameraAdvancedSettings::EFocusModeAuto;
}

CCamera::CCameraAdvancedSettings::TFocusMode CxeDummyAdvancedSettings::FocusMode() const
{
    return iFocusMode;
}

void CxeDummyAdvancedSettings::SetFocusMode(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode)
{
    iFocusMode = aFocusMode;

    // notify client with relevant event..
    queueClientNotification(KUidECamEventCameraSettingFocusMode, KErrNone);
}

TInt CxeDummyAdvancedSettings::SupportedFocusRanges() const
{
    return CCamera::CCameraAdvancedSettings::EFocusRangeAuto
         | CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal
         | CCamera::CCameraAdvancedSettings::EFocusRangeInfinite
         | CCamera::CCameraAdvancedSettings::EFocusRangeMacro
         | CCamera::CCameraAdvancedSettings::EFocusRangeNormal
         | CCamera::CCameraAdvancedSettings::EFocusRangePortrait
         | CCamera::CCameraAdvancedSettings::EFocusRangeSuperMacro
         | CCamera::CCameraAdvancedSettings::EFocusRangeTele;
}

CCamera::CCameraAdvancedSettings::TFocusRange CxeDummyAdvancedSettings::FocusRange() const
{
    return iFocusRange;
}

void CxeDummyAdvancedSettings::SetFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange aFocusRange)
{
    iFocusRange = aFocusRange;
    queueClientNotification(KUidECamEventCameraSettingFocusRange2, KErrNone);
}

TInt CxeDummyAdvancedSettings::SupportedAutoFocusTypes() const
{
    return CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle
         | CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff;
}

CCamera::CCameraAdvancedSettings::TAutoFocusType CxeDummyAdvancedSettings::AutoFocusType() const
{
    return iFocusType;
}

void CxeDummyAdvancedSettings::SetAutoFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType aAutoFocusType)
{
    iFocusType = aAutoFocusType;
    queueClientNotification(KUidECamEventCameraSettingAutoFocusType2, KErrNone);

    // If AF set to OFF, remove any "optimal focus" events from earlier
    // started focusing.
    if (iFocusType == CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff) {
        for (TInt i = iClientNofications.Count()-1; i >= 0; i--) {
             if (iClientNofications[i].iEventUid == KUidECamEventCameraSettingsOptimalFocus) {
                 iClientNofications.Remove(i);
             }
        }
    }
    else {
        // Otherwise queue optimal focus event for this focusing request.
        queueClientNotification(KUidECamEventCameraSettingsOptimalFocus, KErrNone);
    }
}








void CxeDummyAdvancedSettings::queueClientNotification(TUid aUid, TInt aStatus)
{
    CX_DEBUG_ENTER_FUNCTION();
    iClientNofications.Append(TCxeDummyNotification(aUid,aStatus));
    if (iNotificationTimer == NULL) {
        QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION(iNotificationTimer = CPeriodic::NewL(0));
    }

    if (!iNotificationTimer->IsActive()) {
        TCallBack cb(CxeDummyAdvancedSettings::clientNotificationCallback, this);
        iNotificationTimer->Start(100, 100, cb);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeDummyAdvancedSettings::doClientNotification( )
{
    CX_DEBUG_ENTER_FUNCTION();
    const TInt count(iClientNofications.Count());

    if (count != 0) {
        const TCxeDummyNotification& notify(iClientNofications[count-1]);
        const TECAMEvent event(notify.iEventUid, notify.iStatus);
        iClientNofications.Remove(count-1);
        (static_cast<CxeDummyCamera*>(iCamera))->iObserver.HandleEvent(event);
    }
    else {
        delete iNotificationTimer;
        iNotificationTimer = NULL;
    }
    CX_DEBUG_EXIT_FUNCTION();
}

TInt CxeDummyAdvancedSettings::clientNotificationCallback(TAny* aParam)
{
    (static_cast<CxeDummyAdvancedSettings*>(aParam))->doClientNotification();
    return 1; // Not used by CPeriodic
}


#endif // defined(CXE_USE_DUMMY_CAMERA) || defined(__WINSCW__)
