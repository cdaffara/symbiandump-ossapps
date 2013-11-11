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


// INCLUDE FILES
#include <pathinfo.h>

#include "cfilemonitor.h"
#include "cfolderrenamedharvester.h"
#include "harvesterserverlogger.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cfilemonitorTraces.h"
#endif


// -----------------------------------------------------------------------------
// CFileMonitor::NewL
// -----------------------------------------------------------------------------
//
CFileMonitor* CFileMonitor::NewL(  CFilePlugin& aFilePlugin, RFs* aFsSession )
	{
	CFileMonitor* self = new ( ELeave ) CFileMonitor( aFilePlugin, aFsSession );
	CleanupStack::PushL(self);
	self->ConstructL(aFilePlugin, aFsSession);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CFileMonitor::CFileMonitor
// -----------------------------------------------------------------------------
//
CFileMonitor::CFileMonitor( CFilePlugin& aFilePlugin, RFs* aFsSession ) :
                            CActive(CActive::EPriorityStandard),
                            iFilePlugin( aFilePlugin )
    {
    OstTraceFunctionEntry0( CFILEMONITOR_CFILEMONITOR_ENTRY );
    CPIXLOGSTRING("ENTER CFileMonitor::CFileMonitor");
    CActiveScheduler::Add(this);
    iFsSession = aFsSession;
    CPIXLOGSTRING("END CFileMonitor::CFileMonitor");
    OstTraceFunctionExit0( CFILEMONITOR_CFILEMONITOR_EXIT );
    }

// -----------------------------------------------------------------------------
// CFileMonitor::~CFileMonitor
// -----------------------------------------------------------------------------
//
CFileMonitor::~CFileMonitor() 
	{
	CPIXLOGSTRING("ENTER ~CFileMonitor");
	Cancel();
	iEngine.Disable();
	iEngine.Close();
	delete iFolderRenamedHarvester;
	CPIXLOGSTRING("END ~CFileMonitor");	
	}

// -----------------------------------------------------------------------------
// CFileMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CFileMonitor::ConstructL( CFilePlugin& aFilePlugin, RFs* aFsSession )
    {
    iFolderRenamedHarvester = CFolderRenamedHarvester::NewL(aFilePlugin, *aFsSession);
    }

// -----------------------------------------------------------------------------
// CFileMonitor::RunError
// -----------------------------------------------------------------------------
//
TInt CFileMonitor::RunError( TInt aError )
	{
	OstTrace1( TRACE_NORMAL, CFILEMONITOR_RUNERROR, "CFileMonitor::RunError;aError=%d", aError );
	CPIXLOGSTRING2( "CFileMonitor::RunError %d", aError );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CFileMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CFileMonitor::DoCancel()
	{
	iEngine.NotificationCancel();
	}

// -----------------------------------------------------------------------------
// CFileMonitor::RunL
// -----------------------------------------------------------------------------
//	
void CFileMonitor::RunL()
    {
    OstTraceFunctionEntry0( CFILEMONITOR_RUNL_ENTRY );
    CPIXLOGSTRING("ENTER CFileMonitor::RunL");
    Deque();
    CActiveScheduler::Add( this );
    
    /*
    Handle pkg here
    It has event type and other stuff
    Execute event and pass params to FilePlugin
    */

    // TODO TFileName on stack twice - consider allocating on heap!
    TFastFindFSPStatus& status = iPckg();
    const TDesC& fileNameOld = status.iFileName;
    const TDesC& fileNameNew = status.iNewFileName;
    
    /*
    * IMPORTANT:
    * Rename of whole directory happens as change to every file in dir.
    * We get an event of every file in dir.
    */
    OstTrace1( TRACE_NORMAL, DUP1_CFILEMONITOR_RUNL, "CFileMonitor::RunL;status.iFileEventType=%d", status.iFileEventType );
    CPIXLOGSTRING2("CFileMonitor::RunL, status.iFileEventType: %i", status.iFileEventType );    
    switch(status.iFileEventType)
		{
		case EFastFindFileCreated:
			{
			OstTraceExt1( TRACE_NORMAL, DUP4_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileCreated old=%S", fileNameOld );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileCreated old = %S", &fileNameOld);
			OstTraceExt1( TRACE_NORMAL, DUP2_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileCreated new=%S", fileNameNew );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileCreated new = %S", &fileNameNew);
            // File creation (for example over PC suite) gives fileNameOld as the created files, fileNameOld is empty.
			if(iFilePlugin.GetHarvesterState())
			    {
			    iFilePlugin.CreateContentIndexItemL(fileNameOld, ECPixAddAction);
			    iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixAddAction, EFalse);
			    }
			else
			    {
			    iFilePlugin.AddToQueueL(fileNameOld, ECPixAddAction, EFalse);
			    }
			}
		break;
		
		case EFastFindFileModified:
			{
			// This event can be generated by some file manager applications 
			// when copying a file to a new destination.
			// Introduce a small delay, so that file copying can complete before
			// cpix opens the file for indexing. Without the delay, sometimes file 
			// indexing fails as CLucene cannot open the file for indexing.
			
			// Decided to block the CPiXHarvesterServer thread rather than introduce 
			// new active object state into the CFileMonitor.
			User::After(50000); // 0.05 seconds
			
			OstTraceExt1( TRACE_NORMAL, DUP3_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileModified old=%S", fileNameOld );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileModified old = %S", &fileNameOld);
			OstTraceExt1( TRACE_NORMAL, DUP5_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileModified new=%S", fileNameNew );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileModified new = %S", &fileNameNew);
			// File copy, fileNameOld contains the file name, fileNameNew is empty
			if(iFilePlugin.GetHarvesterState())
			    {
			    iFilePlugin.CreateContentIndexItemL(fileNameOld, ECPixUpdateAction);
			    iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixUpdateAction, EFalse);
			    }
			else
			    {
                iFilePlugin.AddToQueueL(fileNameOld, ECPixUpdateAction, EFalse);
                }			    
			}
		break;
		
		case EFastFindFileRenamed:
			{
			OstTraceExt1( TRACE_NORMAL, DUP6_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileRenamed old=%S", fileNameOld );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileRenamed old = %S", &fileNameOld);
			OstTraceExt1( TRACE_NORMAL, DUP7_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileRenamed new=%S", fileNameNew );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileRenamed new = %S", &fileNameNew);
			TEntry entry; 
			if ( iFsSession->Entry( fileNameNew, entry ) == KErrNone ) 
				{
				if ( !entry.IsDir() )
					{
					if (fileNameOld.Length()>0 && fileNameOld.Compare(fileNameNew)!=0)
						{
					    if (iFilePlugin.GetHarvesterState())
					        {
					        iFilePlugin.CreateContentIndexItemL(fileNameOld, ECPixRemoveAction);
					        iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixRemoveAction, EFalse);
					        }
					    else
					        iFilePlugin.AddToQueueL(fileNameOld, ECPixRemoveAction, EFalse);
						}
					if( iFilePlugin.GetHarvesterState())
					    {
					    iFilePlugin.CreateContentIndexItemL(fileNameNew, ECPixUpdateAction);
					    iFilePlugin.CreateFolderFileIndexItemL(fileNameNew, ECPixUpdateAction, EFalse);
					    }
					else
					    iFilePlugin.AddToQueueL(fileNameNew, ECPixUpdateAction, EFalse);
					}
				else
					{
					iFolderRenamedHarvester->StartL(fileNameOld, fileNameNew);
					}
				}
			}
		break;
		
		case EFastFindFileReplaced:
			{
			OstTraceExt1( TRACE_NORMAL, DUP8_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileReplaced old=%S", fileNameOld );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileReplaced old = %S", &fileNameOld);
			OstTraceExt1( TRACE_NORMAL, DUP9_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileReplaced new=%S", fileNameNew );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileReplaced new = %S", &fileNameNew);
			// File rename (funnily), fileNameOld contains the old file name, fileNameNew the new name
			if (fileNameOld.Length()>0 && fileNameOld.Compare(fileNameNew)!=0)
			    {
			    if( iFilePlugin.GetHarvesterState() )
			        {
			        iFilePlugin.CreateContentIndexItemL(fileNameOld, ECPixRemoveAction);
			        iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixRemoveAction, EFalse);
			        }
			    else
			        iFilePlugin.AddToQueueL(fileNameOld, ECPixRemoveAction, EFalse);
			    }
			if( iFilePlugin.GetHarvesterState() )
			    {
			    iFilePlugin.CreateContentIndexItemL(fileNameNew, ECPixUpdateAction);
			    iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixUpdateAction, EFalse);
			    }
			else
			    iFilePlugin.AddToQueueL(fileNameOld, ECPixUpdateAction, EFalse);
			}
		break;
				
		case EFastFindFileDeleted:
			{
			OstTraceExt1( TRACE_NORMAL, DUP10_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileDeleted old=%S", fileNameOld );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileDeleted old = %S", &fileNameOld);
			OstTraceExt1( TRACE_NORMAL, DUP11_CFILEMONITOR_RUNL, "CFileMonitor::RunL;EFastFindFileDeleted new=%S", fileNameNew );
			CPIXLOGSTRING2("CFileMonitor::RunL, EFastFindFileDeleted new = %S", &fileNameNew);
			// File delete, fileNameOld contains the name of the deleted file
			if( iFilePlugin.GetHarvesterState() )
			    {
			    iFilePlugin.CreateContentIndexItemL(fileNameOld, ECPixRemoveAction);
			    iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixRemoveAction, EFalse);
			    }
			else
			    iFilePlugin.AddToQueueL(fileNameOld, ECPixRemoveAction, EFalse);
			}
		break;
		
		case EFastFindDirCreated:
            {
            if (iFilePlugin.GetHarvesterState())
                iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixAddAction);
            else
                iFilePlugin.AddToQueueL(fileNameOld, ECPixAddAction, ETrue);
            }
            break;

		case EFastFindDirDeleted:
            {
            if(iFilePlugin.GetHarvesterState())
                iFilePlugin.CreateFolderFileIndexItemL(fileNameOld, ECPixRemoveAction);
            else
                iFilePlugin.AddToQueueL(fileNameOld, ECPixRemoveAction, ETrue);
            }
            break;
		default:
			{
			OstTraceExt1( TRACE_NORMAL, DUP12_CFILEMONITOR_RUNL, "CFileMonitor::RunL;unknown event old=%S", fileNameOld );
			CPIXLOGSTRING2("CFileMonitor::RunL, unknown event old = %S", &fileNameOld);
			OstTraceExt1( TRACE_NORMAL, DUP13_CFILEMONITOR_RUNL, "CFileMonitor::RunL;unknown event new=%S", fileNameNew );
			CPIXLOGSTRING2("CFileMonitor::RunL, unknown event new = %S", &fileNameNew);
			}
		break;
		}
    ResetStatus();
    iEngine.RegisterNotification(iPckg, iStatus);
    CPIXLOGSTRING("END CFileMonitor::RunL");
    OstTraceFunctionExit0( CFILEMONITOR_RUNL_EXIT );
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CFileMonitor::StartMonitoring
// -----------------------------------------------------------------------------
//
TBool CFileMonitor::StartMonitoring()
	{
	OstTraceFunctionEntry0( CFILEMONITOR_STARTMONITORING_ENTRY );
	CPIXLOGSTRING("ENTER CFileMonitor::StartMonitoring");
    
    if ( !IsActive() )
		{
		OstTrace0( TRACE_NORMAL, CFILEMONITOR_STARTMONITORING, "CFileMonitor::StartMonitoring - IF" );
		CPIXLOGSTRING("CFileMonitor::StartMonitoring - IF ");
		iEngine.Enable();
		ResetStatus();
	    iEngine.RegisterNotification( iPckg, iStatus );
	    SetActive();
		}
    
	CPIXLOGSTRING("END CFileMonitor::StartMonitoring");	
		
	OstTraceFunctionExit0( CFILEMONITOR_STARTMONITORING_EXIT );
	return ETrue;
	}

