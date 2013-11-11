/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Videoteleng  Camera Preferences extension
*
*/


// INCLUDE FILES
#include    "CVtEngCameraPreferences.h"
#include    "VtEngPanic.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "mvtengcamerapreferencesobserver.h"

#include    <capivideosource.h>
#include    <cvtlogger.h>

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::CVtEngCameraPreferences
// C++ constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CVtEngCameraPreferences::CVtEngCameraPreferences():
                        CVtEngExtensionBase(
                        KVtEngExtensionCameraPreferences )
    {
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVtEngCameraPreferences::ConstructL()
    {
    __VTPRINTENTER( "CVtEngCameraPreferences.ConstructL" )
    __VTPRINTEXIT( "CVtEngCameraPreferences.ConstructL" )
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVtEngCameraPreferences* CVtEngCameraPreferences::NewL()
    {
    CVtEngCameraPreferences* self = new( ELeave ) CVtEngCameraPreferences;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::~CVtEngCameraPreferences
// Destructor. Cannot leave.
// ----------------------------------------------------------------------------
//
CVtEngCameraPreferences::~CVtEngCameraPreferences()
    {
    __VTPRINTENTER( "CameraPreferences.~" )
    if ( iObserver )
        {
        iObserver->Detach( *this );
        }
    __VTPRINTEXIT( "CameraPreferences.~" )
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::GetInterface
//
// ----------------------------------------------------------------------------
//
TAny* CVtEngCameraPreferences::GetInterface()
    {
    __VTPRINTENTER( "CameraPreferences.GetInterface")
    // Cast first as API MVtEngCameraPreferences* class
    // and after that to TAny*
    __VTPRINTEXIT( "CameraPreferences.GetInterface" )
    return reinterpret_cast< TAny* >( ( MVtEngCameraPreferences* ) this );

    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::HandleL
//
// ----------------------------------------------------------------------------
//
TBool CVtEngCameraPreferences::HandleL( CVtEngOperation* aOp )
    {
    __VTPRINTENTER( "CameraPreferences.HandleL" )
    const TVtEngCommandId id( aOp->Command() );
    __VTPRINT2( DEBUG_GEN, "CVtEngCameraPreferences.HandleL CMD ID[%d] ", id )

    // Check provider
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR( "CVtEngCameraPreferences.HandleL iProvider [%d]",
                	( TInt )iProvider )
        User::Leave( KErrNotReady );
        }
    // Check which command and call
    // active provider with operation params
    switch ( id )
        {
        case KVtEngSetContrast:
        	{
        	TInt contrast = 0;
        	TVtEngOpParamUtil< TInt >::Set( contrast, *aOp );
            iProvider->SetContrastL( contrast );
            if ( iObserver )
                {
                iObserver->ContrastUpdated( contrast );
                }
            break;
        	}
        case KVtEngSetBrightness:
            {
     		TInt brightness = 0;
        	TVtEngOpParamUtil< TInt >::Set( brightness, *aOp );
            iProvider->SetBrightnessL( brightness );
            if ( iObserver )
                {
                iObserver->BrightnessUpdated( brightness );
                }
            break;
            }
        case KVtEngSetWhiteBalance:
            {
            CCamera::TWhiteBalance whitebalance = CCamera::EWBAuto;
        	TVtEngOpParamUtil< CCamera::TWhiteBalance >::Set( whitebalance,
        		*aOp );
            iProvider->SetWhiteBalanceL( whitebalance );
            if ( iObserver )
                {
                iObserver->WhiteBalanceUpdated( TWhiteBalance( whitebalance ) );
                }
            break;
            }
        case KVtEngSetColorTone:
            {
            CCamera::CCameraImageProcessing::TEffect colortone =
            	CCamera::CCameraImageProcessing::EEffectNone;
        	TVtEngOpParamUtil< CCamera::CCameraImageProcessing::TEffect >::Set(
        		colortone, *aOp );
            iProvider->SetColorToneL( colortone );
            if ( iObserver )
                {
                iObserver->ColorToneUpdated( TColorTone( colortone ) );
                }
            break;
            }
        //Not supported command
        default:
            __VTPRINTEXITR(
            "CVtEngCameraPreferences.HandleL NOT SUPPORTED CMD[%d]", id )
            return EFalse;
        }
    __VTPRINTEXIT( "CameraPreferences.HandleL" )
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::ValidateCommand
//
// ----------------------------------------------------------------------------

TBool CVtEngCameraPreferences::ValidateCommand(
	const TVtEngCommandId aCommandId )
    {
    __VTPRINTENTER( "CameraPreferences.ValidateCommand" )
    // Check provider
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR(
        	"CVtEngCameraPreferences.ValidateCommand iProvider [%d]",
        	( TInt ) iProvider)
        return EFalse;
        }
    // Take provider info is needed down below
    TVSDataProviderInfo info;
    iProvider->ProviderInfo( info );

    // local or hw support
    // set as False
    TBool hworlocalsupport = EFalse;

    // Check which command and check if
    // if provider and local variation supports
     __VTPRINT2( DEBUG_GEN,
     	"CVtEngCameraPreferences.ValidateCommand CMD ID[%d] ", aCommandId )
    switch ( aCommandId )
        {
        case KVtEngSetContrast:
	        {
            if ( info.iOptionsSupported & TCameraInfo::EContrastSupported  )
                {
                hworlocalsupport = ETrue;
                }
            break;
	        }
        case KVtEngSetBrightness:
            {
            if ( info.iOptionsSupported & TCameraInfo::EBrightnessSupported )
                {
                hworlocalsupport = ETrue;
                }
            break;
            }
        case KVtEngSetWhiteBalance:
            {
            if ( info.iWhiteBalanceModesSupported != CCamera::EWBAuto )
                {
                hworlocalsupport = ETrue;
                }
            break;
            }
        case KVtEngSetColorTone:
            {
            if ( info.iSupportedColorTones !=
            	CCamera::CCameraImageProcessing::EEffectNone )
                {
                hworlocalsupport = ETrue;
                }
            break;
            }
        // Not supported command
        default:
        	{
            __VTPRINT2( DEBUG_GEN,
            	"CVtEngCameraPreferences.ValidateCommand NOT SUPPORTED CMD[%d]",
            	 aCommandId)
            hworlocalsupport = EFalse;
        	}
        }
    __VTPRINTEXITR( "CVtEngCameraPreferences.ValidateCommand return value[%d]",
    	hworlocalsupport)
    return hworlocalsupport;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::GetColorTone
//
// ----------------------------------------------------------------------------
//
TInt CVtEngCameraPreferences::GetColorTone(
	MVtEngCameraPreferences::TColorTone& aColortone ) const
    {
    __VTPRINTENTER( "CameraPreferences.GetColorTone" )
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR( "CVtEngCameraPreferences.GetColorTone iProvider[%d]",
        	 ( TInt )iProvider)
        return KErrNotReady;
        }
    TInt error;
    CCamera::CCameraImageProcessing::TEffect colortone;
    error = iProvider->GetColorTone( colortone );
    aColortone = ( MVtEngCameraPreferences::TColorTone ) colortone;
    __VTPRINTEXITR( "CameraPreferences.GetColorTone error [%d]",error )
    return error;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::GetSupportedColorTones
//
// ----------------------------------------------------------------------------
//
TInt CVtEngCameraPreferences::GetSupportedColorTones(
	TUint32& aSupportedColorTones ) const
    {
    __VTPRINTENTER( "CameraPreferences.GetSupportedColorTones" )
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR(
        	"CVtEngCameraPreferences.GetSupportedColorTones iProvider[%d]",
        	( TInt )iProvider)
        return KErrNotReady;
        }

    TVSDataProviderInfo info;
    iProvider->ProviderInfo( info );
    aSupportedColorTones = info.iSupportedColorTones;
    __VTPRINTEXITR( "CameraPreferences.GetSupportedColorTones [%b]",
    	info.iSupportedColorTones)
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::GetWhiteBalance
//
// ----------------------------------------------------------------------------
//
TInt CVtEngCameraPreferences::GetWhiteBalance(
	MVtEngCameraPreferences::TWhiteBalance& aWhiteBalance ) const
    {
    __VTPRINTENTER( "CameraPreferences.GetWhiteBalance" )
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR(
        	"CVtEngCameraPreferences.GetWhiteBalance iProvider [%d]",
        	( TInt )iProvider )
        return KErrNotReady;
        }
    TInt error;
    CCamera::TWhiteBalance whitebalance;
    error = iProvider->GetWhiteBalance( whitebalance );
    aWhiteBalance = (MVtEngCameraPreferences::TWhiteBalance) whitebalance;
	__VTPRINTEXITR( "CameraPreferences.GetWhiteBalance error [%d]",error )
    return error;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::GetSupportedWhiteBalanceModes
//
// ----------------------------------------------------------------------------
//
TInt CVtEngCameraPreferences::GetSupportedWhiteBalanceModes(
	TUint32& aWhiteBalance  ) const
    {
    __VTPRINTENTER( "CameraPreferences.GetSupportedWhiteBalanceModes" )
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR(
        	"CVtEngCameraPreferences.GetSupportedWhiteBalanceModes iProvider [%d]",
        	( TInt )iProvider)
        return KErrNotReady;
        }

    TVSDataProviderInfo info;
    iProvider->ProviderInfo( info );
    aWhiteBalance = info.iWhiteBalanceModesSupported;
    __VTPRINTEXITR( "CameraPreferences.GetSupportedWhiteBalanceModes [%b]",
    	info.iWhiteBalanceModesSupported)
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::GetBrightness
//
// ----------------------------------------------------------------------------
//
TInt CVtEngCameraPreferences::GetBrightness( TInt& aBrightness  ) const
    {
    __VTPRINTENTER( "CameraPreferences.GetBrightness" )
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR( "CVtEngCameraPreferences.GetBrightness iProvider [%d]",
        	( TInt )iProvider)

        return KErrNotReady;
        }
    TInt error;
    error = iProvider->GetBrightness( aBrightness );
	__VTPRINTEXITR( "CameraPreferences.GetBrightness error [%d]",error )
    return error;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::GetContrast
//
// ----------------------------------------------------------------------------
//
TInt CVtEngCameraPreferences::GetContrast( TInt& aContrast  ) const
    {
    __VTPRINTENTER( "CameraPreferences.GetContrast" )
    if ( iProvider == NULL )
        {
        __VTPRINTEXITR( "CVtEngCameraPreferences.GetContrast iProvider [%d]",
        	( TInt )iProvider)
       	return KErrNotReady;
        }
    TInt error;
    error = iProvider->GetContrast( aContrast );
	__VTPRINTEXITR( "CameraPreferences.GetContrast error [%d]",error )

    return error;
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::SetProvider
//
// ----------------------------------------------------------------------------
//
void CVtEngCameraPreferences::SetProvider( CVSDataProvider* aProvider,
    MVtEngCameraPreferencesObserver& aObserver )
    {
    __VTPRINTENTER( "CameraPreferences.SetProvider" )
    if ( iObserver )
        {
        iObserver->Detach( *this );
        iObserver = NULL;
        }
    iProvider = aProvider;
    iObserver = &aObserver;
    if ( iObserver )
        {
        iObserver->Attach( *this );
        }
    __VTPRINTEXIT( "CameraPreferences.SetProvider" )
    }

// ----------------------------------------------------------------------------
// CVtEngCameraPreferences::Detach
//
// ----------------------------------------------------------------------------
//
void CVtEngCameraPreferences::Detach( MVtEngCameraPreferencesObserver&
    aObserver )
    {
    __VTPRINTENTER( "CameraPreferences.Detach" )
    if ( iObserver == &aObserver )
        {
        iObserver = NULL;
        }
    __VTPRINTEXITR( "CameraPreferences.Detach $%x", TInt( iObserver ) )
    }



//  End of File
