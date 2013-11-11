/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Central Repository proxy.
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    <e32def.h>
#include    <e32base.h>
#include    <telinternalcrkeys.h>
#include    <pslninternalcrkeys.h>
#include    <LogsDomainCRKeys.h>
#include    <accessoriescrkeys.h>
#include    <settingsinternalcrkeys.h>
#include    <UiklafInternalCRKeys.h>
#include    <ProfileEngineDomainCRKeys.h>
#include    <DRMHelperServerInternalCRKeys.h>
#include    <btengdomaincrkeys.h> 
#include    <utf.h>

#include    "phoneui.pan"
#include    "cphonecenrepproxy.h"
#include    "mphonecenrepobserver.h"
#include    "cphonecenrepeventhandler.h"
#include    "phoneconstants.h"
#include    "phonelogger.h"
#include    "telprivatecrkeys.h"
#include    "tphonecmdparamappinfo.h"

//CONSTANTS
const TInt KMaxParamLength = 1024;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneCenRepProxy::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneCenRepProxy* CPhoneCenRepProxy::Instance()
    {
    CPhoneCenRepProxy* instance = static_cast<CPhoneCenRepProxy*> 
        ( CCoeEnv::Static ( KUidCenRepProxySingleton ) );
    
    if( !instance )
        {
        TRAPD( err, instance = CPhoneCenRepProxy::NewL() );
        if ( err )
            {
            Panic( EPhoneUtilsCouldNotCreateSingleton );    
            }
        }
    return instance;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::CancelAllNotifies
// Cancel the notification if the singleton still exists.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPhoneCenRepProxy::CancelAllNotifies( 
    MPhoneCenRepObserver* aObserver )
    {
    CPhoneCenRepProxy* instance = static_cast<CPhoneCenRepProxy*> 
        ( CCoeEnv::Static( KUidCenRepProxySingleton ) );
    
    // Ignore the call since the singleton has already been destroyed and the
    // notifications removed
    if( instance != NULL )
        {
        instance->CancelAllObserverNotifies( aObserver );
        }
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::NewL
// ---------------------------------------------------------
//
CPhoneCenRepProxy* CPhoneCenRepProxy::NewL()
    {
    CPhoneCenRepProxy* self = 
        new (ELeave) CPhoneCenRepProxy();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::~CPhoneCenRepProxy
// ---------------------------------------------------------
//
EXPORT_C CPhoneCenRepProxy::~CPhoneCenRepProxy()
    {
    // The Cancel should be always called before destroy the object
    CancelAllNotifies();
    
    delete iCenRepAccessoryEventHandler;
    delete iCenRepNetworkEventHandler;
    delete iCenRepThemesEventHandler;
    delete iCenRepLanguageEventHandler;
    delete iCenRepQwertyEventHandler;
    delete iCenRepKDRMHelperEventHandler;
    delete iObserverArray;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::CPhoneCenRepProxy
// ---------------------------------------------------------
//
CPhoneCenRepProxy::CPhoneCenRepProxy() :
    CCoeStatic( KUidCenRepProxySingleton, EThread )
    {
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::ConstructL
// ---------------------------------------------------------
//
void CPhoneCenRepProxy::ConstructL()
    {
    iObserverArray = new ( ELeave ) CArrayFixFlat<TCenRepObserverTag>( 
        KPhoneCenRepObserversGranularity );

    iTelephonyVariantReadOnlyValues = KPhoneVariantReadOnlyDefaultValue;
    iPhoneUIVariantReadOnlyValues = KPhoneVariantReadOnlyDefaultValue;
    
    User::LeaveIfError( GetTelephonyVariantData() );
    User::LeaveIfError( GetPhoneUIVariantData() );
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::HandleNotifyL
// ---------------------------------------------------------
//
void CPhoneCenRepProxy::HandleNotify( 
    const TUid& aUid, 
    const TUint aId )
    {
    TInt index = FindByUidId( aUid, aId, 0 );
    while ( index != KErrNotFound )
        {
        __ASSERT_DEBUG( index < iObserverArray->Count(), 
            Panic( EPhoneUtilsIndexOutOfBounds ) );
        const TCenRepObserverTag& observerTag = iObserverArray->At( index );
        TRAP_IGNORE( observerTag.iObserver->HandleCenRepChangeL( aUid, aId ) );
        index = FindByUidId( aUid, aId, index+1 );
        }
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::SetInt
// ---------------------------------------------------------
//
EXPORT_C TInt CPhoneCenRepProxy::SetInt( 
    const TUid& aUid, 
    const TUint aId,
    const TInt aValue )
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        err = repository->Set( aId, aValue );
        }
    delete repository;

    if ( err != KErrNone )
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::SetInt() error: %d", 
            err );
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsInvariant ) );
        }

    return err;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::SetString
// ---------------------------------------------------------
//
EXPORT_C TInt CPhoneCenRepProxy::SetString( 
    const TUid& aUid, 
    const TUint aId,
    const TDesC& aValue )
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        err = repository->Set( aId, aValue );
        }
    delete repository;

    if ( err != KErrNone )
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::SetString() error: %d", 
            err );
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsInvariant ) );
        }

    return err;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::SetReal
