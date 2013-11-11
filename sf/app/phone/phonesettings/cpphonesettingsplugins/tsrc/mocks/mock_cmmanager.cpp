/* 
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmanager.h"

class RCmConnectionMethod
{
public:
    RCmConnectionMethod(){};
};

class RCmDestination
{
public:
    RCmDestination(){};
};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RCmManager::OpenL
// -----------------------------------------------------------------------------
//
void RCmManager::OpenL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// RCmManager::OpenLC
// -----------------------------------------------------------------------------
//
void RCmManager::OpenLC(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// RCmManager::Close
// -----------------------------------------------------------------------------
//
void RCmManager::Close(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetBearerInfoIntL
// -----------------------------------------------------------------------------
//
TUint32 RCmManager::GetBearerInfoIntL( 
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( TUint32, TUint32, aBearerType, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetBearerInfoBoolL
// -----------------------------------------------------------------------------
//
TBool RCmManager::GetBearerInfoBoolL( 
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( TBool, TUint32, aBearerType, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetBearerInfoStringL
// -----------------------------------------------------------------------------
//
HBufC * RCmManager::GetBearerInfoStringL( 
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( HBufC *, TUint32, aBearerType, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetBearerInfoString8L
// -----------------------------------------------------------------------------
//
HBufC8 * RCmManager::GetBearerInfoString8L( 
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( HBufC8 *, TUint32, aBearerType, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoIntL
// -----------------------------------------------------------------------------
//
TUint32 RCmManager::GetConnectionMethodInfoIntL( 
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( TUint32, TUint32, aIapId, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoBoolL
// -----------------------------------------------------------------------------
//
TBool RCmManager::GetConnectionMethodInfoBoolL( 
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( TBool, TUint32, aIapId, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoStringL
// -----------------------------------------------------------------------------
//
HBufC * RCmManager::GetConnectionMethodInfoStringL( 
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( HBufC *, TUint32, aIapId, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoString8L
// -----------------------------------------------------------------------------
//
HBufC8 * RCmManager::GetConnectionMethodInfoString8L( 
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    SMC_MOCK_METHOD2( HBufC8 *, TUint32, aIapId, 
        TUint32, aAttribute )
    }


// -----------------------------------------------------------------------------
// RCmManager::ConnectionMethodL
// -----------------------------------------------------------------------------
//
RCmConnectionMethod RCmManager::ConnectionMethodL( 
        TUint32 aConnectionMethodId ) const
    {
    SMC_MOCK_METHOD1( RCmConnectionMethod, TUint32, aConnectionMethodId )
    }


// -----------------------------------------------------------------------------
// RCmManager::ConnectionMethodL
// -----------------------------------------------------------------------------
//
void RCmManager::ConnectionMethodL( 
        RArray<TUint32> & aCMArray,
        TBool aCheckBearerType,
        TBool aLegacyOnly,
        TBool aEasyWlan ) const
    {
    SMC_MOCK_METHOD4( void, RArray<TUint32> &, aCMArray, 
        TBool, aCheckBearerType, 
        TBool, aLegacyOnly, 
        TBool, aEasyWlan )
    }


// -----------------------------------------------------------------------------
// RCmManager::AllDestinationsL
// -----------------------------------------------------------------------------
//
void RCmManager::AllDestinationsL( 
        RArray<TUint32> & aDestArray ) const
    {
    SMC_MOCK_METHOD1( void, RArray<TUint32> &, aDestArray )
    }


// -----------------------------------------------------------------------------
// RCmManager::DestinationL
// -----------------------------------------------------------------------------
//
RCmDestination RCmManager::DestinationL( 
        TUint32 aDestinationId ) const
    {
    SMC_MOCK_METHOD1( RCmDestination, TUint32, aDestinationId )
    }


// -----------------------------------------------------------------------------
// RCmManager::BearerPriorityArrayL
// -----------------------------------------------------------------------------
//
void RCmManager::BearerPriorityArrayL( 
        RArray<TBearerPriority> & aArray ) const
    {
    SMC_MOCK_METHOD1( void, RArray<TBearerPriority> &, aArray )
    }


// -----------------------------------------------------------------------------
// RCmManager::CleanupGlobalPriorityArray
// -----------------------------------------------------------------------------
//
void RCmManager::CleanupGlobalPriorityArray( 
        RArray<TBearerPriority> & aArray ) const
    {
    SMC_MOCK_METHOD1( void, RArray<TBearerPriority> &, aArray )
    }


// -----------------------------------------------------------------------------
// RCmManager::EasyWlanIdL
// -----------------------------------------------------------------------------
//
TUint32 RCmManager::EasyWlanIdL(  ) const
    {
    SMC_MOCK_METHOD0( TUint32 )
    }


// -----------------------------------------------------------------------------
// RCmManager::ReadDefConnL
// -----------------------------------------------------------------------------
//
void RCmManager::ReadDefConnL( 
        TCmDefConnValue &  )
    {
    //SMC_MOCK_METHOD1( void, TCmDefConnValue &, aDCSetting )
    }


// -----------------------------------------------------------------------------
// RCmManager::WriteDefConnL
// -----------------------------------------------------------------------------
//
void RCmManager::WriteDefConnL( 
        const TCmDefConnValue &  )
    {
    //SMC_MOCK_METHOD1( void, const TCmDefConnValue &, aDCSetting )
    }


// -----------------------------------------------------------------------------
// RCmManager::ReadGenConnSettingsL
// -----------------------------------------------------------------------------
//
void RCmManager::ReadGenConnSettingsL( 
        TCmGenConnSettings & aGenConnSettings )
    {
    SMC_MOCK_METHOD1( void, TCmGenConnSettings &, aGenConnSettings )
    }


// -----------------------------------------------------------------------------
// RCmManager::WriteGenConnSettingsL
// -----------------------------------------------------------------------------
//
void RCmManager::WriteGenConnSettingsL( 
        const TCmGenConnSettings & aGenConnSettings )
    {
    SMC_MOCK_METHOD1( void, const TCmGenConnSettings &, aGenConnSettings )
    }


// -----------------------------------------------------------------------------
// RCmManager::CreateTablesAndOpenL
// -----------------------------------------------------------------------------
//
void RCmManager::CreateTablesAndOpenL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// RCmManager::CreateDestinationL
// -----------------------------------------------------------------------------
//
RCmDestination RCmManager::CreateDestinationL( 
        const TDesC & aName )
    {
    SMC_MOCK_METHOD1( RCmDestination, const TDesC &, aName )
    }


// -----------------------------------------------------------------------------
// RCmManager::CreateConnectionMethodL
// -----------------------------------------------------------------------------
//
RCmConnectionMethod RCmManager::CreateConnectionMethodL( 
        TUint32 aBearerType )
    {
    SMC_MOCK_METHOD1( RCmConnectionMethod, TUint32, aBearerType )
    }


// -----------------------------------------------------------------------------
// RCmManager::CopyConnectionMethodL
// -----------------------------------------------------------------------------
//
TInt RCmManager::CopyConnectionMethodL( 
        RCmDestination & aTargetDestination,
        RCmConnectionMethod & aConnectionMethod )
    {
    SMC_MOCK_METHOD2( TInt, RCmDestination &, aTargetDestination, 
        RCmConnectionMethod &, aConnectionMethod )
    }


// -----------------------------------------------------------------------------
// RCmManager::MoveConnectionMethodL
// -----------------------------------------------------------------------------
//
TInt RCmManager::MoveConnectionMethodL( 
        RCmDestination & aSourceDestination,
        RCmDestination & aTargetDestination,
        RCmConnectionMethod & aConnectionMethod )
    {
    SMC_MOCK_METHOD3( TInt, RCmDestination &, aSourceDestination, 
        RCmDestination &, aTargetDestination, 
        RCmConnectionMethod &, aConnectionMethod )
    }


// -----------------------------------------------------------------------------
// RCmManager::RemoveConnectionMethodL
// -----------------------------------------------------------------------------
//
void RCmManager::RemoveConnectionMethodL( 
        RCmDestination & aDestination,
        RCmConnectionMethod & aConnectionMethod )
    {
    SMC_MOCK_METHOD2( void, RCmDestination &, aDestination, 
        RCmConnectionMethod &, aConnectionMethod )
    }


// -----------------------------------------------------------------------------
// RCmManager::RemoveAllReferencesL
// -----------------------------------------------------------------------------
//
void RCmManager::RemoveAllReferencesL( 
        RCmConnectionMethod & aConnectionMethod )
    {
    SMC_MOCK_METHOD1( void, RCmConnectionMethod &, aConnectionMethod )
    }


// -----------------------------------------------------------------------------
// RCmManager::UpdateBearerPriorityArrayL
// -----------------------------------------------------------------------------
//
void RCmManager::UpdateBearerPriorityArrayL( 
        const RArray<TBearerPriority> & aArray )
    {
    SMC_MOCK_METHOD1( void, const RArray<TBearerPriority> &, aArray )
    }


// -----------------------------------------------------------------------------
// RCmManager::CreateDestinationL
// -----------------------------------------------------------------------------
//
RCmDestination RCmManager::CreateDestinationL( 
        const TDesC & aName,
        TUint32 aDestId )
    {
    SMC_MOCK_METHOD2( RCmDestination, const TDesC &, aName, 
        TUint32, aDestId )
    }


// -----------------------------------------------------------------------------
// RCmManager::CreateConnectionMethodL
// -----------------------------------------------------------------------------
//
RCmConnectionMethod RCmManager::CreateConnectionMethodL( 
        TUint32 aImplementationUid,
        TUint32 aConnMethodId )
    {
    SMC_MOCK_METHOD2( RCmConnectionMethod, TUint32, aImplementationUid, 
        TUint32, aConnMethodId )
    }


// -----------------------------------------------------------------------------
// RCmManager::SupportedBearersL
// -----------------------------------------------------------------------------
//
void RCmManager::SupportedBearersL( 
        RArray<TUint32> & aArray ) const
    {
    SMC_MOCK_METHOD1( void, RArray<TUint32> &, aArray )
    }


// -----------------------------------------------------------------------------
// RCmManager::GetUncategorizedIconL
// -----------------------------------------------------------------------------
//
HBufC * RCmManager::GetUncategorizedIconL( ) const
    {
    SMC_MOCK_METHOD0( HBufC * )
    }

