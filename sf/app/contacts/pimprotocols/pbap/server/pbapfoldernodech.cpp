// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "pbapfoldernodech.h"
#include "pbapfolderclient.h"
#include "pbaperrorreporter.h"
#include "pbapappheader.h"
#include "pbapserver.h"
#include "pbapchview.h"
#include "pbapchexporter.h"

#include "btaccesshostlog.h"


// constants
_LIT(KFolderIch, "ich");
_LIT(KFolderOch, "och");
_LIT(KFolderMch, "mch");
_LIT(KFolderCch, "cch");


CFolderNodeCallHistory::CFolderNodeCallHistory(MVirtualFolderClient& aClient, const TDesC& aFolderName, THistoryType aHistoryType)
: CFolderNode(aClient, aFolderName), iHistoryType(aHistoryType), iPendingExport(ENoExport)
	{
	LOG_FUNC
	}

// must be called by any class that inherits from CFolderNodeCallHistory
void CFolderNodeCallHistory::ConstructL()
	{
	LOG_FUNC
	
	// create the view for this folder node
	iFolderView = CPbapChView::NewL(iClient.LogClient(), iHistoryType, *this, KLogNullFlags);	
	}
	
CFolderNodeCallHistory::~CFolderNodeCallHistory()
	{
	LOG_FUNC
	
	delete iAsyncExporter;
	delete iFolderView;
	delete iReadFolderView;
 	}


void CFolderNodeCallHistory::CancelGet()
	{
	LOG_FUNC
 	
	// cancel any asynchronous export operations and avoid iAsyncExporter is deleted
	// by itself
	CPbapChExporter* tmpExporter = iAsyncExporter;
	iAsyncExporter = NULL;
	delete tmpExporter;
	
	delete iAppHeader;
	iAppHeader = NULL;		
	}

void CFolderNodeCallHistory::GetComplete()
	{
	LOG_FUNC
	
	if (iAsyncExporter)
		{
		delete iAsyncExporter;
		iAsyncExporter = NULL;
		}
	}

TInt CFolderNodeCallHistory::DoExport(TExportType aExportType, TInt aHandle)
	{
	LOG_FUNC
	
	if(iAsyncExporter)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderChExportAlreadyExists));
		return KErrAlreadyExists;
		}
	

	TInt err = KErrNone;
		
	if ((aExportType == EExportItem) && (iCallHistoryChanged))
		{
		// the Call History db has been modified since the last get request
		// report precondition failed error to obex until next listing
		// request completes (or new Pbap session initiated)
		iClient.ErrorReporter().SendPreconditionFailedError();
		// error already reported above so don't update error

		delete iAppHeader;
		iAppHeader = NULL;
		}
	else if (iReadyToExport)
		{
		err = StartExport(aExportType, aHandle);
		}
	else
		{
		// views not ready, setup a pending export until they are
		if(iPendingExport != ENoExport)
			{
			__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderExportAlreadyPending));
			return KErrAlreadyExists;
			}

		if (!iFolderView)
			{
			// creating the folder view failed before, try again
			TRAP(err, iFolderView = CPbapChView::NewL(iClient.LogClient(), iHistoryType, *this, KLogNullFlags));
			}
		
		// setup this export as pending until we are ready to export
		if (err == KErrNone)
			{
			iPendingExport = aExportType;
			iPendingHandle = aHandle;
			}
		}
	return err;	
	}

TInt CFolderNodeCallHistory::DoGetItem(TInt aHandle)
	{
	LOG_FUNC
	return DoExport(EExportItem, aHandle);
	}
	
TInt CFolderNodeCallHistory::DoGetListing()
	{
	LOG_FUNC
	return DoExport(EExportListing);
	}

TInt CFolderNodeCallHistory::DoGetCount()
	{
	LOG_FUNC
	return DoExport(EExportCount);
	}

TInt CFolderNodeCallHistory::DoGetFolder()
	{
	LOG_FUNC
	return DoExport(EExportFolder);
	}

 void CFolderNodeCallHistory::HandleExportComplete(TInt aError)
 	{
 	LOG_FUNC
 		
 	if (aError == KErrNotFound && iAppHeader->Operation() == EPullVCard)
 		{
 		// must report not found to client if attempting to pull a vCard which
 		// doesnot exist
 		iClient.ErrorReporter().SendNotFoundError();
 		}
 	else if (aError && iCallHistoryChanged)
 		{
 		iClient.ErrorReporter().SendPreconditionFailedError();
 		}
 	else if (aError)
 		{
 		// error occured during export report service unavailable error to obex 
 		iClient.ErrorReporter().SendServiceUnavailableError();
 		}
 
  	// a successful listing request allows the ch entries to be pulled again
 	// after the database has been modified
 	if (aError == KErrNone && iAppHeader->Operation() == EPullVCardListing)
 		{
 		iCallHistoryChanged = EFalse;
 		}
	
	delete iAppHeader;
	iAppHeader = NULL;
 	}


void CFolderNodeCallHistory::CancelPendingExport(TInt err)
	{
	LOG_FUNC

	if (iPendingExport != ENoExport)
		{
		// complete the export with an error
		HandleExportComplete(err);
		
		// pending export done
		iPendingExport = ENoExport;
		}
	}

