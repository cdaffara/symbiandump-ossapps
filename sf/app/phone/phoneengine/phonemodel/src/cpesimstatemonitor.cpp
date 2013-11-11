/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sim State Monitoring class methods implementation
*
*/



// INCLUDE FILES
#include "cpesimstatemonitor.h"
#include "mpephonemodelinternal.h"
#include <startupdomainpskeys.h>
#include <talogger.h>


// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPESimStateMonitor::CPESimStateMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPESimStateMonitor::CPESimStateMonitor( 
    MPEPhoneModelInternal& aModel, 
    const TEvent aEvent )
   : CActive( EPriorityStandard ),
     iModel( aModel ),
     iSimState( EPESimStatusUninitialized ),
     iStartUp( ETrue )
    {
    if ( aEvent == EEventSimStatus )
        {
        iPropertyKey = KPSSimStatus;
        }
    else
        {
        iPropertyKey = KPSSimChanged;
        }
    }

// -----------------------------------------------------------------------------
// CPESimStateMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPESimStateMonitor::ConstructL()
    {
    TEFLOGSTRING( KTAMESOUT, "PE CPESimStateMonitor::ConstructL start" );

    RProperty::TType type( RProperty::EInt );
    TInt err = iProperty.Define( KPSUidStartup, iPropertyKey, type );

    //An error of KErrAlready exists should be ignored, as this only
    //indicates that some other code int system is also interested in the 
    //value and has created the property.
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    User::LeaveIfError( iProperty.Attach( KPSUidStartup, iPropertyKey ) );

    // NOTE StartMonitoring() is not called here and this is intentional.
    // It is undesirable to send out messages before initialization is complete.
    // The first completion of this monitor is achieved by calling Start() from
    // MPEPhoneModelInternal::StepL()

    TEFLOGSTRING( KTAMESOUT, "PE CPESimStateMonitor::ConstructL complete" );
    }

// -----------------------------------------------------------------------------
// CPESimStateMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPESimStateMonitor* CPESimStateMonitor::NewL( 
    MPEPhoneModelInternal& aModel, 
    const TEvent aEvent )
    {
    TEFLOGSTRING2( KTAOBJECT, "PE CPESimStateMonitor::Newl %d", aEvent );
    CPESimStateMonitor* self = new ( ELeave ) CPESimStateMonitor( aModel, aEvent );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING( KTAMESOUT, "PE CPESimStateMonitor::NewL complete" );

    return self;
    }

// Destructor
CPESimStateMonitor::~CPESimStateMonitor()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CPESimStateMonitor::Start
// Completes monitor to finish initialization
// -----------------------------------------------------------------------------
//
void CPESimStateMonitor::Start()
    {
    if ( !IsActive() )
        {
        CActiveScheduler::Add( this );
        SetActive();
        TRequestStatus* status = &iStatus ;
        User::RequestComplete( status, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CPESimStateMonitor::SimState
// Returns Sim state.
// -----------------------------------------------------------------------------
//
TPESimState CPESimStateMonitor::SimState() const
    {
    return static_cast<TPESimState>( iSimState );
    }

// -----------------------------------------------------------------------------
// CPESimStateMonitor::StartMonitoring
// Starts monitoring Sim state
// -----------------------------------------------------------------------------
//
void CPESimStateMonitor::StartMonitoring()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPESimStateMonitor::DoCancel
// Callback method from CActive.
// -----------------------------------------------------------------------------
//
void CPESimStateMonitor::DoCancel()
    {   
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CPESimStateMonitor::RunL
// In startup reads Sim state from System Agent, otherwise it is called 
// when Sim state is changed.
// -----------------------------------------------------------------------------
//
void CPESimStateMonitor::RunL()
    {
    TInt status = iStatus.Int(); 
    // resubscribe before processing new value to prevent missing updates
    StartMonitoring();

    iProperty.Get( KPSUidStartup, iPropertyKey, iSimState );

    if ( iStartUp )
        {
        iStartUp = EFalse;

        if ( iPropertyKey == KPSSimStatus )
            {
            if( iSimState != EPESimStatusUninitialized && iSimState != EPESimNotSupported )
                {
                iModel.SendMessage( MEngineMonitor::EPEMessageSIMStateChanged );
                }
            }
        else
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageSIMChanged );
            }
        }
    else // Sim or Sim status has changed
        {
        if ( status == KErrNone ) 
            {
            TEFLOGSTRING3( KTAMESINT, 
                "PE CPESimStateMonitor::RunL, event: %d, property: %d"
                , iPropertyKey
                , iSimState );

            if ( iPropertyKey == KPSSimStatus )
                {    
                iModel.SendMessage( MEngineMonitor::EPEMessageSIMStateChanged );
                }
            else 
                {
                if ( iSimState == ESimChanged )
                    {
                    iModel.SendMessage( MEngineMonitor::EPEMessageSIMChanged );
                    }
                }
            }
        else
            {
            TEFLOGSTRING3( KTAERROR, "PE CPESimStateMonitor::RunL, event: %d, status: %d", iPropertyKey, status );
            }   
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// None

//  End of File
