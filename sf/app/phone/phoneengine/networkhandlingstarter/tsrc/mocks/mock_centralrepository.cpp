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
* Description:
*
*/
#include <smcmockclassincludes.h>
#include <centralrepository.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRepository::NewL
// -----------------------------------------------------------------------------
//
CRepository * CRepository::NewL( 
        TUid aRepositoryUid )
    {
    SMC_MOCK_NEWL1( CRepository *, TUid, aRepositoryUid )

    CRepository* self = new( ELeave ) CRepository;
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CRepository::NewLC
// -----------------------------------------------------------------------------
//
CRepository * CRepository::NewLC( 
        TUid aRepositoryUid )
    {
    SMC_MOCK_NEWLC1( CRepository *, TUid, aRepositoryUid )

    CRepository* self = new( ELeave ) CRepository;
    CleanupStack::PushL( self );
    //self->ConstructL()
    return self;
    }


// -----------------------------------------------------------------------------
// CRepository::~CRepository
// -----------------------------------------------------------------------------
//
CRepository::~CRepository(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CRepository::Create
// -----------------------------------------------------------------------------
//
TInt CRepository::Create( 
        TUint32 aKey,
        TInt aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TInt, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Create
// -----------------------------------------------------------------------------
//
TInt CRepository::Create( 
        TUint32 aKey,
        const TReal & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        const TReal &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Create
// -----------------------------------------------------------------------------
//
TInt CRepository::Create( 
        TUint32 aKey,
        const TDesC8 & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        const TDesC8 &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Create
// -----------------------------------------------------------------------------
//
TInt CRepository::Create( 
        TUint32 aKey,
        const TDesC16 & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        const TDesC16 &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Delete
// -----------------------------------------------------------------------------
//
TInt CRepository::Delete( 
        TUint32 aKey )
    {
    SMC_MOCK_METHOD1( TInt, TUint32, aKey )
    }


// -----------------------------------------------------------------------------
// CRepository::Delete
// -----------------------------------------------------------------------------
//
TInt CRepository::Delete( 
        TUint32 aPartialKey,
        TUint32 aMask,
        TUint32 & aErrorKey )
    {
    SMC_MOCK_METHOD3( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        TUint32 &, aErrorKey )
    }


// -----------------------------------------------------------------------------
// CRepository::Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get( 
        TUint32 aKey,
        TInt & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TInt &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Set
// -----------------------------------------------------------------------------
//
TInt CRepository::Set( 
        TUint32 aKey,
        TInt aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TInt, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get( 
        TUint32 aKey,
        TReal & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TReal &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Set
// -----------------------------------------------------------------------------
//
TInt CRepository::Set( 
        TUint32 aKey,
        const TReal & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        const TReal &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get( 
        TUint32 aKey,
        TDes8 & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TDes8 &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get( 
        TUint32 aId,
        TDes8 & aValue,
        TInt & aActualLength )
    {
    SMC_MOCK_METHOD3( TInt, TUint32, aId, 
        TDes8 &, aValue, 
        TInt &, aActualLength )
    }


// -----------------------------------------------------------------------------
// CRepository::Set
// -----------------------------------------------------------------------------
//
TInt CRepository::Set( 
        TUint32 aKey,
        const TDesC8 & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        const TDesC8 &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get( 
        TUint32 aKey,
        TDes16 & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TDes16 &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get( 
        TUint32 aId,
        TDes16 & aValue,
        TInt & aActualLength )
    {
    SMC_MOCK_METHOD3( TInt, TUint32, aId, 
        TDes16 &, aValue, 
        TInt &, aActualLength )
    }


// -----------------------------------------------------------------------------
// CRepository::Set
// -----------------------------------------------------------------------------
//
TInt CRepository::Set( 
        TUint32 aKey,
        const TDesC16 & aValue )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        const TDesC16 &, aValue )
    }


// -----------------------------------------------------------------------------
// CRepository::GetMeta
// -----------------------------------------------------------------------------
//
TInt CRepository::GetMeta( 
        TUint32 aKey,
        TUint32 & aMeta )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TUint32 &, aMeta )
    }


// -----------------------------------------------------------------------------
// CRepository::Move
// -----------------------------------------------------------------------------
//
TInt CRepository::Move( 
        TUint32 aSourcePartialKey,
        TUint32 aTargetPartialKey,
        TUint32 aMask,
        TUint32 & aErrorKey )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aSourcePartialKey, 
        TUint32, aTargetPartialKey, 
        TUint32, aMask, 
        TUint32 &, aErrorKey )
    }


// -----------------------------------------------------------------------------
// CRepository::FindL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD3( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindEqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindEqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        TInt aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        TInt, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindEqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindEqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        const TReal & aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        const TReal &, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindEqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindEqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        const TDesC8 & aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        const TDesC8 &, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindEqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindEqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        const TDesC16 & aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        const TDesC16 &, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindNeqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindNeqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        TInt aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        TInt, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindNeqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindNeqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        const TReal & aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        const TReal &, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindNeqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindNeqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        const TDesC8 & aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        const TDesC8 &, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::FindNeqL
// -----------------------------------------------------------------------------
//
TInt CRepository::FindNeqL( 
        TUint32 aPartialKey,
        TUint32 aMask,
        const TDesC16 & aValue,
        RArray<TUint32> & aFoundKeys )
    {
    SMC_MOCK_METHOD4( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        const TDesC16 &, aValue, 
        RArray<TUint32> &, aFoundKeys )
    }


// -----------------------------------------------------------------------------
// CRepository::NotifyRequest
// -----------------------------------------------------------------------------
//
TInt CRepository::NotifyRequest( 
        TUint32 aKey,
        TRequestStatus & aStatus )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aKey, 
        TRequestStatus &, aStatus )
    }


// -----------------------------------------------------------------------------
// CRepository::NotifyRequest
// -----------------------------------------------------------------------------
//
TInt CRepository::NotifyRequest( 
        TUint32 aPartialKey,
        TUint32 aMask,
        TRequestStatus & aStatus )
    {
    SMC_MOCK_METHOD3( TInt, TUint32, aPartialKey, 
        TUint32, aMask, 
        TRequestStatus &, aStatus )
    }


// -----------------------------------------------------------------------------
// CRepository::NotifyCancel
// -----------------------------------------------------------------------------
//
TInt CRepository::NotifyCancel( 
        TUint32 aKey )
    {
    SMC_MOCK_METHOD1( TInt, TUint32, aKey )
    }


// -----------------------------------------------------------------------------
// CRepository::NotifyCancel
// -----------------------------------------------------------------------------
//
TInt CRepository::NotifyCancel( 
        TUint32 aPartialKey,
        TUint32 aMask )
    {
    SMC_MOCK_METHOD2( TInt, TUint32, aPartialKey, 
        TUint32, aMask )
    }


// -----------------------------------------------------------------------------
// CRepository::NotifyCancelAll
// -----------------------------------------------------------------------------
//
TInt CRepository::NotifyCancelAll(  )
    {
    SMC_MOCK_METHOD0( TInt )
    }


// -----------------------------------------------------------------------------
// CRepository::Reset
// -----------------------------------------------------------------------------
//
TInt CRepository::Reset(  )
    {
    SMC_MOCK_METHOD0( TInt )
    }


// -----------------------------------------------------------------------------
// CRepository::Reset
// -----------------------------------------------------------------------------
//
TInt CRepository::Reset( 
        TUint32 aKey )
    {
    SMC_MOCK_METHOD1( TInt, TUint32, aKey )
    }


// -----------------------------------------------------------------------------
// CRepository::StartTransaction
// -----------------------------------------------------------------------------
//
TInt CRepository::StartTransaction( 
        TTransactionMode aMode )
    {
    SMC_MOCK_METHOD1( TInt, TTransactionMode, aMode )
    }


// -----------------------------------------------------------------------------
// CRepository::StartTransaction
// -----------------------------------------------------------------------------
//
void CRepository::StartTransaction( 
        TTransactionMode aMode,
        TRequestStatus & aStatus )
    {
    SMC_MOCK_METHOD2( void, TTransactionMode, aMode, 
        TRequestStatus &, aStatus )
    }


// -----------------------------------------------------------------------------
// CRepository::CommitTransaction
// -----------------------------------------------------------------------------
//
TInt CRepository::CommitTransaction( 
        TUint32 & aKeyInfo )
    {
    SMC_MOCK_METHOD1( TInt, TUint32 &, aKeyInfo )
    }


// -----------------------------------------------------------------------------
// CRepository::CommitTransaction
// -----------------------------------------------------------------------------
//
void CRepository::CommitTransaction( 
        TDes8 & aKeyInfo,
        TRequestStatus & aStatus )
    {
    SMC_MOCK_METHOD2( void, TDes8 &, aKeyInfo, 
        TRequestStatus &, aStatus )
    }


// -----------------------------------------------------------------------------
// CRepository::CancelTransaction
// -----------------------------------------------------------------------------
//
void CRepository::CancelTransaction(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CRepository::CleanupCancelTransactionPushL
// -----------------------------------------------------------------------------
//
void CRepository::CleanupCancelTransactionPushL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CRepository::FailTransaction
// -----------------------------------------------------------------------------
//
void CRepository::FailTransaction(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CRepository::CleanupFailTransactionPushL
// -----------------------------------------------------------------------------
//
void CRepository::CleanupFailTransactionPushL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


