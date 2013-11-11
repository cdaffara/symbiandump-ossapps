/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client that communicate with CatalogsEnginePCClientFrontEnd
*
*/


#include "catalogspcconnectivityclient.h"
#include "catalogspcconnectivitydefines.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
RCatalogsPCConnectivityClient::RCatalogsPCConnectivityClient()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
RCatalogsPCConnectivityClient::~RCatalogsPCConnectivityClient()
    {
    }

// ---------------------------------------------------------------------------
// Start server and create session with server
// ---------------------------------------------------------------------------
//
TInt RCatalogsPCConnectivityClient::Connect()
    {
    TInt err = StartServer();
        
    if ( err == KErrNone )
        {
        err = CreateSession( 
            KCatalogsPCConnectivityServerName,
            Version(), 
            KCatalogsPCConnectivityDefaultMessageSlots );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Close session (After session is closed will server be closed too)
// ---------------------------------------------------------------------------
//
void RCatalogsPCConnectivityClient::Close()
    {
    RHandleBase::Close();
    }

// ---------------------------------------------------------------------------
// Give MIME types supported by this Module
// ---------------------------------------------------------------------------
//
RPointerArray<TDataType> RCatalogsPCConnectivityClient::
SupportedMimeTypesL()
    {
    RPointerArray<TDataType> supportedMIMEs;
    TBool isThereMore = ETrue;
    TPckgBuf<TBool> pckgIsThereMore;
    TDataType* tempMIME = 0;
    TPckgBuf<TDataType> pckgMIME;
    
    while( isThereMore )
        {
        User::LeaveIfError( SendReceive( 
                ECatalogsPCConnectivitySupportedMimeTypes, 
                TIpcArgs( &pckgMIME, &pckgIsThereMore ) ) );
        isThereMore = pckgIsThereMore();
        tempMIME = new( ELeave ) TDataType( pckgMIME() );
        User::LeaveIfError( supportedMIMEs.Append( tempMIME ) );
        }

    return supportedMIMEs;
    }

// ---------------------------------------------------------------------------
// Data package from CatalogsPCConnectivityPlugin
// ---------------------------------------------------------------------------
//
void RCatalogsPCConnectivityClient::PutDataL( 
    const TDataType& aMimeType, const CBufFlat& aData )
    {
    TInt size = aData.Size();
    
    RBuf8 desData;
    desData.CreateL( size );
    desData.CleanupClosePushL();
    aData.Read( 0, desData, size );
    TPckgBuf<TDataType> pckgMIME( aMimeType );

    User::LeaveIfError( SendReceive( 
            ECatalogsPCConnectivityPutData, 
            TIpcArgs( &pckgMIME, &desData, size ) ) );

    CleanupStack::Pop(); //desData
    desData.Close();
    }

// ---------------------------------------------------------------------------
// Data request from CatalogsPCConnectivityPlugin
// First ask the size from server, then give correct size CBufFlat
// ---------------------------------------------------------------------------
//
void RCatalogsPCConnectivityClient::GetDataL( 
    TDataType& aMimeType, CBufFlat& aData )
    {
    TPckgBuf<TInt> pckgSize;
    TPckgBuf<TDataType> pckgMIME;
    
    User::LeaveIfError( SendReceive( 
            ECatalogsPCConnectivityGetSize, 
            TIpcArgs( &pckgMIME, &pckgSize ) ) );
    
    TInt size = pckgSize();
    aMimeType = pckgMIME();
    
    RBuf8 desData;
    desData.CreateL( size );
    desData.CleanupClosePushL();
    
    User::LeaveIfError( SendReceive( 
            ECatalogsPCConnectivityGetData, 
            TIpcArgs( &desData ) ) );
    
    aData.Reset();
    aData.ExpandL( 0, size );
    aData.Write( 0, desData, size );        
            
    CleanupStack::Pop(); //desData
    desData.Close();
    }

// ---------------------------------------------------------------------------
// Return version number
// ---------------------------------------------------------------------------
//
TVersion RCatalogsPCConnectivityClient::Version() const
    {
    return( TVersion( KCatalogsPCConnectivityMajorVersionNumber,
            KCatalogsPCConnectivityMinorVersionNumber,
            KCatalogsPCConnectivityBuildVersionNumber ) );
    }

// ---------------------------------------------------------------------------
// Start server if it isn't running already
// ---------------------------------------------------------------------------
//
TInt RCatalogsPCConnectivityClient::StartServer()
    {
    // Check if the server is already running
    TFindServer findServer( KCatalogsPCConnectivityServerName );
    TFullName name;

    TInt result = findServer.Next( name );
    if ( result == KErrNone )
        {
        // Server is running
        return KErrNone;
        }

    // Create a semaphore so we can wait while the server starts
    RSemaphore semaphore;
    result = semaphore.CreateGlobal( 
    	KCatalogsPCConnectivityServerSemaphoreName, 0 );
    if ( result != KErrNone )
        {
        return result;
        }

    // Create new Engine service process 
    result = CreateServerProcess();
    if ( result != KErrNone )
        {
        semaphore.Close();
        return result;
        }

    // Wait while the server starts
    semaphore.Wait();

    // Semaphore has been signaled, close and return
    semaphore.Close();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Create server process
// ---------------------------------------------------------------------------
//
TInt RCatalogsPCConnectivityClient::CreateServerProcess()
    {

    RProcess server;

    TInt result = server.Create( 
        KCatalogsPCConnectivityServerFilename, KNullDesC );
    if ( result != KErrNone )
        {
        return result;
        }
    
    // Resume server thread and close handle
    server.Resume();
    server.Close();  

    return KErrNone;
    }


//end
