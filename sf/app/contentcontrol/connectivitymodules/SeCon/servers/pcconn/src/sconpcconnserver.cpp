/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PC Connectivity server
*
*/


#include <s32mem.h> // For RMemReadStream
#include <utf.h>
#include <Etel3rdParty.h>
#include <f32file.h>

#include "sconpcconnclientserver.h"
#include "sconpcconnserver.h"
#include "sconpcd.h"
#include "sconcsc.h"
#include "sconconmlhandler.h"
#include "sconimsireader.h"
#include "sconoviaccounthandler.h"
#include "sconfolderlister.h"
#include "sconbtengine.h"
#include "debug.h"

#ifdef DEBUG_XML
#include <s32file.h>
_LIT8( KTimeFormat, "%02d:%02d:%02d.%03d" );
_LIT8( KXmlBegin, "\nXML:\n" );
#endif

_LIT( KSCONGetMetadataRequest, "METADATA:" );
_LIT8( KSCONSyncRequest, "SYNC:" );
_LIT( KSCONReadImsi, "READIMSI");
_LIT( KSCONReadNetworkInfo, "READNETWORKINFO");
_LIT( KSCONReadOviAccount, "READOVIACCOUNT");
_LIT( KSCONListPath, "LISTP:");
_LIT( KSCONReadBtInfo, "READBTINFO" );
_LIT( KSCONSetBtPower, "SETBTPOWER:" );
_LIT( KSCONSetBtName, "SETBTNAME:" );
_LIT( KSCONSetBtAuthorized, "SETBTAUTHORIZED:" );

//------------------------------------------------------------
// Global functions 
//------------------------------------------------------------

// -----------------------------------------------------------------------------
// E32Main()
// Entry point 
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    TRACE_FUNC_ENTRY;
    __UHEAP_MARK;
    TInt error( KErrNone );
    error = CSConPCConnServer::RunServer();
    __UHEAP_MARKEND;
    TRACE_FUNC_EXIT;
    return error;
    }

// -----------------------------------------------------------------------------
// PanicServer()
// Panics the server with panic reason aPanic 
// -----------------------------------------------------------------------------
//
GLDEF_C void PanicServer(TPCConnServPanic aPanic)
    {
    LOGGER_WRITE_1( "CSConPCConnSession::PanicServer() : Panic code  %d", aPanic );  
    _LIT(KTxtServerPanic,"PCConn server panic");
    User::Panic(KTxtServerPanic,aPanic);
    }

// -----------------------------------------------------------------------------
// CSConPCConnServer::CSConPCConnServer ()
// Default constructor - can not leave
// -----------------------------------------------------------------------------
//
CSConPCConnServer::CSConPCConnServer () : CServer2( EPriorityStandard)
    {
    LOGGER_WRITE( "* * * *  CSConPCConnServer  * * * *" );
    }

