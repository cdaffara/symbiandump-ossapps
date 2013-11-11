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
* Description:  PC Connectivity Plug-in implementation
*
*/


// INCLUDE FILES

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <obexconstantsinternal.h>
#endif
#include "sconpcconnplugin.h"
#include "sconftp.h"
#include "sconpcconnpluginutils.h"
#include "sconservicetimer.h"
#include "catalogspcconnectivityplugin.h"
#include "debug.h"


// CONSTANTS
_LIT(KSConFTPLibName, "sconftp.dll");
const TInt KSConFTPUidValue = 0x10009D8D;
const TUid KSConFTPUid = {KSConFTPUidValue};

// Folder listing type from IrObex specification
_LIT8( KSConFolderListType, "x-obex/folder-listing" );
// Folder listing type from IrObex specification
_LIT8( KSConCapabilityObjectType, "x-obex/capability" );
// PCD types
_LIT8( KSConPCDObjectType, "application/vnd.nokia.pcd" );
_LIT8( KSConPCDXMLObjectType, "application/vnd.nokia.conml+xml" );
_LIT8( KSConPCDWBXMLObjectType, "application/vnd.nokia.conml+wbxml" );
_LIT8( KSConPCDXMLObjectType2, "application/vnd.nokia.pcd+xml" );
_LIT8( KSConPCDWBXMLObjectType2, "application/vnd.nokia.pcd+wbxml" );
_LIT( K_C_ROOT, "C:\\" );

// PCD header commands
const TInt KSCon_PCD_TAG = 0x003A;
const TInt KSCon_PCD_CMD_COPY = 0x0037;
const TInt KSCon_PCD_CMD_MOVE_OR_RENAME = 0x0038;
const TInt KSCon_PCD_CMD_SET_PERMISSION = 0x0039;
// PCD header parameters
const TInt KSCon_PCD_PAR_FULL_PERMISSION = 0x0040;
const TInt KSCon_PCD_PAR_READ_ONLY_PERMISSION = 0x0041;


const TInt KSConBufSize = 262144; // 256KB
// Time (in milliseconds) for the timer
const TInt KSConTimeOutValue =  180000000;
// Flags used to indicate SetPath commands
const TInt KSConNULLSetPath =   0x00;

