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
* Description:  CSconBtEngine implementation
*
*/



#include <centralrepository.h>
#include <btengdomaincrkeys.h>

#include <e32property.h>
#include <bttypes.h> 
#include <bt_subscribe.h>

#include "sconbtengine.h"
#include "debug.h"

CSconBtEngine* CSconBtEngine::NewL()
    {
    TRACE_FUNC;
    CSconBtEngine* self = new (ELeave) CSconBtEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CSconBtEngine::~CSconBtEngine()
    {
    TRACE_FUNC;
    if ( iBtEng )
        {
        iBtEng->Cancel();
        delete iBtEng;
        }
    
    delete iBtSettings;
    }

CSconBtEngine::CSconBtEngine()
    {
    }

void CSconBtEngine::ConstructL()
    {
    iBtEng = CBTEngDevMan::NewL( this );
    iBtSettings = CBTEngSettings::NewL( this );
    iState = EIdle;
    }

void CSconBtEngine::ReadBTInfoL( RBufWriteStream& aStream )
    {
    TRACE_FUNC_ENTRY;
    if ( iState != EIdle )
        {
        User::Leave( KErrInUse );
        }
    iStream = &aStream;
    iOperationError = KErrNone;
    
    // Write local name (8 bit name length + unicode name)
    TBuf<100> localName;
    TInt err = iBtSettings->GetLocalName( localName );
    User::LeaveIfError( err );
    LOGGER_WRITE_1("localName: %S", &localName);
    iStream->WriteUint8L( localName.Length() );
    iStream->WriteL( localName, localName.Length() );
    
    if ( iBtAddress.Length() == 0 )
        {
        LOGGER_WRITE("Try to read bt address");
        
        CRepository* cenRep = NULL;
        // Use TRAP to catch a leave if the CenRep key does not exist.
        TRAP( err, cenRep = CRepository::NewL( KCRUidBluetoothLocalDeviceAddress ) );
        CleanupStack::PushL( cenRep );
        if( !err )
            {
            err = cenRep->Get( KBTLocalDeviceAddress, iBtAddress );
            LOGGER_WRITE_1("cenRep->Get err: %d", err);
            }
        CleanupStack::PopAndDestroy( cenRep );
        
        if ( err )
            {
            iBtAddress.Zero();
            }
        }
    
    // Write bt address (8 bit addr length + unicode address)
    if ( iBtAddress.Length() > 0 )
        {
        LOGGER_WRITE_1("btAddr: %S", &iBtAddress);
        iStream->WriteUint8L( iBtAddress.Length() );
        iStream->WriteL( iBtAddress, iBtAddress.Length() );
        }
    else
        {
        iStream->WriteUint8L( 0 );
        }
    
    // Write BT Power State (8-bit, 0=Off, 1=On)
    TBTPowerStateValue state;
    err = iBtSettings->GetPowerState( state );
    User::LeaveIfError( err );
    LOGGER_WRITE_1("PowerState: %d", state);
    iStream->WriteUint8L( state );
    
    // Get device info
    TBTRegistrySearch crit;
    crit.FindAll();
    if ( !iBtDevArray )
        {
        LOGGER_WRITE("create arr");
        iBtDevArray = new (ELeave) CBTDeviceArray(5);
        LOGGER_WRITE("create arr -ok");
        }
    else
        {
        iBtDevArray->ResetAndDestroy();
        }
    err = iBtEng->GetDevices( crit, iBtDevArray );
    LOGGER_WRITE_1("GetDevices err: %d", err);
    User::LeaveIfError( err );
    iState = ERequestBtInfo;
    
    // Wait until completed
    iWait.Start();
    iState = EIdle;
    if ( iOperationError )
        {
        LOGGER_WRITE_1("Operation failed, leave %d", iOperationError);
        User::Leave( iOperationError );
        }
    TRACE_FUNC_EXIT;
    }

void CSconBtEngine::SetBtAuthorizedL( const TDesC& aBtDevAddress, TBool aAuthorized )
    {
    TRACE_FUNC_ENTRY;
    if ( iState != EIdle )
        {
        User::Leave( KErrInUse );
        }
    
    TBTRegistrySearch crit;
    TBTDevAddr addr;
    addr.SetReadable( aBtDevAddress );
    crit.FindAddress( addr );
    
    if ( !iBtDevArray )
        {
        LOGGER_WRITE("create arr");
        iBtDevArray = new (ELeave) CBTDeviceArray(5);
        LOGGER_WRITE("create arr -ok");
        }
    else
        {
        iBtDevArray->ResetAndDestroy();
        }
    TInt err = iBtEng->GetDevices( crit, iBtDevArray );
    LOGGER_WRITE_1("GetDevices err: %d", err);
    User::LeaveIfError( err );
    iState = EChangeAuthorized;
    iOperationError = KErrNone;
    iAuthorizedValue = aAuthorized;
    
    // Wait until completed
    iWait.Start();
    iState = EIdle;
    if ( iOperationError )
        {
        LOGGER_WRITE_1("Operation failed, leave %d", iOperationError);
        User::Leave( iOperationError );
        
        }
    
    TRACE_FUNC_EXIT;
    }

TInt CSconBtEngine::SetBtPowerState( TBool aPower )
    {
    TRACE_FUNC_ENTRY;
    if ( iState != EIdle )
        {
        return KErrInUse;
        }
    
    TBTPowerStateValue state;
    iOperationError = iBtSettings->GetPowerState( state );
    if ( iOperationError )
        {
        return iOperationError;
        }
    else if ( ( aPower && state == EBTPowerOn )
            || ( !aPower && state == EBTPowerOff ) )
        {
        // No need to change state
        return KErrNone;
        }
    
    state = EBTPowerOff;
    if ( aPower )
        {
        state = EBTPowerOn;
        }
    iOperationError = iBtSettings->SetPowerState( state );
    if ( iOperationError )
        {
        return iOperationError;
        }
    iState = EChangeBtState;
    
    // Wait until completed
    iWait.Start();
    
    iState = EIdle;
    TRACE_FUNC_RET( iOperationError );
    return iOperationError;
    }

TInt CSconBtEngine::SetBtName( const TDesC& aBtName )
    {
    LOGGER_WRITE_1("SetBtName aBtName: %S", &aBtName);
    TInt err = iBtSettings->SetLocalName( static_cast<const TDes&>( aBtName ) );
    TRACE_FUNC_RET( err );
    return err;
    }

// From MBTEngDevManObserver
void CSconBtEngine::HandleDevManComplete( TInt aErr )
    {
    TRACE_FUNC;
    LOGGER_WRITE_1("aErr: %d", aErr );
    if ( iState == EChangeAuthorized )
        {
        if ( iWait.IsStarted() )
            {
            iOperationError = aErr;
            iWait.AsyncStop();
            }
        }
    
    }

void CSconBtEngine::HandleGetDevicesComplete( TInt aErr, 
       CBTDeviceArray* aDeviceArray )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aErr: %d", aErr);
    //iBtReqOngoing = EFalse;
    if ( iState == ERequestBtInfo )
        {
        // Continue to write devices info
        TRAP( iOperationError, PrintDevicesL() );
        LOGGER_WRITE_1("PrintDevicesL err: %d", iOperationError);
        
        if ( iWait.IsStarted() )
            {
            iWait.AsyncStop();
            }
        }
    else if ( iState == EChangeAuthorized )
        {
        iOperationError = aErr;
        if ( !aErr && aDeviceArray->Count() == 1 )
            {
            CBTDevice* btRef = aDeviceArray->At(0);
            TBTDeviceSecurity devSec = btRef->GlobalSecurity();
            if ( btRef->IsPaired() && !devSec.Banned() )
                {
                LOGGER_WRITE8_1("btAdrr: %S", &btRef->BDAddr().Des() );
                CBTDevice* btDevice = CBTDevice::NewLC( btRef->BDAddr() );
                
                btDevice->SetPaired( ETrue );
                
                btDevice->SetDeviceNameL( btRef->DeviceName() );
                btDevice->SetFriendlyNameL( btRef->FriendlyName() );
                LOGGER_WRITE8_1("name: %S", &btRef->DeviceName());
                LOGGER_WRITE8_1("FriendlyName: %S", &btRef->FriendlyName());
                
                LOGGER_WRITE_1("iAuthorizedValue: %d", iAuthorizedValue);
                // Set as authorized and store the device
                TBTDeviceSecurity security;
                security.SetNoAuthenticate( EFalse );
                security.SetNoAuthorise( iAuthorizedValue );
                security.SetBanned( EFalse );
                
                btDevice->SetGlobalSecurity (security );
                
                //iOperationError = iBtEng->ModifyDevice(*aDeviceArray->At(0));
                iOperationError = iBtEng->ModifyDevice( *btDevice );
                LOGGER_WRITE_1( "ModifyDevice err: %d", iOperationError );
                CleanupStack::PopAndDestroy( btDevice );
                }
            else
                {
                LOGGER_WRITE("Device was not paired or it's banned")
                iOperationError = KErrAccessDenied;
                }
            }
        
        // If error occured stop waiting, otherwise continue waiting until
        // HandleDevManComplete has been called
        if ( iOperationError && iWait.IsStarted() )
            {
            iWait.AsyncStop();
            }
        }
    TRACE_FUNC_EXIT;
    }

