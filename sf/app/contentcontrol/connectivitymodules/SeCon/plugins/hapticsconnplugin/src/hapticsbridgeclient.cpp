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
* Description:  Client interface to haptics server for haptics bridge 
*                commands.
*
*/


#include <e32svr.h>
#include <f32file.h>
#include <data_caging_path_literals.hrh> // KDC_PROGRAMS_DIR
#include <hwrmhaptics.h>
#include <hwrmlogicalactuators.h>
#include <hwrmhapticspacketizer.h>

#include "hapticsbridgeclient.h"
#include "hapticsconntrace.h"

const TInt KServerVersionMajor       = 1;
const TInt KServerVersionMinor       = 1;
const TInt KServerVersionBuild       = 1;
_LIT( KServerProcessName, "!hwrmhapticsserver" );
_LIT( KServerExeName, "hwrmhapticsserver.exe" );
const TInt KAsyncMsgSlots = 10;

const TInt KHWRMHapticsServiceCommand      = 2001;
const TInt KHWRMHapticsOpenActuatorCommand = 2004;
const TInt KHWRMHapticsCleanupCommand      = 2008;
const TInt KHWRMHapticsBridgeCommand       = 2011;

// --------------------------------------------------------------------------
// Simple constructor
// 
// --------------------------------------------------------------------------
// 
RHapticsBridgeClient::RHapticsBridgeClient()
    {
    TRAP_IGNORE( iPacketizer = 
                 CHWRMHapticsPacketizer::NewL( EHWRMLogicalActuatorAny ) );
    }

// --------------------------------------------------------------------------
// Simple destructor
// 
// --------------------------------------------------------------------------
// 
RHapticsBridgeClient::~RHapticsBridgeClient()
    {
    delete iPacketizer;
    }

// --------------------------------------------------------------------------
// Method for connecting (creating a new session) with Haptics Server
// 
// --------------------------------------------------------------------------
// 
TInt RHapticsBridgeClient::Connect()
    {
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::Connect()" ) ) );
    TInt ret = CreateSession( KServerProcessName, 
                              ServerVersion(), 
                              KAsyncMsgSlots );
    if ( ret != KErrNone )
        {
        ret = StartServer();
        if ( ret == KErrNone )
            {
            ret = CreateSession( KServerProcessName, 
                                 ServerVersion(),
                                 KAsyncMsgSlots );    
            COMPONENT_TRACE(_L("RHapticsBridgeClient::Connect(), Session created"));
            }       
        }

    // Initialize server components.
    if ( ret == KErrNone )
        {
        ret = SendReceive( KHWRMHapticsServiceCommand );
        }

    // Open the haptic effect ("vibra") device.
    if ( ret == KErrNone )
        {
        ret = OpenHapticsDevice();
        }
    
        
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::Connect - return(%d)" ), ret ) );        
    return ret;
    }

// --------------------------------------------------------------------------
// Method for starting Haptics Server (if not already running)
// 
// --------------------------------------------------------------------------
//
TInt RHapticsBridgeClient::StartServer() const
    {
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::StartServer()" ) ) );

    TParse parser;
    parser.Set( KServerExeName, &KDC_PROGRAMS_DIR, NULL );
        
    RProcess server;
    TInt ret = server.Create( parser.FullName(), KNullDesC );

    if ( ret != KErrNone ) // Loading failed.
        {
        return ret;
        }

    TRequestStatus status;
    server.Rendezvous( status );

    if ( status != KRequestPending )
        {
        server.Kill( 0 ); // Abort startup.
        server.Close();
        return KErrGeneral;
        }
    else
        {
        server.Resume(); // Logon OK - start the server.
        }
        
    User::WaitForRequest( status );
    server.Close();

    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::StartServer() - return %d" ), status.Int() ) );
    return status.Int();    
    }

// --------------------------------------------------------------------------
// Method for closing session to haptics server.
// 
// --------------------------------------------------------------------------
//    
void RHapticsBridgeClient::Close()    
    {
    RSessionBase::Close();
    }

// --------------------------------------------------------------------------
// Getter method for server version.
// 
// --------------------------------------------------------------------------
//
TVersion RHapticsBridgeClient::ServerVersion() const 
    {
    return TVersion( KServerVersionMajor, 
                     KServerVersionMinor,
                     KServerVersionBuild );
    }

// --------------------------------------------------------------------------
// Method for sending commands to Haptics Server.
// 
// --------------------------------------------------------------------------
//    
TInt RHapticsBridgeClient::SendBridgeBuffer( const TDesC8& aReqData, TDes8& aRetDataPckg )
    {
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::SendBridgeBufferL() - Begin" ) ) );

    TInt retVal( KErrDisconnected );
    if ( iHandle )
        {
        TInt vibeStatus(0);
        TPckg<TInt> vibeStatusPckg( vibeStatus );
        retVal = SendReceive( KHWRMHapticsBridgeCommand, 
                              TIpcArgs( &aReqData, 
                                        &vibeStatusPckg, 
                                        &aRetDataPckg ) );

        if ( retVal == KErrNone ) 
            {
            TRAP( retVal, HandleOpenDeviceResponseL( aRetDataPckg ) );
            }
        }
    
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::SendBridgeBufferL() - End (%d)" ), retVal ) );    
    return retVal;
    }