// ---------------------------------------------------------
//
EXPORT_C TInt CPhoneCenRepProxy::SetReal( 
    const TUid& aUid, 
    const TUint aId,
    const TReal aValue )
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        err = repository->Set( aId, aValue );
        }
    delete repository;

    if ( err != KErrNone )
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::SetReal() error: %d", 
            err );
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsInvariant ) );
        }

    return err;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::GetInt
// ---------------------------------------------------------
//
EXPORT_C TInt CPhoneCenRepProxy::GetInt( 
    const TUid& aUid, 
    const TUint aId,
    TInt& aValue ) const
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        err = repository->Get( aId, aValue );
        delete repository;
        }

    if ( err != KErrNone)
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::GetInt() error: %d", 
            err );
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsInvariant ) );
        }

    return err;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::GetString
// ---------------------------------------------------------
//
EXPORT_C TInt CPhoneCenRepProxy::GetString( 
    const TUid& aUid, 
    const TUint aId,
    TDes& aValue ) const
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        err = repository->Get( aId, aValue );
        delete repository;
        }

    if ( err != KErrNone)
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::GetString() error: %d", 
            err );
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsInvariant ) );
        }

    return err;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::GetReal
// ---------------------------------------------------------
//
EXPORT_C TInt CPhoneCenRepProxy::GetReal( 
    const TUid& aUid, 
    const TUint aId,
    TReal& aValue ) const
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        err = repository->Get( aId, aValue );
        delete repository;
        }

    if ( err != KErrNone)
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::GetReal() error: %d", 
            err );
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsInvariant ) );
        }

    return err;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::NotifyChangeL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneCenRepProxy::NotifyChangeL( 
    const TUid& aUid, 
    const TUint aId, 
    MPhoneCenRepObserver* aObserver )
    {
    // Make sure this request hasn't been request by this observer yet
    if ( FindByUidIdObserver( aUid, aId, aObserver ) == KErrNotFound )
        {
        TCenRepObserverTag tag;
        tag.iUid = aUid;
        tag.iId = aId;
        tag.iObserver = aObserver;
    
        iObserverArray->AppendL( tag );
        }
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::CancelNotify
// ---------------------------------------------------------
//
EXPORT_C void CPhoneCenRepProxy::CancelNotify( 
    MPhoneCenRepObserver* aObserver, 
    const TUint aId )
    {
    TInt index = FindByObserverId( aObserver, aId );
    // This class may be destroyded by CCoeEnv::DestroyEnvironmentStatic() 
    // before CancelNotifys are called.
    if( index == KErrNotFound )
        {
        return;            
        }
    
    __ASSERT_DEBUG( index < iObserverArray->Count(),
        Panic( EPhoneUtilsIndexOutOfBounds ) );
        
    TCenRepObserverTag observerTag = iObserverArray->At( index );
    iObserverArray->Delete( index );
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::CancelAllObserverNotifies
// ---------------------------------------------------------
//
void CPhoneCenRepProxy::CancelAllObserverNotifies( 
    MPhoneCenRepObserver* aObserver )
    {
    const TInt count = iObserverArray->Count();
    for ( TInt i = count-1; i >=0; i-- )
        {
        const TCenRepObserverTag& observerTag = iObserverArray->At( i );
        if ( observerTag.iObserver == aObserver )
            {
            iObserverArray->Delete( i );
            }
        }
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::CancelAllNotifies
// ---------------------------------------------------------
//
void CPhoneCenRepProxy::CancelAllNotifies()
    {
    const TInt count = iObserverArray->Count();
    for ( TInt i = count-1; i >=0; i-- )
        {
        iObserverArray->At( i );
        iObserverArray->Delete( i );
        }
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::FindByUidId
// ---------------------------------------------------------
//
TInt CPhoneCenRepProxy::FindByUidId( 
    const TUid& aUid,
    const TUint aId, 
    TInt aFromIndex )
    {
    const TInt count = iObserverArray->Count();
    TInt result = KErrNotFound;
    for ( TInt i = aFromIndex; i < count; i++ )
        {
        const TCenRepObserverTag& observerTag = iObserverArray->At( i );
        if ( observerTag.iUid == aUid  && observerTag.iId == aId ) 
            {
            result = i;
            break;
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::FindByObserverId
// ---------------------------------------------------------
//
TInt CPhoneCenRepProxy::FindByObserverId( 
    const MPhoneCenRepObserver* aObserver,
    const TUint aId )
    {
    const TInt count = iObserverArray->Count();
    TInt result = KErrNotFound;
    for ( TInt i = 0; i < count; i++ )
        {
        const TCenRepObserverTag& observerTag = iObserverArray->At( i );
        if ( observerTag.iObserver == aObserver && observerTag.iId == aId ) 
            {
            result = i;
            break;
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::FindByUidIdObserver
// ---------------------------------------------------------
//
TInt CPhoneCenRepProxy::FindByUidIdObserver( 
    const TUid& aUid, 
    const TUint aId, 
    MPhoneCenRepObserver* aObserver )
    {
    TInt pos = FindByUidId( aUid, aId, 0 );
    TInt result = KErrNotFound;
    while ( pos != KErrNotFound )
        {
        __ASSERT_DEBUG( pos < iObserverArray->Count(), 
            Panic( EPhoneUtilsIndexOutOfBounds ) );
        const TCenRepObserverTag& observerTag = iObserverArray->At( pos );
        if ( observerTag.iObserver == aObserver )
            {
            result = pos;
            break;
            }

        pos++;
        pos = FindByUidId( aUid, aId, pos );
        }

    return result;
    }   

// -----------------------------------------------------------------------------
// CPhoneCenRepProxy::IsTelephonyFeatureSupported
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPhoneCenRepProxy::IsTelephonyFeatureSupported( 
    const TInt aFeatureId )
    {
    return ( aFeatureId & iTelephonyVariantReadOnlyValues );
    }

// -----------------------------------------------------------------------------
// CPhoneCenRepProxy::GetTelephonyVariantData
// -----------------------------------------------------------------------------
//
TInt CPhoneCenRepProxy::GetTelephonyVariantData()
    {
    TInt err = KErrNone;

    // Variation data should be unchangable during run-time, 
    // therefore, if once succesfully read, later reads are
    // not allowed.
    if( iTelephonyVariantReadOnlyValues == KPhoneVariantReadOnlyDefaultValue )
        {
        err = GetInt( 
            KCRUidTelVariation, 
            KTelVariationFlags, 
            iTelephonyVariantReadOnlyValues );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CPhoneCenRepProxy::IsPhoneUIFeatureSupported
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPhoneCenRepProxy::IsPhoneUIFeatureSupported( 
    const TInt aFeatureId )
    {
    return ( aFeatureId & iPhoneUIVariantReadOnlyValues );
    }

// -----------------------------------------------------------------------------
// CPhoneCenRepProxy::GetPhoneUIVariantData
// -----------------------------------------------------------------------------
//
TInt CPhoneCenRepProxy::GetPhoneUIVariantData()
    {
    TInt err = KErrNone;
/*
    // Variation data should be unchangable during run-time, 
    // therefore, if once succesfully read, later reads are
    // not allowed.
    if ( iPhoneUIVariantReadOnlyValues == KPhoneVariantReadOnlyDefaultValue )
        {
        err = GetInt( 
            KCRUidPhoneAppS60Variation, 
            KPhoneAppS60VariationFlags, 
            iPhoneAppS60VariantReadOnlyValues );
        }
*/
    return err;
    }

// ---------------------------------------------------------
// CPhoneCenRepProxy::Find
// ---------------------------------------------------------
//
EXPORT_C TInt CPhoneCenRepProxy::Find(
    const TUid& aUid,
    const TUint32 aPartial, 
    const TUint32 aMask,
    RArray<TUint32>& aValues ) const
    {
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( aUid ) );
    if ( err == KErrNone )
        {
        TRAP( err, repository->FindL( aPartial, aMask, aValues ) );
        delete repository;
        }

    if ( err != KErrNone)
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::GetInt() error: %d", 
            err );
        __ASSERT_DEBUG( EFalse, Panic( EPhoneUtilsInvariant ) );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CPhoneCenRepProxy::FetchValuesFromCenRepL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneCenRepProxy::FetchValuesFromCenRepL( 
                                                TPhoneCmdParamAppInfo& aAppInfo,
                                                TKeyCode aCode, 
                                                HBufC8* aAppParam,
                                                TBool& aValuesFetched )
    {
    const TUint32     KPartialKey = 0xFF0000;
    const TUint32   KLaunchGroupIdMask = 0x00FF00;
    const TUint32   KLaunchPlaceIdMask = 0x0000FF;
    const TUint32     KIdmask = 0xFF0000;
    
    const TUint32   KLaunchGroupIdShift = 8;
    const TInt      KUidId = 2;
    const TInt      KParamId = 3;

    RArray<TUint32> foundKeys;
    CleanupClosePushL( foundKeys );    
    
    CRepository* repository = NULL;
    repository = CRepository::NewL( KCRUidTelPrivateVariation );
    
    TInt err( KErrNone );
    
    if ( repository )
        {
        CleanupStack::PushL( repository );
        err = repository->FindL( KPartialKey, KIdmask, foundKeys );
        }

    if ( KErrNone != err )
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepProxy::FetchValuesFromCenRepL() error: %d", 
            err );
        User::Leave( err );
        }        
    
    TInt uid(0);
    TInt code(0);
    HBufC* param = HBufC::NewL( KMaxParamLength );
    TPtr paramPtr = param->Des();
    CleanupStack::PushL( param );
    
    for ( TInt i = 0; i < foundKeys.Count(); i++ )
        {
        User::LeaveIfError( repository->Get(
                foundKeys[i], code ) );       
        
        //peekIndex is used to check if foundKeys-array has a next 
        //element. For this reason (i+1) is used.
        TInt peekIndex(i+1);
        TInt peekGroupId(0);
        TInt launchGroupID 
            = ( foundKeys[i] & KLaunchGroupIdMask ) >> KLaunchGroupIdShift;        
        
        if( peekIndex < foundKeys.Count() && code == aCode )
            {
            peekGroupId =
            ( foundKeys[peekIndex] & KLaunchGroupIdMask ) 
                    >> KLaunchGroupIdShift;
        
            // Check that group Id is the same as for the key fetched earlier
            // (KKeyId) and the key id for this key is (KUidId).
            if( launchGroupID == peekGroupId &&
                KUidId == ( foundKeys[peekIndex] & KLaunchPlaceIdMask ) )  
                {
                User::LeaveIfError( repository->Get(
                        foundKeys[++i], uid ) );
                }
            
            peekIndex = i+1; // peekIndex update. The actual value is dependant on
                              // whether variable i was incremented or not in previous if.

            if( peekIndex < foundKeys.Count() )
                {
                peekGroupId = 
                       ( foundKeys[peekIndex] & KLaunchGroupIdMask ) 
                               >> KLaunchGroupIdShift;
                // Check that group Id is still the same as for the keys fetched 
                // earlier (KKeyId and KUidId)
                // and the key id for this key is (KParamId).
                if( launchGroupID == peekGroupId &&
                        KParamId == ( foundKeys[peekIndex] & KLaunchPlaceIdMask ) )
                    {                
                    User::LeaveIfError( repository->Get( 
                            foundKeys[++i], paramPtr ) );
                    break;
                    }
                }
            }
        else
            {
            i++; // skip uid index (wrong code)
            i++; // skip parameter index (wrong code)
            }
        }
    
    if ( uid != 0 )
        {
        aAppInfo.SetAppUid( TUid::Uid( uid ) );
        // TDesc16 to TDesc8 conversion. Central repository supports only
        // 16 bit buffers, setParam function in TPhoneCmdParamAppInfo wants
        // 8 bit.
        TPtr8 eightBitParamPtr = aAppParam->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( eightBitParamPtr, 
                paramPtr );         
        
        aAppInfo.SetParam( eightBitParamPtr );
        aValuesFetched = ETrue;
        }
    
    CleanupStack::PopAndDestroy( param ); 
    CleanupStack::PopAndDestroy( repository );
    CleanupStack::PopAndDestroy( &foundKeys );
    }

//  End of File  
