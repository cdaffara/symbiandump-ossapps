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
* Description:   Mobile signal strength handler implementation
*
*/



// INCLUDE FILES
#include <mmtsy_names.h>
#include "cmobilesignalstrengthhandler.h"
#include "msignalstrengthobserver.h"
#include "debugtraces.h"

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::NewL
// -----------------------------------------------------------------
CMobileSignalStrengthHandler* CMobileSignalStrengthHandler::NewL( )
    {
    CMobileSignalStrengthHandler* self = new (ELeave) CMobileSignalStrengthHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::~CMobileSignalStrengthHandler
// -----------------------------------------------------------------
CMobileSignalStrengthHandler::~CMobileSignalStrengthHandler()
    {
    Cancel();
    iMobilePhone.Close();
    iTelServer.Close();    
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::CMobileSignalStrengthHandler
// -----------------------------------------------------------------
CMobileSignalStrengthHandler::CMobileSignalStrengthHandler() :
    CActive( EPriorityNormal )
    {
 
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::ConstructL
// -----------------------------------------------------------------
void CMobileSignalStrengthHandler::ConstructL()
    {
    QDEBUG_WRITE("CMobileSignalStrengthHandler::ConstructL - Enter ");    
    CActiveScheduler::Add( this );
    InitializeSessionsL();
    QDEBUG_WRITE("CMobileSignalStrengthHandler::ConstructL - Exit");
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::RunL
// -----------------------------------------------------------------
void CMobileSignalStrengthHandler::RunL()
    {
    // if operation completed successfully
    if( iStatus.Int() == KErrNone )
        {
        QDEBUG_WRITE_FORMAT("CMobileSignalStrengthHandler::RunL - Signal strength updated ",iSignalStrength );
        NotifyObserver();
        }
        
    // if observing is still active reissue the notification request    
    if( iObserving )
        {
        QDEBUG_WRITE( "CMobileSignalStrengthHandler::RunL - NotifySignalStrengthChange" );
        // reissue the request 
        iMobilePhone.NotifySignalStrengthChange( iStatus, iSignalStrength, iBars );
        SetActive();
        }
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::RunError
// -----------------------------------------------------------------
TInt CMobileSignalStrengthHandler::RunError( TInt /*aError*/)
    {
    return KErrNone;
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::DoCancel
// -----------------------------------------------------------------
void CMobileSignalStrengthHandler::DoCancel()
    {
    if( iObserving )
        {
        iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifySignalStrengthChange );
        }    
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::ObservingSupported
// -----------------------------------------------------------------
TBool CMobileSignalStrengthHandler::ObservingSupported()
    {
    TUint32 caps;
    iMobilePhone.GetSignalCaps( caps );
    return ( caps & RMobilePhone::KCapsNotifySignalStrengthChange );
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::RequestsSupported
// -----------------------------------------------------------------    
TBool CMobileSignalStrengthHandler::RequestsSupported()
    {
    TUint32 caps;
    iMobilePhone.GetSignalCaps( caps );
    return ( caps & RMobilePhone::KCapsGetSignalStrength );
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::SetSignalStrengthObserverL
// -----------------------------------------------------------------
void CMobileSignalStrengthHandler::SetSignalStrengthObserverL(
    MSignalStrengthObserver* aObserver )
    {
    // leave if observing is not supported
    if( !ObservingSupported() )
        {
        User::Leave( KErrNotSupported );
        }
    iObserver = aObserver;
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::StartObservingL
// -----------------------------------------------------------------
void CMobileSignalStrengthHandler::StartObservingL()
    {
    // leave if observing is not supported
    if( !ObservingSupported() )
        {
        User::Leave( KErrNotSupported );
        }        
    
    // update signal str
    TRAPD( err, UpdateSignalStrengthL() );
    
    // check operation
    if( err != KErrNone )
        {        
        QDEBUG_WRITE_FORMAT("CMobileSignalStrengthHandler::StartObservingL - Failed to initialize signal strength (err = )",err );
        iSignalStrength = 0;
        }
    else
        {
        QDEBUG_WRITE_FORMAT("CMobileSignalStrengthHandler::StartObservingL - Signal strength initialized to ",iSignalStrength );
        }
    
    // update state
    iObserving = ETrue;
    
    // Notify current observer about signal strength
    NotifyObserver();
    
    // request for notification    
    iMobilePhone.NotifySignalStrengthChange( iStatus, iSignalStrength, iBars );
    SetActive();                            
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::StopObservingL
// -----------------------------------------------------------------
void CMobileSignalStrengthHandler::StopObservingL()
    {
    if( !iObserving )
        {
        User::Leave( KErrGeneral );
        }
        
    // cancel the request    
    Cancel();
        
    // update state
    iObserving = EFalse;
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::SignalStrengthL
// -----------------------------------------------------------------
TInt CMobileSignalStrengthHandler::SignalStrengthL()
    {
    // leave if requests are not supported
    if( !RequestsSupported() )
        {
        User::Leave( KErrNotSupported );
        }
    // if we're not observing we have to update the value manually,
    // otherwise value is up-to-date    
    if( !iObserving )
        {
        UpdateSignalStrengthL();
        }
 
    // return the current value
    return iSignalStrength;        
    }
    
// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::BarValueL
// -----------------------------------------------------------------
TInt CMobileSignalStrengthHandler::BarValueL()
    {
    // leave if requests are not supported
    if( !RequestsSupported() )
        {
        User::Leave( KErrNotSupported );
        }
    if( !iObserving )
        {
        UpdateSignalStrengthL();
        }
    
    // return the current value
    return iBars;      
    }
    
// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::InitializeSessionsL
// -----------------------------------------------------------------    
void CMobileSignalStrengthHandler::InitializeSessionsL()
    {
    QDEBUG_WRITE("CMobileSignalStrengthHandler::InitializeSessionsL - Enter");
  
    User::LeaveIfError( iTelServer.Connect() );
     
    RTelServer::TPhoneInfo phoneInfo;
    TInt numPhone;
    
    // load tsy module and get number of phones
    User::LeaveIfError( iTelServer.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( iTelServer.EnumeratePhones( numPhone) );
  
    // find the correct phone
    for (TInt i(0); i < numPhone; i++)
        {
        TName tsyName;
        User::LeaveIfError( iTelServer.GetPhoneInfo( i, phoneInfo ) );
        User::LeaveIfError( iTelServer.GetTsyName( i,tsyName ) );
  
        if ( tsyName.CompareF( KMmTsyModuleName ) == 0)
            {
            break;
            }
        }
    // open mobile phone session    
    User::LeaveIfError( iMobilePhone.Open( iTelServer, phoneInfo.iName ) ); 
    QDEBUG_WRITE("CMobileSignalStrengthHandler::InitializeSessionsL - Exit");
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::UpdateSignalStrengthL
// -----------------------------------------------------------------
void CMobileSignalStrengthHandler::UpdateSignalStrengthL()
    {
    // only update the value if requests are supported
    if( RequestsSupported() )
        {
        TRequestStatus status;                
        iMobilePhone.GetSignalStrength( status, iSignalStrength, iBars );
        User::WaitForRequest( status );
        QDEBUG_WRITE_FORMAT("CMobileSignalStrengthHandler::UpdateSignalStrengthL - GetSignalStrength returned with", status.Int() );
        User::LeaveIfError( status.Int() );
        }
    }

// -----------------------------------------------------------------
// CMobileSignalStrengthHandler::NotifyObserver
// -----------------------------------------------------------------    
void CMobileSignalStrengthHandler::NotifyObserver()
    {
    // notify observer, if set
    if( iObserver )
        {
        TRAP_IGNORE( iObserver->SignalStrengthAndBarUpdatedL( iSignalStrength, iBars ) );
        }
    }

//  End of File
