/** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include <QDebug>
#include <smcmockclassincludes.h>
#include <cnetworklistener.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNetworkListener::CNetworkListener
// -----------------------------------------------------------------------------
//
CNetworkListener::CNetworkListener( 
        MNetworkListenerObserver & aObserver ) :
    iRegistered( ETrue ),
    iObserver( aObserver )
    {
    
    }


// -----------------------------------------------------------------------------
// CNetworkListener::ConstructL
// -----------------------------------------------------------------------------
//
void CNetworkListener::ConstructL(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CNetworkListener::NewL
// -----------------------------------------------------------------------------
//
CNetworkListener * CNetworkListener::NewL( 
        MNetworkListenerObserver & aObserver )
    {
    //SMC_MOCK_NEWL1( CNetworkListener *, MNetworkListenerObserver &, aObserver )

    CNetworkListener* self = new( ELeave ) CNetworkListener( aObserver );
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CNetworkListener::~CNetworkListener
// -----------------------------------------------------------------------------
//
CNetworkListener::~CNetworkListener(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleNetworkMessage
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleNetworkMessage( 
        const TNWMessages aMessage )
    {
    SMC_MOCK_METHOD1( void, const TNWMessages, aMessage )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleNetworkError
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleNetworkError( 
        const TNWOperation aOperation,
        TInt aErrorCode )
    {
    SMC_MOCK_METHOD2( void, const TNWOperation, aOperation, 
        TInt, aErrorCode )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::NWLostDelayCallBack
// -----------------------------------------------------------------------------
//
TInt CNetworkListener::NWLostDelayCallBack( 
        TAny * aParam )
    {
    //SMC_MOCK_METHOD1( TInt, TAny *, aParam )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::NWSimRefreshCallBack
// -----------------------------------------------------------------------------
//
TInt CNetworkListener::NWSimRefreshCallBack( 
        TAny * aParam )
    {
    //SMC_MOCK_METHOD1( TInt, TAny *, aParam )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::AllowRefresh
// -----------------------------------------------------------------------------
//
TBool CNetworkListener::AllowRefresh( 
        const TSatRefreshType aType,
        const TSatElementaryFiles aFiles )
    {
    SMC_MOCK_METHOD2( TBool, const TSatRefreshType, aType, 
        const TSatElementaryFiles, aFiles )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::Refresh
// -----------------------------------------------------------------------------
//
void CNetworkListener::Refresh( 
        const TSatRefreshType aType,
        const TSatElementaryFiles aFiles )
    {
    SMC_MOCK_METHOD2( void, const TSatRefreshType, aType, 
        const TSatElementaryFiles, aFiles )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleNetworkInfoReceivedL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleNetworkInfoReceivedL( 
        const CNetworkInfoArray * aInfoArray,
        const TInt aResult )
    {
    SMC_MOCK_METHOD2( void, const CNetworkInfoArray *, aInfoArray, 
        const TInt, aResult )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleCurrentNetworkInfoL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleCurrentNetworkInfoL( 
        const MPsetNetworkSelect::TCurrentNetworkInfo & aCurrentInfo,
        const TInt aResult )
    {
    SMC_MOCK_METHOD2( void, const MPsetNetworkSelect::TCurrentNetworkInfo &, aCurrentInfo, 
        const TInt, aResult )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleNetworkChangedL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleNetworkChangedL( 
        const MPsetNetworkSelect::TNetworkInfo & aCurrentInfo,
        const MPsetNetworkSelect::TCurrentNetworkStatus aStatus,
        const TInt aResult )
    {
    SMC_MOCK_METHOD3( void, const MPsetNetworkSelect::TNetworkInfo &, aCurrentInfo, 
        const MPsetNetworkSelect::TCurrentNetworkStatus, aStatus, 
        const TInt, aResult )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleNetworkChangedL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleNetworkChangedL( 
        const MPsetNetworkSelect::TNetworkInfo & aCurrentInfo,
        const RMobilePhone::TMobilePhoneRegistrationStatus & aStatus,
        const TInt aResult )
    {
    SMC_MOCK_METHOD3( void, const MPsetNetworkSelect::TNetworkInfo &, aCurrentInfo, 
        const RMobilePhone::TMobilePhoneRegistrationStatus &, aStatus, 
        const TInt, aResult )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleSearchingNetworksL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleSearchingNetworksL( 
        TServiceRequest aRequest )
    {
    SMC_MOCK_METHOD1( void, TServiceRequest, aRequest )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleRequestingSelectedNetworkL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleRequestingSelectedNetworkL( 
        TBool aOngoing )
    {
    SMC_MOCK_METHOD1( void, TBool, aOngoing )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleCallActivatedL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleCallActivatedL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CNetworkListener::HandleNetworkErrorL
// -----------------------------------------------------------------------------
//
void CNetworkListener::HandleNetworkErrorL( 
        const MPsetNetworkInfoObserver::TServiceRequest aRequest,
        const TInt aError )
    {
    SMC_MOCK_METHOD2( void, const MPsetNetworkInfoObserver::TServiceRequest, aRequest, 
        const TInt, aError )
    }


