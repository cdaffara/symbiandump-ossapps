// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cntdb.h>

#include "pbapchexporter.h"
#include "pbapserver.h"
#include "pbapexporter.h"
#include "pbapfolderclient.h"
#include "pbapcontactdbviews.h"
#include "pbaplogeng.h"

#include "btaccesshostlog.h"


//
// CPbapChExporter
//
CPbapChExporter::CPbapChExporter(MVirtualFolderClient& aClient, 
								 MPbapExporterCallback& aCallback, 
								 CPbapChView& aViewToExport,
								 CPbapChView* aReadViewToExport, 
								 TUint aStartIndex,
								 TInt aCount)
: CPbapExporterBase(aClient, aCallback),
  iState(EIdle),
  iStartIndex(aStartIndex),
  iCount(aCount)
	{
	LOG_FUNC
	
	// iCurrentEventIndex starts from 0, because CLogView can only start from the first event.
	iCurrentEventIndex = 0;
	
	// get views to export
	iLogView = aViewToExport.LogView();
	
	// read view optional, used for missed calls only
	if (aReadViewToExport)
		{
		iReadEventView = aReadViewToExport->LogView();
		}
		
	CActiveScheduler::Add(this);
	}

void CPbapChExporter::ConstructL()
	{
	LOG_FUNC	

	iState = EStartExport;
	
	// complete ourselves to kick-off the async processing
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
		
	SetActive();
	}
	
CPbapChExporter::~CPbapChExporter()
	{
	LOG_FUNC
	
	Cancel();
	}

TBool CPbapChExporter::PrepareForFetchingL(TUint aNumberOfEvents)
	{
	LOG_FUNC

	// reset the number of items to be exported based on the view size
	iCount = Min(iCount, (aNumberOfEvents - iStartIndex));
	return (iCount > 0);
	}

void CPbapChExporter::StartExportL()
	{
	LOG_FUNC	

	iClient.Exporter().StartExport();
	
	// check to see if we need to export number of unread events
	if (iReadEventView)
		{
		iClient.Exporter().ExportNewMissedCallsL(iLogView->CountL() - iReadEventView->CountL());
		}
	
	TBool startFetching = PrepareForFetchingL(iLogView->CountL());
	if (startFetching && iLogView->FirstL(iStatus))
		{
		iState = EFetchingLogEvents;
		SetActive();
		}
	else
		{
		// no events to fetch
		FinaliseFetchLogEventsL();
		}		
	}

void CPbapChExporter::FetchLogEventsL()
	{
	LOG_FUNC
	
	// call derived class to process fetched event	
	TBool continueFetching = EventFetchedL(iLogView->Event(), iCurrentEventIndex);
	++iCurrentEventIndex;
	if (continueFetching && iLogView->NextL(iStatus))
		{
		SetActive();
		}
	else
		{
		// all events fetched
		FinaliseFetchLogEventsL();
		}				
	}
		
void CPbapChExporter::RunL()
	{
	LOG_FUNC
	
	User::LeaveIfError(iStatus.Int());
	
	switch (iState)
		{
		case EStartExport:
			StartExportL();
			break;
		case EFetchingLogEvents:
			FetchLogEventsL();
			break;
		default:
			break;
		}
	}

void CPbapChExporter::DoCancel()
	{
	LOG_FUNC

	iLogView->Cancel();

	// export aborted
	iClient.Exporter().CancelExport();
	iCallback.HandleExportComplete(KErrCancel);
	}


TInt CPbapChExporter::RunError(TInt aError)
	{
	LOG_FUNC
	
	// abort the export	
	iClient.Exporter().CancelExport();

	// notify observer
	iCallback.HandleExportComplete(aError);
	return KErrNone;
	}
	
void CPbapChExporter::FinaliseFetchLogEventsL()
	{
	LOG_FUNC
	
	// no event to fetch
	FetchingCompleteL();
	iClient.Exporter().FinaliseExportL();
	iCallback.HandleExportComplete(KErrNone);
	}
	
	