void CSconBtEngine::PrintDevicesL()
    {
    TRACE_FUNC_ENTRY;
    // calculate devices count
    TInt devCount(0);
    
    for ( TInt i=0 ; i<iBtDevArray->Count(); i++ )
        {
        CBTDevice* device = iBtDevArray->At(i);
        // show only paired and banned devices
        if ( device->IsPaired() || device->GlobalSecurity().Banned() )
            {
            devCount++;
            }
        }
    
    // Print info
    iStream->WriteUint8L( devCount );
    
    for ( TInt i=0 ; i<iBtDevArray->Count(); i++ )
        {
        CBTDevice* device = iBtDevArray->At(i);
        
        const TBTDeviceSecurity btSec = device->GlobalSecurity();
        
        // show only paired and banned devices
        if ( device->IsPaired() || btSec.Banned() )
            {
            LOGGER_WRITE_1("IsValidDeviceName: %d", device->IsValidDeviceName());
            const TDesC8& deviceName = device->DeviceName();
            LOGGER_WRITE8_1("DeviceName: %S", &deviceName );
            iStream->WriteUint8L( deviceName.Length() );
            iStream->WriteL( deviceName, deviceName.Length() );
            
            LOGGER_WRITE_1("IsValidFriendlyName: %d", device->IsValidFriendlyName());
            LOGGER_WRITE_1("FriendlyName: %S", &device->FriendlyName() );
            
            const TBTDevAddr& addr = device->BDAddr();
            
            TBuf<100> readable;
            addr.GetReadable( readable );
            LOGGER_WRITE_1("readable addr: %S", &readable);
            iStream->WriteUint8L( readable.Length() );
            iStream->WriteL( readable, readable.Length() );
            
            LOGGER_WRITE_1("paired: %d", device->IsPaired() );
            LOGGER_WRITE_1("IsValidPaired: %d", device->IsValidPaired() );
            TUint8 paired(0);
            if ( device->IsPaired() && device->IsValidPaired() )
                {
                paired = 1;
                }
            iStream->WriteUint8L( paired );
            
            TUint8 noAuthorise(0);
            if ( btSec.NoAuthorise() )
                {
                noAuthorise = 1;
                }
            iStream->WriteUint8L( noAuthorise );
            
            TUint8 banned(0);
            if ( btSec.Banned() )
                {
                banned = 1;
                }
            iStream->WriteUint8L( banned );
            
            LOGGER_WRITE("");
            }
        }
    iStream->CommitL();
    iStream = NULL; //delete reference only
    TRACE_FUNC_EXIT;
    }

// From MBTEngSettingsObserver
void CSconBtEngine::PowerStateChanged( TBTPowerStateValue /*aState*/ )
    {
    TRACE_FUNC;
    if ( iState == EChangeBtState && iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    }

void CSconBtEngine::VisibilityModeChanged( TBTVisibilityMode /*aState*/ )
    {
    TRACE_FUNC;
    }
