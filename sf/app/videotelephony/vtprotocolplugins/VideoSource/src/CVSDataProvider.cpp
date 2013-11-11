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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES
#include <badesca.h>
#include <e32svr.h>
#include <featmgr.h>
#include "CApiVideoSource.h"
#include "CVSCameraDataProvider.h"
#include "CVSStillImageDataProvider.h"
#include "CVSOnboardCameraDataProvider.h"

// EXTERNAL FUNCTION PROTOTYPES

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

class TSizeCount
    {
    public:
        TInt    iCount;
        TSize   iSize;
    };

class TRateCount
    {
    public:
        TInt    iCount;
        TReal32 iRate;
    };

// LOCAL FUNCTION PROTOTYPES

static TInt TSizeCountTLinearOrderFunc( 
    const TSizeCount& aSizeCount1, const TSizeCount& aSizeCount2 );
static TBool TSizeCountTIdentityRelationFunc( 
    const TSizeCount& aSizeCount1, const TSizeCount& aSizeCount2 );
static TInt TRateCountTLinearOrderFunc( 
    const TRateCount& aRateCount1, const TRateCount& aRateCount2 );
static TBool TRateCountTIdentityRelationFunc( 
    const TRateCount& aRateCount1, const TRateCount& aRateCount2 );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSizeCountTLinearOrderFunc: Used in RArray<TSizeCount)::InsertInOrder
// -----------------------------------------------------------------------------
//
static TInt TSizeCountTLinearOrderFunc( 
    const TSizeCount& aSizeCount1,
    const TSizeCount& aSizeCount2 )
    {
    if ( aSizeCount1.iSize == aSizeCount2.iSize )
        {
        return 0;
        }
    else
        {
        // It is assumed that width and height fit in 16-bit, if they don't,
        // use TInt64
        TInt32 s1 = 
            ( aSizeCount1.iSize.iWidth & 0xffff ) << 16 | 
            ( aSizeCount1.iSize.iHeight & 0xffff );
        TInt32 s2 = 
            ( aSizeCount2.iSize.iWidth & 0xffff ) << 16 | 
            ( aSizeCount2.iSize.iHeight & 0xffff );
        return s1 - s2;
        }
    }

// -----------------------------------------------------------------------------
// TSizeCountTIdentityRelationFunc: Used in RArray<TSizeCount)::Find
// -----------------------------------------------------------------------------
//
static TBool TSizeCountTIdentityRelationFunc( 
    const TSizeCount& aSizeCount1, 
    const TSizeCount& aSizeCount2 )
    {
    if ( aSizeCount1.iSize == aSizeCount2.iSize )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TRateCountTLinearOrderFunc: Used in RArray<TRateCount)::InsertInOrder
// -----------------------------------------------------------------------------
//
static TInt TRateCountTLinearOrderFunc( 
    const TRateCount& aRateCount1, 
    const TRateCount& aRateCount2 )
    {
    if ( aRateCount1.iRate > aRateCount2.iRate ) 
        {
        return 1;
        }
        
    if ( aRateCount1.iRate < aRateCount2.iRate )
        {
        return -1;
        }
        
    return 0;
    }