// -----------------------------------------------------------------------------
// CSConPCConnServer::~CSConPCConnServer()
// Default destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSConPCConnServer::~CSConPCConnServer()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CSConPCConnServer::NewLC()
// Creates a new instance of CSConPCConnServer
// -----------------------------------------------------------------------------
//
EXPORT_C CSConPCConnServer*  CSConPCConnServer::NewLC()
    {
    TRACE_FUNC_ENTRY;
    CSConPCConnServer* self  = new (ELeave) CSConPCConnServer();
    CleanupStack::PushL( self );
    self->StartL( KSConPCConnServerName );
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CSConPCConnServer::NewSessionL()
// Creates a new session to the client
// -----------------------------------------------------------------------------
//
CSession2* CSConPCConnServer::NewSessionL( 
    const TVersion &aVersion, const RMessage2& /*aMessage*/ ) const
    {
    TRACE_FUNC_ENTRY;
    
    // check version
    TVersion version( KSConPCConnServerVersionMajor, 
                                  KSConPCConnServerVersionMinor, 
                                  KSConPCConnServerVersionBuild);
    
    if (!User::QueryVersionSupported(version, aVersion))
        {
        User::Leave(KErrNotSupported);
        }
    
    // check client identity
    RThread client;
    Message().Client( client );
    TSecureId clientId = client.SecureId();
    
    if ( clientId != KSConPCConnClientSecureId )
        {
        LOGGER_WRITE( "CSConPCConnServer::NewSessionL() : Secure ID does not match" );
        User::Leave( KErrAccessDenied );
        }
    
    TRACE_FUNC_EXIT;
    return CSConPCConnSession::NewL( *CONST_CAST( CSConPCConnServer*, this));
    }

// -----------------------------------------------------------------------------
// CSConPCConnServer::RunServer()
// Starts the server
// -----------------------------------------------------------------------------
//
TInt CSConPCConnServer::RunServer()
    {
    TRACE_FUNC_ENTRY;
    
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt ret = KErrNoMemory;
    if( cleanup )
        {
        TRAP( ret, CSConPCConnServer::RunServerL(  ) );
        delete cleanup;
        }
    if( ret != KErrNone )
        {
        // Signal the client that server creation failed
        RProcess::Rendezvous( ret );
        }
    TRACE_FUNC_EXIT;
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnServer::RunServerL()
// Starts the server
// -----------------------------------------------------------------------------
//
void CSConPCConnServer::RunServerL()
    {
    // Create and install the active scheduler we need
    TRACE_FUNC_ENTRY;
    CActiveScheduler *as=new (ELeave)CActiveScheduler;
    CleanupStack::PushL( as );
    CActiveScheduler::Install( as );

    // Create server
    CSConPCConnServer* server = CSConPCConnServer::NewLC();

    // Initialisation complete, now signal the client
    User::LeaveIfError( RThread().RenameMe( KSConPCConnServerName ) );
    RProcess::Rendezvous( KErrNone );

    // Ready to run
    CActiveScheduler::Start();
    
    // Cleanup the scheduler
    CleanupStack::PopAndDestroy( 2, as );
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnServer::IncSessionCount()
// Increments session count
// -----------------------------------------------------------------------------
//
void CSConPCConnServer::IncSessionCount()
    {
    TRACE_FUNC_ENTRY;
    iSessionCount++;
    LOGGER_WRITE_1( "There are now %d sessions", iSessionCount );
    TRACE_FUNC_EXIT;
    }    
    
// -----------------------------------------------------------------------------
// CSConPCConnServer::DecSessionCount()
// Decrements session count
// -----------------------------------------------------------------------------
//
void CSConPCConnServer::DecSessionCount()
    {
    TRACE_FUNC_ENTRY;
    iSessionCount--;
    LOGGER_WRITE_1( "There are still %d sessions", iSessionCount );
    if ( iSessionCount < 1 )
        {
        Cancel();
        CActiveScheduler::Stop();   
        }
    TRACE_FUNC_EXIT;
    } 

// -----------------------------------------------------------------------------
// CSConPCConnSession::CSConPCConnSession()
// Default constructor 
// -----------------------------------------------------------------------------
//
CSConPCConnSession::CSConPCConnSession (
     CSConPCConnServer& aServer ) : iServer (aServer)
    {
    TRACE_FUNC_ENTRY;
    iServer.IncSessionCount();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::CSConPCConnSession()
// Default destructor - frees resources
// -----------------------------------------------------------------------------
//
CSConPCConnSession::~CSConPCConnSession()
    {
    TRACE_FUNC_ENTRY;
    
    iServer.DecSessionCount();

    if ( iConMLHandler )
        {
        delete iConMLHandler;
        iConMLHandler = NULL;
        iConMLHandlerLib.Close();
        }

    if ( iPCDHandler )
        {
        delete iPCDHandler;
        iPCDHandler = NULL;
        iPCDlib.Close();
        }

    if ( iCSCHandler )
        {
        delete iCSCHandler;
        iCSCHandler = NULL;
        iCSClib.Close();
        }
    
    if ( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }
        
    iChunk.Close();
    delete iFolderLister;
    
    iFs.Close();
    
    delete iBtEngine;
     
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::NewL()
// Creates a new instance of CSConPCConnSession
// -----------------------------------------------------------------------------
//
EXPORT_C CSConPCConnSession *CSConPCConnSession::NewL(
    CSConPCConnServer& aServer)
    {
    TRACE_FUNC_ENTRY;
    CSConPCConnSession* self = new ( ELeave ) CSConPCConnSession(aServer);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    iResult = KErrNone;
    
    User::LeaveIfError( iFs.Connect() );
    
#ifdef DEBUG_XML
    // create log file 
    
    RFile file;
    TInt err = file.Create ( iFs, KSConConMLDebugFile, EFileWrite );
    if( err == KErrNone )
        {
        // file created, close it
        file.Close();
        }              
#endif
    
    // initialize buffer
    iBuffer = CBufFlat::NewL ( KSConPCConnBufferMaxSize );
    
    // load handlers
    if ( !iPCDHandler )
        {
        TRAP( ret, LoadPCDDllL() );
        if ( ret != KErrNone) 
            {
            LOGGER_WRITE_1( "CSConPCConnSession::ConstructL() : PCD load failed with error code %d", ret );
            User::Leave( ret );
            }
        }   
    
    if ( !iCSCHandler )
        {
        TRAP( ret, LoadCSCDllL() );
        if ( ret != KErrNone) 
            {
            LOGGER_WRITE_1( "CSConPCConnSession::ConstructL() : CSC dll load failed with error code %d", ret ); 
            User::Leave ( ret );
            }
        }

    // Load parser
    if ( !iConMLHandler )
        {
        TRAPD( ret, LoadConMLHandlerDllL() );
        if ( ret != KErrNone)
            {
            LOGGER_WRITE_1( "CSConPCConnSession::ConstructL() : Parser dll load failed with error code %d", ret );
            User::Leave ( ret );
            }
        }
    
    iFolderLister = CSconFolderLister::NewL( iFs );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::ServiceL()
// Gets the client's request
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::ServiceL( const RMessage2 &aMessage)
    {
    TRACE_FUNC_ENTRY;
    TRAPD(err,DispatchRequestL(aMessage) );
    if ( err != KErrNone )
        {
        LOGGER_WRITE_1( "CSConPCConnSession::ServiceL() : leave code %d", err );
        PanicServer( E_DispatchRequest );
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::DispatchRequestL()
// Identifies an IPC command from the client 
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::DispatchRequestL(const RMessage2 &aMessage)
    {
    TRACE_FUNC_ENTRY;
    
    TInt ret (KErrNone);
        
    switch (aMessage.Function())
        {
        case EPutMessage:
            LOGGER_WRITE( "CSConPCConnSession::DispatchRequestL() : EPutMessage" );
            ret = HandlePutMessageL();
            break;
        
        case EGetMessage:
            LOGGER_WRITE( "CSConPCConnSession::DispatchRequestL() : EGetMessage" );
            ret = HandleGetMessageL();
            break;
        
        case EResetMessage:
            LOGGER_WRITE( "CSConPCConnSession::DispatchRequestL() : EResetMessage" );
            ret = HandleResetMessage();
            break;
            
        case EChunkMessage:
            LOGGER_WRITE( "CSConPCConnSession::DispatchRequestL() : EChunkMessage" );
            ret = HandleChunkMessage( aMessage );
            break;
                        
        default:
            PanicServer (E_BadRequest);
            break;
        }

    aMessage.Complete( ret );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::HandlePutMessageL()
// Handles a PUT -type message from the client
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::HandlePutMessageL()
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "CSConPCConnSession::HandlePutMessageL() : begin : Heap count : %d", User::Heap().Count() );
    TInt ret ( KErrNone );
        
    if ( !iPCDHandler )
        {
        TRAP( ret, LoadPCDDllL() );
        if ( ret != KErrNone) 
            {
            LOGGER_WRITE_1( "CSConPCConnSession::HandlePutMessageL (): PCD dll load failed with error code %d", ret );    
            return ( ret );
            }
        }   
        
    if ( ! iConMLHandler )
        {
        TRAP ( ret, LoadConMLHandlerDllL() );
        if ( ret != KErrNone )
            {
            LOGGER_WRITE_1( "CSConPCConnSession::HandlePutMessageL (): ConML Handler dll load failed with error code %d", ret );  
            return ( ret );
            }
        }
        
    TInt length ( 0 );
    
    RMemReadStream buf( iChunk.Base(), iChunk.Size() );


    iBuffer->Reset();

    length = buf.ReadInt32L();
    HBufC8* name8 = HBufC8::NewLC( length );
    TPtr8 namePtr8 = name8->Des();
    buf.ReadL( namePtr8, length);
    
    const TUint8* ptr8 = namePtr8.Ptr();
    const TUint16* ptr16 = reinterpret_cast<const TUint16*>( ptr8 );
    TPtrC namePtr;
    namePtr.Set( ptr16, length/2 );
        
    length = buf.ReadInt32L();
    HBufC8* type = HBufC8::NewLC( length );
    TPtr8 typePtr = type->Des();
    buf.ReadL( typePtr, length);
   
    // WBXML Document
    length = buf.ReadInt32L();
    HBufC8* data = HBufC8::NewLC( length );
    TPtr8 dataPtr = data->Des();
    
    buf.ReadL( dataPtr, length );
    iBuffer->ResizeL( length );
    iBuffer->Write( 0, dataPtr );
            
    buf.Close();
    
#ifdef DEBUG_XML

    RFile file;
    if ( file.Open( iFs, KSConConMLDebugFile, EFileWrite ) == KErrNone )
        {
        RFileWriteStream fws;
        TInt fileSize;
        file.Size( fileSize );
        
        TTime now;
        now.HomeTime();
        TDateTime time = now.DateTime();
        TBuf8<16> timeLine;
        timeLine.Format (KTimeFormat, time.Hour(), time.Minute(), 
                                    time.Second(), time.MicroSecond() );
        
        fws.Attach( file, fileSize );
        fws.PushL();
        fws.WriteL( timeLine );
        _LIT8( KPutMessage, "__________PUT-MESSAGE \nWBXML:\n" );
        fws.WriteL( KPutMessage );
        fws.WriteL( iBuffer->Ptr(0), iBuffer->Size() );
        TRAP_IGNORE( fws.CommitL() );
        fws.Close();
 
        CleanupStack::PopAndDestroy( &fws );
        }
    file.Close();
#endif
    if ( ( ( typePtr.CompareC( KSConPCDWBXMLObjectType)== KErrNone ) || 
                 ( typePtr.CompareC( KSConPCDWBXMLObjectType2 )== KErrNone ) )
                 && namePtr8.Find(KSCONSyncRequest) == 0 )
        {
        LOGGER_WRITE_1("Sync request: %S", &namePtr);
        RBufReadStream stream( *iBuffer );
        
        CleanupClosePushL( stream );
        TRAP( ret, iPCDHandler->HandlePutSyncRequestL( namePtr8, stream ));
        if ( ret )
            {
            LOGGER_WRITE_1("HandlePutSyncRequestL() Leaved with %d", ret);
            }
        
        CleanupStack::PopAndDestroy( &stream );
        }
    else if ( ( typePtr.Compare( KSConPCDWBXMLObjectType ) == KErrNone) || 
         ( typePtr.Compare( KSConPCDWBXMLObjectType2 )== KErrNone) )
        {
        LOGGER_WRITE( "CSConPCConnSession::HandlePutMessageL() : Object type PCD " );
        TRAPD( err, ret = iConMLHandler->ParseDocumentL( *iBuffer, this ) );    
        if ( err != KErrNone )
            {
            ret = err;
            }
        if ( ret == KErrNone )
            {
            // Possible error code returned from PCCS 
            ret = iResult;
            }
        }
    else 
        {
        LOGGER_WRITE( "Object type not regognized " );
        ret = KErrNotSupported;
        }
    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( type );
    CleanupStack::PopAndDestroy( name8 );
    LOGGER_WRITE_1( "CSConPCConnSession::HandlePutMessageL() end : Heap count : %d", User::Heap().Count() );
    LOGGER_WRITE_1( "CSConPCConnSession::HandlePutMessageL() : returned %d", ret );    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::HandleGetMessageL()
// Handles a GET -type message from the client
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::HandleGetMessageL()
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    
    if ( !iCSCHandler )
        {
        TRAP( ret, LoadCSCDllL() );
        if ( ret != KErrNone) 
            {
            LOGGER_WRITE_1( "CSConPCConnSession::HandleGetMessageL() : CSC dll load failed with error code %d", ret ); 
            return ret;
            }
        }

    TInt32 length ( 0 );
    RMemReadStream readBuf( iChunk.Base(), iChunk.Size() );
    iBuffer->Reset();
    
    length = readBuf.ReadInt32L();
    HBufC8* name = HBufC8::NewLC( length );
    TPtr8 namePtr8 = name->Des();
    readBuf.ReadL( namePtr8, length);
    namePtr8.SetLength( length );
    LOGGER_WRITE8_1("namePtr: %S", &namePtr8);
    
    const TUint8* ptr8 = namePtr8.Ptr();
    const TUint16* ptr16 = reinterpret_cast<const TUint16*>( ptr8 );
    TPtrC namePtr;
    namePtr.Set( ptr16, length/2 );
    
    length = readBuf.ReadInt32L();
    HBufC8* type = HBufC8::NewLC( length );
    TPtr8 typePtr = type->Des();
    readBuf.ReadL( typePtr, length);
    
    readBuf.Close();
        
    if ( typePtr.Compare( KSConCapabilityObjectType ) == KErrNone )
        {
        ret = iCSCHandler->CapabilityObject( *iBuffer );
        
#ifdef DEBUG_XML
        
        RFile file;
        if ( file.Open(iFs, KSConConMLDebugFile, EFileWrite ) == KErrNone )
            {
            RFileWriteStream fws;
            TInt fileSize;
            file.Size ( fileSize );
            TTime now;
            now.HomeTime();
            TDateTime time = now.DateTime();
            TBuf8<16> timeLine;
            timeLine.Format( KTimeFormat, time.Hour(), time.Minute(), 
                                        time.Second(), time.MicroSecond() );

            fws.Attach ( file, fileSize);
            fws.PushL();
            fws.WriteL( timeLine );
            _LIT8( KGetMessage, "__________GET -MESSAGE - Capability Object \n " );
            fws.WriteL( KGetMessage );
            fws.WriteL( iBuffer->Ptr(0), iBuffer->Size() );
            TRAP_IGNORE( fws.CommitL() );
            fws.Close();

            CleanupStack::PopAndDestroy( &fws );
            }
        file.Close();
#endif

        }
    else if (  typePtr.CompareC( KSConPCDWBXMLObjectType) == KErrNone )
        {
        if ( namePtr8.Find(KSCONSyncRequest) == 0 ) // 8-bit search
            {
            LOGGER_WRITE("Sync request");
            
            RBufWriteStream stream( *iBuffer );
                
            CleanupClosePushL( stream );
            
            TRAP( ret, iPCDHandler->HandleGetSyncRequestL( namePtr8, stream, iChunk.MaxSize() - sizeof(TInt32) ));
            if ( ret )
                {
                LOGGER_WRITE_1("HandleGetSyncRequestL() Leaved with %d", ret);
                }
            stream.CommitL();
            CleanupStack::PopAndDestroy( &stream );
            
            LOGGER_WRITE_1("iBuffer size: %d", iBuffer->Size());
            
            }
        else if ( namePtr.Find(KSCONReadImsi) == 0 ) // 16-bit search
            {
            LOGGER_WRITE("Read IMSI");
            TBuf<CTelephony::KIMSISize> imsi;
            ret = CSconImsiReader::GetImsiL( imsi );
            
            if ( ret == KErrNone )
                {
                TBuf8<CTelephony::KIMSISize> imsi8;
                User::LeaveIfError( CnvUtfConverter::ConvertFromUnicodeToUtf8(imsi8,imsi) );
                
                iBuffer->ResizeL( imsi8.Length() );
                iBuffer->Write( 0, imsi8 );
                }
            else
                {
                // Change error code more generic
                ret = KErrNotFound;
                }
            }
        else if ( namePtr.Find(KSCONReadNetworkInfo) == 0 ) // 16-bit search
            {
            LOGGER_WRITE("Read NetworkInfo");
            CTelephony::TRegistrationStatus regStatus;
            ret = CSconImsiReader::GetNetworkStatusL( regStatus );
            
            if ( ret == KErrNone )
                {
                RBufWriteStream stream( *iBuffer );
                CleanupClosePushL( stream );
                stream.WriteInt8L( regStatus );
                stream.CommitL();
                CleanupStack::PopAndDestroy( &stream );
                }
            else
                {
                // Change error code more generic
                ret = KErrNotFound;
                }
            }
        else if ( namePtr.Find(KSCONReadOviAccount) == 0 )
            {
            LOGGER_WRITE("Read Ovi account");
            RBufWriteStream stream( *iBuffer );
            CleanupClosePushL( stream );
            
            ret = ReadOviAccountInfoL( stream );
            stream.CommitL();
            CleanupStack::PopAndDestroy( &stream );
            
            }
        else if ( namePtr.Find(KSCONListPath) == 0 )
            {
            LOGGER_WRITE("List path");
            // LISTP:0:pathname
            if ( namePtr.Length() > KSCONListPath().Length()+2)
                {
                TPtrC pathname = namePtr.Mid( KSCONListPath().Length()+2 );
            
                const TUint16 levelsChar = namePtr[ KSCONListPath().Length() ];
                TInt level(KErrNotFound);
                if ( levelsChar >= TChar('0') )
                    {
                    level = levelsChar - TChar('0');
                    }
                
                RBufWriteStream stream( *iBuffer );
                CleanupClosePushL( stream );
                TRAP( ret, iFolderLister->GenerateFolderListL( stream, pathname, level ));
                LOGGER_WRITE_1("GenerateFolderListL leaved with err: %d", ret);
                stream.CommitL();
                CleanupStack::PopAndDestroy( &stream );
                }
            else
                {
                ret = KErrArgument;
                }
            
            }
        else if ( namePtr.Find(KSCONReadBtInfo) == 0 )
            {
            LOGGER_WRITE( "Read BT info");
            if ( !iBtEngine )
                {
                iBtEngine = CSconBtEngine::NewL();
                }
            RBufWriteStream stream( *iBuffer );
            CleanupClosePushL( stream );
            TRAP( ret, iBtEngine->ReadBTInfoL( stream ) );
            LOGGER_WRITE_1("ReadBTInfoL leaved with err: %d", ret);
            stream.CommitL();
            CleanupStack::PopAndDestroy( &stream );
            }
        else if ( namePtr.Find(KSCONSetBtPower) == 0 )
            {
            LOGGER_WRITE( "Change BT Power state");
            if ( namePtr.Length() == KSCONSetBtPower().Length()+1 )
                {
                const TUint16 lastChar = namePtr[ KSCONSetBtPower().Length() ];
                TBool changeBtOn;
                if ( lastChar == TChar('0') )
                    {
                    changeBtOn = EFalse;
                    }
                else if ( lastChar == TChar('1') )
                    {
                    changeBtOn = ETrue;
                    }
                else
                    {
                    ret = KErrArgument;
                    }
                
                if ( !ret )
                    {
                    if ( !iBtEngine )
                       {
                       iBtEngine = CSconBtEngine::NewL();
                       }
                    TInt err = iBtEngine->SetBtPowerState( changeBtOn );
                    
                    RBufWriteStream stream( *iBuffer );
                    CleanupClosePushL( stream );
                    stream.WriteInt32L( err );
                    stream.CommitL();
                    CleanupStack::PopAndDestroy( &stream );
                    }
                }
            else
                {
                ret = KErrArgument;
                }
            }
        else if ( namePtr.Find(KSCONSetBtName) == 0 )
            {
            LOGGER_WRITE( "Set BT Name");
            TPtrC btName = namePtr.Mid( KSCONSetBtName().Length() );
            TInt err = iBtEngine->SetBtName( btName  );
            
            RBufWriteStream stream( *iBuffer );
            CleanupClosePushL( stream );
            stream.WriteInt32L( err );
            stream.CommitL();
            CleanupStack::PopAndDestroy( &stream );
            }
        else if ( namePtr.Find(KSCONSetBtAuthorized) == 0 )
            {
            LOGGER_WRITE( "Set BT Authorized");
            ret = KErrArgument;
            // SETBTAUTHORIZED:0:00245f8d6a26
            // 1. param = auth. state ("0"=off, "1"=true)
            // 2. param = bt address (hex string)
            
            if ( namePtr.Length() > KSCONSetBtAuthorized().Length()+2)
                {
                TPtrC btAddr = namePtr.Mid( KSCONSetBtAuthorized().Length()+2 );
                
                const TUint16 authStateChar = namePtr[ KSCONSetBtAuthorized().Length() ];
                TBool authorize(EFalse);
                if ( authStateChar == TChar('0') )
                    {
                    authorize = EFalse;
                    ret = KErrNone;
                    }
                else if ( authStateChar == TChar('1') )
                    {
                    authorize = ETrue;
                    ret = KErrNone;
                    }
                
                if ( !iBtEngine )
                   {
                   iBtEngine = CSconBtEngine::NewL();
                   }
                TRAPD(err, iBtEngine->SetBtAuthorizedL( btAddr, authorize ) );
                
                RBufWriteStream stream( *iBuffer );
                CleanupClosePushL( stream );
                stream.WriteInt32L( err );
                stream.CommitL();
                CleanupStack::PopAndDestroy( &stream );
                }
            
            }
        else 
            {
            ret = HandleWBXMLGetRequestL( namePtr );
            }
        }
    else
        {
        LOGGER_WRITE( "CSConPCConnSession::HandleGetMessageL() : Header type not regognized " );
        ret = KErrNotSupported;
        }
    
    CleanupStack::PopAndDestroy( 2 ); // name, type
    
    if ( ret != KErrNone )
        {
        return ret;
        }
    
    length = iBuffer->Size();
    
    if ( sizeof(TInt32) + length > iChunk.Size() )
        {
        // need to resize chunk
        TInt err = iChunk.Adjust( sizeof(TInt32) + length );
        LOGGER_WRITE_2("iChunk.Adjust( %d ) err: %d", sizeof(TInt32) + length, err);
        if ( err )
            {
            iBuffer->Reset();
            LOGGER_WRITE_1( "CSConPCConnSession::HandleGetMessageL() : returned %d", ret );
            return err;
            }
        }
    
    // copy data to chunk
    RMemWriteStream writeBuf ( iChunk.Base(), iChunk.Size() );
    
    if ( length > 0 )
        {
        writeBuf.WriteInt32L( length );
        writeBuf.WriteL( iBuffer->Ptr(0), length );
        writeBuf.CommitL();
        }
    else 
        {
        writeBuf.WriteInt32L( 0 );
        }
    writeBuf.CommitL();
    writeBuf.Close();
    iBuffer->Reset();
    LOGGER_WRITE_1( "CSConPCConnSession::HandleGetMessageL() : returned %d", ret );
    return ( ret );
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::HandleWBXMLGetRequestL()
// Handles a ConML(wbxml) Get request from the client
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::HandleWBXMLGetRequestL( const TDesC& aFileName )
    {
    TRACE_FUNC_ENTRY;
    TInt ret(KErrNone);
    if ( aFileName.Find(KSCONGetMetadataRequest) == 0 )
        {
        // ConML get metadata request --> Create metadata task
        LOGGER_WRITE( "ConML GetMetadataRequest" );
        TPtrC filename = aFileName.Mid( KSCONGetMetadataRequest().Length() );
        
        TSConMethodName method ( EGetMetadata );
        CSConTask* task = CSConTask::NewLC( method );
        
        if ( filename.Length() > task->iGetMetadataParams->iFilename.MaxLength() )
            {
            User::Leave( KErrTooBig );
            }
        task->iGetMetadataParams->iFilename = filename;
        
        ret = iPCDHandler->PutTaskL( task );
        CleanupStack::Pop( task );
        LOGGER_WRITE_1("iPCDHandler->PutTaskL ret: %d", ret);
        }
    else if ( aFileName.Length() > 0 )
        {
        LOGGER_WRITE("Error: aFilename does not match to any definitions");
        TRACE_FUNC_RET( KErrArgument );
        return KErrArgument;
        }
    
	// Get reply
    LOGGER_WRITE( "CSConPCConnSession::HandleGetMessageL() before ConML GetReplyL" );
    CSConStatusReply* reply = iPCDHandler->GetReply();
    CleanupStack::PushL( reply );
    
    ConML_ConMLPtr_t content = new ( ELeave ) ConML_ConML_t();
    CleanupStack::PushL( content );
    
    AppendStatusL( content, reply );
    ret = iConMLHandler->GenerateDocument( content );
    
    CleanupStack::PopAndDestroy( content );
    CleanupStack::PopAndDestroy( reply );
     
    TPtrC8 ptr( iConMLHandler->WBXMLDocument() );
    LOGGER_WRITE_1("ptr.Size(): %d", ptr.Size());
    iBuffer->ResizeL( ptr.Size() );
    iBuffer->Write( 0, ptr );

#ifdef DEBUG_XML
    RFile file;
    if ( file.Open( iFs, KSConConMLDebugFile, EFileWrite) == KErrNone )
        {
        RFileWriteStream fws;   
        TInt fileSize;
        file.Size ( fileSize );
        fws.Attach ( file, fileSize);
        fws.PushL();
        TTime now;
        now.HomeTime();
        TDateTime time = now.DateTime();
        TBuf8<16> timeLine;
        timeLine.Format( KTimeFormat, time.Hour(), time.Minute(), 
                         time.Second(), time.MicroSecond() );

        fws.WriteL( timeLine );
        _LIT8( KGetMessage, "__________GET -MESSAGE" );
        fws.WriteL( KGetMessage );
        fws.WriteL(KXmlBegin);
        fws.WriteL(iConMLHandler->XMLDocument().Ptr(), 
                          iConMLHandler->XMLDocument().Length());
        fws.WriteL(_L("\n\n"));
        TRAP_IGNORE( fws.CommitL() );
        fws.Close();
        CleanupStack::PopAndDestroy( &fws );
        }
        
    file.Close();
#endif
    TRACE_FUNC_RET(ret);
    return ret;
    }


// -----------------------------------------------------------------------------
// CSConPCConnSession::ReadOviAccountInfoL()
// Reads ovi account information to stream
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::ReadOviAccountInfoL( RWriteStream& aAccountInfoStream )
    {
	TRACE_FUNC_ENTRY;
    RLibrary oviaccounthandlerLib;
    CleanupClosePushL( oviaccounthandlerLib );
    // Dynamically load DLL
    TInt err = oviaccounthandlerLib.Load( KSconOviAccountHandlerDllName );
    if ( err )
        {
        LOGGER_WRITE_1("oviaccounthandlerLib.Load err: %d", err);
        err = KErrNotSupported;
        }
    else if( oviaccounthandlerLib.Type()[1] != KSconOviAccountHandlerDllUid )
        {
        LOGGER_WRITE_1( "KSconOviAccountHandlerDllUid incorrect... (0x%08X)",oviaccounthandlerLib.Type()[1].iUid  );
        err = KErrNotSupported;
        }
    
    if ( err == KErrNone )
        {
        TSConCreateCSconOviAccountHandlerFunc CreateCSconOviAccountHandlerL =
        (TSConCreateCSconOviAccountHandlerFunc)oviaccounthandlerLib.Lookup(1);
        
        CSconOviAccountHandler* oviAccountHandler = (CSconOviAccountHandler*)CreateCSconOviAccountHandlerL();
        
        TRAP(err, oviAccountHandler->GetOviAccountDetailsL( aAccountInfoStream ) );
        LOGGER_WRITE_1("GetOviAccountDetailsL err: %d", err);
        
        delete oviAccountHandler;
        oviAccountHandler = NULL;
        
        if ( err )
            {
            // change error code
            err = KErrNotFound;
            }
        }
    else
        {
        err = KErrNotSupported;
        }
	
    CleanupStack::PopAndDestroy( &oviaccounthandlerLib );
	TRACE_FUNC_EXIT;
    return err;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::HandleResetMessage()
// Resets the PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::HandleResetMessage()   
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    
    // reset PCD
    if ( iPCDHandler )
        {
        iPCDHandler->ResetPCD();
        }
        
    LOGGER_WRITE_1( "CSConPCConnSession::HandleResetMessage() : ret %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::HandleChunkMessage( const RMessage2& aMessage )
// Receives the chunk handle from the client
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::HandleChunkMessage( const RMessage2& aMessage )    
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    
    ret = iChunk.Open( aMessage, 0, EFalse );
        
    LOGGER_WRITE_1( "CSConPCConnSession::HandleChunkMessageL() : ret %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::LoadPCDDllL()
// Loads the PCCS service
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::LoadPCDDllL()
    {
    TRACE_FUNC_ENTRY;
    
    // Dynamically load DLL
    User::LeaveIfError( iPCDlib.Load( KSConPCDLibName ) );
    if( iPCDlib.Type()[1] != KSConPCDUid )
        {
        LOGGER_WRITE( "CSConPCConnSession::LoadPCDDllL() : KSConPCDUidValue incorrect..." );
        iPCDlib.Close();
        User::Leave( KErrNotFound );
        }   
    TSConCreateCSConPCDFunc CreateCSConPCDL =
    (TSConCreateCSConPCDFunc)iPCDlib.Lookup(1);
    
    iPCDHandler = (CSConPCD*)CreateCSConPCDL();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::LoadCSCDllL()
// Loads the CSC service
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::LoadCSCDllL()
    {
    TRACE_FUNC_ENTRY;
    // Dynamically load DLL
    User::LeaveIfError( iCSClib.Load( KSConCSCLibName ) );
    if( iCSClib.Type()[1] != KSConCSCUid )
        {
        LOGGER_WRITE( "CSConPCConnSession::LoadCSCDllL() : KSConCSCUidValue incorrect" );
        iCSClib.Close();
        User::Leave( KErrNotFound );
        }   
    TSConCreateCSConCSCFunc CreateCSConCSCL = 
    (TSConCreateCSConCSCFunc)iCSClib.Lookup(1);
    
    iCSCHandler = (CSConCSC*)CreateCSConCSCL();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::LoadConMLHandlerDllL()
// Loads the ConML handler
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::LoadConMLHandlerDllL()
    {
    TRACE_FUNC_ENTRY;
    // Dynamically load DLL
    User::LeaveIfError( iConMLHandlerLib.Load( KSConConMLHandlerLibName ) );
    if ( iConMLHandlerLib.Type()[1] != KSConConMLHandlerUid )
        {
        LOGGER_WRITE( "CSConPCConnSession::LoadConMLHandlerDllL() : KSConConMLHandlerUidValue incorrect" );
        iConMLHandlerLib.Close();
        User::Leave( KErrNotFound );
        }
    TSConCreateCSConConMLHandlerFunc CreateCSConConMLHandlerL = 
    (TSConCreateCSConConMLHandlerFunc)iConMLHandlerLib.Lookup(1);
    
    iConMLHandler = (CSConConMLHandler*)CreateCSConConMLHandlerL();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::ConMLL()
// Callback function for ConML handler - parsed data processing starts
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::ConMLL( ConML_ConMLPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    
#ifdef DEBUG_XML
    iConMLHandler->GenerateDocument( aContent );
    RFile file;
    if ( file.Open(iFs, KSConConMLDebugFile, EFileWrite) == KErrNone )
        {
        RFileWriteStream fws;
        TInt fileSize;
        file.Size( fileSize );
        fws.Attach ( file, fileSize );
        fws.PushL();
        
        TTime now;
        now.HomeTime();
        TDateTime time = now.DateTime();
        TBuf8<16> timeLine;
        timeLine.Format (KTimeFormat, time.Hour(), time.Minute(), 
                                    time.Second(), time.MicroSecond() );
        
        fws.WriteL( timeLine );
        _LIT8( KPutMessage, "__________PUT-MESSAGE" );
        fws.WriteL( KPutMessage );
        fws.WriteL( KXmlBegin );
        fws.WriteL( iConMLHandler->XMLDocument().Ptr(), 
                    iConMLHandler->XMLDocument().Length());
        fws.WriteL(_L("\n\n"));
        TRAP_IGNORE( fws.CommitL() );
        fws.Close();
        CleanupStack::PopAndDestroy( &fws );
        }
    file.Close();
    
#endif


    if ( aContent )
        {
        if ( aContent->execute )
            {
            ret = OptionsFromExecuteL( aContent->execute );
            }
        else if ( aContent->cancel )
            {
            ret = TaskCancelL( aContent->cancel );
            }
        else if ( aContent->getStatus )
            {
            ret = TaskGetStatusL ( aContent->getStatus );
            }
        else 
            {
            LOGGER_WRITE( "CSConPCConnSession::ConML() : No appropriate content in ConML -element " );
            ret = KErrArgument;
            }
        }
    else 
        {
        ret  = KErrArgument;
        }
    
    // store result for later use
    iResult = ret;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::OptionsFromExecuteL()
// Handles data of an execute -element
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::OptionsFromExecuteL(ConML_ExecutePtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    
    TInt ret ( KErrNone );
    if ( aContent->updateDeviceInfo )
        {
        ret = TaskUpdateDeviceInfoL( aContent->updateDeviceInfo);
        }
    else if ( aContent->listInstalledApps )
        {
        ret = TaskListInstalledAppsL( aContent->listInstalledApps);
        }
    else if ( aContent->install )
        {
        ret = TaskInstallL( aContent->install );
        }       
    else if ( aContent->unInstall )
        {
        ret = TaskUnInstallL( aContent->unInstall );
        }
    else if ( aContent->listDataOwners )
        {
        ret = TaskListDataOwnersL();
        }
    else if ( aContent->setBurMode )    
        {
        ret = TaskSetBURModeL( aContent->setBurMode );
        }
    else if ( aContent->getDataSize )
        {
        ret = TaskGetDataSizeL( aContent->getDataSize );
        }
    else if ( aContent->requestData )
        {
        ret = TaskRequestDataL( aContent->requestData );
        }
    else if ( aContent->listPublicFiles )
        {
        ret = TaskListPublicFilesL( aContent->listPublicFiles );
        }
    else if ( aContent->reboot )
        {
        ret = TaskRebootL();
        }
    else if ( aContent->getDataOwnerStatus )
        {
        ret = TaskGetDataOwnerStatusL( aContent->getDataOwnerStatus );      
        }
    else if ( aContent->supplyData )
        {
        ret = TaskSupplyDataL( aContent->supplyData );
        }
    else if ( aContent->getMetadata )
        {
        ret = TaskGetMetadataL( aContent->getMetadata );
        }
    else 
        {
        LOGGER_WRITE( "CSConPCConnSession::OptionsFromExecute() : No content " );
        ret = KErrNotSupported;
        }
    LOGGER_WRITE_1( "CSConPCConnSession::OptionsFromExecute() : returned %d", ret );    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskCancelL(ConML_CancelPtr_t aContent)
// Sends a Cancel -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskCancelL( ConML_CancelPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( ECancel );
    CSConTask* task = CSConTask::NewLC( method );
    if ( aContent->all )
        {
        task->iCancelTaskAll = ETrue;
        }
    if ( aContent->id )
        {
        task->iCancelTaskId = ( DesToInt( aContent->id->Data() ));
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskCancelL() : returned %d", ret );    
    return ret;
    }


// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskGetStatusL()
// Sends a Get Status -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskGetStatusL( ConML_GetStatusPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EGetStatus );
    CSConTask* task = CSConTask::NewLC( method );
    if ( aContent->all )
        {
        task->iGetStatusParams->iAll = ETrue;
        }
    if ( aContent->id)
        {
        task->iGetStatusParams->iTaskId = ( DesToInt( aContent->id->Data() ));
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskGetStatusL() : returned %d", ret );    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskInstallL()
// Sends an Install -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskInstallL( ConML_InstallPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EInstall );
    CSConTask* task = CSConTask::NewLC( method);
    task->iInstallParams->iMode = EUnknown;
    if ( aContent->name )
        {
        // Unicode conversion from 8-bit to 16-bit
        CnvUtfConverter::ConvertToUnicodeFromUtf8(task->iInstallParams->iPath, 
                                                    aContent->name->Data());    
        }
    if ( aContent->instParams )
        {
        if ( aContent->instParams->param )
            {
            for ( ConML_ParamListPtr_t p = aContent->instParams->param; 
                  p && p->data; p = p->next)
                {
                LOGGER_WRITE( "CSConPCConnSession::TaskInstallL() : Parameters found " );
                if ( p->data->name )
                    {
                    LOGGER_WRITE( "CSConPCConnSession::TaskInstallL() : name param found " );
                    TPtrC8 silent(KParamNameSilent);
                    
                    TInt comp = Mem::Compare((TUint8*)p->data->name->content, (TInt)p->data->name->length, silent.Ptr(), silent.Length());
                    if( comp == 0)
                        {
                        // "Silent"-param found
                        LOGGER_WRITE( "CSConPCConnSession::TaskInstallL() : Silent-param found " );
                        if ( p->data->value )
                            {
                            TPtrC8 dataValue((TUint8*)p->data->value->content, (TInt)p->data->value->length);
                            TInt value = DesToInt( dataValue );
                            if (value == 1)
                                {
                                LOGGER_WRITE( "CSConPCConnSession::TaskInstallL() : ESilentInstall " );
                                task->iInstallParams->iMode = ESilentInstall;
                                }
                            else if( value == 0 )
                                {
                                LOGGER_WRITE( "CSConPCConnSession::TaskInstallL() : EUnsilentInstall " );
                                task->iInstallParams->iMode = EUnsilentInstall;
                                }
                            }
                        }
                    }
                }
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskInstallL() : returned %d", ret );    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskUnInstallL()
// Sends an Uninstall -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskUnInstallL( ConML_UnInstallPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EUninstall );
    CSConTask* task = CSConTask::NewLC( method );
    task->iUninstallParams->iMode = EUnknown;
    
    if ( aContent->applications->application)       
        {
        for ( ConML_ApplicationListPtr_t p = 
              aContent->applications->application; p && p->data; p = p->next )
            {
            if ( p->data )
                {
                if ( p->data->name )
                    {
                    // get application name
                    HBufC* nameBuf = CnvUtfConverter::ConvertToUnicodeFromUtf8L( p->data->name->Data() );
                    task->iUninstallParams->iName.Copy( nameBuf->Des() );
                    delete nameBuf;
                    nameBuf = NULL;
                    }
                if ( p->data->uid )
                    {
                    // parse uid:  UID # Type # Size # Version # Vendor # Parent app. name #
                    // only UID and Vendor are needed from whole uid-field.
                    LOGGER_WRITE( "CSConPCConnSession::TaskUnInstallL() : start parsing uid " );
                    
                    TPtrC8 buf( p->data->uid->Data() );
                    
                    RArray<TPtrC8> arr(6);
                    CleanupClosePushL( arr );
                    TBuf8<1> separator(KSConAppInfoSeparator);
                    
                    SplitL(buf, separator[0], arr);
                    if ( arr.Count() >= 5 )
                    	{
                    	task->iUninstallParams->iUid = DesToUid( arr[0] );
                    	task->iUninstallParams->iType = (TSConAppType)DesToInt( arr[1] );
                    	HBufC* vendorBuf = CnvUtfConverter::ConvertToUnicodeFromUtf8L( arr[4] );
                    	task->iUninstallParams->iVendor.Copy( vendorBuf->Des() );
                    	delete vendorBuf;
                    	vendorBuf = NULL;
                    	}
                    CleanupStack::PopAndDestroy( &arr );
                    
                        
                    } // endif p->data->uid
                }
            }
        }
    
    if ( aContent->instParams)
        {
        if ( aContent->instParams->param )
            {
            for ( ConML_ParamListPtr_t p = aContent->instParams->param; 
                    p && p->data; p = p->next )
                {
                LOGGER_WRITE( "CSConPCConnSession::TaskUnInstallL() : Parameters found " );
                if ( p->data->name )
                    {
                    LOGGER_WRITE( "CSConPCConnSession::TaskUnInstallL() : name param found " );
                    TPtrC8 silent(KParamNameSilent);
                    
                    TInt comp = Mem::Compare((TUint8*)p->data->name->content, (TInt)p->data->name->length, silent.Ptr(), silent.Length());
                    if( comp == 0)
                        {
                        // "Silent"-param found
                        LOGGER_WRITE( "CSConPCConnSession::TaskUnInstallL() : Silent-param found " );
                        if ( p->data->value )
                            {
                            TPtrC8 dataValue((TUint8*)p->data->value->content, (TInt)p->data->value->length);
                            TInt value = DesToInt( dataValue );
                            if ( value == 1 )
                                {
                                LOGGER_WRITE( "CSConPCConnSession::TaskUnInstallL() : ESilentInstall " );
                                task->iUninstallParams->iMode = ESilentInstall;
                                }
                            else if ( value == 0 )
                                {
                                LOGGER_WRITE( "CSConPCConnSession::TaskUnInstallL() : EUnsilentInstall " );
                                task->iUninstallParams->iMode = EUnsilentInstall;
                                }
                            }
                        }
                    }
                }
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskUnInstallL() : returned %d", ret );    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::SplitL(const TDesC& aText, const TChar aSeparator, 
// RArray<TPtrC>& aArray)
// Function splits string (eg "name1, name2, name3") into substrings.
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::SplitL(const TDesC8& aText, const TChar aSeparator, 
                    RArray<TPtrC8>& aArray)
    {
    TRACE_FUNC_ENTRY;
    TPtrC8 ptr;
    ptr.Set(aText);

    for (;;)
        {
        TInt pos=ptr.Locate(aSeparator);
        if (pos==KErrNotFound)
            {
            aArray.AppendL(ptr);
            break;
            }

        TPtrC8 subStr=ptr.Left(pos); // get pos characters starting from position 0
        aArray.AppendL(subStr);

        if (!(ptr.Length()>pos+1))
            {
            break;
            }
            
        ptr.Set(ptr.Mid(pos+1));// get all characters starting from position pos+1
        }
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskListInstalledAppsL()
// Sends a List installed apps -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskListInstalledAppsL(
    ConML_ListInstalledAppsPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EListInstalledApps );
    CSConTask* task = CSConTask::NewLC( method );
    if( aContent-> drives )
        {
        task->iListAppsParams->iDriveList = DriveList( aContent->drives->drive);
        }
    if ( aContent->all )
        {
        task->iListAppsParams->iAllApps = ETrue;
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskListInstalledAppsL() : returned %d", ret);
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskListDataOwnersL()
// Sends a List data owners -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskListDataOwnersL()
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EListDataOwners );
    CSConTask* task = CSConTask::NewLC( method );
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskListDataOwnersL() : returned %d", ret );    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskSetBURModeL()
// Sends a Set BUR mode -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskSetBURModeL(ConML_SetBURModePtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( ESetBURMode );
    CSConTask* task = CSConTask::NewLC( method );
    if ( aContent->drives )
        {
        task->iBURModeParams->iDriveList = DriveList( aContent->drives->drive);
        }
    if ( aContent->partialType )
        {
        TInt intValue = DesToInt( aContent->partialType->Data() );
        task->iBURModeParams->iPartialType = static_cast<TSConBurMode> (intValue) ;
        }
    if ( aContent->incType )
        {
        TInt intValue = DesToInt( aContent->incType->Data() );
        task->iBURModeParams->iIncType = static_cast<TSConIncType> (intValue) ;
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskSetBURModeL() : returned %d", ret );    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskGetDataSizeL()
// Sends a Get data size -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskGetDataSizeL(ConML_GetDataSizePtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EGetDataSize );
    CSConTask* task = CSConTask::NewLC( method );
    if ( aContent->dataOwners )
        {
        for ( ConML_SIDListPtr_t p = aContent->dataOwners->sid; 
                p && p->data; p=p->next )
            {
            CSConDataOwner* dataOwner = new (ELeave) CSConDataOwner();
            CleanupStack::PushL( dataOwner );
            if ( p->data->type )
                {
                dataOwner->iType = TSConDOType (DesToInt( 
                    p->data->type->Data() ));
                }
            if (p->data->uid )
                {
                if( !IsJavaHash( p->data->uid->Data() ) )
                    {
                    dataOwner->iUid = DesToUid( p->data->uid->Data() );
                    }
                else
                    {
                    TPtr hashPtr = DesToHashLC( p->data->uid->Data() );
                    dataOwner->iJavaHash = HBufC::NewL( hashPtr.Size() );
                    dataOwner->iJavaHash->Des().Copy( hashPtr );
                    CleanupStack::PopAndDestroy(); //DesToHashLC()
                    }
                }
            if ( p->data->drives )
                {
                dataOwner->iDriveList = DriveList ( p->data->drives->drive );
                }
            if ( p->data->transferDataType )
                {
                TInt intValue = DesToInt( p->data->transferDataType->Data() );
                dataOwner->iTransDataType = static_cast<TSConTransferDataType> (intValue);
                }
            task->iGetDataSizeParams->iDataOwners.AppendL( dataOwner );
            CleanupStack::Pop( dataOwner );
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskGetDataSizeL() : returned %d", ret );    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskRequestDataL()
// Sends a Request data -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskRequestDataL(ConML_RequestDataPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( ERequestData );
    CSConTask* task = CSConTask::NewLC( method );
    if ( aContent )
        {
        if ( aContent->sid )
            {
            for ( ConML_SIDListPtr_t p=aContent->sid; p && p->data; p=p->next )
                {
                if ( p->data->type )
                    {
                    task->iRequestDataParams->iDataOwner->iType = 
                    TSConDOType ( DesToInt ( p->data->type->Data() ) );
                    }
                if ( p->data->uid )
                    {
                    if( !IsJavaHash( p->data->uid->Data() ) )
                        {
                        task->iRequestDataParams->iDataOwner->iUid = DesToUid(
                                                     p->data->uid->Data() );
                        }
                    else
                        {
                        TPtr hashPtr = DesToHashLC( p->data->uid->Data() );
                        task->iRequestDataParams->iDataOwner->iJavaHash = HBufC::NewL( hashPtr.Size() );
                        task->iRequestDataParams->iDataOwner->iJavaHash->Des().Copy( hashPtr );
                        CleanupStack::PopAndDestroy(); //DesToHashLC()
                        }                                 
                    }
                if ( p->data->drives )
                    {
                    task->iRequestDataParams->iDataOwner->iDriveList = 
                    DriveList ( p->data->drives->drive );
                    }
                if ( p->data->transferDataType )
                    {
                    TInt intValue = DesToInt( p->data->transferDataType->Data() );
                    task->iRequestDataParams->iDataOwner->iTransDataType =
                        static_cast<TSConTransferDataType> (intValue);
                    }
                }
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskRequestDataL() : returned %d", ret );    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskUpdateDeviceInfoL()
// Sends a Update device info -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskUpdateDeviceInfoL(
    ConML_UpdateDeviceInfoPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EUpdateDeviceInfo );
    CSConTask* task = CSConTask::NewLC( method );

    if ( aContent->deviceInfo)
        {
        ConML_DeviceInfoPtr_t dPtr = aContent->deviceInfo;
        if ( dPtr->version )
            {
            task->iDevInfoParams->iVersion.Copy( dPtr->version->Data());
            }
        if ( dPtr->maxObjectSize )
            {
            task->iDevInfoParams->iMaxObjectSize = DesToInt(
                                            dPtr->maxObjectSize->Data());
            }
        if ( dPtr->supportedMethods )
            {
            ConML_SupportedMethodsPtr_t smPtr = dPtr->supportedMethods;
            if ( smPtr->install )
                {
                task->iDevInfoParams->iInstallSupp = ETrue; 
                }
            if ( smPtr->unInstall )
                {
                task->iDevInfoParams->iUninstallSupp = ETrue;
                }
            if ( smPtr->listInstalledApps )
                {
                task->iDevInfoParams->iInstAppsSupp = ETrue;
                }
            if ( smPtr->listDataOwners )
                {
                task->iDevInfoParams->iDataOwnersSupp = ETrue;
                }
            if ( smPtr->setBurMode )
                {
                task->iDevInfoParams->iSetBURModeSupp = ETrue;
                }
            if ( smPtr->getDataSize )
                {
                task->iDevInfoParams->iGetSizeSupp = ETrue;
                }
            if ( smPtr->requestData )
                {
                task->iDevInfoParams->iReqDataSupp = ETrue;
                }
            if ( smPtr->supplyData )
                {
                task->iDevInfoParams->iSupplyDataSupp = ETrue;  
                }
            if ( smPtr->reboot )
                {
                task->iDevInfoParams->iRebootSupp = ETrue;
                }
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskUpdateDeviceInfoL() : returned %d", ret );    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskListPublicFilesL()
// Sends a List public files -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskListPublicFilesL(
    ConML_ListPublicFilesPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EListPublicFiles );
    CSConTask* task = CSConTask::NewLC( method );
    if ( aContent->sid )
        {
        for ( ConML_SIDListPtr_t p = aContent->sid; p && p->data; p = p->next )
            {
            CSConDataOwner* dataOwner = new (ELeave) CSConDataOwner();
            CleanupStack::PushL( dataOwner );
            if ( p->data->type )                
                {
                dataOwner->iType = TSConDOType ( DesToInt( 
                                        p->data->type->Data() ) );
                }
            if ( p->data->uid )
                {
                if( !IsJavaHash( p->data->uid->Data() ) )
                    {
                    dataOwner->iUid = DesToUid( p->data->uid->Data() );
                    }
                else
                    {
                    TPtr hashPtr = DesToHashLC( p->data->uid->Data() );
                    dataOwner->iJavaHash = HBufC::NewL( hashPtr.Size() );
                    dataOwner->iJavaHash->Des().Copy( hashPtr );
                    CleanupStack::PopAndDestroy(); //DesToHashLC()
                    }
                }
            if ( p->data->drives )
                {
                dataOwner->iDriveList = DriveList ( p->data->drives->drive );
                }
            if ( p->data->packageInfo && p->data->packageInfo->name )
                {
                // Unicode conversion from 8-bit to 16-bit
                CnvUtfConverter::ConvertToUnicodeFromUtf8(
                        dataOwner->iPackageName, 
                        p->data->packageInfo->name->Data());
                }
            task->iPubFilesParams->iDataOwners.AppendL( dataOwner );
            CleanupStack::Pop( dataOwner );
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskListPublicFilesL() : returned %d", ret );    
    return ret;
    }
        
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskSupplyDataL()
// Sends a Supply data -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskSupplyDataL ( ConML_SupplyDataPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( ESupplyData );
    CSConTask* task = CSConTask::NewLC( method );
    if (aContent )
        {
        for ( ConML_SIDListPtr_t p = aContent->sid; p && p->data; p = p->next )
            {
            if ( p->data->type )
                {
                task->iSupplyDataParams->iDataOwner->iType = 
                TSConDOType ( DesToInt( p->data->type->Data() ) );
                }
            if ( p->data->uid )
                {
                if( !IsJavaHash( p->data->uid->Data() ) )
                    {
                    task->iSupplyDataParams->iDataOwner->iUid = DesToUid( 
                                                p->data->uid->Data() );
                    }
                else
                    {
                    TPtr hashPtr = DesToHashLC( p->data->uid->Data() );
                    task->iSupplyDataParams->iDataOwner->iJavaHash = HBufC::NewL( hashPtr.Size() );
                    task->iSupplyDataParams->iDataOwner->iJavaHash->Des().Copy( hashPtr );
                    CleanupStack::PopAndDestroy(); //DesToHashLC()
                    }
                }
            if ( p->data->drives )
                {
                task->iSupplyDataParams->iDataOwner->iDriveList = 
                DriveList ( p->data->drives->drive );
                }
            if ( p->data->transferDataType )
                {
                TInt intValue = DesToInt ( p->data->transferDataType->Data() );
                task->iSupplyDataParams->iDataOwner->iTransDataType =
                    static_cast<TSConTransferDataType> (intValue);
                }
            if ( p->data->data )
                {
                task->iSupplyDataParams->iRestoreData = HBufC8::NewL(
                                            p->data->data->Data().Size() );
                *task->iSupplyDataParams->iRestoreData = p->data->data->Data();
                }
            if ( p->data->moreData )
                {
                task->iSupplyDataParams->iMoreData = ETrue;
                }
            else
                {
                task->iSupplyDataParams->iMoreData = EFalse;
                }
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskSupplyDataL() : returned %d", ret );    
    return ret;
}

// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskGetDataOwnerStatusL()
// Sends a Get data owner status -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskGetDataOwnerStatusL
    ( ConML_GetDataOwnerStatusPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EGetDataOwnerStatus );
    CSConTask* task = CSConTask::NewLC( method );

    if ( aContent->dataOwners )
        {
        for ( ConML_SIDListPtr_t p = aContent->dataOwners->sid; 
                p && p->data; p=p->next )
            {
            CSConDataOwner* dataOwner = new (ELeave) CSConDataOwner();
            CleanupStack::PushL( dataOwner );
            if ( p->data->type )    
                {
                dataOwner->iType = TSConDOType (DesToInt(
                                     p->data->type->Data() ));
                }
            if ( p->data->uid )             
                {
                if( !IsJavaHash( p->data->uid->Data() ) )
                    {
                    dataOwner->iUid = DesToUid( p->data->uid->Data() );
                    }
                else
                    {
                    TPtr hashPtr = DesToHashLC( p->data->uid->Data() );
                    dataOwner->iJavaHash = HBufC::NewL( hashPtr.Size() );
                    dataOwner->iJavaHash->Des().Copy( hashPtr );
                    CleanupStack::PopAndDestroy(); //DesToHashLC()
                    }
                }           
            task->iGetDataOwnerParams->iDataOwners.AppendL( dataOwner );
            CleanupStack::Pop( dataOwner );
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskGetDataOwnerStatusL() : returned %d", ret );    
    return ret; 
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskRebootL()
// Sends a Reboot -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskRebootL( )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EReboot );
    CSConTask* task = CSConTask::NewLC( method );
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskRebootL() : returned %d", ret );    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskGetMetadataL()
// Sends a GetMetadata -task to PCCS service
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::TaskGetMetadataL( ConML_GetMetadataPtr_t aContent )
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TSConMethodName method ( EGetMetadata );
    CSConTask* task = CSConTask::NewLC( method );
    if( aContent )
        {
        if ( aContent->filename )
            {
            // Unicode conversion from 8-bit to 16-bit
            CnvUtfConverter::ConvertToUnicodeFromUtf8(task->iGetMetadataParams->iFilename, 
                                                        aContent->filename->Data());    
            }
        }
    ret = iPCDHandler->PutTaskL( task );
    CleanupStack::Pop( task );
    LOGGER_WRITE_1( "CSConPCConnSession::TaskGetMetadataL() : returned %d", ret );    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::TaskRebootL()
// Appends a status element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendStatusL( 
    ConML_ConMLPtr_t aContent, CSConStatusReply* reply )
    {
    TRACE_FUNC_ENTRY;
    
    if ( !reply )
        {
        LOGGER_WRITE( "CSConPCConnSession::AppendStatus() : No data in reply!" );
        return;
        }
    aContent->status = new ( ELeave ) ConML_Status_t();
        
    if ( reply->iTasks.Count() > 0 ) 
        {
        for ( TInt i=0; i < reply->iTasks.Count(); i ++)
            {
            ConML_TaskListPtr_t task = new ( ELeave ) ConML_TaskList_t();
            CleanupStack::PushL( task );
            GenericListAddL ( &aContent->status->task, task );
            CleanupStack::Pop(); // task
            
            task->data = new ( ELeave ) ConML_Task_t();
            task->data->id = new ( ELeave ) pcdata_t();
            task->data->id->SetDataL ( IntToDesLC(reply->iTasks[i]->iTaskId) );
            CleanupStack::PopAndDestroy(); // IntToDesLC 
            TSConMethodName method ( reply->iTasks[i]->iMethod );
            
            switch ( method )
                {
                case EUpdateDeviceInfo:
                    task->data->updateDeviceInfo = 
                        new ( ELeave ) ConML_UpdateDeviceInfo_t();
                    task->data->updateDeviceInfo->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendUpdateDeviceInfoResultsL( 
                        task->data->updateDeviceInfo->results,
                        reply->iTasks[i]->iDevInfoParams );
                    break;
                
                case ESetBURMode:
                    task->data->setBurMode = 
                        new ( ELeave ) ConML_SetBURMode_t();
                    task->data->setBurMode->results = 
                        new ( ELeave) ConML_Results_t();
                    AppendSetBURModeResultsL( 
                        task->data->setBurMode->results,
                        reply->iTasks[i]->iBURModeParams );
                    break;
            
                case EListInstalledApps:
                    task->data->listInstalledApps = 
                        new ( ELeave ) ConML_ListInstalledApps_t();
                    task->data->listInstalledApps->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendListInstalledAppsResultsL( 
                        task->data->listInstalledApps->results,
                        reply->iTasks[i]->iListAppsParams);
                    break;
                
                case EInstall:
                    task->data->install = new ( ELeave ) ConML_Install_t();
                    task->data->install->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendInstallResultsL( 
                        task->data->install->results, 
                        reply->iTasks[i]->iInstallParams );
                    break;
            
                case EUninstall:
                    task->data->unInstall = new ( ELeave ) ConML_UnInstall_t();
                    task->data->unInstall->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendUninstallResultsL(
                        task->data->unInstall->results, 
                        reply->iTasks[i]->iUninstallParams );
                    break;
                
                case EListDataOwners:
                    task->data->listDataOwners = 
                        new ( ELeave ) ConML_ListDataOwners_t();
                    task->data->listDataOwners->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendListDataOwnersResultsL( 
                        task->data->listDataOwners->results, 
                        reply->iTasks[i]->iListDataOwnersParams );
                    break;
                
                case EGetDataOwnerStatus:
                    task->data->getDataOwnerStatus = 
                        new ( ELeave ) ConML_GetDataOwnerStatus_t();
                    task->data->getDataOwnerStatus->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendGetDataOwnerStatusResultsL( 
                        task->data->getDataOwnerStatus->results,
                        reply->iTasks[i]->iGetDataOwnerParams);
                    break;
                
                case EGetDataSize:
                    task->data->getDataSize = 
                        new ( ELeave ) ConML_GetDataSize_t();
                    task->data->getDataSize->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendGetDataSizeResultsL( 
                        task->data->getDataSize->results,
                        reply->iTasks[i]->iGetDataSizeParams );
                    break;
                                
                case EListPublicFiles:
                    task->data->listPublicFiles = 
                        new ( ELeave ) ConML_ListPublicFiles_t();
                    task->data->listPublicFiles->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendListPublicFilesResultsL ( 
                        task->data->listPublicFiles->results,
                        reply->iTasks[i]->iPubFilesParams );
                    break;
            
                case ERequestData:
                    task->data->requestData = 
                        new ( ELeave ) ConML_RequestData_t();
                    task->data->requestData->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendRequestDataResultsL( 
                        task->data->requestData->results, 
                        reply->iTasks[i]->iRequestDataParams );
                    break;
                
                case ESupplyData:
                    task->data->supplyData = 
                        new ( ELeave ) ConML_SupplyData_t();
                    task->data->supplyData->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendSupplyDataResultsL ( 
                        task->data->supplyData->results, 
                        reply->iTasks[i]->iSupplyDataParams );
                    break;
                
                case EGetMetadata:
                    task->data->getMetadata = 
                        new ( ELeave ) ConML_GetMetadata_t();
                    task->data->getMetadata->results = 
                        new ( ELeave ) ConML_Results_t();
                    AppendGetMetadataResultsL ( 
                        task->data->getMetadata->results, 
                        reply->iTasks[i]->iGetMetadataParams );
                    break;
                    
                default:
                    LOGGER_WRITE_1( "CSConPCConnSession:: AppendStatus() : Unknown method  %d ", method );
                    break;
                }
            }
        }
    else
        {
        LOGGER_WRITE( "CSConPCConnSession::AppendStatus() : No Task " );     
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendUpdateDeviceInfoResultsL()
// Appends a update device info -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendUpdateDeviceInfoResultsL 
    ( ConML_ResultsPtr_t aContent, CSConUpdateDeviceInfo* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )  
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL ( aContent, aResult->iProgress );
    
        aContent->deviceInfo = new ( ELeave ) ConML_DeviceInfo_t();
        aContent->deviceInfo->version = new ( ELeave ) pcdata_t();
        aContent->deviceInfo->version->SetDataL ( aResult->iVersion );

        aContent->deviceInfo->supportedMethods = 
            new ( ELeave ) ConML_SupportedMethods_t();
            
        if ( aResult->iInstallSupp )
            {
            aContent->deviceInfo->supportedMethods->install = 
                new ( ELeave ) pcdata_t();
            }
        if ( aResult->iUninstallSupp )
            {
            aContent->deviceInfo->supportedMethods->unInstall = 
                new ( ELeave ) pcdata_t();
            }
        if ( aResult->iInstAppsSupp )
            {
            aContent->deviceInfo->supportedMethods->listInstalledApps = 
                new ( ELeave ) pcdata_t();
            }
        if ( aResult->iDataOwnersSupp )
            {
            aContent->deviceInfo->supportedMethods->listDataOwners = 
                new ( ELeave ) pcdata_t();
            }
        if ( aResult->iSetBURModeSupp )
            {
            aContent->deviceInfo->supportedMethods->setBurMode = 
                new ( ELeave ) pcdata_t();
            }
        if ( aResult->iGetSizeSupp )
            {
            aContent->deviceInfo->supportedMethods->getDataSize = 
                new ( ELeave ) pcdata_t();
            }
        if ( aResult->iReqDataSupp )
            {
            aContent->deviceInfo->supportedMethods->requestData = 
                new (ELeave ) pcdata_t();
            }
        if ( aResult->iSupplyDataSupp )
            {
            aContent->deviceInfo->supportedMethods->supplyData = 
                new ( ELeave ) pcdata_t();
            }
        if ( aResult->iRebootSupp )
            {
            aContent->deviceInfo->supportedMethods->reboot = 
                new ( ELeave ) pcdata_t();
            }
        aContent->deviceInfo->maxObjectSize = new ( ELeave ) pcdata_t();
        aContent->deviceInfo->maxObjectSize->SetDataL ( IntToDesLC(
            aResult->iMaxObjectSize ));
        CleanupStack::PopAndDestroy(); // IntToDesLC
        }
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendSetBURModeResultsL()
// Appends a Set BUR mode -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendSetBURModeResultsL (
    ConML_ResultsPtr_t aContent, CSConSetBURMode* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL ( aContent, aResult->iProgress );
        }
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendListInstalledAppsResultsL()
// Appends a List installed apps -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendListInstalledAppsResultsL ( 
    ConML_ResultsPtr_t aContent, CSConListInstApps* aResult )
    {
    TRACE_FUNC_ENTRY;

    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL ( aContent, aResult->iProgress );
        
        if ( aResult->iApps.Count() > 0 )
            {
            // 6 * KMaxFileName should be enought
            // ( 5 items of TFileName and uid + type + size + 7* "#" )
            HBufC8* buf = HBufC8::NewLC( 6 * KMaxFileName );
            TPtr8 ptrBuf = buf->Des();
            
            aContent->applications = new ( ELeave ) ConML_Applications_t();
            for ( TInt i=0; i<aResult->iApps.Count(); i++)
                {
                ConML_ApplicationListPtr_t app = 
                    new ( ELeave )ConML_ApplicationList_t();
                CleanupStack::PushL ( app );
                GenericListAddL ( &aContent->applications->application, app );
                CleanupStack::Pop(); // app
                
                app->data = new ( ELeave ) ConML_Application_t();
                app->data->name = new ( ELeave ) pcdata_t();
                app->data->name->SetDataL( BufToDesLC( 
                    aResult->iApps[i]->iName));
                CleanupStack::PopAndDestroy(); // BufToDesLC
                
                // create uid: UID # Type # Size # Version # Vendor # Parent app. name # WidgetBundleId #
                LOGGER_WRITE( "CSConPCConnSession::AppendListInstalledAppsResultsL() : Create Uid" );
                
                ptrBuf.Copy( UidToDesLC( aResult->iApps[i]->iUid ) );
                CleanupStack::PopAndDestroy(); // UidToDesLC
                
                ptrBuf.Append( KSConAppInfoSeparator );
                
                ptrBuf.Append( IntToDesLC(aResult->iApps[i]->iType ));
                CleanupStack::PopAndDestroy(); // IntToDesLC
                
                ptrBuf.Append( KSConAppInfoSeparator );
                
                ptrBuf.Append( IntToDesLC(aResult->iApps[i]->iSize ) );
                CleanupStack::PopAndDestroy(); // IntToDesLC
                
                ptrBuf.Append( KSConAppInfoSeparator );
                ptrBuf.Append( aResult->iApps[i]->iVersion );
                
                ptrBuf.Append( KSConAppInfoSeparator );
                ptrBuf.Append( BufToDesLC( aResult->iApps[i]->iVendor ) );
                CleanupStack::PopAndDestroy(); // BufToDesLC
                
                ptrBuf.Append( KSConAppInfoSeparator );
                ptrBuf.Append( BufToDesLC( aResult->iApps[i]->iParentName ) );
                CleanupStack::PopAndDestroy(); // BufToDesLC
                
                ptrBuf.Append( KSConAppInfoSeparator );
                if (aResult->iApps[i]->iWidgetBundleId)
                    {
                    ptrBuf.Append( BufToDesLC( *aResult->iApps[i]->iWidgetBundleId ) );
                    CleanupStack::PopAndDestroy(); // BufToDesLC
                    }
                
                ptrBuf.Append( KSConAppInfoSeparator );
                
                LOGGER_WRITE( "CSConPCConnSession::AppendListInstalledAppsResultsL() : set data" );
                app->data->uid = new ( ELeave ) pcdata_t();
                app->data->uid->SetDataL( *buf );
                    
                LOGGER_WRITE( "CSConPCConnSession::AppendListInstalledAppsResultsL() : Info added" );
                }
            CleanupStack::PopAndDestroy(buf);       
            }
        }
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendListPublicFilesResultsL()
// Appends a List public files -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendListPublicFilesResultsL ( 
    ConML_ResultsPtr_t aContent, CSConListPublicFiles* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL ( aContent, aResult->iProgress );
        
        if ( aResult->iFiles.Count() > 0 )
            {
            aContent->files = new ( ELeave ) ConML_Files_t();
            for ( TInt i=0;i<aResult->iFiles.Count(); i++ )
                {
                ConML_FileListPtr_t file = new ( ELeave ) ConML_FileList_t();
                CleanupStack::PushL( file );
                GenericListAddL ( &aContent->files->file, file );
                CleanupStack::Pop(); // file
                
                file->data = new ( ELeave ) ConML_File_t();
                file->data->name = new ( ELeave ) pcdata_t();
                file->data->name->SetDataL( BufToDesLC (
                    aResult->iFiles[i]->iPath ) );
                CleanupStack::PopAndDestroy(); // BufToDesLC
                
                file->data->modified = new ( ELeave ) pcdata_t();
                file->data->modified->SetDataL( BufToDesLC (
                    aResult->iFiles[i]->iModified ) );
                CleanupStack::PopAndDestroy(); // BufToDesLC
                
                file->data->size = new ( ELeave ) pcdata_t();
                file->data->size->SetDataL( IntToDesLC ( 
                    aResult->iFiles[i]->iSize ) );
                CleanupStack::PopAndDestroy(); // IntToDesLC
                
                file->data->userPerm = new ( ELeave ) pcdata_t();
                switch ( aResult->iFiles[i]->iUserPerm )
                    {
                    case EPermReadOnly:
                        file->data->userPerm->SetDataL ( KSConPermReadOnly );
                        break;
                    
                    case EPermNormal:
                        file->data->userPerm->SetDataL ( KSConPermNormal );
                        break;
                    
                    default:
                        LOGGER_WRITE( "CSConPCConnSession:: AppendListPublicFilesResultsL() : Unknown userPerm! " );
                        break;          
                    }
                }
        }
    }
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendRequestDataResultsL()
// Appends a Request data -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendRequestDataResultsL( 
    ConML_ResultsPtr_t aContent, CSConRequestData* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL ( aContent, aResult->iProgress );
        
        if ( aResult->iMoreData )
            {
            aContent->moreData = new ( ELeave ) pcdata_t();
            }
        if ( aResult->iBackupData )
            {
            aContent->data = new ( ELeave ) pcdata_t();
            aContent->data->SetDataL( aResult->iBackupData->Des() );
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendSupplyDataResultsL()
// Appends a Supply data -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendSupplyDataResultsL( 
    ConML_ResultsPtr_t aContent, CSConSupplyData* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL( aContent, aResult->iProgress );
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendInstallResultsL()
// Appends an Install -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendInstallResultsL( 
    ConML_ResultsPtr_t aContent, CSConInstall* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL( aContent, aResult->iProgress );
        
        if ( aResult->iData && aResult->iData->Length() > 0 )
            {
            aContent->data = new ( ELeave ) pcdata_t();
            aContent->data->SetDataL( aResult->iData->Des() );
            }
        }
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendUninstallResultsL()
// Appends an Uninstall -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendUninstallResultsL( 
    ConML_ResultsPtr_t aContent, CSConUninstall* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            LOGGER_WRITE( "CSConPCConnSession::AppendUninstallResultsL() Complete" );
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        LOGGER_WRITE_1( "CSConPCConnSession::AppendUninstallResultsL() iProgress: %d", aResult->iProgress );
        AppendProgressL( aContent, aResult->iProgress );
        
        if ( aResult->iData && aResult->iData->Length() > 0 )
            {
            aContent->data = new ( ELeave ) pcdata_t();
            aContent->data->SetDataL( aResult->iData->Des() );
            }
        }
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendListDataOwnersResultsL()
// Appends a List data owners -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendListDataOwnersResultsL ( 
    ConML_ResultsPtr_t aContent, CSConListDataOwners* aResult  )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )  
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL( aContent, aResult->iProgress );
        
        if ( aResult->iDataOwners.Count() > 0 )
            {
            aContent->dataOwners = new ( ELeave ) ConML_DataOwners_t();
            for ( TInt i=0; i < aResult->iDataOwners.Count(); i ++)
                {
                ConML_SIDListPtr_t sid = new ( ELeave ) ConML_SIDList_t();
                CleanupStack::PushL( sid );
                GenericListAddL ( &aContent->dataOwners->sid, sid );
                CleanupStack::Pop();
            
                sid->data = new ( ELeave ) ConML_SID_t();
                
                sid->data->type = new ( ELeave ) pcdata_t();
                sid->data->type->SetDataL ( IntToDesLC ( 
                    aResult->iDataOwners[i]->iType) );  
                CleanupStack::PopAndDestroy(); // IntToDesLC
                
                sid->data->uid = new ( ELeave ) pcdata_t();
                
                if( aResult->iDataOwners[i]->iUid.iUid )
                    {
                    sid->data->uid->SetDataL ( UidToDesLC ( 
                        aResult->iDataOwners[i]->iUid ) );
                    CleanupStack::PopAndDestroy(); // UidToDesLC    
                    }
                    
                if( aResult->iDataOwners[i]->iJavaHash )
                    {
                    sid->data->uid->SetDataL ( HashToDesLC ( 
                        aResult->iDataOwners[i]->iJavaHash->Des() ) );
                    CleanupStack::PopAndDestroy(); // HashToDesLC
                    }
                                
                if ( HasDrives ( aResult->iDataOwners[i]->iDriveList ) )
                    {
                    sid->data->drives = new ( ELeave ) ConML_Drives_t();
                    AppendDrivesL ( sid->data->drives, 
                        aResult->iDataOwners[i]->iDriveList );
                    }       
            
                if ( aResult->iDataOwners[i]->iPackageName.Length() > 0 ) 
                    {
                    sid->data->packageInfo = 
                        new ( ELeave ) ConML_PackageInfo_t();
                    sid->data->packageInfo->name = new ( ELeave ) pcdata_t();
                    sid->data->packageInfo->name->SetDataL ( BufToDesLC ( 
                        aResult->iDataOwners[i]->iPackageName  ) );

                    CleanupStack::PopAndDestroy(); // BufToDesLC
                    }           
            
                if ( aResult->iDataOwners[i]->iHasFiles || 
                aResult->iDataOwners[i]->iSupportsInc ||
                aResult->iDataOwners[i]->iDelayToPrep ||
                aResult->iDataOwners[i]->iReqReboot )
                    {
                    sid->data->burOptions = new ( ELeave ) ConML_BUROptions_t();
                    if ( aResult->iDataOwners[i]->iHasFiles ) 
                        {
                        sid->data->burOptions->hasFiles = 
                            new ( ELeave ) pcdata_t();
                        sid->data->burOptions->hasFiles->SetDataL( IntToDesLC(
                                aResult->iDataOwners[i]->iHasFiles ));
                                
                        CleanupStack::PopAndDestroy(); // IntToDesLC
                        }
                    if ( aResult->iDataOwners[i]->iSupportsInc )
                        {
                        sid->data->burOptions->supportsInc = 
                            new ( ELeave ) pcdata_t();
                        }
                    if ( aResult->iDataOwners[i]->iDelayToPrep )                        
                        {
                        sid->data->burOptions->delayToPrepareData = 
                            new ( ELeave ) pcdata_t();
                        }
                    if ( aResult->iDataOwners[i]->iReqReboot )
                        {
                        sid->data->burOptions->requiresReboot = 
                            new ( ELeave ) pcdata_t();
                        }
                    }
                }
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendGetDataOwnerStatusResultsL()
// Appends a Get data owner status -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendGetDataOwnerStatusResultsL 
    ( ConML_ResultsPtr_t aContent, CSConGetDataOwnerStatus* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL( aContent, aResult->iProgress );

        if ( aResult->iDataOwners.Count() > 0 )
            {
            aContent->dataOwners = new ( ELeave ) ConML_DataOwners_t();
            for ( TInt i=0; i < aResult->iDataOwners.Count(); i ++)
                {
                ConML_SIDListPtr_t sid = new ( ELeave ) ConML_SIDList_t();
                CleanupStack::PushL( sid );
                GenericListAddL ( &aContent->dataOwners->sid, sid );
                CleanupStack::Pop();
            
                sid->data = new ( ELeave ) ConML_SID_t();
                
                sid->data->type = new ( ELeave ) pcdata_t();
                sid->data->type->SetDataL ( IntToDesLC ( 
                    aResult->iDataOwners[i]->iType) );  
                CleanupStack::PopAndDestroy(); // IntToDesLC
                
                sid->data->uid = new ( ELeave ) pcdata_t();
                sid->data->uid->SetDataL ( UidToDesLC ( 
                    aResult->iDataOwners[i]->iUid ) );
                CleanupStack::PopAndDestroy(); // UidToDesLC    
                
                sid->data->dataOwnerStatus = new ( ELeave ) pcdata_t();
                sid->data->dataOwnerStatus->SetDataL ( IntToDesLC( 
                        aResult->iDataOwners[i]->iDataOwnStatus ));
                
                CleanupStack::PopAndDestroy(); // IntToDesLC
                }
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendGetDataSizeResultsL()
// Appends a Get data owner size -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendGetDataSizeResultsL ( 
    ConML_ResultsPtr_t aContent, CSConGetDataSize* aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aResult )  
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            }
        AppendProgressL( aContent, aResult->iProgress );
        
        if ( aResult->iDataOwners.Count() > 0 )
            {
            aContent->dataOwners = new ( ELeave ) ConML_DataOwners_t();
                
            for ( TInt i=0; i< aResult->iDataOwners.Count(); i++ )      
                {
                ConML_SIDListPtr_t sid = new ( ELeave ) ConML_SIDList_t();
                CleanupStack::PushL( sid );
                GenericListAddL ( &aContent->dataOwners->sid, sid );
                CleanupStack::Pop();
                
                sid->data = new ( ELeave ) ConML_SID_t();
                
                sid->data->type = new ( ELeave ) pcdata_t();
                sid->data->type->SetDataL ( IntToDesLC ( 
                    aResult->iDataOwners[i]->iType) );  
                CleanupStack::PopAndDestroy(); // IntToDesLC
                    
                sid->data->uid = new ( ELeave ) pcdata_t();
                
                if( aResult->iDataOwners[i]->iUid.iUid )
                    {
                    sid->data->uid->SetDataL ( UidToDesLC ( 
                        aResult->iDataOwners[i]->iUid ) );
                    CleanupStack::PopAndDestroy(); // UidToDesLC    
                    }

                if( aResult->iDataOwners[i]->iJavaHash )
                    {
                    sid->data->uid->SetDataL ( HashToDesLC ( 
                        aResult->iDataOwners[i]->iJavaHash->Des() ) );
                    CleanupStack::PopAndDestroy(); // HashToDesLC
                    }
    
                if ( HasDrives (  aResult->iDataOwners[i]->iDriveList ) )
                    {
                    sid->data->drives = new ( ELeave ) ConML_Drives_t();
                    AppendDrivesL ( sid->data->drives, 
                        aResult->iDataOwners[i]->iDriveList );
                    }       
                
                sid->data->size = new ( ELeave ) pcdata_t();
                sid->data->size->SetDataL( IntToDesLC( 
                    aResult->iDataOwners[i]->iSize ) );
                CleanupStack::PopAndDestroy(); // IntToDesLC
                
                sid->data->transferDataType = new ( ELeave ) pcdata_t();
                sid->data->transferDataType->SetDataL( IntToDesLC( 
                        aResult->iDataOwners[i]->iTransDataType ) );

                CleanupStack::PopAndDestroy(); // IntToDesLC
                }
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendGetMetadataResultsL()
// Appends a GetMetadata -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendGetMetadataResultsL(
    ConML_ResultsPtr_t aContent, CSConGetMetadata* aResult )
    {
    TRACE_FUNC_ENTRY;
    
    if ( aResult )
        {
        if ( aResult->iComplete )
            {
            aContent->complete = new ( ELeave ) pcdata_t();
            
            // add filename only if task is completed
            aContent->filename = new ( ELeave ) pcdata_t();
            aContent->filename->SetDataL( BufToDesLC(aResult->iFilename ) );
            CleanupStack::PopAndDestroy(); // BufToDesLC
            }
        AppendProgressL( aContent, aResult->iProgress );
        
        if ( aResult->iData )
            {
            aContent->data = new ( ELeave ) pcdata_t();
            aContent->data->SetDataL( aResult->iData->Des() );
            }
        
        if ( aResult->iMoreData )
            {
            aContent->moreData = new ( ELeave ) pcdata_t();
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendProgressL()
// Appends a Progress -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendProgressL ( 
    ConML_ResultsPtr_t aContent, TInt progress )
    {
    TRACE_FUNC_ENTRY;
    aContent->progress = new ( ELeave ) ConML_Progress_t();
    aContent->progress->value = new ( ELeave ) pcdata_t();
    aContent->progress->value->SetDataL ( IntToDesLC ( progress ));
    CleanupStack::PopAndDestroy(); // IntToDesLC
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::AppendDrivesL()
// Appends a Drives -element from the reply data of PCCS
// -----------------------------------------------------------------------------
//
void CSConPCConnSession::AppendDrivesL( 
    ConML_DrivesPtr_t aContent, TDriveList aDrives )
    {
    TRACE_FUNC_ENTRY;
    
    for ( TInt i = 0; i<KMaxDrives; i++ )
        {
        if ( aDrives[i] )
            {
            ConML_DriveListPtr_t drive = new ( ELeave ) ConML_DriveList_t();
            CleanupStack::PushL ( drive );
            GenericListAddL ( &aContent->drive, drive );
            CleanupStack::Pop(); // drive
            
            drive->data = new ( ELeave ) ConML_Drive_t();
            drive->data->name = new ( ELeave ) pcdata_t();
            drive->data->name->SetDataL( DriveNumToDesLC(i+KSConFirstDrive  ));
            CleanupStack::PopAndDestroy(); // IntToDesLC
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::DriveList()
// Converts an Drives -element to TDriveList
// -----------------------------------------------------------------------------
//
TDriveList CSConPCConnSession::DriveList( ConML_DriveListPtr_t aContent )
    {
    TBuf8<KMaxDrives>  driveBuf;
    TDriveList driveList;

    for ( ConML_DriveListPtr_t p = aContent; p && p->data; p = p->next )
        {
        if ( p->data->name )            
            {
            driveBuf.Append( p->data->name->Data() );
            }
        }

    for ( TInt i = 0; i<KMaxDrives; i++ )
        {
        if ( driveBuf.Locate ( TChar( i + KSConFirstDrive ) ) != KErrNotFound )
            {
            driveList.Append( KSConDriveExists );
            }
        else
            {
            driveList.Append( KSConNoDrive );
            }
        }
    return driveList;
    }

    
// -----------------------------------------------------------------------------
// CSConPCConnSession::IntToDesLC()
// Converts an integer to descriptor
// -----------------------------------------------------------------------------
//
TDesC8& CSConPCConnSession::IntToDesLC(const TInt& anInt)
    {
    HBufC8* buf = HBufC8::NewLC(20);
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(anInt);
    return *buf;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::DriveNumToDesLC()
// Convers a drive number to equivalent drive letter 
// -----------------------------------------------------------------------------
//
TDesC8& CSConPCConnSession::DriveNumToDesLC( const TInt& anInt )
    {
    TChar mark ( anInt );
    HBufC8* buf = HBufC8::NewLC(1);
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Append(mark);
    return *buf;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::BufToDesLC()
// converts a buffer to descriptor
// -----------------------------------------------------------------------------
//
TDesC8& CSConPCConnSession::BufToDesLC( const TDesC& aBuf)
    {
    HBufC8* buf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aBuf );
    CleanupStack::PushL( buf );
    return *buf;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::UidToDesLC()
// converts an UID to a descriptor
// -----------------------------------------------------------------------------
//
TDesC8& CSConPCConnSession::UidToDesLC( const TUid& aUid )
    {
    HBufC8* buf = HBufC8::NewLC(10);
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Copy (aUid.Name().Mid(1, 8) );
    return *buf;
    }
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::DesToUid()
// converts a descriptor to a UID value
// -----------------------------------------------------------------------------
//
TUid CSConPCConnSession::DesToUid ( const TDesC8& aDes )
    {
    TLex8 lex( aDes );
    TUint32 value;
    lex.Val( value, EHex );
    TUid uid = TUid::Uid( value );
    return uid;
    }   
    
// -----------------------------------------------------------------------------
// CSConPCConnSession::DesToInt()
// converts a descriptor to a integer value
// -----------------------------------------------------------------------------
//
TInt CSConPCConnSession::DesToInt( const TDesC8& aDes)
    {
    TLex8 lex(aDes);
    TInt value = 0;
    lex.Val(value);
    return value;
    }

// -----------------------------------------------------------------------------
// CSConPCConnSession::HasDrives()
// Returns ETrue if at least one drive is found from the given TDriveList
// -----------------------------------------------------------------------------
//
TBool CSConPCConnSession::HasDrives( TDriveList& aDrive )
    {
    TBool hasDrives ( EFalse );     
    for ( TInt i = 0; i<KMaxDrives; i++ )
        {
        if ( aDrive[i] )
            {
            hasDrives = ETrue;
            }
        }
    return hasDrives;
    }
// -----------------------------------------------------------------------------
// CSConPCConnSession::HashToDesLC()
// converts a Java hash to descriptor
// -----------------------------------------------------------------------------
//
TDesC8& CSConPCConnSession::HashToDesLC( const TDesC& aBuf)
    {
    HBufC8* buf = HBufC8::NewLC(aBuf.Size()+5);
    TPtr8 ptrBuf = buf->Des();
    // Unicode conversion from 16-bit to 8-bit
    CnvUtfConverter::ConvertFromUnicodeToUtf8(ptrBuf, aBuf);
    //Add JAVA_ identifier to the begining of the hash
    ptrBuf.Insert(0, KSConJavaHashId);
    return *buf;
    }
// -----------------------------------------------------------------------------
// CSConPCConnSession::DesToHashLC()
// converts descriptor to Java hash
// -----------------------------------------------------------------------------
//    
TPtr CSConPCConnSession::DesToHashLC( const TDesC8& aDes )
    {
    HBufC* buf = HBufC::NewLC(aDes.Size());
    TPtr ptrBuf = buf->Des();
    // Unicode conversion from 8-bit to 16-bit
    CnvUtfConverter::ConvertToUnicodeFromUtf8(ptrBuf, aDes);
    //Delete JAVA_
    ptrBuf.Delete(0, KSConJavaHashId().Length());   
    return ptrBuf;
    }
// -----------------------------------------------------------------------------
// CSConPCConnSession::IsJavaHash()
// Returns ETrue if descriptor is Java hash, else EFalse
// -----------------------------------------------------------------------------
//  
TBool CSConPCConnSession::IsJavaHash( const TDesC8& aDes )
    {
    if ( aDes.FindC(KSConJavaHashId) == 0 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