//
// CPbapChListingExporter
//
CPbapChListingExporter* CPbapChListingExporter::NewL(MVirtualFolderClient& aClient, 
													MPbapExporterCallback& aCallback, 
													CPbapChView& aViewToExport,
													CPbapChView* aReadViewToExport, 
													TUint aStartIndex, 
													TInt aCount)
	{
	LOG_STATIC_FUNC
	CPbapChListingExporter* self = 
				new(ELeave) CPbapChListingExporter(aClient, aCallback, aViewToExport,
												   aReadViewToExport, aStartIndex, aCount);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbapChListingExporter::CPbapChListingExporter(MVirtualFolderClient& aClient,
											   MPbapExporterCallback& aCallback,
											   CPbapChView& aViewToExport,
											   CPbapChView* aReadViewToExport, 
											   TUint aStartIndex,
											   TInt aCount)
: CPbapChExporter(aClient, aCallback, aViewToExport, aReadViewToExport, aStartIndex, aCount)
	{
	LOG_FUNC
	}

TBool CPbapChListingExporter::PrepareForFetchingL(TUint aNumberOfEvents)
	{
	LOG_FUNC
	// add listing header before first entry is exported
	iClient.Exporter().ExportListingBeginL();

	return CPbapChExporter::PrepareForFetchingL(aNumberOfEvents);
	}
	
TBool CPbapChListingExporter::EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex)
	{
	LOG_FUNC

	TBool moreItemsToExport = EFalse;
		
	if (aEventIndex < iStartIndex)
		{
		// skip this event
		moreItemsToExport = ETrue;
		}
	else if (aEventIndex < iStartIndex + iCount)
		{
		// export event as a listing entry
		TInt handle = aEventIndex + 1; // call history handles start from 1

		TContactItemId contactId = aEvent.Contact();
		
		if (contactId == KNullContactId && aEvent.RemoteParty() != KNullDesC)
			{
			// The S60 phonebook uses the remote party for the name, so if there isn't a name
			// there won't be a contact, so we only look up the contact if there is a Remote Party
			contactId = iClient.ExporterUtil().FindContactIdFromNumberL(aEvent.Number());
			}
		
		if (contactId != KNullContactId)
			{
			// a contact is associated with the log event so read its name from the database
			RBuf name;
			TRAPD(error , name.Assign(iClient.ContactDbViews().GetContactNameFromIdL(contactId)));
			if (error != KErrNone || !name.Size())
				{
				// cannot read contact name (e.g. the contact id doesnot exist
				// anymore) so fallback to using the log events phone number
				name.Assign(aEvent.Number().AllocL());
				}
			name.CleanupClosePushL();
			iClient.Exporter().ExportListingEntryL(handle, name);
			CleanupStack::PopAndDestroy(&name);

			}
		else
			{
			// no contact associated with log event so use number instead of name
			const TDesC& phoneNumber = aEvent.Number();
			iClient.Exporter().ExportListingEntryL(handle, phoneNumber);
			}			
		moreItemsToExport = ETrue;		
		}
	return moreItemsToExport;
	}

void CPbapChListingExporter::FetchingCompleteL()
	{
	LOG_FUNC	
	// no more listing entries to be exported so close listing
	iClient.Exporter().ExportListingEndL();	
	}

	