// -----------------------------------------------------------------------------
// TRateCountTIdentityRelationFunc: Used in RArray<TRateCount)::Find
// -----------------------------------------------------------------------------
//
static TBool TRateCountTIdentityRelationFunc( 
    const TRateCount& aRateCount1, 
    const TRateCount& aRateCount2 )
    {
    if ( aRateCount1.iRate == aRateCount2.iRate )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVSDataProvider::NewL
// -----------------------------------------------------------------------------
//
CVSDataProvider* CVSDataProvider::NewL( 
    TInt aProviderIndex, 
    MVSDataProviderObserver* aObserver, 
    MVSBufferPool* aPool )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::NewL() [aProviderIndex = %d] >>"), RThread().Id().operator TUint(), aProviderIndex));
    
    CVSDataProvider* self = NULL;
    
    // Is it ecam
    TInt count = CamerasAvailable();
    
    // < 0 == error
    if( count < 0 )
        {
        User::Leave( count );
        }

    if ( aProviderIndex < count )
        {
        self = CVSCameraDataProvider::NewL( aObserver, aProviderIndex, aPool );
        }
    else
        {
        // Is it still image
        count += 1;

        if ( aProviderIndex == ( count - 1) )
            {
            self = CVSStillImageDataProvider::NewL( aObserver, aPool );
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::~CVSDataProvider
// -----------------------------------------------------------------------------
//
CVSDataProvider::~CVSDataProvider()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::~CVSDataProvider() >>"), RThread().Id().operator TUint()));
    delete iErrorNotifier;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::~CVSDataProvider() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::ProvidersAvailable
// -----------------------------------------------------------------------------
//
TInt CVSDataProvider::ProvidersAvailable()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ProvidersAvailable() >>"), RThread().Id().operator TUint()));
    
    // Count ecams
    TInt numProviders = CamerasAvailable();

    // < 0 == error
    if ( numProviders < 0 )
        {
        numProviders = 1; // only still image provider
        }
    else
        {
        numProviders += 1; // add still
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ProvidersAvailable() [numProviders = %d] <<"), RThread().Id().operator TUint(), numProviders));

    return numProviders;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::ProviderInfoL
// -----------------------------------------------------------------------------
//
void CVSDataProvider::ProviderInfoL( 
    TInt aProviderIndex, 
    TVSDataProviderInfo& aInfo )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ProviderInfoL() >>"), RThread().Id().operator TUint()));
    CVSDataProvider* provider = NewL( aProviderIndex, NULL, NULL);
    provider->ProviderInfo( aInfo );
    delete provider;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ProviderInfoL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::SetFillBufferParams
// -----------------------------------------------------------------------------
//
void CVSDataProvider::SetFillBufferParams( 
    MVTVideoSink* aConsumer, 
    TMediaId aMediaId )
    {
    iConsumer = aConsumer;
    iMediaId = aMediaId;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::NotifyError
// -----------------------------------------------------------------------------
//
void CVSDataProvider::NotifyError( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::NotifyError() >>"), RThread().Id().operator TUint()));
    iErrorNotifier->NotifyError( &Observer(), aError );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::NotifyError() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::CVSDataProvider
// -----------------------------------------------------------------------------
//
CVSDataProvider::CVSDataProvider( 
    MVSDataProviderObserver* aObserver, 
    MVSBufferPool* aPool ) : 
        iObserver( aObserver ), 
        iPool ( aPool )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::CVSDataProvider() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::CVSDataProvider() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CVSDataProvider::ConstructL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ConstructL() >>"), RThread().Id().operator TUint()));
    iErrorNotifier = CProviderErrorNotifierAO::NewL();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ConstructL() <<"), RThread().Id().operator TUint()));
    }


// -----------------------------------------------------------------------------
// CVSDataProvider::TimeToPlay
// -----------------------------------------------------------------------------
//
const TTimeIntervalMicroSeconds& CVSDataProvider::TimeToPlay()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::TimeToPlay() >>"), RThread().Id().operator TUint()));

    TTime now;
    now.HomeTime();

    TTimeIntervalMicroSeconds timeToPlay( now.MicroSecondsFrom( iStartTime ) );

    if( timeToPlay <= iPreviousTimeToPlay )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::TimeToPlay(): *WARNING* Time warp backwards! Correcting.."), RThread().Id().operator TUint()));
        iPreviousTimeToPlay = 
            TTimeIntervalMicroSeconds( iPreviousTimeToPlay.Int64() + 1 );
        }
    else
        {
        iPreviousTimeToPlay = timeToPlay;
        }

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::TimeToPlay() <<"), RThread().Id().operator TUint()));
    return iPreviousTimeToPlay;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::ResetStartTime
// -----------------------------------------------------------------------------
//
void CVSDataProvider::ResetStartTime()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ResetStartTime() >>"), RThread().Id().operator TUint()));
    iStartTime.HomeTime();
    iPreviousTimeToPlay = iStartTime.MicroSecondsFrom( iStartTime );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::ResetStartTime() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::CamerasAvailable()
// -----------------------------------------------------------------------------
//
TInt CVSDataProvider::CamerasAvailable()
    {
    TRAPD( result, FeatureManager::InitializeLibL() );
    
    if( result != KErrNone )
        {
        return result;
        }
    
    TInt count( 0 );

    if( FeatureManager::FeatureSupported( KFeatureIdCamera ) )
        {
        count = CCamera::CamerasAvailable();
        }

    FeatureManager::UnInitializeLib();

    return count;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::SwitchFrom
// -----------------------------------------------------------------------------
//
void CVSDataProvider::SwitchFrom( const CVSDataProvider& anOldProvider )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::SwitchFrom() >>"), RThread().Id().operator TUint()));
    iStartTime = anOldProvider.iStartTime;
    iPreviousTimeToPlay = anOldProvider.iPreviousTimeToPlay;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::SwitchFrom() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::EnumerateVideoFrameFormatsL
// -----------------------------------------------------------------------------
//
void CVSDataProvider::EnumerateVideoFrameFormatsL( 
    CDesC8Array* aSupportedFormats )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::EnumerateVideoFrameFormats() >>"), RThread().Id().operator TUint()));
    TInt providerCount = ProvidersAvailable();    
    TInt countOfProvidersThatSupportYUV420Planar = 0;
    for ( TInt i = 0; i < providerCount; i++ )
        {
        TVSDataProviderInfo info;
        ProviderInfoL( i, info );
        if ( info.iVideoFrameFormatsSupported & CCamera::EFormatYUV420Planar )
            {
            countOfProvidersThatSupportYUV420Planar++;
            }
        }
    if( providerCount > 0 && 
        providerCount <= countOfProvidersThatSupportYUV420Planar )
        {
        aSupportedFormats->AppendL( KVtVideoMIMETypeYUV420 );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::EnumerateVideoFrameFormats() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::EnumerateVideoFrameSizesL
// -----------------------------------------------------------------------------
//
void CVSDataProvider::EnumerateVideoFrameSizesL( 
    RArray<TSize>& aSupportedSizes, 
    const TDesC8& aFormat )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::EnumerateVideoFrameSizes() >>"), RThread().Id().operator TUint()));
    CleanupClosePushL( aSupportedSizes );
    RArray<TSizeCount> sizeCountArray;
    CleanupClosePushL( sizeCountArray );
    TInt providerCount = ProvidersAvailable();
    for ( TInt i = 0; i < providerCount; i++ )
        {
        CVSDataProvider* p = CVSDataProvider::NewL( i, NULL, NULL );
        CleanupStack::PushL( p );
        TVSDataProviderInfo info;
        ProviderInfoL( i, info );
        for ( TInt j = 0; j < info.iNumVideoFrameSizesSupported; j++ )
            {
            TSize size;
            p->GetVideoFrameSize( size, j, aFormat );
            if ( size != TSize(0,0) )
                {
                TSizeCount sizeCount;
                sizeCount.iCount = 1;
                sizeCount.iSize = size;
                TInt err = sizeCountArray.InsertInOrder( 
                    sizeCount, 
                    TLinearOrder<TSizeCount>( TSizeCountTLinearOrderFunc ) );
                if ( err == KErrAlreadyExists )
                    {
                    TInt idx = sizeCountArray.Find( 
                        sizeCount, 
                        TIdentityRelation<TSizeCount>( 
                            TSizeCountTIdentityRelationFunc ) );
                    if ( idx == KErrNotFound )
                        {
                        User::Leave( KErrUnknown );
                        }
                    else
                        {
                        // Increment count of providers that supports the size
                        sizeCountArray[idx].iCount++;
                        }
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
            }
        CleanupStack::PopAndDestroy(); // p
        }
    aSupportedSizes.Reset();
    for ( TInt k = 0; k < sizeCountArray.Count(); k++ )
        {
        // Do all providers support the size
        if ( providerCount > 0 && providerCount <= sizeCountArray[k].iCount ) 
            {
            User::LeaveIfError( 
                aSupportedSizes.Append( sizeCountArray[k].iSize ) );
            }
        }
    CleanupStack::PopAndDestroy(); // sizeCountArray
    CleanupStack::Pop(); // aSupportedSizes
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::EnumerateVideoFrameSizes() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::EnumerateVideoFrameRatesL
// -----------------------------------------------------------------------------
//
void CVSDataProvider::EnumerateVideoFrameRatesL( 
    RArray<TReal32>& aSupportedRates, 
    const TDesC8& aFormat, 
    const TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::EnumerateVideoFrameRates() >>"), RThread().Id().operator TUint()));
    CleanupClosePushL( aSupportedRates );
    RArray<TRateCount> rateCountArray;
    CleanupClosePushL( rateCountArray );
    TInt providerCount = ProvidersAvailable();
    __IF_DEBUG(Print(_L("   LOOKING FOR COMMON SUPPORTED RATES, PROVIDER COUNT: %d"), providerCount));
    for ( TInt i = 0; i < providerCount; i++ )
        {
        __IF_DEBUG(Print(_L("      PROVIDER %d"), i));
        CVSDataProvider* p = CVSDataProvider::NewL( i, NULL, NULL );
        CleanupStack::PushL( p );
        TVSDataProviderInfo info;
        ProviderInfoL( i, info );
        __IF_DEBUG(Print(_L("         info.iNumVideoFrameRatesSupported: %d"), info.iNumVideoFrameRatesSupported));
        for ( TInt j = 0; j < info.iNumVideoFrameRatesSupported; j++ )
            {
            TReal32 rate = 0.0;
            p->GetVideoFrameRate( rate, j, aFormat, aSize );
            __IF_DEBUG(Print(_L("         RATE FOR RATE INDEX %d IS %f"), j, rate));
            if ( rate != 0.0 )
                {
                TRateCount rateCount;
                rateCount.iCount = 1;
                rateCount.iRate = rate;
                TInt err = rateCountArray.InsertInOrder( 
                    rateCount, 
                    TLinearOrder<TRateCount>( TRateCountTLinearOrderFunc ) );
                if ( err == KErrAlreadyExists )
                    {
                    TInt idx = rateCountArray.Find( 
                        rateCount, 
                        TIdentityRelation<TRateCount>( 
                            TRateCountTIdentityRelationFunc ) );
                    if ( idx == KErrNotFound )
                        {
                        User::Leave( KErrUnknown );
                        }
                    else
                        {
                        // Increment count of providers that supports the rate
                        rateCountArray[idx].iCount++;
                        }
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
            }
        CleanupStack::PopAndDestroy(); // p
        }
    aSupportedRates.Reset();
    for ( TInt k = 0; k < rateCountArray.Count(); k++ )
        {
        // Do all providers support the rate
        if ( providerCount > 0 && providerCount <= rateCountArray[k].iCount )
            {
            User::LeaveIfError( 
                aSupportedRates.Append( rateCountArray[k].iRate ) );
            }
        }
    CleanupStack::PopAndDestroy( ); // rateCountArray
    CleanupStack::Pop(); // aSupportedRates
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataProvider::EnumerateVideoFrameRates() <<"), RThread().Id().operator TUint()));
    }

//  End of File
