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
* Description: 
*
*/


//  INCLUDES 
#include <e32base.h>
#include <pathinfo.h>
#include <bautils.h>

// For Logging
#include "harvesterserverlogger.h"
#include "cfileharvester.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cfileharvesterTraces.h"
#endif


// CONSTANTS
_LIT( KFileMask, "*.*" );
const TInt KItemsPerRun = 1;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CFileHarvester::NewL
// ---------------------------------------------------------------------------
//
CFileHarvester* CFileHarvester::NewL( CFilePlugin& aFilePlugin, RFs& aFs )
	{
	CFileHarvester* self = new (ELeave) CFileHarvester( aFilePlugin, aFs );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CFileHarvester::~CFileHarvester
// ---------------------------------------------------------------------------
//
CFileHarvester::~CFileHarvester()
    {
    Cancel();
    delete iDir;
    delete iDirscan;
    iIgnorePaths.ResetAndDestroy();
    iIgnorePaths.Close();
    }


// ---------------------------------------------------------------------------
// CFileHarvester::CFileHarvester
// ---------------------------------------------------------------------------
//
CFileHarvester::CFileHarvester( CFilePlugin& aFilePlugin, RFs& aFs )
  : CActive( CActive::EPriorityIdle ),
    iFilePlugin( aFilePlugin ),
    iFs( aFs )
    {
    OstTraceFunctionEntry0( CFILEHARVESTER_CFILEHARVESTER_ENTRY );
    CPIXLOGSTRING("ENTER CFileHarvester::CFileHarvester");
    CActiveScheduler::Add( this );
    CPIXLOGSTRING("END CFileHarvester::CFileHarvester");
    OstTraceFunctionExit0( CFILEHARVESTER_CFILEHARVESTER_EXIT );
    }


// ---------------------------------------------------------------------------
// CFileHarvester::ConstructL
// ---------------------------------------------------------------------------
//
void CFileHarvester::ConstructL()
    {
    iDirscan = CDirScan::NewL( iFs );
    }


// ---------------------------------------------------------------------------
// CFileHarvester::StartL
// ---------------------------------------------------------------------------
//
void CFileHarvester::StartL( const TDriveNumber aDriveNumber )
    {
    OstTraceFunctionEntry0( CFILEHARVESTER_STARTL_ENTRY );
    CPIXLOGSTRING("ENTER CFileHarvester::Start");

    TFileName rootPath;
    User::LeaveIfError( PathInfo::GetRootPath( rootPath, aDriveNumber ) );

	iDirscan->SetScanDataL( rootPath, KEntryAttDir|KEntryAttMatchExclusive,
				ESortNone, // No need to sort data
				CDirScan::EScanDownTree );
	iCurrentHarvestDrive = aDriveNumber;
	SetNextRequest( EHarvesterStartHarvest );
    CPIXLOGSTRING("END CFileHarvester::Start");
    OstTraceFunctionExit0( CFILEHARVESTER_STARTL_EXIT );
    }


// -----------------------------------------------------------------------------
// CFileHarvester::AddIgnorePathsL
// -----------------------------------------------------------------------------
//
void CFileHarvester::AddIgnorePathsL( const TDriveNumber aDriveNumber )
    {
    TFileName ignorePath;
    TChar chr;
    User::LeaveIfError( RFs::DriveToChar( aDriveNumber, chr ) );

    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathSystem );
    AddIgnorePathL( ignorePath );
    OstTraceExt1( TRACE_NORMAL, CFILEHARVESTER_ADDIGNOREPATHSL, "CFileHarvester::AddIgnorePathsL;AddIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileHarvester::AddIgnorePathsL - AddIgnorePath: %S", &ignorePath );

    ignorePath.Zero();

    // As index databases are located under \\Private\\ path,
    // this ignore path will mean index databases are also ignored.
    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathPrivate );
    AddIgnorePathL( ignorePath );
    OstTraceExt1( TRACE_NORMAL, DUP1_CFILEHARVESTER_ADDIGNOREPATHSL, "CFileHarvester::AddIgnorePathsL;AddIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileHarvester::AddIgnorePathsL - AddIgnorePath: %S", &ignorePath );

    ignorePath.Zero();

    // Maps data must not be indexed
    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathMapsCities );
    AddIgnorePathL( ignorePath );
    OstTraceExt1( TRACE_NORMAL, DUP2_CFILEHARVESTER_ADDIGNOREPATHSL, "CFileHarvester::AddIgnorePathsL;AddIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileHarvester::AddIgnorePathsL - AddIgnorePath: %S", &ignorePath );
    }


