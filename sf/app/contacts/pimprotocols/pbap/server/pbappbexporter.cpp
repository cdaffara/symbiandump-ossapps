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

#include "pbappbexporter.h"
#include "pbapfolderclient.h"
#include "pbapexporter.h"
#include "pbaperrorreporter.h"
#include "pbapcontactdbviews.h"
#include "pbappbhandlecache.h"

#include "btaccesshostlog.h"


//
// CPbapPbExporter
//
// N.B. CPbapPbExporter is a one shot class.
CPbapPbExporter::CPbapPbExporter(MVirtualFolderClient& aClient,
								 MPbapExporterCallback& aCallback)
: CPbapExporterBase(aClient, aCallback)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}


CPbapPbExporter::~CPbapPbExporter()
	{
	LOG_FUNC
	Cancel();
	}


void CPbapPbExporter::DoCancel()
	{
	LOG_FUNC
	// export aborted
	iClient.Exporter().CancelExport();
	iCallback.HandleExportComplete(KErrCancel);
	}

		
void CPbapPbExporter::ConstructL()
	{
	LOG_FUNC
	iClient.Exporter().StartExport();
	Queue();
	}


void CPbapPbExporter::Queue()
	{
	LOG_FUNC
	if(!IsActive())
		{
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

		
void CPbapPbExporter::RunL()
	{
	LOG_FUNC
	
	// call derived class to export a single item
	TBool moreToExport = DoExportItemL();
	if (moreToExport)
		{
		// queue another export 
		Queue();
		}
	else
		{
		// all items exported
		iClient.Exporter().FinaliseExportL();
		iCallback.HandleExportComplete(KErrNone);
		}
	}

	
TInt CPbapPbExporter::RunError(TInt aError)
	{
	LOG_FUNC
	
	// an export step failed so cancel and report error
	iClient.Exporter().CancelExport();
	iCallback.HandleExportComplete(aError);
	return KErrNone;
	}

	
//
// CPbapPbListingExporter
//
/*static*/ CPbapPbListingExporter* CPbapPbListingExporter::NewL(MVirtualFolderClient& aClient, 
																MPbapExporterCallback& aCallback, 
																CPbapPbHandleCache& aHandles, 
																RIndexArray& aCacheIndexesToExport)
	{
	LOG_STATIC_FUNC
	CPbapPbListingExporter* self = new(ELeave) CPbapPbListingExporter(aClient, 
																	aCallback, 
																	aHandles, 
																	aCacheIndexesToExport);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CPbapPbListingExporter::CPbapPbListingExporter(MVirtualFolderClient& aClient, 
											MPbapExporterCallback& aCallback, 
											CPbapPbHandleCache& aHandles, 
											RIndexArray& aCacheIndexesToExport)
: CPbapPbExporter(aClient, aCallback), iHandles(aHandles), iCacheIndexesToExport(aCacheIndexesToExport)
	{
	LOG_FUNC
	}
							
TBool CPbapPbListingExporter::DoExportItemL()
	{
	LOG_FUNC
	
	TBool moreItemsToExport = EFalse;
	if (iCurrentIndex==0)
		{
		// add listing header
		iClient.Exporter().ExportListingBeginL();	
		}
		
	if (iCurrentIndex < iCacheIndexesToExport.Count())
		{
		// get the handle,contact id pair at the current handle cache index	
		TInt handleCacheIndex = iCacheIndexesToExport[iCurrentIndex];
		TContactItemId contactId = iHandles.ContactIdAtL(handleCacheIndex);
		TInt handle = iHandles.HandleAtL(handleCacheIndex);
		
		RBuf name;
		// get the name from the contact database view
		TRAP_IGNORE(name.Assign(iClient.ContactDbViews().GetContactNameFromIdL(contactId)));
		//if cannot read contact name, just export empty string instead
		name.CleanupClosePushL();
		iClient.Exporter().ExportListingEntryL(handle, name);
		CleanupStack::PopAndDestroy(&name);
		
		++iCurrentIndex;
		moreItemsToExport = ETrue;
		}
	else
		{
		// last item has been exported so add listing footer
		iClient.Exporter().ExportListingEndL();
		moreItemsToExport = EFalse;
		}
	return moreItemsToExport;
	}

	
//
// CPbapPbItemExporter
//
/*static*/ CPbapPbItemExporter* CPbapPbItemExporter::NewL(MVirtualFolderClient& aClient, 
														MPbapExporterCallback& aCallback, 
														CPbapPbHandleCache& aHandles, 
														TInt aStartIndex, 
														TInt aCount, 
														TVCardVersion aFormat, 
														TUint64 aFilter)
	{
	LOG_STATIC_FUNC
	CPbapPbItemExporter* self = new(ELeave) CPbapPbItemExporter(aClient, 
																aCallback, 
																aHandles, 
																aStartIndex, 
																aCount, 
																aFormat, 
																aFilter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}


CPbapPbItemExporter::CPbapPbItemExporter(MVirtualFolderClient& aClient, 
										MPbapExporterCallback& aCallback, 
										CPbapPbHandleCache& aHandles, 
										TInt aStartIndex, 
										TInt aCount, 
										TVCardVersion aFormat, 
										TUint64 aFilter)
: CPbapPbExporter(aClient, aCallback), 
iHandles(aHandles), 
iStartIndex(aStartIndex), 
iCount(aCount), 
iFormat(aFormat), 
iFilter(aFilter)
	{
	LOG_FUNC
	iCurrentIndex = iStartIndex;
	}

							
TBool CPbapPbItemExporter::DoExportItemL()
	{
	LOG_FUNC
	
	TBool moreItemsToExport = EFalse;
	if (iCurrentIndex < (iStartIndex + iCount))
		{
		// retrieve contact id associated with the handle at the current cache index
		TContactItemId contactId = iHandles.ContactIdAtL(iCurrentIndex);
		
		TInt handle = iHandles.HandleAtL(iCurrentIndex);
				
		if (handle == KOwnCardHandle && contactId == KNullContactId)
			{
			// the own card handle is not associated with a contact in the database
			// but we must still export a vCard, so create an empty one
			iClient.Exporter().ExportEmptyVCardL(iFormat);
			}
		else
			{
			// convert the contact to a vCard
			iClient.Exporter().ExportContactL(contactId, iFormat, iFilter);			
			}
		++iCurrentIndex;
		moreItemsToExport = ETrue;
		}
	return moreItemsToExport;
	}	


//
// CPbapPbCountExporter
//
/*static*/ CPbapPbCountExporter* CPbapPbCountExporter::NewL(MVirtualFolderClient& aClient, 
															MPbapExporterCallback& aCallback, 
															TInt aCount)
	{
	LOG_STATIC_FUNC
	CPbapPbCountExporter* self = new(ELeave) CPbapPbCountExporter(aClient, aCallback, aCount);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}


CPbapPbCountExporter::CPbapPbCountExporter(MVirtualFolderClient& aClient, 
										MPbapExporterCallback& aCallback, 
										TInt aCount)
: CPbapPbExporter(aClient, aCallback), iCount(aCount)
	{
	LOG_FUNC
	}

							
TBool CPbapPbCountExporter::DoExportItemL()
	{
	LOG_FUNC	
	iClient.Exporter().ExportPhonebookSizeL(iCount);
	return EFalse;
	}
