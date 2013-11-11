/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File Transfer Profile Plug-in implementation
*
*/


// INCLUDE FILES

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <obexconstantsinternal.h>
#endif
#include "sconftpplugin.h"
#include "sconftp.h"
#include "sconftppluginutils.h"
#include "sconservicetimer.h"
#include "debug.h"


// CONSTANTS
_LIT(KSConFTPLibName, "sconftp.dll");
const TInt KSConFTPUidValue = 0x10009D8D;
const TUid KSConFTPUid = {KSConFTPUidValue};

// Folder listing type from IrObex specification
_LIT8( KSConFolderListType, "x-obex/folder-listing" );

_LIT( K_C_ROOT, "C:\\" );
const TInt KSConBufSize = 262144; // 256KB
// Time (in milliseconds) for the timer
const TInt KSConTimeOutValue =  60000000;
// Flags used to indicate SetPath commands
const TInt KSConNULLSetPath =   0x00;

const TInt KSConHeaderMaxLength = 256;

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSConFTPplugin::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConFTPplugin* CSConFTPplugin::NewL()
    {
    TRACE_FUNC_ENTRY;
    CSConFTPplugin* self = new ( ELeave ) CSConFTPplugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return( self );
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::~CSConFTPplugin()
// Destructor
// -----------------------------------------------------------------------------
//
CSConFTPplugin::~CSConFTPplugin()
    {
    TRACE_FUNC_ENTRY;
    
    // Disconnect from services.
    Disconnect();
    
    if ( iFTPHandler )
        {
        delete iFTPHandler;
        iFTPHandler = NULL;
        iFTPlib.Close();
        }
    
    delete iBuffer;
    iBuffer = NULL;
    delete iObject;
    iObject = NULL;

    if ( iServiceTimer )
        {
        iServiceTimer->Cancel();
        }

    delete iServiceTimer;
    iServiceTimer = NULL;

    if ( iFileObject )
        {
        delete iFileObject;
        iFileObject = NULL;
        }


    delete iShutdownWatcher;
    iShutdownWatcher = NULL;
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::IsOBEXActive()
// Returns active status of OBEX session
// -----------------------------------------------------------------------------
//
TBool CSConFTPplugin::IsOBEXActive()
    {
    TRACE_FUNC;
    return iSessionActive;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::NotifyShutdown()
// System is shutting down
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::NotifyShutdown()
    {
    TRACE_FUNC;
    iShutdownInProgress = ETrue;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::LoadFTPDllL()
// Loads sconftp.dll module
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::LoadFTPDllL()
    {
    TRACE_FUNC_ENTRY;
    // Dynamically load DLL
    User::LeaveIfError( iFTPlib.Load( KSConFTPLibName ) );
    if ( iFTPlib.Type()[1] != KSConFTPUid )
        {
        LOGGER_WRITE( "KSConFTPUidValue incorrect" );
        iFTPlib.Close();
        User::Leave( KErrNotFound );
        }
    TSConCreateCSConFTPFunc CreateCSConFTPL =
    (TSConCreateCSConFTPFunc)iFTPlib.Lookup(1);
    iFTPHandler = (CSConFTP*)CreateCSConFTPL();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::Disconnect()
// Closes initialized services
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::Disconnect()
    {
    TRACE_FUNC_ENTRY;
    
    iLinkAdapter.Close();
    iSocketServer.Close();
    
    if ( iStartTimer != EFalse )
        {
        iServiceTimer->Cancel();
        iServiceTimer->StartTimer();
        }
    
    delete iObject;
    iObject = NULL;
    
    if ( iFileObject )
        {
        delete iFileObject;
        iFileObject = NULL;
        }
    if ( iFTPHandler )
        {
        delete iFTPHandler;
        iFTPHandler = NULL;
        iFTPlib.Close();
        }

    TRACE_FUNC_EXIT
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::ErrorIndication( TInt aError )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::ErrorIndication( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aError : %d", aError );
    
    if ( iFTPHandler )
        {
        iFTPHandler->AbortFileTransfer( iObject );
        }

    if ( iObject )
        {
        delete iObject;
        iObject = NULL;
        }
    
    if ( iFileObject )
        {
        iFileObject->Reset();
        delete iFileObject;
        iFileObject = NULL;
        }

    // Keep compiler happy
    (void)aError;
    
    delete iShutdownWatcher;
    iShutdownWatcher = NULL;
    
	if ( iLinkAdapter.IsOpen() )
        {
        // Cancel ActivateActiveRequester & allow going to low power mode
        TInt err2 = iLinkAdapter.CancelLowPowerModeRequester();
        LOGGER_WRITE_1( "iLinkAdapter.CancelLowPowerModeRequester() err: %d", err2 );
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::AbortIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::AbortIndication()
    {
    TRACE_FUNC_ENTRY;
    if ( iFTPHandler )
        {
        iFTPHandler->AbortFileTransfer( iObject );
        }

    if ( iObject )
        {
        delete iObject;
        iObject = NULL;
        }
    
    if ( iFileObject )
        {
        iFileObject->Reset();
        delete iFileObject;
        iFileObject = NULL;
        }
    
    delete iShutdownWatcher;
    iShutdownWatcher = NULL;
    
    if ( iLinkAdapter.IsOpen() )
        {
        // Cancel ActivateActiveRequester & allow going to low power mode
        TInt err = iLinkAdapter.CancelLowPowerModeRequester();
        LOGGER_WRITE_1( "iLinkAdapter.CancelLowPowerModeRequester() err: %d", err );
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::TransportUpIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::TransportUpIndication()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::ObexConnectIndication( const TObexConnectInfo& aRemoteInfo,
// const TDesC8& aInfo )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConFTPplugin::ObexConnectIndication(
                        const TObexConnectInfo& /*aRemoteInfo*/,
                        const TDesC8& /*aInfo*/ )
    {
    TRACE_FUNC_ENTRY;
    TInt err( KErrNone );
    iStartTimer = ETrue;
    
    if ( iMediaType == ESrcsMediaBT && iObexServer )
        {
        TSockAddr remoteAddr;
        iObexServer->RemoteAddr( remoteAddr );
        
        TBTSockAddr btSockAddr( remoteAddr );
        TBTDevAddr devAddr = btSockAddr.BTAddr();
        
        err = iSocketServer.Connect();
        LOGGER_WRITE_1("iSocketServer.Connect err: %d", err );
        if ( !err )
            {
            err = iLinkAdapter.Open( iSocketServer, devAddr );
            LOGGER_WRITE_1("iLinkAdapter.Open err: %d", err );
            }
        // Ignore all BT link errors
        err = KErrNone;
        }
    
    if ( err == KErrNone && !iFTPHandler )
        {
        TRAP( err, LoadFTPDllL() );
        LOGGER_WRITE_1( "LoadFTPDllL returned : %d", err );
        }
    
    if ( err == KErrNone )
        {
        iFTPHandler->SetProfile( EStandard );
        LOGGER_WRITE( "CSConFTPplugin::ObexConnectIndication() : iServiceTimer->StopTimer()" );
        iServiceTimer->Cancel();
        iServiceTimer->StopTimer();
        }

    TRACE_FUNC_EXIT;
    LOGGER_WRITE_1( "CSConFTPplugin::ObexConnectIndication returned %d", err );
    return TFTPpluginUtils::ConvertFTPResponseCode( err );
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::ObexDisconnectIndication( const TDesC8& )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::ObexDisconnectIndication( const TDesC8& )
    {
    TRACE_FUNC_ENTRY;
    Disconnect();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::TransportDownIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::TransportDownIndication()
    {
    TRACE_FUNC_ENTRY;
    Disconnect();
    if ( iBuffer )
        {
        iBuffer->Reset();
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::PutRequestIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
CObexBufObject* CSConFTPplugin::PutRequestIndication()
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    TInt err( KErrNone );
    iPutError = KErrNone;

    if ( iBuffer )
        {
        iBuffer->Reset();
        }

    if ( iObject )
        {
        delete iObject;
        iObject = NULL;
        }

    TRAP( err, iObject = CObexBufObject::NewL ( iBuffer ) );

    if ( err != KErrNone )
        {
        return NULL;
        }

    if ( !iFTPHandler )
        {
        LOGGER_WRITE( "Creating iFTPHandler has failed" );
        return NULL;
        }

    TRAP( err, ret = iFTPHandler->PutFileObjectInitL( iObject, iBuffer ) );

    if ( err != KErrNone )
        {
        LOGGER_WRITE_1( "iFTPHandler->PutFileObjectInitL() leaves: %d", err );
        return NULL;
        }
    if ( ret != KErrNone )
        {
        LOGGER_WRITE_1( "PutFileObjectInitL failed - returning error in next PutPacketIndication : %d", ret );
        iPutError = ret;
        }
    iPutPacketIndicationCalled = EFalse;
    
    if ( !iShutdownWatcher )
        {
        TRAP( err, iShutdownWatcher = CShutdownWatcher::NewL( this ) );
        if ( err == KErrNone )
            {
            iShutdownWatcher->StartShutdownWatcher();
            }
        else
            {
            LOGGER_WRITE_1( "CShutdownWatcher::NewL leaves: %d", err );
            }
        }
    
    if ( iLinkAdapter.IsOpen() )
        {
        // request active BT mode (high power mode)
        err = iLinkAdapter.ActivateActiveRequester();
        LOGGER_WRITE_1( "iLinkAdapter.ActivateActiveRequester() err: %d", err );
        }
    
    TRACE_FUNC_EXIT;
    return( iObject );
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::PutPacketIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConFTPplugin::PutPacketIndication()
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    // always normal file transfer
    if ( (TInt)iObject->Length() > 0 )
        {
        if ( !iPutPacketIndicationCalled )
            {
            // This is first check, need to check only once
            TFileName path;
            iFTPHandler->GetPath( path );
            if ( path.CompareF( K_C_ROOT ) == 0 )
                {
                // path forbidden
                ret = KErrAccessDenied;
                }
            else
                {
                if ( iPutError != KErrNone )
                    {
                    ret = iPutError;
                    LOGGER_WRITE_1( "CSConFTPplugin::PutPacketIndication() : PutError : %d", iPutError );
                    }
                else
                    {
                    // check required free space
                    // if filesize is small ( <65k ) it fits into one package and its already saved to filesystem.
                    // if file is larger, need to check is there enought free space in device.
                    
                    const TUint32  filesize = iObject->Length();
                    LOGGER_WRITE_1( "CSConFTPplugin::PutPacketIndication() filesize %d", filesize );
                    LOGGER_WRITE_1( "CSConFTPplugin::PutPacketIndication() iObject->BytesReceived() %d", iObject->BytesReceived() );
                    if ( filesize > iObject->BytesReceived() )
                        {
                        LOGGER_WRITE( "CSConFTPplugin::PutPacketIndication() : check freespace" );
                        // file does not fit into one obex packet, check is there enought free space in current drive
                        if ( iFTPHandler->IsCurrentDiskSpaceBelowCritical( filesize ) )
                            {
    	                    LOGGER_WRITE( "CSConFTPplugin::PutPacketIndication() : returning KErrNoMemory" );
                            ret = KErrNoMemory;
                            }
                        }
                    }
                }
            }
        
        if ( iShutdownInProgress )
            {
            LOGGER_WRITE( "ShutdownInProgress, abort" );
            ret = KErrDisconnected;
            }
        }
    
    if ( !iPutPacketIndicationCalled )
        {
        // Need to check only once
        iPutPacketIndicationCalled = ETrue;
        
        //Check if filename is too long.
        TFileName path;
        iFTPHandler->GetPath( path );
        if ( ret == KErrNone && path.Length() + iObject->Name().Length() > KMaxFileName )
            {
            LOGGER_WRITE_1( "Name length overflow! : %d", path.Length() + iObject->Name().Length() );
            ret = KErrBadName;
            }
        }
    
    if ( ret != KErrNone )
        {
        if ( iFTPHandler )
            {
            iFTPHandler->AbortFileTransfer( iObject );
            }
        
        delete iObject;
        iObject = NULL;
        
        if ( iLinkAdapter.IsOpen() )
            {
            // Cancel ActivateActiveRequester & allow going to low power mode
            TInt err = iLinkAdapter.CancelLowPowerModeRequester();
            LOGGER_WRITE_1( "iLinkAdapter.CancelLowPowerModeRequester() err: %d", err );
            }
        }
    
    LOGGER_WRITE_1( "CSConFTPplugin::PutPacketIndication returned: %d", ret );
    return TFTPpluginUtils::ConvertFTPResponseCode( ret );
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::PutCompleteIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConFTPplugin::PutCompleteIndication()
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    TInt err( KErrNone );
    
    if ( iLinkAdapter.IsOpen() )
        {
        // Cancel ActivateActiveRequester & allow going to low power mode
        TInt err2 = iLinkAdapter.CancelLowPowerModeRequester();
        LOGGER_WRITE_1( "iLinkAdapter.CancelLowPowerModeRequester() err: %d", err2 );
        }
    
    TObexHeaderMask validHdrs = iObject->ValidHeaders();
    TTime time = iObject->Time();
    
    // shutdownWatcher is not needed anymore
    delete iShutdownWatcher;
    iShutdownWatcher = NULL;

    TInt size = iObject->BytesReceived();

    if (!(validHdrs & KObexHdrTime))
        {
        LOGGER_WRITE( "CSConFTPplugin::PutCompleteIndication() no valid time header received - using hometime" );
        time.HomeTime();
        TRAP( ret, iObject->SetTimeL(time) );
        LOGGER_WRITE_1( "CSConFTPplugin::PutCompleteIndication SetTimeL  %d", ret );
        }

    if ( ( validHdrs & KObexHdrBody ) || ( validHdrs & KObexHdrEndOfBody ) )
        {
        LOGGER_WRITE_1( "CSConFTPplugin::PutCompleteIndication : number of received bytes  %d", size );
        TSConUsedMedia media( ESConNoMedia );

        switch ( iMediaType )
            {
            case ESrcsMediaBT:
                media = ESConBTMedia;
                break;
            case ESrcsMediaIrDA:
                media = ESConIRMedia;
                break;
            case ESrcsMediaUSB:
                media = ESConUSBMedia;
                break;
            default:
                media = ESConNoMedia;
                break;
            }
        iFTPHandler->SetUsedMedia( media );
        
        TRAP( ret, err =
        this->iFTPHandler->PutFileObjectFinalizeL( iObject ) );
        LOGGER_WRITE_1( "CSConFTPplugin::PutCompleteIndication :PutFileObjectL  %d", ret );
        }
    else
        {
        LOGGER_WRITE( "CSConFTPplugin::PutCompleteIndication() : Delete  starts" );
        TRAP( ret, err = this->iFTPHandler->DeleteObjectL( iObject->Name() ) );
        LOGGER_WRITE_1( "CSConFTPplugin::PutCompleteIndication : DeleteObjectL  %d", ret );
        }

    if ( iObject )
        {
        delete iObject;
        iObject = NULL;
        }

    iFTPHandler->DeleteTempFile();

    if ( ret != KErrNone )
        {
        LOGGER_WRITE_1( "CSConFTPplugin::PutCompleteIndication : returned  %d", ret );
        return TFTPpluginUtils::ConvertFTPResponseCode( ret );
        }

    LOGGER_WRITE_1( "CSConFTPplugin::PutCompleteIndication : returned  %d", err );
    return TFTPpluginUtils::ConvertFTPResponseCode( err );
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::GetRequestIndication( CObexBaseObject* aRequiredObject )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
CObexBufObject* CSConFTPplugin::GetRequestIndication(
                                            CObexBaseObject* aRequiredObject )
    {
    TRACE_FUNC_ENTRY;
    CObexBufObject* bufObject(NULL);
    if ( aRequiredObject->Type().Length() > KSConHeaderMaxLength )
        {
        LOGGER_WRITE("TypeHeader too big");
        return NULL;
        }
    if ( aRequiredObject->Name().Length() > KMaxFileName )
        {
        LOGGER_WRITE("NameHeader is too long");
        return NULL;
        }
    TInt ret( KErrNone );
    TInt err( KErrNone );
    TBuf8<KSConHeaderMaxLength> typeHeader( aRequiredObject->Type() );
    TrimRightSpaceAndNull( typeHeader );
    LOGGER_WRITE8_1("type: %S", &typeHeader);
    LOGGER_WRITE_1("name: %S", &aRequiredObject->Name());

    iBuffer->Reset();
    
    delete iObject;
    iObject = NULL;
    
    TRAP( err, iObject = CObexBufObject::NewL ( iBuffer ) );
    if ( err != KErrNone )
        {
        return NULL;
        }
    
    // Client requests folder listing
    if ( typeHeader == KSConFolderListType )
        {
        LOGGER_WRITE( "Client requests folder listning" );
        if ( !iFTPHandler )
            {
            LOGGER_WRITE( "Creating iFTPHandler has failed" );
            return NULL;
            }

        TRAP( err, ret = iFTPHandler->GetFolderObjectL( iObject ) );
        LOGGER_WRITE_1( "iFTPHandler->GetFolderObjectL() returned: %d", ret );
        if ( err == KErrNone && ret == KErrNone )
            {
            bufObject = iObject;
            }
        else
            {
            LOGGER_WRITE_1( "CSConFTPplugin::GetRequestIndication : Leavecode returned  %d", err );
            return NULL;
            }
        }

    // Client requests a file
    else
        {
        if ( iFileObject )
            {
            delete iFileObject;
            iFileObject = NULL;
            }
        
        TRAP( err, iFileObject = CObexFileObject::NewL() );
        if ( err != KErrNone )
            {
            LOGGER_WRITE( "Creating iFileObject has failed" );
            return NULL;
            }
        iFileObject->Reset();
        TRAP( err, iFileObject->SetNameL( aRequiredObject->Name() ) );

        if ( err == KErrNone )
            {
            if ( !iFTPHandler )
                {
                LOGGER_WRITE( "CSConFTPplugin::GetRequestIndication() Creating iFTPHandler has failed" );
                return NULL;
                }

            TRAP( err, ret =
            this->iFTPHandler->GetFileObjectL( iFileObject ) );
            LOGGER_WRITE_1( "CSConFTPplugin::GetRequestIndication :  GetFileObjectL returned  %d", ret );
            }
        if ( err == KErrNone && ret == KErrNone )
            {
            bufObject = (CObexBufObject*)iFileObject;
            }
        else
            {
            LOGGER_WRITE_1( "CSConFTPplugin::GetRequestIndication : Leavecode   %d", err );
            return NULL;
            }
        }
    if ( bufObject && iLinkAdapter.IsOpen() )
        {
        // request active BT mode (high power mode)
        err = iLinkAdapter.ActivateActiveRequester();
        LOGGER_WRITE_1( "iLinkAdapter.ActivateActiveRequester() err: %d", err );
        }
    return bufObject;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::GetPacketIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConFTPplugin::GetPacketIndication()
    {
    TRACE_FUNC;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::GetCompleteIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConFTPplugin::GetCompleteIndication()
    {
    TRACE_FUNC_ENTRY;
    if ( iLinkAdapter.IsOpen() )
        {
        // Cancel ActivateActiveRequester & allow going to low power mode
        TInt err = iLinkAdapter.CancelLowPowerModeRequester();
        LOGGER_WRITE_1( "iLinkAdapter.CancelLowPowerModeRequester() err: %d", err );
        }
    
    TInt ret( KErrNone );
    if ( iFileObject )
        {
        iFileObject->Reset();
        delete iFileObject;
        iFileObject = NULL;
        }

    LOGGER_WRITE_1( "CSConFTPplugin::GetCompleteIndication() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::SetPathIndication( const CObex::TSetPathInfo& aPathInfo,
// const TDesC8& aInfo )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConFTPplugin::SetPathIndication( const CObex::TSetPathInfo& aPathInfo,
                                        const TDesC8& /*aInfo*/ )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    TInt err( KErrNone );

    if ( !iFTPHandler )
        {
        LOGGER_WRITE( "Creating iFTPHandler has failed" );
        return TFTPpluginUtils::ConvertFTPResponseCode( KErrNoMemory );
        }

    if ( aPathInfo.iFlags == KSConNULLSetPath )
        {
        TRAP( err, ret = this->iFTPHandler->CreateFolderL( aPathInfo.iName ) );
        }
    else
        {
        TRAP( err, ret = this->iFTPHandler->SetPathL( aPathInfo.iName,
        aPathInfo.iFlags ) );
        }
    
    if ( err != KErrNone )
        {
        LOGGER_WRITE_1( "CSConFTPplugin::SetPathIndication : returned   %d", err );
        return TFTPpluginUtils::ConvertFTPResponseCode( err );
        }
    LOGGER_WRITE_1( "CSConFTPplugin::SetPathIndication : returned %d", ret );
    return TFTPpluginUtils::ConvertFTPResponseCode( ret );
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::TrimRightSpaceAndNull( TDes8& aDes ) const
// Removes spaces and nulls from the end of the string
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::TrimRightSpaceAndNull( TDes8& aDes ) const
    {
    TRACE_FUNC;
    aDes.TrimRight();
    if ( aDes.Length() > 0 )
        {
        if ( !aDes[aDes.Length() - 1] )
            {
            aDes.SetLength( aDes.Length() - 1 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::SetMediaType ( TSrcsMediaType aMediaType )
// Current used transfer media (IR,USB,BT)
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::SetMediaType ( TSrcsMediaType aMediaType )
    {
    TRACE_FUNC;
    iMediaType = aMediaType;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::CSConFTPplugin()
// Constructor
// -----------------------------------------------------------------------------
//
CSConFTPplugin::CSConFTPplugin()
    {
    TRACE_FUNC;
    iBuffer = NULL;
    iObject = NULL;
    iFileObject = NULL;
    iSessionActive = EFalse;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConFTPplugin::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iBuffer = CBufFlat::NewL( KSConBufSize );
    iObject = CObexBufObject::NewL ( iBuffer );
    iServiceTimer = new (ELeave) CSConServiceTimer( this, KSConTimeOutValue );
    iServiceTimer->ConstructL();
    CActiveScheduler::Add( iServiceTimer );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTPplugin::SetObexServer( CObexServer* aObexServer )
// SetObexServer
// -----------------------------------------------------------------------------
//
TInt CSConFTPplugin::SetObexServer( CObexServer* aObexServer )
    {
    iObexServer = aObexServer;
    TInt ret = iObexServer->Start(this);
    LOGGER_WRITE_1( "CSConFTPplugin::SetObexServer() ret: %d", ret );
    return ret;
    }

// End of file

