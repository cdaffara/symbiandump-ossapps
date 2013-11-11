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
* Description:  SAT listener.
*
*/



// INCLUDE FILES
#include    "CVtEngStateManager.h"
#include    "CVtEngSATClient.h"
#include    "VtEngUtils.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngSATClient::CVtEngSATClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngSATClient::CVtEngSATClient() : 
    iSatSession(),
    iSatRefresh( *this )
    {
    __VTPRINT( DEBUG_GEN | DEBUG_CONSTRUCT, "CSATClient.C++" )
    }

// -----------------------------------------------------------------------------
// CVtEngSATClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngSATClient* CVtEngSATClient::NewL( )
    {
    __VTPRINTENTER( "CSATClient.NewL" )
    CVtEngSATClient* self = new( ELeave ) CVtEngSATClient;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    __VTPRINTEXIT( "CSATClient.NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngSATClient::~CVtEngSATClient
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngSATClient::~CVtEngSATClient()
    {
    __VTPRINTENTER( "CSATClient.~" )
    iSatRefresh.Close();
    iSatSession.Close();
    __VTPRINTEXIT( "CSATClient.~" )
    }
    
// -----------------------------------------------------------------------------
// CVtEngSATClient::ConstructL
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CVtEngSATClient::ConstructL()
    {
    iSatSession.ConnectL();
    iSatRefresh.OpenL( iSatSession );
    iSatRefresh.NotifyFileChangeL();
    }

// -----------------------------------------------------------------------------
// CVtEngSATClient::AllowRefresh
// Callback from SAT.
// -----------------------------------------------------------------------------
//    
TBool CVtEngSATClient::AllowRefresh( 
    TSatRefreshType  aType, 
    const TSatRefreshFiles& /*aFiles*/ )
    {
    __VTPRINTENTER( "CSATClient.AllowRefresh" )    
    TBool ret( EFalse );
    const MVtEngSessionInfo::TSessionState state( 
        CVtEngUtility::StateManager()->SessionState() );
    if ( state == MVtEngSessionInfo::EIdle || aType == EFileChangeNotification )
        {
        __VTPRINT2( DEBUG_GEN, " TSatRefreshType aType=%d", aType )
        ret = ETrue;
        }
    __VTPRINTEXITR( "CSATClient.AllowRefresh ret=%d", ret )            
    return ret;
    }

// -----------------------------------------------------------------------------
// CVtEngSATClient::Refresh
// Callback from SAT.
// -----------------------------------------------------------------------------
//
void CVtEngSATClient::Refresh( 
    TSatRefreshType /* aType */,
    const TSatRefreshFiles& /* aFiles */ )
    {
    // Not needed.
    __VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "CSATClient.Refresh" )
    iSatRefresh.RefreshEFRead( EFalse );
    }

// end of file