// ---------------------------------------------------------------------------
// CFileMonitor::ResetStatus
// ---------------------------------------------------------------------------
//
void CFileMonitor::ResetStatus()
    {
    OstTrace0( TRACE_NORMAL, CFILEMONITOR_RESETSTATUS, "CFileMonitor::ResetStatus" );
    CPIXLOGSTRING( "CFileMonitor::ResetStatus" );
    
    TFastFindFSPStatus& status = iPckg();
    
    status.iDriveNumber = 0;
    status.iFileEventType = EFastFindFileUnknown;
    status.iFileName.Zero();
    status.iNewFileName.Zero();
    status.iProcessId = TUid::Null();
    }

// -----------------------------------------------------------------------------
// CFileMonitor::Initialize
// -----------------------------------------------------------------------------
//	
TInt CFileMonitor::Initialize()
    {
    OstTraceFunctionEntry0( CFILEMONITOR_INITIALIZE_ENTRY );
    CPIXLOGSTRING("ENTER CFileMonitor::Initialize");
    TInt err = OpenEngine();
    OstTrace1( TRACE_NORMAL, CFILEMONITOR_INITIALIZE, "CFileMonitor::Initialize;OpenEngine=%d", err );
    CPIXLOGSTRING2("CFileMonitor::Initialize - OpenEngine: %i", err );
	if ( err != KErrNone )
		{
		OstTrace0( TRACE_NORMAL, DUP1_CFILEMONITOR_INITIALIZE, "CFileMonitor::Initialize if( err != KErrNone )" );
		CPIXLOGSTRING("CFileMonitor::Initialize if( err != KErrNone ) ");
		OstTraceFunctionExit0( CFILEMONITOR_INITIALIZE_EXIT );
		return err;
		}
	
	CPIXLOGSTRING("END CFileMonitor::Initialize");
	OstTraceFunctionExit0( DUP1_CFILEMONITOR_INITIALIZE_EXIT );
	return err;
    }

