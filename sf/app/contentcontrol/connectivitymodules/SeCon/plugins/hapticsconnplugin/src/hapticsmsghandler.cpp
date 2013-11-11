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
* Description:  Handles messaging between file system and haptics 
*                client interface.
*
*/


#include <f32file.h> 
#include <bautils.h> 
#include <pathinfo.h>
#include <hwrmhaptics.h>

#include "hapticsmsghandler.h"
#include "hapticsconntimer.h"
#include "hapticsconntrace.h"

_LIT( KVibeTonzFolder,"VibeTonz\\" );
_LIT( KSourceFileName,"VibeTonzDataReq" );
_LIT( KTargetFileName,"VibeTonzDataRsp" );

// ---------------------------------------------------------------------------
// CHapticsMsgHandler* CHapticsMsgHandler::NewL()
// ---------------------------------------------------------------------------
//
CHapticsMsgHandler* CHapticsMsgHandler::NewL()
    {
    CHapticsMsgHandler* self = new (ELeave) CHapticsMsgHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Default C++ constructor
// ---------------------------------------------------------------------------
//
CHapticsMsgHandler::CHapticsMsgHandler () : CActive( EPriorityStandard )
    {
    }    

// ---------------------------------------------------------------------------
// void CHapticsMsgHandler::ConstructL()
// ---------------------------------------------------------------------------
//
void CHapticsMsgHandler::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::ConstructL - Begin") ) );    
    CActiveScheduler::Add( this );
    ConnectToFilesystemL();
    iConnectionTimeout = CHapticsConnTimer::NewL(this);
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::ConstructL - End") ) );
    }

// ---------------------------------------------------------------------------
// CHapticsMsgHandler::~CHapticsMsgHandler()
// ---------------------------------------------------------------------------
//
CHapticsMsgHandler::~CHapticsMsgHandler()
    {    
    Cancel();

    if ( iConnectionTimeout )
        {
        iConnectionTimeout->Cancel();
        delete iConnectionTimeout;
        }

    delete iReqBuf;
        
    iClient.Close();
    iFs.Close();
    }

// ---------------------------------------------------------------------------
// void ClearRequestFile()
// ---------------------------------------------------------------------------
//   
void CHapticsMsgHandler::ClearRequestFile()
	{
	COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::ClearRequestFile - Begin") ) );
	
    TFindFile findDataFile( iFs );
    
    TFileName sourcefile = PathInfo::PhoneMemoryRootPath();
    sourcefile.Append( PathInfo::OthersPath() );
    sourcefile.Append( KVibeTonzFolder );
    sourcefile.Append( KSourceFileName );
    
    if( findDataFile.FindByDir( sourcefile, KNullDesC ) == KErrNone )
    	{
    	COMPONENT_TRACE( _L("CHapticsMsgHandler::ClearRequestFile - Deleting REQ file") );
    	iFs.Delete( KSourceFileName ); 
    	}
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::ClearRequestFile - End") ) );    	
	}
// ---------------------------------------------------------------------------
// void ConnectToFilesystem()
// ---------------------------------------------------------------------------
//    
void CHapticsMsgHandler::ConnectToFilesystemL()
    {
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::ConnectToFilesystemL - Begin") ) );
    User::LeaveIfError( iFs.Connect() );
    
    TFileName sessionPath = PathInfo::PhoneMemoryRootPath();
    sessionPath.Append( PathInfo::OthersPath() );
    sessionPath.Append( KVibeTonzFolder );
    TBool folderExists = BaflUtils::FolderExists( iFs, sessionPath );
    
    if( folderExists )
        {
        User::LeaveIfError( iFs.SetSessionPath( sessionPath ) );
        ClearRequestFile();
        }
    else
        {
        User::LeaveIfError( iFs.MkDir( sessionPath ) );
        User::LeaveIfError( iFs.SetSessionPath( sessionPath ) );
        }
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::ConnectToFilesystemL - End") ) );
    }    

// ---------------------------------------------------------------------------
// CHapticsMsgHandler::StartNotifier()
// ---------------------------------------------------------------------------
//
void CHapticsMsgHandler::StartNotifier()
	{
	COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::StartNotifier - Begin" ) ) );
    if ( !IsActive() )
        {
        TFileName sessionPath = PathInfo::PhoneMemoryRootPath();
        sessionPath.Append( PathInfo::OthersPath() );
        sessionPath.Append( KVibeTonzFolder );        
        iFs.NotifyChange( ENotifyAll, iStatus, sessionPath );
        SetActive();
	    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::StartNotifier - Started" ) ) );        
        }
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::StartNotifier - End" ) ) );
    }

// ---------------------------------------------------------------------------
// CHapticsMsgHandler::RunL()
// ---------------------------------------------------------------------------
//
void CHapticsMsgHandler::RunL()
    {
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::RunL - Begin" ) ) );

    StartNotifier(); // re-subscribe for change notifications.

    TFindFile findDataFile(iFs); 
    TFileName sourcefileName = PathInfo::PhoneMemoryRootPath();
    sourcefileName.Append( PathInfo::OthersPath() );
    sourcefileName.Append( KVibeTonzFolder );
    sourcefileName.Append( KSourceFileName ); 
          
    if( findDataFile.FindByDir( sourcefileName, KNullDesC ) == KErrNone )
        {
        COMPONENT_TRACE( ( _L("CHapticsMsgHandler::RunL - found request file") ) );
        
        RFile sourceFile;
		TInt sourceFileSize;

		User::LeaveIfError( sourceFile.Open( iFs, sourcefileName, EFileShareAny )  );
		CleanupClosePushL( sourceFile );
		sourceFile.Size( sourceFileSize );
		
		iReqBuf = HBufC8::NewL( sourceFileSize );
		TPtr8 reqBufPtr = iReqBuf->Des();		
		sourceFile.Read( reqBufPtr );
		CleanupStack::PopAndDestroy( &sourceFile );
			
		ClearRequestFile();

        DATADUMP_TRACE( _L("CHapticsMsgHandler::RunL - request data dump:"), reqBufPtr );

        if ( !iClient.Handle() )
            {
            User::LeaveIfError( iClient.Connect() );
            }

		TBuf8<256> retData;	
		TInt bridgeErr = iClient.SendBridgeBuffer( reqBufPtr, retData );
					
	    delete iReqBuf;
	    iReqBuf = NULL;
	    
		if ( bridgeErr == KErrNone )
			{		
            iConnectionTimeout->Cancel();
            iConnectionTimeout->Start();
            			
			DATADUMP_TRACE( _L("CHapticsMsgHandler::RunL - response data dump:"), retData );
            RFile targetFile;
			targetFile.Replace( iFs, KTargetFileName, EFileWrite );
			targetFile.Write( retData );
        	targetFile.Close();
        	}
        }      
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::RunL - End" ) ) );
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CHapticsMsgHandler::RunError( TInt aError )
    {
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::RunError: %d" ), aError ) );
    return aError;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CHapticsMsgHandler::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Inside CHapticsMsgHandler::DoCancel" ) ) );
    if ( IsActive() )
        {
        iFs.NotifyChangeCancel( iStatus );
        }
    }

// -----------------------------------------------------------------------------
// CHapticsConnPlugin::CHapticsConnPlugin()
// Constructor
// -----------------------------------------------------------------------------
//
void CHapticsMsgHandler::NotifyShutdown()
    {
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::NotifyShutdown - Begin" ) ) );
    iClient.CleanUp();
    iClient.Close(); 
    COMPONENT_TRACE( ( _L( "CHapticsMsgHandler::NotifyShutdown - End" ) ) );    
    } 
	
// end of file