// --------------------------------------------------------------------------
// Method for cleaning up session to Haptics Server.
// 
// --------------------------------------------------------------------------
//
void RHapticsBridgeClient::CleanUp()
    {
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::CleanUp() - Begin" ) ) );
    if ( iHandle )
        {
        SendReceive ( KHWRMHapticsCleanupCommand );
        }
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::CleanUp() - End" ) ) );
    }

// --------------------------------------------------------------------------
// Method for handling Open Device responses (basically for requesting the 
// setting of the license key).
// 
// --------------------------------------------------------------------------
//
void RHapticsBridgeClient::HandleOpenDeviceResponseL( const TDesC8& aResponse )
    {
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::HandleOpenDeviceResponse() - Begin" ) ) );
    TInt err( KErrNone );
    if ( !iPacketizer )
        {
        COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::HandleOpenDeviceResponse() - No Packetizer!" ) ) );
        User::Leave( KErrGeneral );
        }
    else    
        {
        const TUint8* pRsp ( aResponse.Ptr() );
        if ( 0x12 == pRsp[0] && 0x00 == pRsp[1] && 0x00 == pRsp[2] )
            {
            COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::HandleOpenDeviceResponse() - OPEN DEVICE RESPONSE handling" ) ) );
            iPacketizer->DecodeMessageL( aResponse, err );
            if ( !err )
                {
                RBuf8 reqBuf;
                err = iPacketizer->EncSetPlatformLicenseKeyReq( 
                    iPacketizer->DeviceHandle(), reqBuf );
                if ( !err )
                    {
                    TBuf8<50> dummyRetBuf;
                    TInt dummyStatus;
                    TPckg<TInt> dummyStatusPckg( dummyStatus );
                    err = SendReceive( KHWRMHapticsBridgeCommand, 
                                       TIpcArgs ( &reqBuf,
                                                  &dummyStatusPckg, 
                                                  &dummyRetBuf ) );
                    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::HandleOpenDeviceResponse() - Sent set license command (%d)"), err ) );
                    }
                reqBuf.Close();
                }
            User::LeaveIfError( err );
            }
        }
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::HandleOpenDeviceResponse() - End" ) ) );
    }


// --------------------------------------------------------------------------
// Method for opening haptics device. This is needed since there's possibility
// that no other haptics client has opened the device prior to this bridge
// connectivity. This method also call set license key, i.e., completes the
// device opening. 
//
// --------------------------------------------------------------------------
//
TInt RHapticsBridgeClient::OpenHapticsDevice()
    {
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::OpenHapticsDevice - Begin" ) ) );
    TInt err( KErrGeneral );
    if ( iPacketizer )
        {
        RBuf8 openDevReqBuf;
        RBuf8 setLicenseReqBuf;
        TBuf8<50> setLicenseRspBuf;
        TInt dummyStatus;
        TPckg<TInt> dummyStatusPckg( dummyStatus );
        
        err = iPacketizer->EncOpenDeviceReq( EHWRMLogicalActuatorAny,
                                             openDevReqBuf );

        COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::OpenHapticsDevice - EncOpenDevice (%d)" ), err ) );
        if ( !err )
            {
            TInt deviceHandle;
            TPckg<TInt> deviceHandlePckg( deviceHandle );
            err = SendReceive( KHWRMHapticsOpenActuatorCommand,
                               TIpcArgs ( &openDevReqBuf, 
                                          &deviceHandlePckg,
                                          EHWRMLogicalActuatorAny ) );
            COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::OpenHapticsDevice - Send OpenDevice (%d)" ), err ) );
            COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::OpenHapticsDevice - Device handle = %d" ), deviceHandle ) );
            if ( !err )
                {
                err = iPacketizer->EncSetPlatformLicenseKeyReq( 
                    deviceHandle, setLicenseReqBuf );
                COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::OpenHapticsDevice - EncSetEmptyLicenseKey (%d)" ), err ) );    
                if ( !err )
                    {
                    TBuf8<50> dummyRspBuf;
                    TInt dummyStatus;
                    TPckg<TInt> dummyStatusPckg( dummyStatus );
                    err = SendReceive( KHWRMHapticsBridgeCommand, 
                                       TIpcArgs ( &setLicenseReqBuf,
                                                  &dummyStatusPckg, 
                                                  &dummyRspBuf ) );
                    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::OpenHapticsDevice - Send SetLicense (%d)" ), err ) );
                    }
                }
            }

        openDevReqBuf.Close();
        setLicenseReqBuf.Close();
        }
    COMPONENT_TRACE( ( _L( "RHapticsBridgeClient::OpenHapticsDevice - End (err=%d)" ),err ) );    
    return err;
    }

// eof
