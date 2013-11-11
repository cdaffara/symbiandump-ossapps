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
#include <etelmm.h>
#include <mmretrieve.h>
#include <RmmCustomApi.h>

#include <e32svr.h>
#include <smcmockclassincludes.h>
#include <PsetNetwork.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsetNetwork::CPsetNetwork
// -----------------------------------------------------------------------------
//
CPsetNetwork::CPsetNetwork( 
        RMobilePhone & aPhone )
    :
    CActive(EPriorityStandard),
    iPhone(aPhone)
    {
    
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::ConstructL
// -----------------------------------------------------------------------------
//
void CPsetNetwork::ConstructL( 
        MPsetNetworkInfoObserver & /*aObserver*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::NewL
// -----------------------------------------------------------------------------
//
CPsetNetwork * CPsetNetwork::NewL( 
        RMobilePhone & aPhone,
        MPsetNetworkInfoObserver & aObserver )
    {
    SMC_MOCK_NEWL2( CPsetNetwork *, RMobilePhone &, aPhone, 
        MPsetNetworkInfoObserver &, aObserver )

    CPsetNetwork* self = new ( ELeave ) CPsetNetwork( aPhone );
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::NewL
// -----------------------------------------------------------------------------
//
CPsetNetwork * CPsetNetwork::NewL( 
        RMobilePhone & aPhone,
        MPsetNetworkModeObserver & aObserver )
    {
    SMC_MOCK_NEWL2( CPsetNetwork *, RMobilePhone &, aPhone, 
        MPsetNetworkModeObserver &, aObserver )

    CPsetNetwork* self = new ( ELeave ) CPsetNetwork( aPhone );
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::~CPsetNetwork
// -----------------------------------------------------------------------------
//
CPsetNetwork::~CPsetNetwork(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::GetAvailableNetworksL
// -----------------------------------------------------------------------------
//
void CPsetNetwork::GetAvailableNetworksL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::GetCurrentNetworkInfo
// -----------------------------------------------------------------------------
//
TInt CPsetNetwork::GetCurrentNetworkInfo( 
        TCurrentNetworkInfo & aInfo )
    {
    SMC_MOCK_METHOD1( TInt, TCurrentNetworkInfo &, aInfo )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::GetNetworkSelectMode
// -----------------------------------------------------------------------------
//
TInt CPsetNetwork::GetNetworkSelectMode( 
        TSelectMode & aMode )
    {
    SMC_MOCK_METHOD1( TInt, TSelectMode &, aMode )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::SelectNetworkL
// -----------------------------------------------------------------------------
//
void CPsetNetwork::SelectNetworkL( 
        const TNetworkInfo & aInfo )
    {
    SMC_MOCK_METHOD1( void, const TNetworkInfo &, aInfo )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::CancelProcess
// -----------------------------------------------------------------------------
//
void CPsetNetwork::CancelProcess(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::SetNetworkModeSelectionL
// -----------------------------------------------------------------------------
//
void CPsetNetwork::SetNetworkModeSelectionL( 
        TUint32 & aNetworkMode )
    {
    // Reference parameter is not really used to return some value so do not 
    // save it as a reference to the symbianmock, because test will fail
    // if stack-based parameter is saved as a reference.
    SMC_MOCK_METHOD1( void, TUint32/* &*/, aNetworkMode )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::GetCurrentNetworkModeSelectionL
// -----------------------------------------------------------------------------
//
void CPsetNetwork::GetCurrentNetworkModeSelectionL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::IsCallActive
// -----------------------------------------------------------------------------
//
TInt CPsetNetwork::IsCallActive(  )
    {
    SMC_MOCK_METHOD0( TInt )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::IsGPRSConnected
// -----------------------------------------------------------------------------
//
TInt CPsetNetwork::IsGPRSConnected(  )
    {
    SMC_MOCK_METHOD0( TInt )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::ResetNetworkSearch
// -----------------------------------------------------------------------------
//
TInt CPsetNetwork::ResetNetworkSearch(  )
    {
    SMC_MOCK_METHOD0( TInt )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::SetNetSAObserver
// -----------------------------------------------------------------------------
//
void CPsetNetwork::SetNetSAObserver( 
        MPsetNetworkInfoObserver & aObserver )
    {
    SMC_MOCK_METHOD1( void, MPsetNetworkInfoObserver &, aObserver )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::SetNetworkModeObserver
// -----------------------------------------------------------------------------
//
void CPsetNetwork::SetNetworkModeObserver( 
        MPsetNetworkModeObserver & aObserver )
    {
    SMC_MOCK_METHOD1( void, MPsetNetworkModeObserver &, aObserver )
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::DoCancel
// -----------------------------------------------------------------------------
//
void CPsetNetwork::DoCancel()
    {
        
    }


// -----------------------------------------------------------------------------
// CPsetNetwork::RunL
// -----------------------------------------------------------------------------
//
void CPsetNetwork::RunL()
    {
        
    }