//
// CPbapChItemExporter
//
CPbapChItemExporter* CPbapChItemExporter::NewL(MVirtualFolderClient& aClient, 
											MPbapExporterCallback& aCallback,
										    CPbapChView& aViewToExport,
										    CPbapChView* aReadViewToExport, 
											TUint aStartIndex, 
											TInt aCount, 
											TVCardVersion aFormat, 
											TUint64 aFilter)
	{
	LOG_STATIC_FUNC
	CPbapChItemExporter* self = 
			new(ELeave) CPbapChItemExporter(aClient, aCallback, aViewToExport, aReadViewToExport,
											aStartIndex, aCount, aFormat, aFilter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbapChItemExporter::CPbapChItemExporter(MVirtualFolderClient& aClient,
										 MPbapExporterCallback& aCallback,
									     CPbapChView& aViewToExport,
									     CPbapChView* aReadViewToExport, 
										 TUint aStartIndex,
										 TInt aCount,
										 TVCardVersion aFormat,
										 TUint64 aFilter)
: CPbapChExporter(aClient, aCallback, aViewToExport, aReadViewToExport, aStartIndex, aCount),
iFormat(aFormat),
iFilter(aFilter)
	{
	LOG_FUNC
	}
		
TBool CPbapChItemExporter::EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex)
	{
	LOG_FUNC

	TBool moreItemsToExport = EFalse;

	if(aEventIndex < iStartIndex)
		{
		// skip this event
		moreItemsToExport =ETrue;
		}
	else if (aEventIndex < iStartIndex + iCount)
		{
		// export the event
		iClient.Exporter().ExportCallHistoryL(aEvent, iFormat, iFilter);
		moreItemsToExport = ETrue;
		}
	return moreItemsToExport;
	}

void CPbapChItemExporter::FetchingCompleteL()
	{
	LOG_FUNC
	// do nothing
	}

//
// CPbapChSingleItemExporter
//
CPbapChSingleItemExporter* CPbapChSingleItemExporter::NewL(MVirtualFolderClient& aClient,
														   MPbapExporterCallback& aCallback,
													       CPbapChView& aViewToExport,
														   TInt aIndex,
														   TVCardVersion aFormat,
														   TUint64 aFilter)
	{
	LOG_STATIC_FUNC
	CPbapChSingleItemExporter* self =
		new(ELeave) CPbapChSingleItemExporter(aClient, aCallback, aViewToExport, aIndex, aFormat, aFilter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CPbapChSingleItemExporter::CPbapChSingleItemExporter(MVirtualFolderClient& aClient,
													 MPbapExporterCallback& aCallback,
													 CPbapChView& aViewToExport,
													 TInt aIndex,
													 TVCardVersion aFormat,
													 TUint64 aFilter)
: CPbapChExporter(aClient, aCallback, aViewToExport), iIndex(aIndex), iFormat(aFormat), iFilter(aFilter)
	{
	LOG_FUNC
	}

TBool CPbapChSingleItemExporter::PrepareForFetchingL(TUint aNumberOfEvents)
	{
	LOG_FUNC
	if (iIndex < 0 || iIndex >= aNumberOfEvents)
		{
		// attempting to export an event which doesnot exist 
		User::Leave(KErrNotFound);
		}
	return ETrue;	
	}
		
TBool CPbapChSingleItemExporter::EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex)
	{
	LOG_FUNC

	if (aEventIndex == iIndex)
		{
		iClient.Exporter().ExportCallHistoryL(aEvent, iFormat, iFilter);
		return EFalse;
		}
	return ETrue;
	}

void CPbapChSingleItemExporter::FetchingCompleteL()
	{
	LOG_FUNC
	// do nothing
	}
	
//
// CPbapChCountExporter
//
CPbapChCountExporter* CPbapChCountExporter::NewL(MVirtualFolderClient& aClient,
												 MPbapExporterCallback& aCallback,
											     CPbapChView& aViewToExport,
											     CPbapChView* aReadViewToExport) 
	{
	LOG_STATIC_FUNC
	CPbapChCountExporter* self = new(ELeave) CPbapChCountExporter(aClient, aCallback,
																  aViewToExport, aReadViewToExport);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbapChCountExporter::CPbapChCountExporter(MVirtualFolderClient& aClient,
										   MPbapExporterCallback& aCallback,
									       CPbapChView& aViewToExport,
									       CPbapChView* aReadViewToExport) 
										   
: CPbapChExporter(aClient, aCallback, aViewToExport, aReadViewToExport)
	{
	LOG_FUNC
	}

TBool CPbapChCountExporter::PrepareForFetchingL(TUint aNumberOfEvents)
	{
	LOG_FUNC

	// the size of the phonebook is equal to the number of events in the log view
	iClient.Exporter().ExportPhonebookSizeL(aNumberOfEvents);
	
	// the phonebook size has been exported, so abort fetch
	return EFalse;
	}
	
TBool CPbapChCountExporter::EventFetchedL(const CLogEvent& /*aEvent*/, TInt /*aEventIndex*/)
	{
	LOG_FUNC
	//Release build should silently ignore the event
	__ASSERT_DEBUG(EFalse, Panic(EPbapChCountExporterFetchingFails));
	// do nothing (fetching should have been aborted)
	return EFalse;
	}

void CPbapChCountExporter::FetchingCompleteL()
	{
	LOG_FUNC
	// do nothing
	}