// -----------------------------------------------------------------------------
// CFileHarvester::RemoveIgnorePaths
// -----------------------------------------------------------------------------
//
void CFileHarvester::RemoveIgnorePaths( const TDriveNumber aDriveNumber )
    {
    TFileName ignorePath;
    TChar chr;
    RFs::DriveToChar( aDriveNumber, chr );

    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathSystem );
    RemoveIgnorePath( ignorePath );
    OstTraceExt1( TRACE_NORMAL, CFILEHARVESTER_REMOVEIGNOREPATHS, "CFileHarvester::RemoveIgnorePaths;RemoveIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileHarvester::RemoveIgnorePaths - RemoveIgnorePath: %S", &ignorePath );

    ignorePath.Zero();

    // As index databases are located under \\Private\\ path,
    // this ignore path will mean index databases are also ignored.
    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathPrivate );
    RemoveIgnorePath( ignorePath );
    OstTraceExt1( TRACE_NORMAL, DUP1_CFILEHARVESTER_REMOVEIGNOREPATHS, "CFileHarvester::RemoveIgnorePaths;RemoveIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileHarvester::RemoveIgnorePaths - RemoveIgnorePath: %S", &ignorePath );

    ignorePath.Zero();

    // Maps
    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathMapsCities );
    RemoveIgnorePath( ignorePath );
    OstTraceExt1( TRACE_NORMAL, DUP2_CFILEHARVESTER_REMOVEIGNOREPATHS, "CFileHarvester::RemoveIgnorePaths;RemoveIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileHarvester::RemoveIgnorePaths - RemoveIgnorePath: %S", &ignorePath );
    }


// -----------------------------------------------------------------------------
// CFileHarvester::AddIgnorePathL
// -----------------------------------------------------------------------------
//
TInt CFileHarvester::AddIgnorePathL( const TFileName& aIgnorePath )
    {
    TInt err( KErrNotFound );

    if ( aIgnorePath.Length() > 0 )
        {
        // check if already exist
        if ( iIgnorePaths.Count() > 0 )
            {
            for ( TInt i = 0; i < iIgnorePaths.Count(); i++ )
                {
                TFileName* tf = iIgnorePaths[i];
                if( tf->Compare(aIgnorePath) == 0 )
					{
					return KErrNone;
					}
				}
			}
        TFileName* fn = new(ELeave) TFileName;
        
        if ( fn )
            {
            fn->Copy( aIgnorePath );
            iIgnorePaths.Append( fn ); // ownership is transferred
            }
        else
        	{
        	err = KErrNoMemory;
        	}
        }
    else
        {
        err = KErrNotFound;
        }
        
    return err;
	}


// -----------------------------------------------------------------------------
// CFileHarvester::RemoveIgnorePath
// -----------------------------------------------------------------------------
//
TInt CFileHarvester::RemoveIgnorePath( const TFileName& aIgnorePath )
    {
    TInt err( KErrNotFound );

    if ( aIgnorePath.Length() > 0 )
        {
        // check if already exist
        if ( iIgnorePaths.Count() > 0 )
            {
            for ( TInt i = 0; i < iIgnorePaths.Count(); i++ )
                {
                TFileName* tf = iIgnorePaths[i];
                if ( tf->Compare(aIgnorePath) == 0 )
                    {
                    OstTraceExt1( TRACE_NORMAL, CFILEHARVESTER_REMOVEIGNOREPATH, "CFileHarvester::RemoveIgnorePath;remove path=%S", aIgnorePath );
                    CPIXLOGSTRING2( "CFileHarvester::RemoveIgnorePath() - remove path: %S", &aIgnorePath );
                    delete tf;
                    tf = NULL;
                    iIgnorePaths.Remove( i );
                    }
                }
            }
        }
    else
        {
        err = KErrNotFound;
        }
        
    return err;
    }