// -----------------------------------------------------------------------------
// CFileMonitor::Release
// -----------------------------------------------------------------------------
//
TInt CFileMonitor::Release()
    {
    iEngine.Disable();
	iEngine.Close();
	
	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CFileMonitor::OpenEngine
// -----------------------------------------------------------------------------
//
TInt CFileMonitor::OpenEngine()
    {
    TInt err = iEngine.Open( *iFsSession, KFastFindFSPluginPosition );
	return err;
    }
    
// -----------------------------------------------------------------------------
// CFileMonitor::AddNotificationPathsL
// -----------------------------------------------------------------------------
//
void CFileMonitor::AddNotificationPathsL( const TDriveNumber aDriveNumber )
    {
    TFileName notificationPath;
    TChar chr;
    User::LeaveIfError( RFs::DriveToChar( aDriveNumber, chr ) );

    notificationPath.Append( chr );
    notificationPath.Append( KExcludePathSystem );
    iEngine.AddIgnorePath( notificationPath );
	OstTraceExt1( TRACE_NORMAL, CFILEMONITOR_ADDNOTIFICATIONPATHSL, "CFileMonitor::AddNotificationPathsL;AddIgnorePath=%S", notificationPath );
	CPIXLOGSTRING2("CFileMonitor::AddNotificationPathsL - AddIgnorePath: %S", &notificationPath );

    notificationPath.Zero();

    // As index databases are located under \\Private\\ path,
    // this ignore path will mean index databases are also ignored.
    notificationPath.Append( chr );
    notificationPath.Append( KExcludePathPrivate );
    iEngine.AddIgnorePath( notificationPath );
    OstTraceExt1( TRACE_NORMAL, DUP1_CFILEMONITOR_ADDNOTIFICATIONPATHSL, "CFileMonitor::AddNotificationPathsL;AddIgnorePath=%S", notificationPath );
    CPIXLOGSTRING2("CFileMonitor::AddNotificationPathsL - AddIgnorePath: %S", &notificationPath );

    notificationPath.Zero();

    // Maps data must not be indexed
    notificationPath.Append( chr );
    notificationPath.Append( KExcludePathMapsCities );
    iEngine.AddIgnorePath( notificationPath );
    OstTraceExt1( TRACE_NORMAL, DUP2_CFILEMONITOR_ADDNOTIFICATIONPATHSL, "CFileMonitor::AddNotificationPathsL;AddIgnorePath=%S", notificationPath );
    CPIXLOGSTRING2("CFileMonitor::AddNotificationPathsL - AddIgnorePath: %S", &notificationPath );

    notificationPath.Zero();

    User::LeaveIfError( PathInfo::GetRootPath( notificationPath, aDriveNumber ) );
    iEngine.AddNotificationPath( notificationPath );
    OstTraceExt1( TRACE_NORMAL, DUP3_CFILEMONITOR_ADDNOTIFICATIONPATHSL, "CFileMonitor::AddNotificationPathsL;AddIgnorePath=%S", notificationPath );
    CPIXLOGSTRING2("CFileMonitor::AddNotificationPathsL - AddNotificationPath: %S", &notificationPath );
    }

// -----------------------------------------------------------------------------
// CFileMonitor::RemoveNotificationPaths
// -----------------------------------------------------------------------------
//
void CFileMonitor::RemoveNotificationPaths( const TDriveNumber aDriveNumber )
    {
    TFileName ignorePath;
    TChar chr;
    RFs::DriveToChar( aDriveNumber, chr );

    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathSystem );
    iEngine.RemoveIgnorePath( ignorePath );
    OstTraceExt1( TRACE_NORMAL, CFILEMONITOR_REMOVENOTIFICATIONPATHS, "CFileMonitor::RemoveNotificationPaths;RemoveIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileMonitor::RemoveNotificationPaths - RemoveIgnorePath: %S", &ignorePath );

    ignorePath.Zero();

    // As index databases are located under \\Private\\ path,
    // this ignore path will mean index databases are also ignored.
    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathPrivate );
    iEngine.RemoveIgnorePath( ignorePath );
    OstTraceExt1( TRACE_NORMAL, DUP1_CFILEMONITOR_REMOVENOTIFICATIONPATHS, "CFileMonitor::RemoveNotificationPaths;RemoveIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileMonitor::RemoveNotificationPaths - RemoveIgnorePath: %S", &ignorePath );

    ignorePath.Zero();

    // Maps
    ignorePath.Append( chr );
    ignorePath.Append( KExcludePathMapsCities );
    iEngine.RemoveIgnorePath( ignorePath );
    OstTraceExt1( TRACE_NORMAL, DUP2_CFILEMONITOR_REMOVENOTIFICATIONPATHS, "CFileMonitor::RemoveNotificationPaths;RemoveIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileMonitor::RemoveNotificationPaths - RemoveIgnorePath: %S", &ignorePath );

    ignorePath.Zero();

    PathInfo::GetRootPath( ignorePath, aDriveNumber );
    iEngine.RemoveNotificationPath( ignorePath );
    OstTraceExt1( TRACE_NORMAL, DUP3_CFILEMONITOR_REMOVENOTIFICATIONPATHS, "CFileMonitor::RemoveNotificationPaths;RemoveIgnorePath=%S", ignorePath );
    CPIXLOGSTRING2("CFileMonitor::RemoveNotificationPaths - RemoveNotificationPath: %S", &ignorePath );
    }

// -----------------------------------------------------------------------------
// CFileMonitor::Enable
// -----------------------------------------------------------------------------
//
TInt CFileMonitor::Enable()
    {
    return iEngine.Enable();
    }
    
// -----------------------------------------------------------------------------
// CFileMonitor::Disable
// -----------------------------------------------------------------------------
//
TInt CFileMonitor::Disable()
    {
    return iEngine.Disable();
    }

// End Of File