TInt CFolderNodeCallHistory::StartExport(TExportType aExportType, TInt aHandle)
	{
	LOG_FUNC
	
	if(iAsyncExporter)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderChExportAlreadyExists));
		return KErrAlreadyExists;
		}
	if(iReadyToExport == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderNotReadyToExport));
		return KErrNotReady;
		}
	if(!iFolderView)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderIncompleteViews));
		return KErrNotFound;
		}
	
	TInt err = KErrNone;
	
	switch(aExportType)
		{
		case EExportItem:
			// aHandle is decremented here as the PBAP handles for call histories start at 1 and our event index starts at 0
			TRAP(err, iAsyncExporter = CPbapChSingleItemExporter::NewL(iClient, *this, *iFolderView, aHandle - 1, iAppHeader->VCardVersion(), iAppHeader->Filter()));			
			break;

		case EExportListing:
			TRAP(err, iAsyncExporter = CPbapChListingExporter::NewL(iClient, *this, *iFolderView, iReadFolderView, iAppHeader->ListStartOffset(), iAppHeader->MaxListCount()));
			break;
		
		case EExportCount:
			TRAP(err, iAsyncExporter = CPbapChCountExporter::NewL(iClient, *this, *iFolderView, iReadFolderView));
			break;
			
		case EExportFolder:
			TRAP(err, iAsyncExporter = CPbapChItemExporter::NewL(iClient, *this, *iFolderView, iReadFolderView, iAppHeader->ListStartOffset(), iAppHeader->MaxListCount(), iAppHeader->VCardVersion(), iAppHeader->Filter()));
			break;
			
		case ENoExport:
			// nothing to do
			break;
		default:
			break;			
		}
	return err;
	}
	
// From MPbapChViewObserver.
void CFolderNodeCallHistory::CallHistoryViewReady(TInt aError)
	{
	LOG_FUNC

	if(aError != KErrNone)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderIncompleteViews));
		}

	// assume we are ready to export unless we determine otherwise
	iReadyToExport = ETrue;
	
	// make sure that we have all the view required to be able to export
	if ((aError == KErrNone) && (iHistoryType == EMissed) && (!iReadFolderView))
		{
		// create view of read missed events, used to determine the number of unread events
		TRAP(aError, iReadFolderView  = CPbapChView::NewL(iClient.LogClient(), iHistoryType, *this, KLogEventRead));

		if (aError == KErrNone)
			{
			// we will be notified again when the read missed events view is ready so wait for
			// that notification before we are ready to export
			iReadyToExport = EFalse;
			}
		else
			{
			// log error but still allow exports to take place, they will just not include the
			// new unread event count in any export
			LOG1(_L("Failed to create read missed call view, error: %d"), aError);
			}
		}
	else if (aError != KErrNone)
		{
		if (iReadFolderView)
			{
			// log error but still allow exports to take place, they will just not include the
			// new unread event count in any export
			LOG1(_L("Failed to create read missed call view, error: %d"), aError);
			
			delete iReadFolderView;
			iReadFolderView = NULL;
			}
		else
			{
			// log error and attempt to create views again on next export
			LOG1(_L("Failed to create folder call history view, error: %d"), aError);
			
			delete iFolderView;
			iFolderView = NULL;

			iReadyToExport = EFalse;
			CancelPendingExport(aError);
			}
		}
		
	// check for any pending exports
	if ((iReadyToExport) && (iPendingExport != ENoExport))
		{
		aError = StartExport(iPendingExport, iPendingHandle);
		
		if (aError != KErrNone)
			{
			// complete the export with an error
			HandleExportComplete(aError);
			}
	
		// pending export done
		iPendingExport = ENoExport;
		}
	}
	
void CFolderNodeCallHistory::CallHistoryChangeNotification(TBool aViewReady)
	{
	LOG_FUNC
	
	// Force the PCE to issue a PullvCardListing before we will respond to another
	// PullvCardEntry request for this folder
	iCallHistoryChanged = ETrue;

	// check to see if there is an export in progress
	if (iAsyncExporter)
		{
		// cancel the export and report error according to the error reporting
		// scheme for modified/deleted handles
		CancelGet();
		}
	
	// the view may need refreshing after a change and therefore we are ready to 
	// export if the view is ready to use. If not, then we will get notification
	// via the CallHistoryViewReady callback
	iReadyToExport = aViewReady;
	}


//
// CFolderNodeIch
//
/*static*/ CFolderNodeIch* CFolderNodeIch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	CFolderNodeIch* self = new (ELeave) CFolderNodeIch(aClient);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFolderNodeIch::CFolderNodeIch(MVirtualFolderClient& aClient)
: CFolderNodeCallHistory(aClient, KFolderIch(), EIncoming)
	{
	LOG_FUNC
	}	
	
	
//
// CFolderNodeOch
//
/*static*/ CFolderNodeOch* CFolderNodeOch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	CFolderNodeOch* self = new (ELeave) CFolderNodeOch(aClient);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFolderNodeOch::CFolderNodeOch(MVirtualFolderClient& aClient)
: CFolderNodeCallHistory(aClient, KFolderOch(), EOutgoing)
	{
	LOG_FUNC
	}
	
	
//
// CFolderNodeMch
//
/*static*/ CFolderNodeMch* CFolderNodeMch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	CFolderNodeMch* self = new (ELeave) CFolderNodeMch(aClient);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFolderNodeMch::CFolderNodeMch(MVirtualFolderClient& aClient)
: CFolderNodeCallHistory(aClient, KFolderMch(), EMissed)
	{
	LOG_FUNC
	}
	
			
//
// CFolderNodeCch
//
/*static*/ CFolderNodeCch* CFolderNodeCch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	CFolderNodeCch* self = new (ELeave) CFolderNodeCch(aClient);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFolderNodeCch::CFolderNodeCch(MVirtualFolderClient& aClient)
: CFolderNodeCallHistory(aClient, KFolderCch(), ECombined)
	{
	LOG_FUNC
	}
