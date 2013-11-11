/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "cfilemanager.h"
#include "rsutils.h"
#include "clog.h"

//  CONSTANTS
namespace
	{
	const TInt KBufferSize = 1000;
	}
	
#define USE_CFILEMAN_FIRST	

CFileManager* CFileManager::NewL( RFs& aFs )
    {	
	CFileManager* self = new ( ELeave ) CFileManager( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();    // self
    return self;
    }

CFileManager::CFileManager( RFs& aFs ) : CActive( CActive::EPriorityHigh ),
										iFs( aFs ),
										iBuffer( NULL )
										
    {
    CActiveScheduler::Add( this );
    }

CFileManager::~CFileManager()
    {
    Cancel(); 
    CloseCopy();
    if(iBuffer)
    {
    	delete iBuffer;
    	iBuffer = NULL;    	
    }
    delete iFileMan; 
    }

void CFileManager::ConstructL()
    {
    iFileMan = CFileMan::NewL( iFs );
    }
	
	
TInt CFileManager::Copy( const TDesC& aFile, const TDesC& aDir, TRequestStatus& aStatus )
	{	
	LOG( "CFileManager::Copy begin" );
	Cancel();
	CloseCopy();
	iCallerStatus = &aStatus;
	TInt err( KErrNone );
#ifdef USE_CFILEMAN_FIRST
	// check if file is used by another application
	err = iSource.Open( iFs, aFile, EFileRead | EFileShareReadersOnly );
	LOG1("CFileManager::Copy check err: %d", err);
	iSource.Close();
	if( !err )
		{
		err = iFileMan->Copy( aFile, aDir, CFileMan::EOverWrite, iStatus );
		LOG1("CFileManager::Copy iFileMan->Copy err: %d", err);
		if( !err )
			{
			iOperation = EFileManCopy;
			SetActive();				
			}					
		}
	else
#endif	
		{
		iOperation = EBufferCopy;
		TRAP( err, InitCopyL( aFile, aDir ) );
		LOG1( "CFileManager::Copy InitCopyL err: %d", err );
		if( err )
			{
			CloseCopy();
			}
		else
			{
			TPtr8 ptr = iBuffer->Des();
			LOG1( "CFileManager::Copy iBytesNum: %d", iBytesNum );
			iSource.Read( ptr, iBytesNum, iStatus );
			LOG( "CFileManager::Copy call to SetActive()" );
			SetActive();	
			}	
		}	
	LOG1( "CFileManager::Copy end with: %d", err );
	return err;
	}

	
void CFileManager::DoCancel()
	{
	LOG( "CFileManager::DoCancel begin" );
	CloseCopy();
	LOG( "CFileManager::DoCancel end" );	
	}

void CFileManager::RunL()
	{
	LOG1( "CFileManager::RunL iStatus: %d", iStatus.Int() );
	LOG1( "CFileManager::RunL iOperation: %d", iOperation );
	
	if( iStatus == KErrNone )	
		{
		switch( iOperation )
			{
			case ERemoveDir:
				{
				LOG( "CFileManager::RunL directory removed" );
				User::RequestComplete( iCallerStatus, iStatus.Int() );
				}
				break;				
			case EFileManCopy:
				{
				LOG( "CFileManager::RunL file copied" );
				User::RequestComplete( iCallerStatus, iStatus.Int() );
				}
				break;				
			case EBufferCopy:
				{
				TPtr8 ptr = iBuffer->Des();
				LOG( "CFileManager::RunL call to RFile::Write()" );
				User::LeaveIfError( iDest.Write( ptr, iBytesNum ) );
				iBytesLeft -= iBytesNum;
				if( iBytesLeft )
					{
					iBytesNum = ( KBufferSize < iBytesLeft ) ? KBufferSize : iBytesLeft;
					LOG1( "CFileManager::RunL iBytesLeft: %d", iBytesLeft );
					iSource.Read( ptr, iBytesNum, iStatus );
					SetActive();	
					}
				else
					{
					User::LeaveIfError( iDest.Flush() );
					CloseCopy();
					LOG( "CFileManager::RunL file copied" );
					User::RequestComplete( iCallerStatus, iStatus.Int() );	
					}	
				}
				break;				
			default:
				break;
			}						
		}
	else
		{
		CloseCopy();
		User::RequestComplete( iCallerStatus, iStatus.Int() );	
		}
	}
	
TInt CFileManager::RunError( TInt aError )
	{
	LOG1( "CFileManager::RunError aError: %d", aError );
	return KErrNone;	
	}
	
void CFileManager::CloseCopy()
	{
	LOG( "CFileManager::CloseCopy begin" );
	iSource.Close();
	iDest.Close();
	delete iBuffer;
	iBuffer = NULL;	
	iBytesLeft = 0;
	iBytesNum = 0;
	LOG( "CFileManager::CloseCopy end" );	
	}
	
void CFileManager::InitCopyL( const TDesC& aFile, const TDesC& aDir )		
	{
	LOG( "CFileManager::InitCopyL begin" );		
	TInt err = iSource.Open( iFs, aFile, EFileRead | EFileShareReadersOnly );
	LOG1("CFileManager::InitCopyL file.Open (shareread mode): %d", err);
	if ( err != KErrNone )
		{
		err = iSource.Open( iFs, aFile, EFileRead | EFileShareAny );
		LOG1("CFileManager::InitCopyL file.Open (shareany mode): %d", err);
		}
	User::LeaveIfError( err );
	
	TParsePtrC parse( aFile );
	TFileName writeFileName( aDir );
	writeFileName.Append( parse.NameAndExt() );	
	User::LeaveIfError( iDest.Replace( iFs, writeFileName, EFileWrite ) );
	iBuffer = HBufC8::NewL( KBufferSize );
	User::LeaveIfError( iSource.Size( iBytesLeft ) );
	LOG1( "CFileManager::InitCopyL file size: %d", iBytesLeft );
	iBytesNum = ( KBufferSize < iBytesLeft ) ? KBufferSize : iBytesLeft;
	LOG( "CFileManager::InitCopyL end" );	
	}

void CFileManager::CancelCopy()
	{
	LOG("CFileManager::CancelCopy begin");
	if( IsActive() )
		{
		Cancel();
		User::RequestComplete( iCallerStatus, KErrCancel );
		}
	LOG("CFileManager::CancelCopy end");
	}
	
void CFileManager::CancelRmDir()
	{
	LOG("CFileManager::CancelRmDir begin");
	if( IsActive() )
		{
		Cancel();
		User::RequestComplete( iCallerStatus, KErrCancel );
		}
	LOG("CFileManager::CancelRmDir end");
	}
								
TInt CFileManager::RmDir( const TDesC& aDir, TRequestStatus& aStatus )
	{
	LOG1("CFileManager::RmDir aDir: %S", &aDir);
	TInt err( KErrNone );
	Cancel();
	iCallerStatus = &aStatus;
	if( RsUtils::PathExists( aDir, iFs ) )
		{
		err = iFileMan->RmDir( aDir, iStatus );
		iOperation = ERemoveDir;				
		if( !err ) SetActive();
		}
	else
		{
		User::RequestComplete( iCallerStatus, KErrNone );
		}			
	LOG1("CFileManager::RmDir return: %d", err);
	return err;
	}

//  End of File