const TInt KSConHeaderMaxLength = 256;

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSConPCConnplugin::NewL()
// Two-phase constructing
// -----------------------------------------------------------------------------
//
CSConPCConnplugin* CSConPCConnplugin::NewL()
    {
    TRACE_FUNC_ENTRY;
    CSConPCConnplugin* self = new ( ELeave ) CSConPCConnplugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return( self );
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::~CSConPCConnplugin()
// Destructor
// -----------------------------------------------------------------------------
//
CSConPCConnplugin::~CSConPCConnplugin()
    {
    TRACE_FUNC_ENTRY;
    
    // Disconnect from services.
    Disconnect();
    
    delete iBuffer;
    iBuffer = NULL;
    
    if ( iServiceTimer )
        {
        iServiceTimer->Cancel();
        }
    
    delete iServiceTimer;
    iServiceTimer = NULL;
    
    delete iCatalogs;
    iCatalogs = NULL;
    delete iCatalogsMimeType;
    iCatalogsMimeType = NULL;
    
    iMimeArray.Reset();
    iMimeArray.Close();
    
    delete iShutdownWatcher;
    iShutdownWatcher = NULL;
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::IsOBEXActive()
// Returns active status of OBEX session
// -----------------------------------------------------------------------------
//
TBool CSConPCConnplugin::IsOBEXActive()
    {
    TRACE_FUNC;
    return iSessionActive;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::NotifyShutdown()
// System is shutting down
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::NotifyShutdown()
    {
    TRACE_FUNC;
    iShutdownInProgress = ETrue;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::LoadFTPDllL()
// Loads sconftp.dll module
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::LoadFTPDllL()
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
// CSConPCConnplugin::Disconnect()
// Closes initialized services
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::Disconnect()
    {
    TRACE_FUNC_ENTRY;
    
    iLinkAdapter.Close();
    iSocketServer.Close();
    
    //Disconnect from server
    iPCConnSession.Close();
    iPCConnSessionConnected = EFalse;
    
    if ( iStartTimer != EFalse )
        {
        iServiceTimer->Cancel();
        iServiceTimer->StartTimer();
        }
    
    delete iObject;
    iObject = NULL;
    
    if ( iFTPHandler )
        {
        delete iFTPHandler;
        iFTPHandler = NULL;
        iFTPlib.Close();
        }
    if ( iFileObject )
        {
        delete iFileObject;
        iFileObject = NULL;
        }
    if ( iCatalogsConnected )
        {
        iCatalogs->Close();
        iCatalogsConnected = EFalse;
        iCatalogsMimeTypesExist = EFalse;
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::ErrorIndication( TInt aError )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::ErrorIndication( TInt aError )
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
    
    if ( iPCConnSessionConnected )
        {
        iPCConnSession.ResetServer();
        }
    
    // Keep compiler happy
    (void)aError;
    
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
// CSConPCConnplugin::AbortIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::AbortIndication()
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
    
    if ( iPCConnSessionConnected )
        {
        iPCConnSession.ResetServer();
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
// CSConPCConnplugin::TransportUpIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::TransportUpIndication()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::ObexConnectIndication( const TObexConnectInfo&
// aRemoteInfo, const TDesC8& aInfo )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::ObexConnectIndication( const TObexConnectInfo&
                                    /*aRemoteInfo*/, const TDesC8& /*aInfo*/ )
    {
    TRACE_FUNC_ENTRY;
    TInt err( KErrNone );
    TInt catalogsErr( KErrNone );
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
    
    //Connect to server
    if ( !iPCConnSessionConnected )
        {
        LOGGER_WRITE("Try to connect to iPCConnSession");
        err = iPCConnSession.Connect();
        LOGGER_WRITE_1( "iPCConnSession.Connect returned : %d", err );
        if ( err == KErrNone )
            {
            iPCConnSessionConnected = ETrue;
            }
        }
    
    if ( err == KErrNone && !iFTPHandler )
        {
        TRAP( err, LoadFTPDllL() );
        LOGGER_WRITE_1( "LoadFTPDllL returned : %d", err );
        }
    
    if ( err == KErrNone )
        {
        iFTPHandler->SetProfile( EProprietary );
        LOGGER_WRITE( "iServiceTimer->StopTimer()" );
        iServiceTimer->Cancel();
        iServiceTimer->StopTimer();
        }

    if ( !iCatalogsConnected )
        {
        LOGGER_WRITE( "CSConPCConnplugin::ObexConnectIndication() before iCatalogs->Connect()" );
        catalogsErr = iCatalogs->Connect();
        LOGGER_WRITE_1( "CSConPCConnplugin::ObexConnectIndication() iCatalogs->Connect() err: %d", catalogsErr );
        if ( catalogsErr == KErrNone )
            {
            LOGGER_WRITE( "CSConPCConnplugin::ObexConnectIndication() iCatalogs->Connect() success" );
            iCatalogsConnected = ETrue;
            TRAPD( ret, iMimeArray = iCatalogs->SupportedMimeTypesL() );
            if ( ret == KErrNone )
                {
                LOGGER_WRITE_1( "iCatalogs->SupportedMimeTypesL count: %d", iMimeArray.Count() );
                iCatalogsMimeTypesExist = ETrue;
                }
            }
        }
    TRACE_FUNC_EXIT;
    return TPCConnpluginUtils::ConvertFTPResponseCode( err );
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::ObexDisconnectIndication( const TDesC8& )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::ObexDisconnectIndication( const TDesC8& )
    {
    TRACE_FUNC_ENTRY;
    Disconnect();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::TransportDownIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::TransportDownIndication()
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
// CSConPCConnplugin::PutRequestIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
CObexBufObject* CSConPCConnplugin::PutRequestIndication()
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
// CSConPCConnplugin::PutPacketIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::PutPacketIndication()
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );

    TBool catalogsMime( EFalse );
    if ( iObject->Type().Length() > KSConHeaderMaxLength )
        {
        LOGGER_WRITE("TypeHeader too big, return KErrNotSupported");
        return TPCConnpluginUtils::ConvertFTPResponseCode( KErrNotSupported );
        }
    TBuf8<KSConHeaderMaxLength> typeHeader( iObject->Type() );
    TrimRightSpaceAndNull( typeHeader );
    
    if ( iCatalogsConnected && IsCatalogsMimeType( TDataType( typeHeader ) ) )
        {
        catalogsMime = ETrue;
        }

    if ( (TInt)iObject->Length() > 0 
        && typeHeader != KSConPCDWBXMLObjectType
        && typeHeader != KSConPCDWBXMLObjectType2
        && !catalogsMime
        && typeHeader != KSConPCDObjectType )
        {
        // normal file transfer
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
                    LOGGER_WRITE_1( "CSConPCConnplugin::PutPacketIndication() : PutError : %d", iPutError );
                    }
                else
                    {
                    // check required free space
                    // if filesize is small ( <65k ) it fits into one package and its already saved to filesystem.
                    // if file is larger, need to check is there enought free space in device.
                    
                    const TUint32  filesize = iObject->Length();
                    LOGGER_WRITE_1( "CSConPCConnplugin::PutPacketIndication() filesize %d", filesize );
                    LOGGER_WRITE_1( "CSConPCConnplugin::PutPacketIndication() iObject->BytesReceived() %d", iObject->BytesReceived() );
                    if ( filesize > iObject->BytesReceived() )
                        {
                        LOGGER_WRITE( "CSConPCConnplugin::PutPacketIndication() : check freespace" );
                        // file does not fit into one obex packet, check is there enought free space in current drive
                        if ( iFTPHandler->IsCurrentDiskSpaceBelowCritical( filesize ) )
                            {
                            LOGGER_WRITE( "CSConPCConnplugin::PutPacketIndication() : returning KErrNoMemory" );
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
    
    LOGGER_WRITE_1( "CSConPCConnplugin::PutPacketIndication(): ret %d", ret );
    return TPCConnpluginUtils::ConvertFTPResponseCode( ret );
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::PutCompleteIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::PutCompleteIndication()
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
    
    if ( iObject->Type().Length() > KSConHeaderMaxLength )
        {
        LOGGER_WRITE("TypeHeader too big, return KErrNotSupported");
        delete iObject;
        iObject = NULL;
                
        iFTPHandler->DeleteTempFile();
        return TPCConnpluginUtils::ConvertPCDResponseCode( KErrNotSupported );
        }
    if ( iObject->Name().Length() > KSConHeaderMaxLength )
        {
        LOGGER_WRITE("NameHeader too big, return KErrNotSupported");
        delete iObject;
        iObject = NULL;
                
        iFTPHandler->DeleteTempFile();
        return TPCConnpluginUtils::ConvertPCDResponseCode( KErrNotSupported );
        }
    
    // check if ConML signal is received
    TBuf8<KSConHeaderMaxLength> typeHeader( iObject->Type() );
    TBuf<KSConHeaderMaxLength> nameHeader( iObject->Name() );
    TrimRightSpaceAndNull( typeHeader );
    LOGGER_WRITE8_1("type: %S", &typeHeader);
    LOGGER_WRITE_1("name: %S", &nameHeader);
    LOGGER_WRITE_1("description: %S", &iObject->Description());
    TInt size = iObject->BytesReceived();
    TTime time = iObject->Time();
    
    // shutdownWatcher is not needed anymore
    delete iShutdownWatcher;
    iShutdownWatcher = NULL;
    
    if ( IsCatalogsMimeType( TDataType( typeHeader ) ) )
        {
        if ( iCatalogsConnected )
            {
            LOGGER_WRITE( "iCatalogs->PutDataL" );
            TRAP( err, iCatalogs->PutDataL( typeHeader, *iBuffer ) );
            LOGGER_WRITE_1( "iCatalogs->PutDataL : %d", err );
            if ( err == KErrServerTerminated )
                {
                err = iCatalogs->Connect();
                LOGGER_WRITE_1( "CSConPCConnplugin::PutCompleteIndication() iCatalogs->Connect() err: %d", err );
                if ( err == KErrNone )
                    {
                    LOGGER_WRITE( "iCatalogs->PutDataL" );
                    TRAP( err, iCatalogs->PutDataL( typeHeader, *iBuffer ) );
                    LOGGER_WRITE_1( "iCatalogs->PutDataL : %d", err );
                    }
                }
            }
        else
            {
            LOGGER_WRITE( "Catalogs mime type recognised - failing due no connection" );
            err = KErrNotSupported;
            }
        
        delete iObject;
        iObject = NULL;
        
        iFTPHandler->DeleteTempFile();
        return TPCConnpluginUtils::ConvertPCDResponseCode( err );
        }

    if ( typeHeader == KSConPCDObjectType )
        {
        LOGGER_WRITE( "CSConPCConnplugin::PutCompleteIndication KSConPCDObjectType" );
        err = HandlePCDObjectPut( iObject->Description(), iObject->Name() );
        delete iObject;
        iObject = NULL;
        
        iFTPHandler->DeleteTempFile();
        LOGGER_WRITE_1( "CSConPCConnplugin::PutCompleteIndication() end, err: %d", err );
        return TPCConnpluginUtils::ConvertObexMoveResponseCode( err );
        }

    if ( typeHeader != KSConPCDWBXMLObjectType &&
        typeHeader != KSConPCDWBXMLObjectType2 )
        {
        TObexHeaderMask validHdrs = iObject->ValidHeaders();
        if ( ( validHdrs & KObexHdrBody ) || ( validHdrs & KObexHdrEndOfBody ) )
            {
            LOGGER_WRITE( "CSConPCConnplugin::PutCompleteIndication() normal filetransfer" );
            LOGGER_WRITE_1( "Number of received bytes : %d", size );
            TSConUsedMedia media( ESConNoMedia );
            
            switch( iMediaType )
                {
                case ESrcsMediaBT :
                    media = ESConBTMedia;
                    break;
                case ESrcsMediaIrDA :
                    media = ESConIRMedia;
                    break;
                case ESrcsMediaUSB :
                    media = ESConUSBMedia;
                    break;
                default :
                    media = ESConNoMedia;
                    break;
                }
            
            iFTPHandler->SetUsedMedia( media );
            
            TRAP( ret, err = iFTPHandler->PutFileObjectFinalizeL( iObject ) );
            LOGGER_WRITE_1( "PutFileObjectL() returned : %d", ret );
            }
        else
            {
            LOGGER_WRITE( "Delete starts" );
            TRAP( ret, err = iFTPHandler->DeleteObjectL( iObject->Name() ) );
            LOGGER_WRITE_1( "DeleteObjectL() returned : %d", ret );
            }
        }
    
    delete iObject;
    iObject = NULL;
    
    if ( typeHeader == KSConPCDWBXMLObjectType ||
        typeHeader == KSConPCDWBXMLObjectType2 )
        {
        LOGGER_WRITE( "CSConPCConnplugin::PutCompleteIndication() ReadWBXMLDataL" );
        TRAP( ret, err = iFTPHandler->ReadWBXMLDataL( iBuffer ) );
        
        if ( err == KErrNone && ret == KErrNone )
            {
            TRAP( ret, err = iPCConnSession.PutPacketL( nameHeader,
            typeHeader, iBuffer ) );
            LOGGER_WRITE_1( "iPCConnSession.PutPacketL() leaveCode : %d", ret );
            LOGGER_WRITE_1( "iPCConnSession.PutPacketL() returned : %d", err );
            }
        
        iFTPHandler->DeleteTempFile();
        
        return TPCConnpluginUtils::ConvertPCDResponseCode( err );
        }
    
    iFTPHandler->DeleteTempFile();
    
    TRACE_FUNC_EXIT;
    if ( ret != KErrNone )
        {
        return TPCConnpluginUtils::ConvertFTPResponseCode( ret );
        }
    return TPCConnpluginUtils::ConvertFTPResponseCode( err );
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::GetRequestIndication( CObexBaseObject* aRequiredObject )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
CObexBufObject* CSConPCConnplugin::GetRequestIndication( CObexBaseObject*
                                                        aRequiredObject )
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
            return NULL;
            }
        }
    // Client requests capability object or ConML packet
    else if ( typeHeader == KSConCapabilityObjectType
            || typeHeader == KSConPCDWBXMLObjectType
            || typeHeader == KSConPCDWBXMLObjectType2)
        {
        LOGGER_WRITE( "Client requests capability object or ConML packet" );
        // HandleGetPCConnObjectL will set some data to iObject, or leaves if error occurs
        TRAP( err, HandleGetPCConnObjectL( aRequiredObject->Name(), typeHeader ) );
        
        if ( err == KErrNone )
            {
            bufObject = iObject;
            }
        else
            {
            LOGGER_WRITE_1("Get PCConn object failed, err: %d", err);
            return NULL;
            }
        }
    // Client requests file conversion
    else if ( typeHeader == KSConPCDObjectType )
        {
        LOGGER_WRITE("request of KSConPCDObjectType is not supported.")
        return NULL;
        }
    // catalogs
    else if ( IsCatalogsMimeType( TDataType( typeHeader ) ) )
        {
        TRAP( err, HandleGetCatalogsObjectL( typeHeader ) );
        if ( err == KErrNone )
            {
            bufObject = iObject;
            }
        else
            {
            LOGGER_WRITE_1("Get catalogs object failed, err: %d", err);
            return NULL;
            }
        }
    // Client requests file
    else
        {
        LOGGER_WRITE( "Client requests file" );
        TRAP( err, HandleGetFileObjectL( aRequiredObject->Name() ) );
        if ( err == KErrNone )
            {
            bufObject = (CObexBufObject*)iFileObject;
            }
        else
            {
            LOGGER_WRITE_1("Get file object failed, err: %d", err);
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
// CSConPCConnplugin::GetPacketIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::GetPacketIndication()
    {
    TRACE_FUNC;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::GetCompleteIndication()
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::GetCompleteIndication()
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

    LOGGER_WRITE_1( "CSConPCConnplugin::GetCompleteIndication() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::SetPathIndication( const CObex::TSetPathInfo& aPathInfo,
// const TDesC8& aInfo )
// MObexServerNotify implementation
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::SetPathIndication( const CObex::TSetPathInfo& aPathInfo,
                                            const TDesC8& /*aInfo*/ )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    TInt err( KErrNone );
    
    if ( !iFTPHandler )
        {
        LOGGER_WRITE( "Creating iFTPHandler has failed" );
        return TPCConnpluginUtils::ConvertFTPResponseCode( KErrNoMemory );
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
        LOGGER_WRITE_1( "CSConPCConnplugin::SetPathIndication : returned   %d", err );
        return TPCConnpluginUtils::ConvertFTPResponseCode( err );
        }
    LOGGER_WRITE_1( "CSConPCConnplugin::SetPathIndication : returned %d", ret );
    return TPCConnpluginUtils::ConvertFTPResponseCode( ret );
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::TrimRightSpaceAndNull( TDes8& aDes ) const
// Removes spaces and nulls from the end of the string
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::TrimRightSpaceAndNull( TDes8& aDes ) const
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
// CSConPCConnplugin::SetMediaType( TSrcsMediaType aMediaType )
// Current used transfer media (IR,USB,BT)
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::SetMediaType( TSrcsMediaType aMediaType )
    {
    TRACE_FUNC;
    iMediaType = aMediaType;
    }
// -----------------------------------------------------------------------------
// CSConPCConnplugin::TBool IsCatalogsMimeType( TDataType aMime )
// Checks if mime type belogs to Catalogs
// -----------------------------------------------------------------------------
//
TBool CSConPCConnplugin::IsCatalogsMimeType( TDataType aMime )
    {
    TRACE_FUNC_ENTRY;
    TBool result = EFalse;

    if ( !iCatalogsConnected
        && aMime.Des8() != KSConPCDObjectType
        && aMime.Des8() != KSConPCDXMLObjectType
        && aMime.Des8() != KSConPCDWBXMLObjectType
        && aMime.Des8() != KSConPCDXMLObjectType2
        && aMime.Des8() != KSConPCDWBXMLObjectType2
        )
        {
        // Catalogs never connected and not secon mime type
        // Try to connect to catalogs to get catalogstypes
        TInt catalogsErr = iCatalogs->Connect();
        LOGGER_WRITE_1( "CSConPCConnplugin::IsCatalogsMimeType() iCatalogs->Connect() err: %d", catalogsErr );
        if ( catalogsErr == KErrNone )
            {
            LOGGER_WRITE( "CSConPCConnplugin::IsCatalogsMimeType() iCatalogs->Connect() success" );
            iCatalogsConnected = ETrue;
            TRAPD( ret, iMimeArray = iCatalogs->SupportedMimeTypesL() );
            if ( ret == KErrNone )
                {
                LOGGER_WRITE_1( "iCatalogs->SupportedMimeTypesL count: %d", iMimeArray.Count() );
                iCatalogsMimeTypesExist = ETrue;
                }
            }
        }

    if ( iCatalogsMimeTypesExist )
        {
        // Compare to supported values
        for ( TInt j = 0; j < iMimeArray.Count(); j++ )
            {
            if ( (*iMimeArray[j]) == aMime )
                {
                LOGGER_WRITE( "CSConPCConnplugin::IsCatalogsMimeTypeL returns TRUE" );
                result = ETrue;
                break;
                }
            }
        }
    TRACE_FUNC_EXIT;
    return result;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::CSConPCConnplugin()
// Default constructor
// -----------------------------------------------------------------------------
//
CSConPCConnplugin::CSConPCConnplugin()
    {
    TRACE_FUNC;
    iBuffer = NULL;
    iObject = NULL;
    iFileObject = NULL;
    iSessionActive = EFalse;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iBuffer = CBufFlat::NewL( KSConBufSize );
    iObject = CObexBufObject::NewL ( iBuffer );
    iServiceTimer = new (ELeave) CSConServiceTimer( this, KSConTimeOutValue );
    iServiceTimer->ConstructL();
    CActiveScheduler::Add( iServiceTimer );
    
    // catalogs
    iCatalogs = CCatalogsPCConnectivityPlugin::NewL();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::SetObexServer( CObexServer* aObexServer )
// SetObexServer
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::SetObexServer( CObexServer* aObexServer )
    {
    iObexServer = aObexServer;
    TInt ret = iObexServer->Start(this);
    LOGGER_WRITE_1( "CSConPCConnplugin::SetObexServer() ret: %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::HandlePCDObjectPut
// Handles PCD object. Used for advanced file handling.
// -----------------------------------------------------------------------------
//
TInt CSConPCConnplugin::HandlePCDObjectPut( const TDesC& aDescriptionHeader,
        const TDesC& aNameHeader )
    {
    TRACE_FUNC_ENTRY;
    TInt err( KErrNotSupported);
    if ( aDescriptionHeader.Length() >= 3 )
        {
        TInt commandTag = aDescriptionHeader[0];
        TInt commandID = aDescriptionHeader[1];
        TInt commandParam1 = aDescriptionHeader[2];

        TPtrC destination;
        if ( aDescriptionHeader.Length() >= 3 )
            {
            destination.Set( aDescriptionHeader.Mid(3) );
            }
        TPtrC source;
        if ( aNameHeader.Length() >= 1 )
            {
            // remove ":" character from beginning
            source.Set( aNameHeader.Mid(1) );
            }

        if ( commandTag == KSCon_PCD_TAG )
            {
            LOGGER_WRITE( "CSConPCConnplugin::HandlePCDObjectPut KSCon_PCD_TAG found" );
            switch (commandID)
                {
                case KSCon_PCD_CMD_COPY:
                    err = iFTPHandler->CopyFile( source, destination );
                    break;

                case KSCon_PCD_CMD_MOVE_OR_RENAME:
                    err = iFTPHandler->MoveFile( source, destination );
                    break;

                case KSCon_PCD_CMD_SET_PERMISSION:
                    if ( commandParam1 == KSCon_PCD_PAR_FULL_PERMISSION )
                        {
                        err = iFTPHandler->SetReadOnly( source, EFalse );
                        }
                    else if ( commandParam1 == KSCon_PCD_PAR_READ_ONLY_PERMISSION )
                        {
                        err = iFTPHandler->SetReadOnly( source, ETrue );
                        }
                    else
                        {
                        err = KErrNotSupported;
                        }
                    break;

                default:
                    LOGGER_WRITE( "CSConPCConnplugin::HandlePCDObjectPut unknown commandID" );
                    err = KErrNotSupported;
                    break;
                }

            }

        }
    TRACE_FUNC_EXIT;
    return err;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::HandleGetPCConnObjectL
// Get Capbility object or ConML message from PCConnServer
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::HandleGetPCConnObjectL( const TDesC& aNameHeader,
        const TDesC8& aTypeHeader )
    {
    TRACE_FUNC_ENTRY;
    if ( !iPCConnSessionConnected )
        {
        LOGGER_WRITE( "ERROR, iPCConnSession was not connected" );
        User::Leave( KErrNotReady );
        }
    TInt err = iPCConnSession.GetPacketL( aNameHeader, aTypeHeader, iBuffer );
    LOGGER_WRITE_1( "iPCConnSession.GetPacketL() returned: %d", err );
    User::LeaveIfError( err );
    iObject->SetDataBufL( iBuffer );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::HandleGetCatalogsObjectL
// Get Catalogs object
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::HandleGetCatalogsObjectL( const TDesC8& aTypeHeader )
    {
    TRACE_FUNC_ENTRY;
    TInt err(KErrNone);
    if ( !iCatalogsConnected )
        {
        LOGGER_WRITE( "catalogs mime type recognised - failing due to no connection" );
        User::Leave( KErrNotReady );
        }
    else
        {
        LOGGER_WRITE( "iCatalogs->GetDataL" );
        TDataType mime( aTypeHeader );
        TRAP( err, iCatalogs->GetDataL( mime, *iBuffer ) );
        LOGGER_WRITE_1( "iCatalogs->GetDataL : %d", err );
        if ( err == KErrServerTerminated )
            {
            // server was terminated, try to reconnect
            err = iCatalogs->Connect();
            LOGGER_WRITE_1( "iCatalogs->Connect() err: %d", err );
            User::LeaveIfError( err );
            LOGGER_WRITE( "iCatalogs->GetDataL" );
            iCatalogs->GetDataL( mime, *iBuffer );
            }
        }
    User::LeaveIfError( err );
    iObject->SetDataBufL( iBuffer );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnplugin::HandleGetFileObjectL
// Get file object
// -----------------------------------------------------------------------------
//
void CSConPCConnplugin::HandleGetFileObjectL( const TDesC& aNameHeader )
    {
    TRACE_FUNC_ENTRY;
    delete iFileObject;
    iFileObject = NULL;
    iFileObject = CObexFileObject::NewL();
    
    iFileObject->Reset();
    iFileObject->SetNameL( aNameHeader );

    if ( !iFTPHandler )
        {
        LOGGER_WRITE( "Creating iFTPHandler has failed" );
        User::Leave( KErrNotReady );
        }

    TInt err = iFTPHandler->GetFileObjectL( iFileObject );
    User::LeaveIfError( err );
    TRACE_FUNC_EXIT;
    }

// End of file