// -----------------------------------------------------------------------------
// CFileHarvester::CheckPath
// -----------------------------------------------------------------------------
//
TBool CFileHarvester::CheckPath( const TFileName& aFileName )
	{
	// check if ignored pathlist
	const TInt count( iIgnorePaths.Count() );

	if ( count > 0 )
		{
		for ( TInt i = 0; i < count; i++ )
			{    
			TFileName* pathName = iIgnorePaths[i];
			pathName->LowerCase();
			TFileName tempFileName;
			tempFileName.Copy( aFileName );
			tempFileName.LowerCase();
			if ( tempFileName.Find(*pathName) != KErrNotFound )
				{
				OstTrace0( TRACE_NORMAL, CFILEHARVESTER_CHECKPATH, "CFileHarvester::CheckPath() - is ignore path" );
				CPIXLOGSTRING( "CFileHarvester::CheckPath() - is ignore path" );
				return EFalse;
				}
			}
		}
	return ETrue;
	}


// ---------------------------------------------------------------------------
// CFileHarvester::GetNextFolderL
// ---------------------------------------------------------------------------
//		
void CFileHarvester::GetNextFolderL()
    {
    OstTraceFunctionEntry0( CFILEHARVESTER_GETNEXTFOLDERL_ENTRY );
    CPIXLOGSTRING("ENTER CFileHarvester::GetNextFolderL");

    delete iDir;
    iDir = NULL;
    // Documentation: CDirScan::NextL() The caller of this function 
    // is responsible for deleting iDir after the function has returned.
    iDirscan->NextL(iDir);

    if ( iDir )
    	{
        // if folder is in ignore path then skip it
        if ( !CheckPath( iDirscan->FullPath() ) )
        	{
			OstTrace0( TRACE_NORMAL, CFILEHARVESTER_GETNEXTFOLDERL, "CFileHarvester::GetNextFolderL - IF EHarvesterStartHarvest" );
			CPIXLOGSTRING("CFileHarvester::GetNextFolderL - IF EHarvesterStartHarvest");
        	SetNextRequest( EHarvesterStartHarvest );
        	}
        else
			{
			OstTrace0( TRACE_NORMAL, DUP1_CFILEHARVESTER_GETNEXTFOLDERL, "CFileHarvester::GetNextFolderL - IF EHarvesterGetFileId" );
			CPIXLOGSTRING("CFileHarvester::GetNextFolderL - IF EHarvesterGetFileId");
			SetNextRequest( EHarvesterGetFileId );
			}
    	}
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CFILEHARVESTER_GETNEXTFOLDERL, "CFileHarvester::GetNextFolderL - IF EHarvesterIdle" );
        CPIXLOGSTRING("CFileHarvester::GetNextFolderL - IF EHarvesterIdle");
        SetNextRequest( EHarvesterIdleState );
        }
    CPIXLOGSTRING("END CFileHarvester::GetNextFolderL");
    OstTraceFunctionExit0( CFILEHARVESTER_GETNEXTFOLDERL_EXIT );
    }


// ---------------------------------------------------------------------------
// CFileHarvester::GetFileIdL
// 
// ---------------------------------------------------------------------------
//
void CFileHarvester::GetFileIdL()
    {
    OstTraceFunctionEntry0( CFILEHARVESTER_GETFILEIDL_ENTRY );
    CPIXLOGSTRING("ENTER CFileHarvester::GetFileId");

    if( iCurrentIndex == 0 )
        {
        TParse parse;
        parse.Set(KFileMask(), &( iDirscan->FullPath() ), NULL);

        // FindWildByPath assigns iDir = NULL, then allocates the memory for it.
        // Therefore must delete iDir first.
        delete iDir;
        iDir = NULL;

        TFindFile find( iFs );
        find.FindWildByPath(parse.FullName(), NULL, iDir);
        }

    if( iDir )
        {
        const TInt count(iDir->Count());
        while( ( iCurrentIndex < count ) && ( iStepNumber < KItemsPerRun ) )
            {
            TEntry aEntry = (*iDir)[iCurrentIndex];
            // Check if entry is a hidden or system file
            // if true -> continue until find something to index or have checked whole directory
            // if the entry is a file, index both its filename and contents, for folder index the 
            // folder name
            if( !aEntry.IsHidden() && !aEntry.IsSystem())
                {
                TParse fileParser;
                fileParser.Set( iDirscan->FullPath(), &(*iDir)[iCurrentIndex].iName, NULL );
                if( iFilePlugin.GetHarvesterState() )
                    {
                    if(aEntry.IsDir())
                        {                    
                        iFilePlugin.CreateFolderFileIndexItemL(fileParser.FullName(), ECPixAddAction);
                        }
                    else
                        {
                        iFilePlugin.CreateContentIndexItemL(fileParser.FullName(), ECPixAddAction);
                        iFilePlugin.CreateFolderFileIndexItemL(fileParser.FullName(), ECPixAddAction, false);
                        }
                    }
                else
                    {
                    TBool isFolder = EFalse;
                    if(aEntry.IsDir())
                        {                    
                        isFolder = ETrue;
                        }
                    iFilePlugin.AddToQueueL(fileParser.FullName(), ECPixAddAction, isFolder);
                    }
                // TODO: If this is not TRAPPED, state machine breaks 
                iStepNumber++;
                }
            iCurrentIndex++;
            }

        iStepNumber = 0;

        if( iCurrentIndex >= count )
            {
            iCurrentIndex = 0;
            SetNextRequest( EHarvesterStartHarvest );
            }
        else
            {
            SetNextRequest( EHarvesterGetFileId );
            }
        }
    else
        {
        SetNextRequest( EHarvesterStartHarvest );
        }
    CPIXLOGSTRING("END CFileHarvester::GetFileId");
    OstTraceFunctionExit0( CFILEHARVESTER_GETFILEIDL_EXIT );
    }


