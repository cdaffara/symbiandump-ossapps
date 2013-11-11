/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class responsible for application exit.
*
*/


#include "clcvtappshutter.h"
#include "mlcvtshutterobserver.h"
#include "mlcvtstatecontext.h"
//#include "mlcvtnumbersource.h"
//#include "tvtuilocalvariation.h"
#include <cvtlogger.h>
//#include <eikenv.h>
//#include <AknUtils.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>

// singleton instance
static CLcVtAppShutter* iAppShutter = NULL;

// -----------------------------------------------------------------------------
// CLcVtAppShutter::InstanceL
// -----------------------------------------------------------------------------
//
CLcVtAppShutter* CLcVtAppShutter::InstanceL(
    MLcVtShutterObserver& aObserver )
    {
    __VTPRINTENTER( "CLcVtAppShutter.InstanceL" )
    if ( iAppShutter == NULL )
        {
        iAppShutter = new ( ELeave )
                        CLcVtAppShutter( aObserver );
        }
    __VTPRINTEXIT( "CLcVtAppShutter.InstanceL" )
    return iAppShutter;
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::CLcVtAppShutter
// -----------------------------------------------------------------------------
//
CLcVtAppShutter::CLcVtAppShutter(
    MLcVtShutterObserver& aObserver ) :
    iPendingStateFlags( 0 ),
    iObserver( aObserver )
    {
    iPendingStateFlags.Set( EShutterWaitingStart );
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::~CLcVtAppShutter
// -----------------------------------------------------------------------------
//
CLcVtAppShutter::~CLcVtAppShutter()
    {
    __VTPRINTENTER( "CLcVtAppShutter.~" )
    // mark singleton null
    iAppShutter = NULL;
    delete iCallback;
    __VTPRINTEXIT( "CLcVtAppShutter.~" )
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::StartShutdown
// Called when application may exit, i.e TVtUiStateResetting state receives
// ShutdownDoneL message.
// -----------------------------------------------------------------------------
//
void CLcVtAppShutter::StartShutdown()
    {
    __VTPRINTENTER( "CLcVtAppShutter.StartShutdown" )
    iPendingStateFlags.Clear( EShutterWaitingStart );
    ShutdownIfReadyAndDestroy();
    __VTPRINTEXIT( "CLcVtAppShutter.StartShutdown" )
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::ShutdownWithEmergencyCallL
// -----------------------------------------------------------------------------
//
void CLcVtAppShutter::ShutdownWithEmergencyCallL(
    MLcVtStateContext& aStateContext )
    {
    // Leaves if other than emergency number is supplied...
    CEmergencyCaller* emergency = CEmergencyCaller::DialEmergencyL( *this,
        aStateContext );
    // therefore the flag and member variable must not be set before above call.
    iPendingStateFlags.Set( EShutterWaitingEmergencyCallback );
    iEmergencyCaller = emergency;
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::EmergencyCallDoneL
// -----------------------------------------------------------------------------
//
void CLcVtAppShutter::EmergencyCallDoneL()
    {
    // Destruction of emergency handler is done asynchrounously to avoid
    // problems that might
    iPendingStateFlags.Clear( EShutterWaitingEmergencyCallback );
    TCallBack cb( EmergencyResponseCallback, this );
    EnqueCallbackL( cb );
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::EmergencyResponseCallback
// -----------------------------------------------------------------------------
//
TInt CLcVtAppShutter::EmergencyResponseCallback( TAny* aAny )
    {
    CLcVtAppShutter* shutter =
        reinterpret_cast<CLcVtAppShutter*>( aAny );
    // delete callback
    delete shutter->iCallback;
    shutter->iCallback = NULL;
    // delete emergency caller
    delete shutter->iEmergencyCaller;
    shutter->iEmergencyCaller = NULL;

    shutter->ShutdownIfReadyAndDestroy();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::EnqueCallbackL
// -----------------------------------------------------------------------------
//
void CLcVtAppShutter::EnqueCallbackL( TCallBack& aCallback )
    {
    __VTPRINTENTER( "CLcVtAppShutter.EnqueAsyncCommsCommandL" )
    if ( !iCallback )
        {
        iCallback = new ( ELeave ) CAsyncCallBack( aCallback, EPriorityHigh );
        }
    iCallback->Call();
    __VTPRINTEXIT( "CLcVtAppShutter.EnqueAsyncCommsCommandL" )
    __VTPRINTEXIT( "CLcVtAppShutter.EnqueCallbackL" )
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::ShutdownIfReadyAndDestroy
// -----------------------------------------------------------------------------
//
void CLcVtAppShutter::ShutdownIfReadyAndDestroy()
    {
    __VTPRINTENTER( "CLcVtAppShutter.ShutdownIfReadyAndDestroy" )
    __VTPRINT2( DEBUG_GEN, "  shutter flags %d",
        iPendingStateFlags.Value() );
    if ( !iPendingStateFlags.Value() )
        {
        iObserver.HandleShutdownReady();
        delete this;
        }
    __VTPRINTEXIT( "CLcVtAppShutter.ShutdownIfReadyAndDestroy" )
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::CEmergencyCaller::DialEmergencyL
// -----------------------------------------------------------------------------
//
CLcVtAppShutter::CEmergencyCaller* CLcVtAppShutter::CEmergencyCaller::
    DialEmergencyL(
    CLcVtAppShutter& aObserver,
    MLcVtStateContext& aStateContext )
    {
    __VTPRINTENTER( "CEmergencyCaller.DialEmergencyL" )
    CLcVtAppShutter::CEmergencyCaller* self = new ( ELeave )
        CLcVtAppShutter::CEmergencyCaller( aObserver, aStateContext );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    __VTPRINTEXIT( "CEmergencyCaller.DialEmergencyL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::CEmergencyCaller::ConstructL
// -----------------------------------------------------------------------------
//
void CLcVtAppShutter::CEmergencyCaller::ConstructL()
    {
    __VTPRINTENTER( "CEmergencyCaller.ConstructL" )
    /*
    const MVtUiNumberSource* source = iStateContext.NumberSource();
    if ( source )
        {
        source->GetContents( iNumber );
        AknTextUtils::ConvertDigitsTo( iNumber, EDigitTypeWestern );
        User::LeaveIfError( iServer.Connect() );
        iEmergency = CPhCltEmergencyCall::NewL( this );
        TBool isEmergenyNumber = EFalse;
        const TInt err =
            iEmergency->FindEmergencyPhoneNumber( iNumber, isEmergenyNumber );

        if ( err == KErrNone && isEmergenyNumber )
            {
            __VTPRINT( DEBUG_GEN, "iEmergency->DialEmergencyCallL")
            iEmergency->DialEmergencyCallL( iNumber );
            }
        if ( !isEmergenyNumber || err )
            {
            __VTPRINT( DEBUG_GEN, "  not EC number => leave" )
            // Not an emergency call number. Abort emergency call process.
            User::Leave( KErrArgument );
            }
        }
        */
    __VTPRINTEXIT( "CEmergencyCaller.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::CEmergencyCaller::CEmergencyCaller
// -----------------------------------------------------------------------------
//
CLcVtAppShutter::CEmergencyCaller::CEmergencyCaller(
    CLcVtAppShutter& aObserver,
    MLcVtStateContext& aStateContext ) :
    iObserver( aObserver ),
    iStateContext( aStateContext )
    {
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::CEmergencyCaller::~CEmergencyCaller
// -----------------------------------------------------------------------------
//
CLcVtAppShutter::CEmergencyCaller::~CEmergencyCaller()
    {
    __VTPRINTENTER( "CEmergencyCaller.~" )
    delete iEmergency;
    iServer.Close();
    __VTPRINTEXIT( "CEmergencyCaller.~" )
    }

// -----------------------------------------------------------------------------
// CLcVtAppShutter::CEmergencyCaller::HandleEmergencyDialL
// -----------------------------------------------------------------------------
//
void CLcVtAppShutter::CEmergencyCaller::HandleEmergencyDialL(
    const TInt )
    {
    __VTPRINTENTER( "CEmergencyCaller.HandleEmergencyDialL" )
    iObserver.EmergencyCallDoneL();
    __VTPRINTEXIT( "CEmergencyCaller.HandleEmergencyDialL" )
    }