// -----------------------------------------------------------------------------
// CFileHarvester::DoCancel
// -----------------------------------------------------------------------------
//   
void CFileHarvester::DoCancel()
	{
    OstTrace0( TRACE_NORMAL, CFILEHARVESTER_DOCANCEL, "CFileHarvester::DoCancel" );
    CPIXLOGSTRING("CFileHarvester::DoCancel");
	}


// -----------------------------------------------------------------------------
// CFileHarvester::RunL
// -----------------------------------------------------------------------------
//   
void CFileHarvester::RunL()
    {
    OstTraceFunctionEntry0( CFILEHARVESTER_RUNL_ENTRY );
    CPIXLOGSTRING("ENTER CFileHarvester::RunL");
    // Simple Round-Robin scheduling.
    Deque();
    CActiveScheduler::Add( this );

    switch ( iHarvestState )
        {
		case EHarvesterIdleState:
			{
			iFilePlugin.HarvestingCompleted(iCurrentHarvestDrive, KErrNone);
			break;
			}

		case EHarvesterGetFileId:
			{
			GetFileIdL();
			break;
			}
		
		case EHarvesterStartHarvest:
			{
			GetNextFolderL();
			break;
			}

		default:
			break;
		}
	CPIXLOGSTRING("END CFileHarvester::RunL");
	OstTraceFunctionExit0( CFILEHARVESTER_RUNL_EXIT );
	}


// -----------------------------------------------------------------------------
// CFileHarvester::RunError
// -----------------------------------------------------------------------------
//   
TInt CFileHarvester::RunError(TInt aError)
	{
    OstTrace1( TRACE_NORMAL, CFILEHARVESTER_RUNERROR, "CFileHarvester::RunError;aError=%d", aError );
    CPIXLOGSTRING2("CFileHarvester::RunError - aError: %d", aError );
	iHarvestState = EHarvesterIdleState;
    iFilePlugin.HarvestingCompleted(iCurrentHarvestDrive, aError);
	return KErrNone;
	}


// ---------------------------------------------------------------------------
// SetNextRequest
// ---------------------------------------------------------------------------
//
void CFileHarvester::SetNextRequest( TFileHarvesterState aState )
    {
    OstTrace0( TRACE_NORMAL, CFILEHARVESTER_SETNEXTREQUEST, "CFileHarvester::SetNextRequest" );
    CPIXLOGSTRING("CFileHarvester::SetNextRequest");
    if ( !IsActive()  && (iFilePlugin.GetHarvesterState()) )
        {
        iHarvestState = aState;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    else
        iHarvestState = aState; 
    }

void CFileHarvester::ResumeRequest()
    {
    OstTraceFunctionEntry0( CFILEHARVESTER_RESUMEREQUEST_ENTRY );
    if(iHarvestState != EHarvesterIdleState)
        SetNextRequest(iHarvestState);
    OstTraceFunctionExit0( CFILEHARVESTER_RESUMEREQUEST_EXIT );
    }

// End of File
