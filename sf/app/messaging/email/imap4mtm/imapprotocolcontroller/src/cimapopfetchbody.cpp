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
// cimapopfetchbody.cpp
//

#include <imapset.h>
#include <mentact.h>

#include "cimapopfetchbody.h"
#include "cimapbodystructure.h"
#include "cimapenvelope.h"
#include "cimapfetchresponse.h"
#include "cfetchbodyinfo.h"
#include "cimapfetchbodyresponse.h"
#include "cimaprfc822headerfields.h"
#include "cimapmimeheaderfields.h"
#include "cimapsession.h"
#include "cimapsettings.h"
#include "cimapmailstore.h"
#include "cimaputils.h"
#include "cimaplogger.h"
#include <imcvtext.h>
#include <imcvcodc.h>
#include <imcvutil.h>
#include <cimcaf.h>
#include "cimapsyncmanager.h"
#include "cimapfolder.h"

#include <charconv.h>
#include <miuthdr.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"
#include "cimconvertheader.h"
#endif

_LIT8(KIMAP_NIL, "NIL");

// MIME message types
_LIT8(KMIME_TEXT, "TEXT");
_LIT8(KMIME_HTML, "HTML");
_LIT8(KMIME_XVCARD, "X-VCARD");
_LIT8(KMIME_VCALENDAR, "X-VCALENDAR");
_LIT8(KMIME_ICALENDAR, "CALENDAR");
_LIT8(KMIME_RTF, "RTF");
_LIT8(KMIME_NAME, "NAME");
_LIT8(KMIME_NAME_RFC2231, "NAME*");
_LIT8(KMIME_ATTACHMENT, "ATTACHMENT");
_LIT8(KMIME_FILENAME, "FILENAME");
_LIT8(KMIME_FILENAME_RFC2231, "FILENAME*");

// Encoding types
_LIT8(KMIME_BASE64, "BASE64");

_LIT8(KIMAP_REL_PATH_SEPARATOR, ".");

// Header fields to fetch when fetching the body structure.
_LIT8(KImapFetchLargeHeaderFields, "From Subject Reply-to To Cc Bcc Message-ID Return-Receipt-To X-Return-Receipt-To Disposition-Notification-To Disposition-Notification-Options");

// No longer fetch the following fields
// Received - date set on sync
// Date     - date set on sync
// Subject  - fetched during sync - copy from TMsvEmailEntry into CImHeader
// From		- fetched during sync - copy from TMsvEmailEntry into CImHeader

// Positive completion errors for FindFilenameL()
const TInt KErrRFC2231Encoded = 2;


// Efficient and SAFE way of comparing TBools which might have different integers representing TRUE
inline TBool BoolsAreEqual( TBool aA, TBool aB )
	{
	return ((aA && aB) || (!aA && !aB));
	}
	
inline TBool BoolsAreNotEqual( TBool aA, TBool aB )
	{
	return ((!aA || !aB) && (aA || aB));
	}

CImapOpFetchBody::CImapOpFetchBody( CImapSession*& aSession,
									CImapSyncManager& aSyncManager, 
					  				CMsvServerEntry& aServerEntry,
					  				CImapSettings& aImapSettings, 
					  				CImapMailStore& aMailStore)
: CImapOperation(aSession, aServerEntry, EPriorityStandard),
  iSyncManager(aSyncManager),
  iImapSettings(aImapSettings), iMailStore(aMailStore)
  
   	{
	
	}

CImapOpFetchBody::~CImapOpFetchBody()
	{
	Cancel();
	// Fetch Response objects
	delete iFetchResponse;
	delete iFetchBodyResponse;
	iFetchList.ResetAndDestroy();

	// CAF support
	delete iCaf;

	// Characterset conversion
	delete iHeaderConverter;
	delete iCharConv;
	delete iCharacterConverter;
	}

/**
Static construction for CImapOpFetchBody class.

@param aSession      connected IMAP Session object to use.
@param aServerEntry  access to the message entry array.
@param aImapSettings access to the settings for the IMAP Service.
@return the newly created CImapOpFetchBody object. The caller is responsible for
deletion.
*/
CImapOpFetchBody* CImapOpFetchBody::NewL(CImapSession*& aSession,CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aMailStore)
	{
	CImapOpFetchBody* self = new (ELeave) CImapOpFetchBody(aSession, aSyncManager, aServerEntry, aImapSettings, aMailStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CImapOpFetchBody::ConstructL()
	{
	// Caf utils
	iCaf = new (ELeave) CImCaf(CImapUtils::GetRef().Fs());
	
	// Create converter objects
	iCharacterConverter=CCnvCharacterSetConverter::NewL();
	iCharConv=CImConvertCharconv::NewL(*iCharacterConverter, CImapUtils::GetRef().Fs());
	iHeaderConverter=CImConvertHeader::NewL(*iCharConv); 
	
	// we assume that this message is MIME as we have no way of
	// detecting otherwise (without sending a new FETCH to the
	// server to get the MIME-Version).
	iHeaderConverter->SetMessageType(ETrue);
	
	CActiveScheduler::Add(this);
	}

/**
Fetches the specified bodypart and all parts that exist below it that match
the specified fetch criteria. Thus if called with the root of a message as
the specified part, then all message parts that satisfy the criteria will
be fetched, up to the entire message contents.

@param aStatus request status of the Proctocol Controller
@param aPart the ID in the local message store of the message part to fetch.
@param aGetPartialMailInfo: partial fetch settings.
*/
void CImapOpFetchBody::FetchBodyL(TRequestStatus& aRequestStatus, const TMsvId aPart, const TImImap4GetPartialMailInfo& aGetPartialMailInfo)
	{
	__LOG_TEXT(iSession->LogId(), "CImapOpFetchBody::FetchBodyL(): fetching message");
	
	if(IsActive())
	{
	    Cancel();
	}
	
	iRequestedPart=aPart;
	iGetPartialMailInfo=aGetPartialMailInfo;
	Queue(aRequestStatus);
	// sets the next state and sets active
	DoFetchL();
	}

/**
Initialises the fetch of the requested body parts.
If there is nothing to fetch (ie the client has requested a fetch of headers 
only, which are fetched during synchronise) the user request is completed.
Otherwise, if the body structure representation for the message already exists
in the message store, the tree structure is parsed to identify parts to fetch.
Otherwise, a request is issued for the required message headers and bodystructure.
*/
void CImapOpFetchBody::DoFetchL()
	{
 	// Reset stats
 	iBytesToDo=0;
 	iBytesDone=0;
 	iFetchList.ResetAndDestroy();
	iHtmlEntryPart = 0;
	iBodyTextSize = 0;
	iHtmlEntrySize = 0;
	iBodyPartRemainingSize = 0;
	iSizeOfToBeFetchedAttachments=0;
	iHasTextParts = 0;

	CheckForPartialPopulate();
	
	// if we only want headers then there is nothing to do as they
	// have already been fetched on synchronisation. We complete here.
	if(!iFetchPartialMail && iGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailHeaders)
		{
		__LOG_TEXT(iSession->LogId(), "CImapOpFetchBody:DoFetchL(): Nothing to fetch, completing");
		Complete(KErrNone);
		return;
		}

	// Get info on what is being fetching
	SetEntryL(iRequestedPart);
	
	// Find the root entry for the message this part belongs to.
	// No need to check the selected folder is correct - the 
	// compound operation checks this.
  	TBool messageFound = EFalse;
  	while(!messageFound)
		{
		// Reached the message ?
		if (iServerEntry.Entry().iType==KUidMsvMessageEntry)
			{
			messageFound = ETrue;
			TMsvEmailEntry entry = iServerEntry.Entry();
			iMessageUid  = entry.UID();
			iMessageMsvId  = entry.Id();
			
			// indicate that a fetch operation has been performed on this message.
			// This is used to prevent subsequent fetches during 2-phase sync.
			// ((TMsvEmailEntry&)iServerEntry.Entry()).
			entry.SetValidUID(ETrue);
			User::LeaveIfError(iServerEntry.ChangeEntry(entry));

  			if (!(iServerEntry.Entry().Owner()))
  				{
				// If there are no child entries then we need to fetch 
				// the envelope and body structure
  				FetchLargeHeaderL();
  				}
  			else
  				{
  				// If the structure is already present then build up an array of
  				// parts to be fetched from the data in the message server entry
  				// array and associated mime parts. Do this in the RunL
  				iCurrentStep=iNextStep=EBuildFetchList;
  				// complete self
  				SetActive();
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
  				}
   			}
		// Up a level
		SetEntryL(iServerEntry.Entry().Parent());
		} // end while(!messageFound)
	}

/**
Checks if the partial mail download parameters are set to default 
and the full download mail option is set, then this is a request for full download.
Allows for efficiency when identifying message parts to fetch.
*/
void CImapOpFetchBody::CheckForPartialPopulate()
	{
	if(iGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits &&
		iGetPartialMailInfo.iTotalSizeLimit == KMaxTInt &&
		iGetPartialMailInfo.iBodyTextSizeLimit == KMaxTInt && 
		iGetPartialMailInfo.iAttachmentSizeLimit == KMaxTInt && 
		(iGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailHeaders || 
		iGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailBodyText ||
		iGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailBodyTextAndAttachments ||
		iGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailAttachments ||
		iGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailBodyAlternativeText))
		{
		__LOG_TEXT(iSession->LogId(), "CImapOpFetchBody: Doing non-partial fetch");
		iFetchPartialMail = EFalse;
		}
	else
		{
		__LOG_TEXT(iSession->LogId(), "CImapOpFetchBody: Doing partial fetch");
		iFetchPartialMail = ETrue;
		}
	}

/**
Issues a request to IMAP Session to fetch the IMAP Header details and bodystructure.
*/
void CImapOpFetchBody::FetchLargeHeaderL()
 	{
  	// Issue the fetch request to the imap session
  	delete iFetchResponse;
  	iFetchResponse = NULL;
  	iFetchResponse = CImapFetchResponse::NewL();
  	iCurrentStep = EGetBodyStructure;
  	iNextStep = EProcessBodyStructure;
  	iSession->FetchBodyStructureAndHeadersL(iStatus, iMessageUid, KImapFetchLargeHeaderFields, *iFetchResponse);
  	SetActive();
   	}



/**
Called when asynchronous service requests complete.
Handles errors returned by aSynchronous services. 
Implements the state handling for message fetching.
*/
void CImapOpFetchBody::DoRunL()
	{
	// Handle any server errors
	if (iStatus.Int()!=KErrNone)
		{
		if (TInt err=ProcessServerError() != KErrNone)
			{
			Complete(err);
			return;
			}
		}
	
	iCurrentStep = iNextStep;

	switch (iCurrentStep)
		{
	case EProcessBodyStructure:
		{
		if (ProcessBodyStructureL())
			{
			iNextStep = EFetchFirstPart;
			// complete self
  			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		else
			{
			// No body structure. Message has probably been deleted from server but local
			// copy has not yet been removed.
			Complete(KErrNone);
			return;
			}
		break;
		}
	case EBuildFetchList:
		{
		DoBuildFetchListL();
		iCurrentStep = EFetchFirstPart;
		// falls through to fetch the first item in the fetch array.
		}
	case EFetchFirstPart:
		{
		// initialise progress counters:
		iPartsToDo=iFetchList.Count();
		iPartsDone=0;

 		if (FetchPartL())
 			{
 			//DBG((LogText(_L8("Starting body fetch of %d parts (%d bytes)"),
	 		iNextStep = EFetchNext;
 			}
 		else
 			{
 			// No parts identified for fetch - complete the user request.
 			Complete(KErrNone);
 			return;
 			}
	 	break;
		}
	case EFetchNext:
		{
		FetchPartCompleteL();
		
 		// Issue fetch for the next part in the array
		if (FetchPartL())
 			{
	 		iNextStep = EFetchNext;
 			}
 		else
 			{
 			// No more parts to fetch - complete the user request.
 			iCurrentStep=EFinished;
 			Complete(KErrNone);
 			return;
 			}
 		break;
		}
	default:
		{
		User::Leave(KErrNotSupported);
		}
		}

	if (!IsActive())
		{
		SetActive();
		}
	}

/**
Special form of Cancel(), which enables message currently being fetched to be resumed.
*/
void CImapOpFetchBody::CancelEnableResume()
	{
	Cancel();
	TRAP_IGNORE(ClearFetchAttemptedL());
	}

/** 
Clears the fetch-attempted flag (re-use of ValidUID flag) to indicate that the
cancelled fetch may be resumed according to download rules.
*/
void CImapOpFetchBody::ClearFetchAttemptedL()
	{
	SetEntryL(iMessageMsvId);
	TMsvEmailEntry entry = iServerEntry.Entry();
	entry.SetValidUID(EFalse);
	User::LeaveIfError(iServerEntry.ChangeEntry(entry));
	}

/**
Called by Cancel() to cancel asynchronous service requests
*/
void CImapOpFetchBody::DoCancel()
	{
	switch (iCurrentStep)
		{
	// States involving a session request
	case EGetBodyStructure:
	case EFetchFirstPart:
	case EFetchNext:
		{
		iSession->Cancel();
		break;
		}
	// Self completing or synchronous states	
	case EProcessBodyStructure:
	case EBuildFetchList:
	default:
		{
		break;
		}
		} // end of switch (iCurrentStep)
	CMsgActive::DoCancel();
	}

/**
Called when a user request is completed.
*/
#ifdef __IMAP_LOGGING
void CImapOpFetchBody::DoComplete(TInt& aErr)
#else
void CImapOpFetchBody::DoComplete(TInt& /*aErr*/)
#endif //__IMAP_LOGGING
	{
	__LOG_FORMAT((iSession->LogId(), "CImapOpFetchBody::DoComplete(aErr = %d) CurrentStep = %d", aErr, iCurrentStep));
	}

/**
Builds a list of parts to fetch from the server entry array.
This method is used when the mailstore representation of the bodystructure 
for the message has previously been constructed.
*/
void CImapOpFetchBody::DoBuildFetchListL()
	{
 	SetEntryL(iRequestedPart);
 	TMsvEmailEntry temp = (TMsvEmailEntry)iServerEntry.Entry();
 	TUid type=temp.iType;
 
 	// if we are not asking for a Message type then override the get
	// options to ensure that this is fetched
 	if(!iFetchPartialMail)
		{
		if (type != KUidMsvMessageEntry)
 			iGetPartialMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
		}
 				
 	// Build a list of parts to fetch: 
 	BuildFetchListL(iRequestedPart);

	UpdateBodyTextRemainingSizeForHtml();
	}


/**
Add the message part identified by aMessage and aMimeHeader to the array of 
parts to fetch.

Creates a CFetchBodyInfo object for each part to be fetched. This is the data
that the session requires to fetch each message part.

Note that this function should only be called when the part has been correctly
identified as a part for fetching.

@param aMessage     the TMsvEmailEntry for the part.
@param aMimeHeader  the mime header for the part.
@param aIsText	    bool indicating whether this is a text part.
@param aSizeToFetch	amount of data to fetch - this may indicate a partial fetch
                    if less than the total size of the part.
*/
void CImapOpFetchBody::AddFetchItemL(CImMimeHeader& aMimeHeader, TMsvEmailEntry& aMessage, TBool aIsText, TInt32 aSizeToFetch)
	{
	CFetchBodyInfo* fetchInfo=CFetchBodyInfo::NewLC(aMessage.Id());
	fetchInfo->SetSizeToFetch(aSizeToFetch);
	fetchInfo->SetBodyPartRemainingSize(aMessage.iBioType - aSizeToFetch);
	fetchInfo->SetRelativePathL(aMimeHeader.RelativePath()); 
	fetchInfo->SetIsText(aIsText);
	fetchInfo->SetContentTransferEncoding(aMimeHeader.ContentTransferEncoding());

	if (aIsText)
		{
		fetchInfo->SetCharsetId(aMimeHeader.MimeCharset());
		}
	else if (aMimeHeader.ContentType().MatchF(KImcvApplication) == 0)
		{
		// Set the CAF pointer if the attachment is an application.
		// This is not registered at this stage, this is done just
		// prior to issuing the fetch request. This allows a single
		// CImCaf instance to be re-used.
		fetchInfo->SetCaf(iCaf);
		}

	iFetchList.AppendL(fetchInfo);
	CleanupStack::Pop(fetchInfo);

	// Add this part's size to the size total
	iBytesToDo+=aMessage.iBioType;
	}

/**
Determines if an item is to be fetched, depending on the parttypes specified.
Items are also rejected if they have been marked complete due to having 0 
length. An attachment info object is created for attachment items rejected 
this way.

@param aMessage    the TMsvEmailEntry for the part.
@param aMimeHeader the mime header for the part.
@param aPartTypes  Get-Mail options specifying the parts to be fetched.
*/
void CImapOpFetchBody::AddFilterFetchItemL(CImMimeHeader& aMimeHeader, TMsvEmailEntry& aEntry, TImap4GetMailOptions& aPartTypes)
	{
	__LOG_FORMAT((iSession->LogId(), "CImapOpFetchBody::AddFilterFetchItemL(entry id = %x) using get mail options", aEntry.Id()));
	TUid type = aEntry.iType;
	
	// Defensive - nothing to fetch for folder and message entry types...
	if (type == KUidMsvFolderEntry || type == KUidMsvMessageEntry)
		{
		return;
		}
	
	// Part may be marked complete because it has 0 size,
	// therefore we don't need to fetch anything.
	if (aEntry.Complete() && !aEntry.PartialDownloaded())
		{
		__LOG_TEXT(iSession->LogId(), "  Skipping, already complete.");
		
		// If this is an attachment which has been marked complete because it has
  		// zero size, we still need to add it to the attachment manager.
 		if ((type == KUidMsvAttachmentEntry || type == KUidMsvEmailExternalBodyEntry) &&
		     (aEntry.iSize == 0))
			{
			CreateAttachmentInfoL(iServerEntry.Entry());
 			}
		return;
		}
	
	// All other entry types may require fetching
	TBool addPart = EFalse;
	TBool isText  = EFalse;
	
	TBool addingTextParts = (aPartTypes == EGetImap4EmailBodyText ||
							 aPartTypes == EGetImap4EmailBodyTextAndAttachments ||
							 aPartTypes == EGetImap4EmailBodyAlternativeText);
	
	if (type == KUidMsvEmailTextEntry || type == KUidMsvEmailHtmlEntry || type == KUidMsvEmailRtfEntry)
		{
		if (type == KUidMsvEmailTextEntry)
			{
			isText = ETrue;
			}

		iHasTextParts = ETrue;

		if (addingTextParts)
			{
			addPart = ETrue;
			}
		}
	else if (type == KUidMsvAttachmentEntry || type == KUidMsvEmailExternalBodyEntry)
		{
		if (aPartTypes == EGetImap4EmailBodyTextAndAttachments ||
			aPartTypes == EGetImap4EmailAttachments)
			{
			addPart = ETrue;
			}
		else
			{
			SetEntryL(aEntry.Parent());
			TImEmailFolderType folderType = static_cast<TMsvEmailEntry>((iServerEntry.Entry())).MessageFolderType();
			SetEntryL(aEntry.Id());
			
			if(folderType==EFolderTypeRelated && addingTextParts)
				{
				// if asked for bodytext and it is an attachment then fetch it if 
				// attachment is in a folder of type Multipart/Related as it is most 
				// likely part of an MHTML document
				addPart = ETrue;
				}
			else if( ( folderType == EFolderTypeAlternative || folderType == EFolderTypeUnknown )
			        && aPartTypes == EGetImap4EmailBodyAlternativeText)
				{
				// if non-HTML text alternative parts are requested, the alternative
				// folder is checked and get the mime content type for the part
				if(aMimeHeader.ContentType().CompareF(KMIME_TEXT)==0)
					{
					// This is a alternative text part, and should be treated
					// as a text part
					addPart = ETrue;
					}
				}

			// Create attachment info in the messaging store
			if(!addPart)
				{
				CreateAttachmentInfoL(iServerEntry.Entry());
				}		
			}
		}
	else
		{
		__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyUnknownMsvType));
		addPart = ETrue;
		}

	if (addPart)
		{
		AddFetchItemL(aMimeHeader, aEntry, isText, aEntry.iBioType);
		}
	}

/**
Determines if an item is to be fetched, depending on the partial mail 
options specified.
Items are also rejected if they have been marked complete due to having 0 
length. An attachment info object is created for attachment items rejected 
this way.

@param aMessage    the TMsvEmailEntry for the part.
@param aMimeHeader the mime header for the part.
@param aGetPartialMailInfo partial mail options describes parts to fetch.
*/
void CImapOpFetchBody::AddFilterFetchItemL(CImMimeHeader& aMimeHeader, TMsvEmailEntry& aEntry, TImImap4GetPartialMailInfo& aGetPartialMailInfo)
	{
	__LOG_FORMAT((iSession->LogId(), "CImapOpFetchBody::AddFilterFetchItemL(entry id = %x) using partial mail options", aEntry.Id()));
	TUid type = aEntry.iType;
	
	// Defensive - nothing to fetch for folder and message entry types...
	if (type == KUidMsvFolderEntry || type == KUidMsvMessageEntry)
		{
		return;
		}
	
	// Part may be marked complete because it has 0 size,
	// therefore we don't need to fetch anything.
	if (aEntry.Complete() && !aEntry.PartialDownloaded())
		{
		__LOG_TEXT(iSession->LogId(), "  Skipping, already complete.");
		
		// If this is an attachment which has been marked complete because it has
  		// zero size, we still need to add it to the attachment manager.
 		if ((aEntry.iType == KUidMsvAttachmentEntry || aEntry.iType == KUidMsvEmailExternalBodyEntry) 
 			&& (aEntry.iSize == 0))
			{
			CreateAttachmentInfoL(iServerEntry.Entry());
 			}
		return;
		}
	
	// All other entry types may require fetching
	TBool addPart = EFalse;
	TBool isText  = EFalse;
	TInt  sizeToFetch = aEntry.iBioType;
	
	if (type == KUidMsvEmailTextEntry)
		{
		iHasTextParts = ETrue;
		isText = ETrue;
		if(aGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits)
			{
			addPart = ETrue;
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == ECumulative)
			{
			if(iGetPartialMailInfo.iTotalSizeLimit > 0)
				{
				addPart = ETrue;
				sizeToFetch = Minimum(aEntry.iBioType, iGetPartialMailInfo.iTotalSizeLimit);
				iBodyTextSize = aEntry.iBioType;
				}
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == EBodyTextOnly ||
			 	 aGetPartialMailInfo.iPartialMailOptions == EBodyTextAndAttachments ||
				 aGetPartialMailInfo.iPartialMailOptions == EBodyAlternativeText )
			{
			addPart = ETrue;
			sizeToFetch = Minimum(aEntry.iBioType, iGetPartialMailInfo.iBodyTextSizeLimit);
			iBodyTextSize = aEntry.iBioType;
			}
		}
	else if (type == KUidMsvEmailHtmlEntry)
		{
		iHasTextParts = ETrue;
		iHtmlEntrySize = aEntry.iBioType;
		if(aGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits)
			{
			addPart = ETrue;
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == ECumulative)
			{
			if((iGetPartialMailInfo.iTotalSizeLimit > 0 ) && 
				((iBodyTextSize + aEntry.iBioType) <= iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				}
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == EBodyTextOnly ||
			 	 aGetPartialMailInfo.iPartialMailOptions == EBodyTextAndAttachments ||
				 aGetPartialMailInfo.iPartialMailOptions == EBodyAlternativeText )
				
			{	
			if(iBodyTextSize + aEntry.iBioType <=
				Minimum(iGetPartialMailInfo.iBodyTextSizeLimit,iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				}
			}
		// In case of html entry, store html entry id to check later,(when attaching partial footer 
		// message)if whole body text is downloaded and the html size is not to be downloaded 
		if(addPart)
			{
			iHtmlEntryPart = aEntry.Id();
			}
		}
	else if (type == KUidMsvAttachmentEntry || type == KUidMsvEmailExternalBodyEntry)
		{
		if(aGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits)
			{
			addPart = ETrue;
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == ECumulative)
			{
			if(iGetPartialMailInfo.iTotalSizeLimit > 0 && ((iBodyTextSize + iSizeOfToBeFetchedAttachments + aEntry.iBioType) <= iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				if((iBodyTextSize + iSizeOfToBeFetchedAttachments + aEntry.iBioType + iHtmlEntrySize) >= iGetPartialMailInfo.iTotalSizeLimit)
					{
					RemoveHtmlPart(iHtmlEntryPart);
					}
				iSizeOfToBeFetchedAttachments+=aEntry.iBioType;
				}	
			else
				{
				CreateAttachmentInfoL(aEntry);
				// for Ecumulative option ,after the body part downloading, check if there is any 
				// attachment which can be downloaded , then check if the html part can be included.
				if((iBodyTextSize + iSizeOfToBeFetchedAttachments + iHtmlEntrySize) >= iGetPartialMailInfo.iTotalSizeLimit)
					{
					RemoveHtmlPart(iHtmlEntryPart);
					}
				}
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == EAttachmentsOnly ||
				 aGetPartialMailInfo.iPartialMailOptions == EBodyTextAndAttachments)
			{
			if(aEntry.iBioType <= Minimum(iGetPartialMailInfo.iAttachmentSizeLimit,iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				}
			else
				{
				CreateAttachmentInfoL(aEntry);
				}
			}
		else
			{
			SetEntryL(iServerEntry.Entry().Parent());
			TImEmailFolderType folderType = static_cast<TMsvEmailEntry>((iServerEntry.Entry())).MessageFolderType();
			SetEntryL(aEntry.Id());
			
			if( folderType==EFolderTypeRelated )
				{
				// if asked for bodytext and it is an attachment then fetch it
				// if attachment is in a folder of Multipart/Related as it is 
				// most likely part of an MHTML document
				addPart = ETrue;
				}
			else if( folderType==EFolderTypeAlternative && 
					 aGetPartialMailInfo.iPartialMailOptions==EBodyAlternativeText &&
					 aEntry.iBioType <= Minimum(iGetPartialMailInfo.iAttachmentSizeLimit,
					 									 iGetPartialMailInfo.iTotalSizeLimit) )
				{
				// if non-HTML text alternative parts are requested, the alternative
				// folder is checked and get the mime content type for the part
				if( aMimeHeader.ContentType().CompareF(KMIME_TEXT)==0 )
					{
					// This is a alternative text part, and should be treated
					// as a text part
					addPart = ETrue;
					}
				}
				
			if(!addPart)
				{
				CreateAttachmentInfoL(aEntry);
				}
			}
		}
	else
		{
		__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyUnknownMsvType));

		// for anything else, if not debug mode then fetch anyway
		addPart = ETrue;
		}

	if (addPart)
		{
		AddFetchItemL(aMimeHeader, aEntry, isText, sizeToFetch);	
		}
	}

/**
Builds an array of message parts to fetch.
This is a function is recursive, calling itself for entry parts that have
children, thus building up an array of parts to fetch for the entire message.

This is a simple form of this function, used when performing a non-partial
fetch of the message.

@param aPart         The part to process
@param aPartTypes    Specifies which components to fetch
*/
void CImapOpFetchBody::BuildFetchListL(TMsvId aPart)
	{
	__LOG_FORMAT((iSession->LogId(), "CImapOpFetchBody::BuildFetchListL(%x)", aPart));

	// Is this part fetchable?
	SetEntryL(aPart);

	TUid type = iServerEntry.Entry().iType;
	
	// Folder and Message Entry entry types...
	if (type == KUidMsvFolderEntry || type == KUidMsvMessageEntry)
		{
		// Nothing to fetch at this entry - check child entries
		CMsvEntrySelection *selection=new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(selection);
		User::LeaveIfError(iServerEntry.GetChildren(*selection));
		
		__LOG_FORMAT((iSession->LogId(), "...ID %x has %d children", iServerEntry.Entry().Id(),selection->Count()));
		
		TInt count = selection->Count();	
		for(TInt a=0;a<count;++a)
			{
			// Process child
			BuildFetchListL((*selection)[a]);
			}
		CleanupStack::PopAndDestroy(selection);
		return;
		}


	// All other entry types may require fetching
	// load the mime header for the entry.
	// Information will be required for parts to be fetched.
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
	CMsvStore* store = iServerEntry.ReadStoreL();
	CleanupStack::PushL(store);
	mimeHeader->RestoreL(*store);
	// finished with the read store
	CleanupStack::PopAndDestroy(store);
	
	// get the entry for the part
	TMsvEmailEntry entry = (TMsvEmailEntry)iServerEntry.Entry();
	
	// Apply filters and add to fetch list
	if (iFetchPartialMail)
		{
		AddFilterFetchItemL(*mimeHeader, entry, iGetPartialMailInfo);
		}
	else
		{
		AddFilterFetchItemL(*mimeHeader, entry, iGetPartialMailInfo.iGetMailBodyParts);
		}

	// tidyup
	CleanupStack::PopAndDestroy(mimeHeader);
	}
	

	
/** 
Removes the html part from the download list if it exists in the list
Used when partial fetching.

@param aPart  ID of the HTML part to be removed from the fetch list
*/
void CImapOpFetchBody::RemoveHtmlPart(TMsvId aPart)
	{
	if(aPart)
		{
		TInt count = iFetchList.Count();
		for (TInt i=0 ; i<count ; ++i)
			{
			CFetchBodyInfo* info = iFetchList[i];
			if (info->PartId()==aPart)
				{
				iFetchList.Remove(i);
				delete info;
				break;
				}
			}
		}
	}

/**
Checks for the minimum size limit between message type size limit 
(attachment size limit/body text sizelimit)

@param aThisPartTypeSizeLimit the size limit for the part-type in question
@param aTotalMailSizeLimit    the total mail size limit
*/
TInt32 CImapOpFetchBody::Minimum(TInt32 aThisPartTypeSizeLimit,TInt32 aTotalMailSizeLimit)
	{
	if(aTotalMailSizeLimit > 0)
		{
		if(aThisPartTypeSizeLimit > aTotalMailSizeLimit)
			return aTotalMailSizeLimit;
		else
			return aThisPartTypeSizeLimit;
		}
	else
		return aThisPartTypeSizeLimit;
	}

/**
Entry point for Protocol Controller parsing of the received body structure
and message header information.

This method builds the a TMsvEntry tree representing the bodystructure.
This method and the methods it calls use CreateEntryBulk() and ChangeEntryBulk() 
in place of CreateEntry() and ChangeEntry().
This means that the TMsvEntry tree is not left half-built should a leave occur.
The tree is commited using CompleteBulk as the final step of the method.

@return ETrue if complete body structure exists, EFalse if not
*/
TBool CImapOpFetchBody::ProcessBodyStructureL()
	{
	// We expect to get the header fields and bodystructure in the response. If we
	// don't get them, then that probably indicates that the message we are fetching
	// is no longer on the server. If so, we should exit straight away.
	if ((iFetchResponse->HeaderFields() == NULL) ||
	    (iFetchResponse->BodyStructure() == NULL))
		{
		delete iFetchResponse;
		iFetchResponse = NULL;
		return EFalse;
		}

	SetEntryL(iRequestedPart);

	// defensive - check that the structure has not already been constructed
	if (!(iServerEntry.Entry().Owner()))
		{
		TMsvEmailEntry entry = iServerEntry.Entry();

		// Build a CImHeader object from the returned header fields data.
		CImHeader* imHeader = CImHeader::NewLC();
		PopulateImHeaderL(*(iFetchResponse->HeaderFields()), entry, *imHeader);
		User::LeaveIfError(iServerEntry.ChangeEntry(entry));   
		StoreHeadersL(imHeader, NULL);
		CleanupStack::PopAndDestroy(imHeader);
	
		TInt attachments=0;
		TInt relatedAttachments=0;
		TBool isMHTML=EFalse;
		iDecodedSizeOfAllParts = 0;

		// Create the message entry structure under the root message
		CImapBodyStructure* bodystructure = iFetchResponse->BodyStructure();
		BuildTreeL(entry.Id(),bodystructure,KNullDesC8,entry.Id(),attachments,isMHTML,relatedAttachments);

		UpdateBodyTextRemainingSizeForHtml();

		if(isMHTML==EFalse)
			{
			attachments+=relatedAttachments;
			}
			
		// recover server entry context
		SetEntryL(iRequestedPart);
		entry = iServerEntry.Entry();

		// Now that the structure has been created we can set the real message attributes.
		// The MHTML, attachment flags and size were estimated (hopefully correctly) when the envelope was downloaded.
		entry.iSize = iDecodedSizeOfAllParts;
		entry.SetMHTMLEmail(isMHTML);
		entry.SetAttachment(attachments);
		entry.SetICalendar(iIsICalendar);
		entry.SetVCalendar(iIsVCalendar);
		//Resetting the values
		iIsICalendar=EFalse;
		iIsVCalendar=EFalse;
		
		if( !iHasTextParts && entry.iType == KUidMsvMessageEntry )
			{
			// There are no text parts to this message - need to set body text 
			// complete flag to true otherwise UI may allow such a message to 
			// repeatedly be 'fetched' even though there is no text to fetch!
			//
			// So, set body text message complete flags on the entry
			__LOG_FORMAT((iSession->LogId(), "Message %d has no text parts - setting body text complete flag to ETrue", iRequestedPart));
			entry.SetBodyTextComplete(ETrue);
			}
		
		SetEntryL(entry.Id());
		User::LeaveIfError(iServerEntry.ChangeEntryBulk(entry));
		}

	// Commit the tree created during this method();
	iServerEntry.CompleteBulk();

	// No longer need the fetch response
	delete iFetchResponse;
	iFetchResponse = NULL;
	
	return ETrue;
	}


/**
Populates a CImHeader object with data returned in the header and bodystructure 
fetch.

@param aHeaderFields Header field data structure returned by the header fetch operation
@param aEntry        Message server entry for the message being fetched
@param aImHeader     Target CImHeader object to be populated
*/
void CImapOpFetchBody::PopulateImHeaderL(CImapRfc822HeaderFields& aHeaderFields, TMsvEmailEntry& aEntry, CImHeader& aImHeader)
	{
	// Subject, From 
	// Don't use the fields in aEntry as they may be in Unicode, which causes problems for
	// the call to iHeaderConverter->DecodeAllHeaderFieldsL() below - which expects 8 bit data
	aImHeader.SetSubjectL(aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapSubject));
	aImHeader.SetFromL(aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapFrom));

	// Return-Receipt-To, X-Return-Receipt-To	
	const TDesC8& returnReceipt = aHeaderFields.ReturnReceiptField();
	if (returnReceipt != KNullDesC8())
		{
		aImHeader.SetReceiptAddressL(returnReceipt);
		if (!aEntry.SeenIMAP4Flag())
			{
			aEntry.SetReceipt(ETrue);
			}
		}

	// Reply-to
	if (aHeaderFields.FieldExists(CImapRfc822HeaderFields::EImapReplyTo))
		{
		aImHeader.SetReplyToL(aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapReplyTo));
		}
	else
		{
		aImHeader.SetReplyToL(aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapFrom));
		}

	// Message-ID
	if(aHeaderFields.FieldExists(CImapRfc822HeaderFields::EImapMessageId))
		{
		aImHeader.SetImMsgIdL(aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapMessageId));
		}
	
	// To
	if(aHeaderFields.FieldExists(CImapRfc822HeaderFields::EImapTo))
		{
		ProcessAddressListL(aImHeader.ToRecipients(), aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapTo));
		}
	
	// Cc
	if(aHeaderFields.FieldExists(CImapRfc822HeaderFields::EImapCc))
		{
		ProcessAddressListL(aImHeader.CcRecipients(), aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapCc));
		}
	
	// Bcc
	if(aHeaderFields.FieldExists(CImapRfc822HeaderFields::EImapBcc))
		{
		ProcessAddressListL(aImHeader.BccRecipients(), aHeaderFields.FieldValue(CImapRfc822HeaderFields::EImapBcc));
		}
	
	// Decode any QP encoding in header fields
	iHeaderConverter->DecodeAllHeaderFieldsL(aImHeader);
	}


/**
Builds attachment tree for an email message.

Messaging stores email messages in the message store as a tree structure of
TMsvEntrys representing the mime structure of the message as described by the
BODYSTRUCTURE returned by the imap server. Multipart bodystructure parts are 
represented by an entry of type KUidMsvFolderEntry and TImEmailFolderType as
appropriate to the multipart type string. Content parts (text, attachments etc)
are built by method BuildContentEntryL() below. For each mime part an associated
CImMimeHeader object is created and streamed to the messaging store.

An array of parts to fetch according to message get-options is built up as the
bodystructure is parsed. The information gathered is that which the IMAP Session
requires to successfully fetch a specific message part.

@param aParent         The parent-part to be processed
@param aBodyStructure  Parsed bodystructure data for the current part
@param aPath           The IMAP relative path for message parts.
@param aThisMessage    TMsvId of the root entry for the message.
@param aAttachments    Counter for attachments.
@param aIsMHTML        Flag indicating that the message has been identified as MHTML
@param aRelatedAttachments Counter for related attachments. 
*/
void CImapOpFetchBody::BuildTreeL(TMsvId aParent, 
								  CImapBodyStructure* aBodyStructure, 
								  const TDesC8& aPath,
								  const TMsvId aThisMessage, 
								  TInt& aAttachments, 
								  TBool& aIsMHTML, 
								  TInt& aRelatedAttachments)
	{
	__LOG_FORMAT((iSession->LogId(), "CImapOpFetchBody::BuildTreeL(message=%x, parent=%x", aThisMessage, aParent));

	// Is this a content entry? (ie, not a multipart entry)
	if (aBodyStructure->BodyStructureType()!=CImapBodyStructure::ETypeMultipart)
		{
		// Build the content entry for the message part
		HBufC8* newpath=HBufC8::NewLC(aPath.Length()+4);
		*newpath=aPath;
		if (aPath.Length())
			newpath->Des().Append(KIMAP_REL_PATH_SEPARATOR);
		newpath->Des().AppendNum(1);
		BuildContentEntryL(aParent, aBodyStructure, newpath->Des(), aThisMessage, aAttachments, aIsMHTML, aRelatedAttachments);
		CleanupStack::PopAndDestroy(newpath);
		}
	// otherwise is a multipart entry
	else
		{
		// Nest down a level: create a folder entry
		SetEntryL(aParent);
		TMsvEmailEntry message;
		message.iMtm=KUidMsgTypeIMAP4;
		message.iServiceId=iImapSettings.ServiceId();
		message.iType=KUidMsvFolderEntry;
		message.iSize=0;
		message.SetComplete(EFalse);
		User::LeaveIfError(iServerEntry.CreateEntryBulk(message)); // bulk op completed at the end of ProcessBodyStructureL()

		__LOG_FORMAT((iSession->LogId(), "   Created attachment folder id %x as child of %x", message.Id(), aParent));

		aParent=message.Id();

		TPtrC8 multipart = aBodyStructure->SubType();
		// Got anything?
		if (multipart.Size()>0)
			{
			// Parse multipart type string, do this first so
			// information is available when parsing children
			TImEmailFolderType ft=EFolderTypeUnknown;
			if (multipart.CompareF(KImcvRelated)==0)
				{
				__LOG_TEXT(iSession->LogId(), "   Folder type MULTIPART/RELATED");
				ft=EFolderTypeRelated;
				}
			else if (multipart.CompareF(KImcvMixed)==0)
				{
				__LOG_TEXT(iSession->LogId(), "   Folder type MULTIPART/MIXED");
				ft=EFolderTypeMixed;
				}
			else if (multipart.CompareF(KImcvParallel)==0)
				{
				__LOG_TEXT(iSession->LogId(), "   Folder type MULTIPART/PARALLEL");
				ft=EFolderTypeParallel;
				}
			else if (multipart.CompareF(KImcvAlternative)==0)
				{
				__LOG_TEXT(iSession->LogId(), "   Folder type MULTIPART/ALTERNATIVE");
				ft=EFolderTypeAlternative;
				}
			else if (multipart.CompareF(KImcvDigest)==0)
				{
				__LOG_TEXT(iSession->LogId(), "   Folder type MULTIPART/DIGEST");
				ft=EFolderTypeDigest;
				}

			SetEntryL(aParent);
			
			// ...and save it
			TMsvEmailEntry folder=iServerEntry.Entry();
			folder.SetMessageFolderType(ft);
#if SET_RELATED_ID
			// Set message's iRelatedId to messageId
			folder.iRelatedId=folder.Id();
#endif
			User::LeaveIfError(iServerEntry.ChangeEntryBulk(folder));

			// Process each of the multi-parts...
			TInt subnr=1;
			TInt numParts=aBodyStructure->EmbeddedBodyStructureList().Count();
			for (TInt i=0;i<numParts;++i)
				{
				// Process item (doesn't use AllocL)
				HBufC8* newpath=HBufC8::NewLC(aPath.Length()+4);
				*newpath=aPath;
				if (aPath.Length())
					newpath->Des().Append(KIMAP_REL_PATH_SEPARATOR);
				newpath->Des().AppendNum(subnr++);
				BuildContentEntryL(aParent, 
								   aBodyStructure->EmbeddedBodyStructureList()[i],
								   newpath->Des(),
								   aThisMessage, 
								   aAttachments, 
								   aIsMHTML, 
								   aRelatedAttachments);
				CleanupStack::PopAndDestroy(newpath);
				}
			}
		}
	}



/**
Builds the TMsvEmailEntry and associated CImMimeHeader for an individual message content part. 

@param aParent         TMsvId of the parent of the current part. When called
                         initially, this is the root entry representing the message
@param aBodyStructure  Parsed bodystructure data for the current part
@param aPath           The IMAP relative path for message parts.
@param aThisMessage    TMsvId of the root entry for the message.
@param aAttachments    Counter for attachments.
@param aIsMHTML        Flag indicating that the message has been identified as MHTML
@param aRelatedAttachments Counter for related attachments. 
*/
void CImapOpFetchBody::BuildContentEntryL(const TMsvId aParent, CImapBodyStructure* aBodyStructure, const TDesC8& aPath, const TMsvId aThisMessage, TInt& aAttachments, TBool& aIsMHTML, TInt& aRelatedAttachments)
	{

	// First, is this actually an entry, or another level of nesting?
	if (aBodyStructure->BodyStructureType()==CImapBodyStructure::ETypeMultipart)
		{
		// Another level of nesting? Call BuildTreeL()
		BuildTreeL(aParent, aBodyStructure, aPath, aThisMessage, aAttachments, aIsMHTML, aRelatedAttachments);
		return;
		}

	// Skeleton for new entry
	SetEntryL(aParent);

	TFileName attachmentFilename;	// somewhere to store an attachment filename
	TMsvEmailEntry message;
	message.iSize=0;
	message.iMtm=KUidMsgTypeIMAP4;
	message.iServiceId=iImapSettings.ServiceId();
	message.SetUID(iMessageUid);
	message.SetValidUID(EFalse);  // ValidUID Flag used to indicate if the message has ever been fetched
	message.SetComplete(EFalse);

	// Save mime TYPE/SUBTYPE
	CImMimeHeader* mime=CImMimeHeader::NewLC();
	TPtrC8 type    = aBodyStructure->Type();
	TPtrC8 subtype = aBodyStructure->SubType();
	mime->SetContentTypeL(type);
	mime->SetContentSubTypeL(subtype);

	__LOG_FORMAT((iSession->LogId(), "   MIME: %S/%S", &type, &subtype));

	// initialise the data type to be an attachment
	message.iType=KUidMsvAttachmentEntry;
	
	// Store the relative path for the content part
	mime->SetRelativePathL(aPath);

	if (aBodyStructure->BodyStructureType()==CImapBodyStructure::ETypeText)
		{
		ProcessTextSubtypeL(aBodyStructure, *mime, message, aIsMHTML);
		}

	// Process the Parameter list
	ProcessParameterListL(aBodyStructure->ParameterList(), *mime, message, attachmentFilename);

	// ID: save it
	TPtrC8 id = aBodyStructure->BodyId();
	if (id.Length() > 0)
		{
		__LOG_FORMAT((iSession->LogId(), "   Content ID: %S", &id));
		mime->SetContentIDL(StripAngledBrackets(id));
		}

	// Description: save it
	TPtrC8 description = aBodyStructure->BodyDescription();
	if (description.Length() > 0)
		{
		__LOG_FORMAT((iSession->LogId(), "   Content Description: %S", &description));
		mime->SetContentDescriptionL(description);
		}

	// Encoding
	TPtrC8 encoding = aBodyStructure->BodyEncoding();
	mime->SetContentTransferEncodingL(encoding);
	__LOG_FORMAT((iSession->LogId(), "   Content Transfer Encoding: %S", &encoding));

	// Octets (encoded form)
	TInt actualsize;
	TLex8 lex(aBodyStructure->BodySizeOctets());
	lex.Val(actualsize);

    // Some servers gives a negative size value when the body text is empty. Need to reset this to
 	// zero to prevent corruption error later on.
 	if(actualsize < 0)
 		{
 		actualsize = 0;
 		}

	// Modify actualsize to show *decoded* size: this is basically the size of
	// this part, multiplied by 6/8 if it's BASE64 encoded. For all other
	// encodings, we leave the size as-is as there's no hard & fast rule
	// which can be applied.
	if (encoding.CompareF(KMIME_BASE64)==0)
		{
		message.iSize=(actualsize*6)/8;
		}
	else
		{
		message.iSize=actualsize;
		}

	// Add into total message size
	iDecodedSizeOfAllParts+=message.iSize;

	// Use iBioType message entry data member to store size on remote server
	message.iBioType=actualsize;

	//If any part of email (text/plain mime, text/html mime, attachment....) 
	// is empty then should not fetch it.
	if(actualsize == 0)
		{
		message.SetComplete(ETrue);
		}

	// mark attached emails as such
	if (aBodyStructure->BodyStructureType()==CImapBodyStructure::ETypeMessageRfc822)
		{
		// embedded message - marked as a message
		// NB needs to be set before ProcessExtendedFieldsL() can be called
		message.iType=KUidMsvMessageEntry;
		iDecodedSizeOfAllParts-=message.iSize;
		}

	// Process any extended fields
	ProcessExtendedFieldsL(aBodyStructure, *mime, message, attachmentFilename);

	// Now we're working on the type
	if (message.iType==KUidMsvMessageEntry)
		{
		BuildEmbeddedMessageL(aBodyStructure, *mime, message, aPath, aAttachments);
		}
	else
		{
		BuildNonMessageEntryL(aParent, *mime, message, aAttachments, aRelatedAttachments);
		}
	CleanupStack::PopAndDestroy(mime);
	__LOG_FORMAT((iSession->LogId(), "  BuildContentEntryL done: created id %x, attachments so far %d", message.Id(), aAttachments));
	}

/**
Sets flags in the TMsvEmailEntry according to the subtype for TEXT/xxx mime parts.

@param aBodyStructure bodystructure component being processed
@param aMime	(partially) parsed mime information
@param aMessage the email entry under construction
@param aIsMHTML Flag indicating that the message has been identified as MHTML
*/
void CImapOpFetchBody::ProcessTextSubtypeL(CImapBodyStructure* aBodyStructure, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TBool& aIsMHTML)
	{
	// text/rtf?
	if (aMime.ContentSubType().CompareF(KMIME_RTF)==0)
		{
		if (!(aBodyStructure->ExtDispositionName().CompareF(KMIME_ATTACHMENT)==0))
			{
			aMessage.iType=KUidMsvEmailRtfEntry;
			}	
		}
	// text/html?
	else if (aMime.ContentSubType().CompareF(KMIME_HTML)==0)
		{
		// If this is not an attachment (ie disposition field 
		// is not "attachment") then this is a MHTML Message.
		if (!(aBodyStructure->ExtDispositionName().CompareF(KMIME_ATTACHMENT)==0))
			{
			aMessage.iType=KUidMsvEmailHtmlEntry;
			aIsMHTML=ETrue;	
			}	
		}
	// text/x-vcard?
	else if (aMime.ContentSubType().CompareF(KMIME_XVCARD)==0)
		{
		// Set vCard flag in message
		aMessage.SetVCard(ETrue);
		aMessage.iType=KUidMsvEmailTextEntry;
		}
	// text/x-vcalendar
	else if (aMime.ContentSubType().CompareF(KMIME_VCALENDAR)==0)
		{
		// Set vCalendar flag in message
		aMessage.SetVCalendar(ETrue);
		iIsVCalendar = ETrue;
		aMessage.iType=KUidMsvEmailTextEntry;
		}
	// text/calendar
	else if (aMime.ContentSubType().CompareF(KMIME_ICALENDAR)==0)
		{
		// Set iCalendar flag in message
		aMessage.SetICalendar(ETrue);
		iIsICalendar = ETrue;
		aMessage.iType=KUidMsvEmailTextEntry;
		}
	else
		aMessage.iType=KUidMsvEmailTextEntry;
	}



/**
Populates the parameter lists of the CImMimeHeader object 

@param aParamList parameter list, retrieved from the bodystructure
@param aMime	  mime header object to be populated
@param aMessage   the email entry under construction
@param aAttachmentFilename updated with a file name, if one is found
*/
void CImapOpFetchBody::ProcessParameterListL(const CImapBodyStructure::RAttributeValuePairList& aParamList, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TFileName& aAttachmentFilename)
	{
	TUint charsetId = KUidMsvCharsetNone;

	TInt paramCount = aParamList.Count();
	if (paramCount > 0)
		{
		__LOG_TEXT(iSession->LogId(), "   Parameter list:");

		for(TInt i=0;i<paramCount;++i)
			{
			TPtrC8 param=aParamList[i].iAttribute;
			TPtrC8 value=aParamList[i].iValue;
			
			__LOG_FORMAT((iSession->LogId(), "    %S %S", &param, &value));

			aMime.ContentTypeParams().AppendL(param);
			aMime.ContentTypeParams().AppendL(value);

			// Have we come across a 'NAME' tuple? If so, force the MIME type of this
			// entry to be an attachment.
			if ((param.CompareF(KMIME_NAME)==0)
				|| (param.CompareF(KMIME_NAME_RFC2231)==0))
				{
				__LOG_TEXT(iSession->LogId(), "   Attachment filename found, therefore this is an attachment");

				FindFilenameDecodeL(aMime,aAttachmentFilename);
				StripIllegalCharactersFromFileName(aAttachmentFilename);
				aMessage.iDetails.Set(aAttachmentFilename);

				// If embedded message do not save as an attachment
				if (aMessage.iType!=KUidMsvMessageEntry)
					{
					aMessage.iType=KUidMsvAttachmentEntry;
					}
				}
			else if (param.CompareF(KImcvCharset) == 0)
				{
				// We have found a charset parameter tuple. Convert the value to a
				// charset ID for storage in the MIME headers.
				if (value.Length() > 0)
					{
					charsetId = iCharConv->GetMimeCharsetUidL(value);
					}
				}
			}
		}

	aMime.SetMimeCharset(charsetId);
	}

/**
Populates extended header fields in the CImMimeHeader object 

@param aBodyStructure bodystructure object returned by the imap session
@param aMime	 mime header object to be populated
@param aMessage  the email entry under construction
@param aAttachmentFilename updated with a file name, if one is found
*/
void CImapOpFetchBody::ProcessExtendedFieldsL(CImapBodyStructure* aBodyStructure, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TFileName& aAttachmentFilename)
	{
	// Add the diposition name as the first pair of parameters.
	TPtrC8 dispositionName = aBodyStructure->ExtDispositionName();
	if (dispositionName.Size() != 0)
		{
		aMime.SetContentDispositionL(dispositionName);	
		__LOG_FORMAT((iSession->LogId(), "    adding disp name: %S", &dispositionName));
		aMime.ContentDispositionParams().AppendL(dispositionName);
		aMime.ContentDispositionParams().AppendL(KNullDesC8);
		}
	
	// Now add the actual diposition parameters as name-value pairs
	const CImapBodyStructure::RAttributeValuePairList& dispParams = aBodyStructure->ExtDispositionParameterList();
	TInt dispParamsCount = dispParams.Count(); // Note that this will be 0 if there is no disposition available
	
	for (TInt i=0; i < dispParamsCount; ++i)
		{
		__LOG_FORMAT((iSession->LogId(), "    disp: %S %S", &dispParams[i].iAttribute, &dispParams[i].iValue));
		
		aMime.ContentDispositionParams().AppendL(dispParams[i].iAttribute);
		aMime.ContentDispositionParams().AppendL(dispParams[i].iValue);

		// Filename? If so, force this as an attachment
		if ((dispParams[i].iAttribute.CompareF(KMIME_FILENAME)==0)
		 || (dispParams[i].iAttribute.CompareF(KMIME_FILENAME_RFC2231)==0))
			{
			__LOG_TEXT(iSession->LogId(), "   Attachment filename found in extended fields.");
			FindFilenameDecodeL(aMime,aAttachmentFilename);
			StripIllegalCharactersFromFileName(aAttachmentFilename);
			aMessage.iDetails.Set(aAttachmentFilename);

			// If embedded message do not save as an attachment
			if (aMessage.iType!=KUidMsvMessageEntry)
				{
				aMessage.iType=KUidMsvAttachmentEntry;
				}
			}
		
		} // end for
	}




/**
Performs final parsing and construction of an embedded MESSAGE/RFC822 message

As an MESSAGE/RFC822 part, the structure will contain envelope info. This is
parsed via ProcessEnvelopeL to construct a CImHeader object for the embedded
message. This is streamed to file along with the mime header information for 
the message part.

The CImapBodyStructure referred to via aBodyStructure->iMultiParts is the body 
structure of the embedded message. This is an entire message-within-a-message 
and so gets treated as a whole new mail.

@param aBodyStructure  Parsed bodystructure data for the current MESSAGE/RFC822 part
@param aMime	 	   mime header object to be populated
@param aMessage  	   the email entry under construction
@param aPath           The IMAP relative path for message parts.
@param aAttachments    Counter for attachments.
*/
void CImapOpFetchBody::BuildEmbeddedMessageL(CImapBodyStructure* aBodyStructure, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, const TDesC8& aPath, TInt& aAttachments)
	{
	// Prepare a CImHeader object for the message,
	// update flags as appropriate on the message entry.
	CImHeader *messageheader=CImHeader::NewLC();
	ProcessEnvelopeL(messageheader, aMessage, aBodyStructure->GetRfc822EnvelopeStructureL());

	if(aBodyStructure->EmbeddedBodyStructureList().Count() == 0)
	    {
        aMessage.iType=KUidMsvAttachmentEntry;
	    }
	
	// Create message
	User::LeaveIfError(iServerEntry.CreateEntryBulk(aMessage)); // bulk op completed at the end of ProcessBodyStructureL()
	SetEntryL(aMessage.Id());

	// Store CImHeader and CImMimeHeader for the MESSAGE/RFC822
	StoreHeadersL(messageheader, &aMime);
	CleanupStack::PopAndDestroy(messageheader);

#if SET_RELATED_ID
	// Set message's iRelatedId to messageId
	TMsvEntry entryToChange(iServerEntry.Entry());
	entryToChange.iRelatedId=entryToChange.Id();
	ChangeEntryBulkL(entryToChange); // bulk op completed at the end of ProcessBodyStructureL()
#endif
	// Process the bodystructure for this embedded message...
	TInt attachments=0;
	TInt relatedAttachments;
	TBool isMHTML=EFalse;
	CImapBodyStructure* bodystructure = NULL;
	
	if(aBodyStructure->EmbeddedBodyStructureList().Count() > 0)
	    {
	    bodystructure = aBodyStructure->EmbeddedBodyStructureList()[0];
        }	
	if (bodystructure)
	    {
        BuildTreeL(aMessage.Id(), bodystructure, aPath, aMessage.Id(), attachments, isMHTML, relatedAttachments);
	    }
	
	__LOG_FORMAT((iSession->LogId(), "  Built embedded message id %x attachments %d MHTML %d", aMessage.Id(), attachments, isMHTML));

	// Save attachment and MHTML flags
	if (attachments>0 || isMHTML)
		{
		SetEntryL(aMessage.Id());
		TMsvEmailEntry thisMessage = iServerEntry.Entry();

		if (attachments>0)
			{
			thisMessage.SetAttachment(ETrue);
			}

		if (isMHTML)
			{
			thisMessage.SetMHTMLEmail(ETrue);
			}

		User::LeaveIfError(iServerEntry.ChangeEntryBulk(thisMessage));
		}

	// embedded messages are counted as attachments
	++aAttachments;
	}




/**
Performs final processing and construction of non-MESSAGE/RFC822 parts.
Stores the mime header information for the message part.

This method is called for all message parts that are not multi-part and
not message/rfc822. Thus, each part for which this method is called is
eligible for fetch. This method calls AddFilterFetchItem to build up
the array of parts to fetch.

@param aParent         TMsvId of the parent of the current part.
@param aMime	 	   mime header object to be populated
@param aMessage  	   the email entry under construction
@param aAttachments    Counter for attachments.
@param aRelatedAttachments Counter for related attachments. 
*/
void CImapOpFetchBody::BuildNonMessageEntryL(const TMsvId& aParent, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TInt& aAttachments, TInt& aRelatedAttachments)
	{
	// Some other type of entry... 
	SetEntryL(aParent);
		
	// save parent folder type
	TImEmailFolderType parentFolderType = ((TMsvEmailEntry)iServerEntry.Entry()).MessageFolderType();

	// set attachment and HTML flags on item
	if (aMessage.iType==KUidMsvAttachmentEntry)
		{
		aMessage.SetAttachment(ETrue);
		}

	if (aMessage.iType==KUidMsvEmailHtmlEntry)
		{
		aMessage.SetMHTMLEmail(ETrue);
		}

	// ensure there is a filename if it is a non-text item 
	if (aMessage.iType!=KUidMsvEmailTextEntry && aMessage.iDetails.Length() == 0)
		{
		// use iAttachmentName for temporary buffer
		GetDefaultFilenameL(iAttachmentName, aMessage, &aMime);
		aMessage.iDetails.Set(iAttachmentName);
		}
	
	// Create the Entry
	User::LeaveIfError(iServerEntry.CreateEntryBulk(aMessage)); // bulk op completed at the end of ProcessBodyStructureL()
	SetEntryL(aMessage.Id());
	
	__LOG_FORMAT((iSession->LogId(), "   Created attachment id %x as child of %x - type %d", aMessage.Id(), aParent, parentFolderType));

#if SET_RELATED_ID
	// Set message's iRelatedId to messageId
	TMsvEntry entryToChange(iServerEntry.Entry());
	entryToChange.iRelatedId=entryToChange.Id();
	ChangeEntryBulkL(entryToChange); // bulk op completed at the end of ProcessBodyStructureL()
#endif

	// Stream the MIME info out into the newly created attachment entry.
	StoreHeadersL(NULL, &aMime);

	// This entry is NOT an attachment in the following cases - 
	// 1)	This is an attachment whose parent is a MULTIPART/RELATED folder.
	//		In this case, this entry could be a image entity for an MHTML
	//		entry with the same parent.
	// 2)	This is an MHTML entry whose parent is a MULTIPART/ALTERNATIVE 
	//		folder. In this case, this entry is the MHTML alternative to a
	//		text entry with the same parent.
	// 3)	This is an MHTML entry whose parent is MESSAGE folder. In this
	//		case, the message is a simple MHTML message with no text 
	//		alternative or embedded image.
	// 4)	This is an MHTML entry whose parent is a MULTIPART/RELATED folder.
	//		In this case, this entry is the MHTML for the message.
	// 5)	This is an MHTML entry whose parent is a MULTIPART/MIXED folder.
	//		In this case, this entry is the MHTML for the message. It cannot
	//		be the attachment it self as then it would be of type attachment.
	// Therefore, an entry is only an attachment if is of type attachment and
	// its parent is not a MULTIPART/RELATED folder.
	if(aMessage.iType==KUidMsvAttachmentEntry && parentFolderType!=EFolderTypeRelated)
		{
		++aAttachments;
		}
	// if it is related we might want to include it if the message
	// turns out not to be MHTML
	else if (aMessage.iType==KUidMsvAttachmentEntry &&	parentFolderType==EFolderTypeRelated)
		{
		++aRelatedAttachments;
		}
	
	// Apply filters and add to fetch list
	if (iFetchPartialMail)
		{
		AddFilterFetchItemL(aMime, aMessage, iGetPartialMailInfo);
		}
	else
		{
		AddFilterFetchItemL(aMime, aMessage, iGetPartialMailInfo.iGetMailBodyParts);
		}
	}



/**
Issues a request to fetch the next item in the array of parts to fetch.
@return EFalse if no parts remaining in the array.
*/
TBool CImapOpFetchBody::FetchPartL()
	{
 	// Anything to do?
 	if (iFetchList.Count() <= 0)
 		{
 		return EFalse;
 		}
	else
		{
		// Create a new body response store to hold any response information
		// from the fetch command.
		delete iFetchBodyResponse;
		iFetchBodyResponse = CImapFetchBodyResponse::NewL();
		
		// If part to fetch was of type attachment, this must be registered
		// with the CAF framework. This is indicated in the FetchBodyInfo by
		// a non-NULL CAF pointer.
		if ((iFetchList[0])->Caf()!=NULL)
			{
			RegisterWithCafL(*(iFetchList[0]));
			}

		// UpdatingSeenFlags == ETrue means that we want to set the seen flag explicitly, 
		//						using CImapSession::StoreL(), so peek should be ETrue too.
		// UpdatingSeenFlags == EFalse meanse that we want the server to set the seen flag
		//						implicitly when the message is downloaded,
		//						- i.e. peek should be EFalse too.			
		TBool peek = iImapSettings.UpdatingSeenFlags();
		
		iSession->FetchBodyL(iStatus, iMessageUid, peek, *(iFetchList[0]), *iFetchBodyResponse);
 		}
 	return ETrue;
	}
	
	
/**
Registers the part with the CAF framework.

This method is called for all parts of type Application that are
to be fetched. CAF registration requires concatenated content-type 
and subtype. The type and subtype have previously been received 
and stored in the CImMimeHeader for the part.

If the part is not registered as a result of the call to 
CImCaf::RegisterL(), the FetchBodyInfo object is updated to show
this.

@param 	fetchInfo information about the part to be fetched.
*/
void CImapOpFetchBody::RegisterWithCafL(CFetchBodyInfo& fetchInfo)
	{		
	// load the mime header for the entry.
	// Information will be required for parts to be fetched.
	SetEntryL(fetchInfo.PartId());
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
	CMsvStore* store = iServerEntry.ReadStoreL();
	CleanupStack::PushL(store);
	mimeHeader->RestoreL(*store);
	// finished with the read store
	CleanupStack::PopAndDestroy(store);
	
	// Create buffer for concatenating. + 1 creates space for '/' 
	HBufC8* buf = HBufC8::NewLC(mimeHeader->ContentSubType().Length() + mimeHeader->ContentType().Length() + 1);
	TPtr8 ptr(buf->Des());
	ptr.Copy(mimeHeader->ContentType());
	ptr.Append(KImcvForwardSlash);
	ptr.Append(mimeHeader->ContentSubType());

	// Attempt to register with CAF - this may not succeed.
	iCaf->RegisterL(ptr);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(mimeHeader);
	
	// Clear the iCaf pointer in fetchInfo if CAF is not interested.
	if(!iCaf->Registered())
		{
		fetchInfo.ResetCaf();
		}
	}
	
	
/**
Populates a CImHeader object given a CImapEnvelope abject.
Updates the passed TMsvEntry with appropriate information (date, from and subject)

@param aHeader       Header information object to populate
@param aServerEntry        Server entry id associated with the envelope object
@param aImapEnvelope Parsed IMAP Envelope object.
*/
void CImapOpFetchBody::ProcessEnvelopeL(CImHeader* aHeader, TMsvEntry& aEntry, CImapEnvelope& aImapEnvelope)
	{
	__LOG_FORMAT((iSession->LogId(), "   Processing envelope data, id=%x", aEntry.Id()));

	TPtrC8  tptr;
	// Parse date information
	tptr.Set(aImapEnvelope.EnvDate());
	TImRfc822DateField date;
	date.ParseDateField(tptr, aEntry.iDate);

	// Subject in CImHeader (TMsvEntry is later on after post-processing)
	if (aImapEnvelope.EnvSubject().CompareF(KIMAP_NIL)!=0)
		{
		aHeader->SetSubjectL(aImapEnvelope.EnvSubject());
		}
	
	// From information: both in CImHeader and TMsvEntry
	if (aImapEnvelope.EnvFrom().Count() != 0)
		{
		__LOG_TEXT(iSession->LogId(), "   Processing 'From' information");
		HBufC16* fromAddr = aImapEnvelope.EnvFrom()[0].CreateAddressStringL();
		CleanupStack::PushL(fromAddr);
		aHeader->SetFromL(*fromAddr);
		CleanupStack::PopAndDestroy(fromAddr);
		}
	else
		{
		// No From information. Set blank
		aHeader->SetFromL(KNullDesC);
		}

	// Sender information is ignored.

	// ReplyTo information
	if (aImapEnvelope.EnvReplyTo().Count()!=0)
		{
		__LOG_TEXT(iSession->LogId(), "   Processing 'ReplyTo' information");
		HBufC16* replyToAddr = aImapEnvelope.EnvFrom()[0].CreateAddressStringL();
		CleanupStack::PushL(replyToAddr);
		aHeader->SetReplyToL(*replyToAddr);
		CleanupStack::PopAndDestroy(replyToAddr);
		}
	else
		{
		// No replyto. Use From info
		aHeader->SetReplyToL(aHeader->From());
		}

	// To information
	ProcessAddressListL(aHeader->ToRecipients(), aImapEnvelope.EnvTo());
	
	// CC list
	ProcessAddressListL(aHeader->CcRecipients(),aImapEnvelope.EnvCc());

	// BCC list
	ProcessAddressListL(aHeader->BccRecipients(),aImapEnvelope.EnvBcc());

	// Message-Id
	aHeader->SetImMsgIdL(StripAngledBrackets(aImapEnvelope.EnvMessageId()));

	// Decode any QP encoding in header fields
	iHeaderConverter->DecodeAllHeaderFieldsL(*aHeader);

	// Set from line in TMsvEntry
	if(aHeader->From().Length() > 0)
	    {
        aEntry.iDetails.Set(aHeader->From());
	    }
	if(aHeader->Subject().Length() > 0)
	    {
	// Set subject in TMsvEntry
	    aEntry.iDescription.Set(aHeader->Subject());
	    }

	__LOG_TEXT(iSession->LogId(), "   Finished processing envelope information");
	}


/**
Retrieves the default filename and appends the appropriate extension

@param  aWhere         Destination descriptor array
@param  aAddressArray  Source address array
*/
void CImapOpFetchBody::GetDefaultFilenameL(TDes& aName, const TMsvEmailEntry& aMessage, const CImMimeHeader* mime)
	{
	aName = iImapSettings.DefaultAttachmentName();

	// Add on appropriate extension
	if (aMessage.iType == KUidMsvEmailTextEntry)
		{
		aName.Append(KTextExtension);
		}
	else if(aMessage.iType == KUidMsvEmailRtfEntry)
		{
		aName.Append(KRtfExtension);	
		}
	else if (aMessage.MHTMLEmail())
		{
		aName.Append(KHtmlExtension);
		}
	else if (aMessage.VCard() || aMessage.VCalendar())
		{
		aName.Append(KVCardExtension);
		}
	else if (aMessage.ICalendar())
		{
		aName.Append(KICalExtension);
		}
	else if ( aMessage.iType == KUidMsvAttachmentEntry )
		{
 		if ( (mime->ContentSubType()==KImcvBmp) ||
			 (mime->ContentSubType()==KImcvGif) ||
			 (mime->ContentSubType()==KImcvJpeg) ||
			 (mime->ContentSubType()==KImcvTiff) ||
			 (mime->ContentSubType()==KImcvWav) )
			{
			// Copy the 8-bit ContentSubType into the 16-bit buf
			RBuf buf;
			buf.CleanupClosePushL();
			buf.CreateL(mime->ContentSubType().Length());
			
			buf.Copy(mime->ContentSubType()); // 16-bit <== 8-bit
			
			aName.Append(KImcvFullStop);
			aName.Append(buf);
			
			CleanupStack::PopAndDestroy(&buf);
			}
		}
	}

/**
Copies addresses from an array of addresses into a target descriptor array.
The source array is of the form returned by the CImapEnvelope class

@param  aWhere         Destination descriptor array
@param  aAddressArray  Source address array.
*/
void CImapOpFetchBody::ProcessAddressListL(CDesCArray& aWhere, const CImapEnvelope::RArrayTAddress& aAddressArray)
	{
	for(TInt i=0;i < aAddressArray.Count();++i)
		{
		HBufC16* address = aAddressArray[i].CreateAddressStringL();
		CleanupStack::PushL(address);
		aWhere.AppendL(address->Des());
		CleanupStack::PopAndDestroy(address);
		}
	}

/**
Copies addresses from a Source descriptor containing one or more addresses 
into a target descriptor array of individual addresses

@param  aWhere      Destination descriptor array
@param  aAddresses	Source descriptor containing one or more addresses.
*/
void CImapOpFetchBody::ProcessAddressListL(CDesCArray& aWhere, const TDesC8& aAddresses)
 	{
	TInt length(aAddresses.Length());
 	HBufC8* pBuf=HBufC8::NewLC(length);
 	TPtrC8 source(aAddresses.Ptr(), length);
 	const TUint8* ptr(source.Ptr());
 	const TUint8* lastCharPtr(ptr + source.Length() - 1);
 	TUint8 lookFor(0);
 	TInt count(0);
 	TBool finishedEntry(EFalse);
 
 	// get past white space
 	while(*ptr&&((*ptr==KImcvSP)||(*ptr==KImcvSemiColon))) ptr++;
 
 	// Entries are separated by commas or semicolons.
 	// Separators do not count if they appear within
 	// "", <>, () or embedded series of these, eg "(one, two)"
 	// so we need to keep track of these, including nesting.
 	while(*ptr && ptr <= lastCharPtr)
 		{
 		if(pBuf->Length()==0)
 			{
 			finishedEntry = EFalse;
 			}
 
 		switch(*ptr)
 			{
 			case KImcvLeftBracket:
 				if(lookFor==KImcvRightBracket)
 					{ // We've already had a "(", so now we need another one
 					count++;
 					}
 				else if(lookFor==0)
 					{ //We weren't looking for anything else, now we need to
 					lookFor = KImcvRightBracket;
 					count = 1;
 					}
 				// else we were already looking for something else, ignore this
 				break;
 			case KImcvLeftChevron:
 				if(lookFor==KImcvRightChevron)
 					{ //We've already had a "<", so now we need another one
 					count++;
 					}
 				else if(lookFor==0)
 					{ //We weren't looking for anything else
 					lookFor = KImcvRightChevron;
 					count = 1;
 					}
 				// else we were already looking for something else, ignore this
 				break;
 			case KImcvDoubleQuote:
 				if(lookFor==KImcvDoubleQuote)
 					{ // We already had a quote, so this matches it
 					lookFor = 0;
 					}
 				else if(lookFor==0)
 					{ //We weren't looking for anything else
 					lookFor = KImcvDoubleQuote;
 					}
 				// else we were already looking for something else, ignore this
 				break;
 			case KImcvRightBracket:
 			case KImcvRightChevron:
 				if(*ptr == lookFor)
 					{ //If we have found what we were looking for, decrease the count
 					count--;
 					if(count==0)
 						{ // Got everything, now we're not looking for anything
 						lookFor = 0;
 						}
 					// else keep looking for the same thing	again
 					}
 				// else we're looking for something else, ignore it
 				break;
 			case KImcvComma:
 			case KImcvSemiColon:
 				// If we're not looking for anything, we're finished
 				if (lookFor == 0)
 					finishedEntry = ETrue;
 				// else this comma or semicolon is part of a different token, ignore it
 				break;
 			}
 
 		if(!finishedEntry)
 			{
 			pBuf->Des().Append((TChar)*ptr);
 			// move to the next character
 			ptr++;
 			}
 		else
 			{
 			// that's it! store the address away
 			HBufC16* pBuf16 = HBufC16::NewLC(pBuf->Des().Length());
 			pBuf16->Des().Copy(pBuf->Des());
 			aWhere.AppendL( (HBufC16&) *pBuf16 );
 			CleanupStack::PopAndDestroy(pBuf16); // pBuf16
 			pBuf->Des().SetLength(0);
 			finishedEntry = EFalse; //Ready for next entry
 
 			// get past the separator
 			ptr++;
 
 			// get past white space (& any other separators)
 			while(*ptr && (*ptr==KImcvSP || *ptr==KImcvTab || *ptr==KImcvComma || *ptr==KImcvSemiColon)) ptr++;
 			}
 		}
 		// catch the last name in the list
 		if (pBuf)
 			{
 			TInt recipientLength(pBuf->Length());
 			if (recipientLength > 0)
 				{
 				HBufC16* pBuf16 = HBufC16::NewLC(recipientLength);
 				pBuf16->Des().Copy(*pBuf);
 				aWhere.AppendL(*pBuf16);
 				CleanupStack::PopAndDestroy(pBuf16); // pBuf16
 				}
 			}
 		CleanupStack::PopAndDestroy(pBuf); // pBuf
 	}
 	
	
/**
Searches mime information for a filename for an attachment. 
If not found, the default attachment name is used.
If found but encoded according to RFC2231, the default attachment name is used 
  with the appropriate file extension appended.
Otherwise the found filename is QP decoded and cropped to be within max filename size.

@param  aMimeInfo  The mime header associated with the attachment
@param  aFileName  Returns the filename, if found.
@return KErrNotFound       if no filename found
        KErrRFC2231Encoded if RFC2231 encoded filename found
        KErrNone           otherwise
*/
TInt CImapOpFetchBody::FindFilenameL(const CImMimeHeader& aMimeInfo, TPtrC8& aFilename)
	{
	// Look in content-type list
	const CDesC8Array& ctype=aMimeInfo.ContentTypeParams();

	__LOG_FORMAT((iSession->LogId(), "CImapOpFetchBody::FindFilename: Checking %d entries in content-type list", ctype.Count()));

	TInt tuple=0;
	TInt count = ctype.Count(); 
	while (tuple+1 < count) // we step in pairs, and use tuple and tuple+1, so tuple+1 needs to be less than count in order
		{

#ifdef PRINTING
		TPtrC8 t1 = ctype[tuple];
		TPtrC8 t2 = (tuple+1 < count) ? ctype[tuple+1] : KNullDesC8();
		__LOG_FORMAT((iSession->LogId(),"    [%S] [%S]", &t1, &t2));
#endif

		// Look for "name xxx"
		if (ctype[tuple].CompareF(KMIME_NAME)==0)
			{
			// Got it: report that we found it
			aFilename.Set(ctype[tuple+1]);

			// Check whether aFilename contains a meaningful file name
			RBuf8 buf;
			buf.CleanupClosePushL();
			buf.CreateL(aFilename);
			buf.Trim();
			if(buf.Length()==0)
				{
				CleanupStack::PopAndDestroy(&buf);
				return KErrNotFound;
				}

			CleanupStack::PopAndDestroy(&buf);
			return KErrNone;
			}
		else if (ctype[tuple].CompareF(KMIME_NAME_RFC2231)==0)
			{
			// Got it: report that we found it
			aFilename.Set(ctype[tuple+1]);
			return KErrRFC2231Encoded;
			}
		tuple+=2;
		}

	// Not found in the content type, try content disposition
	const CDesC8Array& cdisp=aMimeInfo.ContentDispositionParams();
	__LOG_FORMAT((iSession->LogId(), "CImapOpFetchBody::FindFilename: Checking %d entries in disposition list", cdisp.Count()));
	
	tuple=0;
	count = cdisp.Count();
	while(tuple+1 < count) // we step in pairs, and use tuple and tuple+1, so tuple+1 needs to be less than count in order
		{

#ifdef PRINTING
		TPtrC8 t1 = cdisp[tuple];
		TPtrC8 t2 = (tuple+1 < count) ? cdisp[tuple+1] : KNullDesC8();
		__LOG_FORMAT((iSession->LogId(),"disp    [%S] [%S]", &t1, &t2));
#endif

		// Look for "filename xxx"
		if (cdisp[tuple].CompareF(KMIME_FILENAME)==0)
			{
			// Got it: report that we found it
			aFilename.Set(cdisp[tuple+1]);
			return KErrNone;
			}
		else if (cdisp[tuple].CompareF(KMIME_FILENAME_RFC2231)==0)
			{
			// Got it: report that we found it
			aFilename.Set(cdisp[tuple+1]);
			return KErrRFC2231Encoded;
			}

		tuple+=2;
		}

	// Didn't find it
	return KErrNotFound;
	}


/**
Builds attachment filename, using data from the mime header information, if available.
If not found, the default attachment name is used.
If found but encoded according to RFC2231, the default attachment name is used 
  with the appropriate file extension appended.
Otherwise the found filename is QP decoded and cropped to be within max filename size.

@param  aMimeInfo  The mime header associated with the attachment
@return aFileName  The decoded filename for the attachment.
*/
void CImapOpFetchBody::FindFilenameDecodeL(const CImMimeHeader& aMimeInfo, TFileName& aFileName)
	{
	// Make an attachment name
	aFileName.Zero();

	TPtrC8 origFileName;

	// Look for filename in Content-Type list
	TInt err = FindFilenameL(aMimeInfo, origFileName);
	if (KErrNotFound == err)
		{
		// Fall back to simple "attachment" (language specific)
		aFileName=iImapSettings.DefaultAttachmentName();
		}
	else if (KErrRFC2231Encoded == err)
		{
		// A file name has been found but it is encoded (RFC2231)
		// Use the default file name but append the file extension so that its type can be recognised
		aFileName=iImapSettings.DefaultAttachmentName();
		TInt dotPos = origFileName.Length() - 1;
		TBool dotFound = EFalse;
		
		// Find the extension
		while ((dotPos != 0) && (!dotFound))
			{
			if (origFileName[dotPos] == '.')
				{
				dotFound = ETrue;
				// Extension found: append it to the filename
				TInt extensionLength = origFileName.Length() - dotPos;
				if ((aFileName.Length() + extensionLength) <= aFileName.MaxLength())
					{
					HBufC* extension = HBufC::NewLC(extensionLength);
					extension->Des().Copy(origFileName.Right(extensionLength));
					aFileName.Append(*extension);
					CleanupStack::PopAndDestroy(extension);
					}
				}
			--dotPos;
			} // end while ((dotPos != 0) && (!dotFound))
		}
	else
		{
		// Run it through the QP decoder
		HBufC *decoded=HBufC::NewLC(origFileName.Length());
		TPtr decoded_ptr(decoded->Des());

		// Decode filename from the header
		iHeaderConverter->DecodeHeaderFieldL(origFileName, decoded_ptr);
		
		__LOG_FORMAT((iSession->LogId(), "FindFilenameDecode: '%S' to '%S' ", &origFileName, &decoded_ptr));

		// Need to do a check on the filename length here.
		// If it is too long, set to the max possible, keeping extension.
		TFileName path;
		TInt fileNameLength = path.Length() + decoded_ptr.Length();
		if( fileNameLength > KMaxFileName)
			{
			TInt prefixLen = 0;
			// Crop the Old File Name
			TInt lengthToCrop = (fileNameLength - KMaxFileName) + prefixLen;
			// Use LocateReverse rather than TParsePtr as decoded_ptr may be > 256 chars
			TInt dot = decoded_ptr.LocateReverse( '.' );
			TPtrC extension = decoded_ptr.Mid(dot != KErrNotFound ? dot : decoded_ptr.Length());
			TInt newFileNameLength = decoded_ptr.Length() - extension.Length() - lengthToCrop;
			TPtrC newFileName=decoded_ptr.Left(newFileNameLength);

			// Create the New File Name (ie File Name & Extension)
			aFileName.Zero();
			aFileName.Append(newFileName);
			aFileName.Append(extension);
			}
		else
			{
			aFileName.Copy(decoded_ptr);
			}
		CleanupStack::PopAndDestroy(decoded);
		}
	}


/**
Replaces illegal characters in a filename with a default
@param aName  The filename to check.
*/
void CImapOpFetchBody::StripIllegalCharactersFromFileName(TDes16& aName)
    {
    TInt length=aName.Length();
    for(TInt index=0; index < length; ++index)
        {
        TUint charr=(TUint)aName[index];
        if(	charr == '*' || charr == '\\' || charr == '<' || charr == '>' ||
            charr == ':'  || charr == '"' || charr == '/' || charr == '|' ||
			charr == '?' || charr < ' ')
            {
            aName[index] = KImcvDefaultChar;
            }
        }
   }


/**
Synchronously stores IM Headers and Mime Headers
@param aImHeader   an IM header to store. May be NULL.
@param aMimeHeader a Mime part header to store. May be NULL.
*/
void CImapOpFetchBody::StoreHeadersL(CImHeader* aImHeader, CImMimeHeader* aMimeHeader)
	{
	CMsvStore* entryStore=iServerEntry.EditStoreL();
	CleanupStack::PushL(entryStore);
	if (aImHeader != NULL)
		{
		__LOG_FORMAT((iSession->LogId(), "   Streaming ImHeader info into id %x", iServerEntry.Entry().Id()));
		aImHeader->StoreL(*entryStore);
		}
	if (aMimeHeader != NULL)
		{
		__LOG_FORMAT((iSession->LogId(), "   Streaming MIME info into id %x", iServerEntry.Entry().Id()));
		aMimeHeader->StoreL(*entryStore);
		}
	entryStore->CommitL();
	CleanupStack::PopAndDestroy(entryStore);
	}

/**
Check if the partial download options mean that we will download a text plain
part, but the text HTML alternative will not be downloaded. In this case, we
need to make sure the footer message is displayed on the text plain part to
show that the text HTML part is not being downloaded.
*/
void CImapOpFetchBody::UpdateBodyTextRemainingSizeForHtml()
	{
	if ((iHtmlEntryPart == 0) && (iHtmlEntrySize > 0) && (iFetchList.Count() > 0))
		{
		for (TInt part(0); part < iFetchList.Count(); ++part)
			{
			if (iFetchList[part]->IsText())
				{
				iFetchList[part]->SetBodyPartRemainingSize(iFetchList[part]->BodyPartRemainingSize() + iHtmlEntrySize);
				return;
				}
			}
		}
	}

/** 
Static method that strips the given string of any enclosing angled brackets < >
@param aString the descriptor that will have its brackets removed
@return a descriptor that points into aString, excluding any enclosing angled brackets
*/
TPtrC8 CImapOpFetchBody::StripAngledBrackets(const TDesC8& aString)
// static method
	{
	TInt strLen = aString.Length();

	if (strLen>2 && aString[0]==KImcvLeftChevron && aString[strLen-1]==KImcvRightChevron)
		{
		return aString.Mid(1, strLen-2);
		}
	
	// If the string was not enclosed with angled brackets then just return the original string.
	return aString;
	}

/**
Creates an empty attachment to store the attachment infomation, for the case
where the attachment is not downloaded due to download limits.
@param aMsvEmailEntry The email entry the attachment is associate with.
*/
void CImapOpFetchBody::CreateAttachmentInfoL(const TMsvEntry& aMsvEmailEntry)
	{
	__LOG_TEXT(iSession->LogId(), "Creating zero length attachment");
	iMailStore.CreateAttachmentInfoL(aMsvEmailEntry.Id());
	}


/**
Called when a requsted part has been sucessfully fetched.
Updates flags on the TMsvEntry indicating that it is complete or
partially fetched as appropriate.

Note - deletes the 0th entry from the array of parts to fetch.
*/
void CImapOpFetchBody::FetchPartCompleteL()
	{
	CFetchBodyInfo* fetchInfo = iFetchList[0];
	CleanupStack::PushL(fetchInfo);
	iFetchList.Remove(0);
	SetEntryL(fetchInfo->PartId());
	TMsvEmailEntry message = iServerEntry.Entry();
	message.SetComplete(ETrue);

	// Store mime information returned from N.MIME

	CImapMimeHeaderFields* headerFields = iFetchBodyResponse->HeaderFields();

	if (headerFields != NULL)
		{
		CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
		CMsvStore* store = iServerEntry.ReadStoreL();
		CleanupStack::PushL(store);
		mimeHeader->RestoreL(*store);
		CleanupStack::PopAndDestroy(store);

		if (headerFields->FieldExists(CImapMimeHeaderFields::EImapContentBase))
			{
			mimeHeader->SetContentBaseL(headerFields->FieldValue(CImapMimeHeaderFields::EImapContentBase));
			}

		if (headerFields->FieldExists(CImapMimeHeaderFields::EImapContentLocation))
			{
			const TDesC8& fieldValue = headerFields->FieldValue(CImapMimeHeaderFields::EImapContentLocation);

			HBufC* decodedBuffer = HBufC::NewLC(fieldValue.Length());
			TPtr decodedPtr(decodedBuffer->Des());

			iHeaderConverter->DecodeHeaderFieldL(fieldValue, decodedPtr);
			mimeHeader->SetContentLocationL(decodedPtr);
			CleanupStack::PopAndDestroy(decodedBuffer);
			}

		StoreHeadersL(NULL, mimeHeader);
		CleanupStack::PopAndDestroy(mimeHeader);
		}

	TBool hasBodyText = message.iType == KUidMsvEmailTextEntry || message.iType == KUidMsvEmailHtmlEntry;
	TBool partiallyDownloaded = EFalse;

	if(iFetchPartialMail && 
	   fetchInfo->BodyPartRemainingSize() != 0 && 
	   message.iType == KUidMsvEmailTextEntry) 
		{
		message.SetPartialDownloaded(ETrue);
		partiallyDownloaded = ETrue;
		}
	else
		{
		message.SetPartialDownloaded(EFalse);
		}

	if (hasBodyText)
		{
		message.SetBodyTextComplete(ETrue);
		}		
	
	User::LeaveIfError(iServerEntry.ChangeEntry(message));


	// Checking the flags
	SetEntryL(iMessageMsvId);
	TMsvEmailEntry messageentry = iServerEntry.Entry();
	CImapFolder* folder = iSyncManager.GetFolderL(messageentry.Parent());
	
	if(folder!=NULL)
		{
		TMessageFlagInfo& flaginfo = iFetchBodyResponse->MessageFlagInfo();
	
		// Flags from the fetch message
	    TBool seen = flaginfo.QueryFlag(TMessageFlagInfo::ESeen);
		TBool answered = flaginfo.QueryFlag(TMessageFlagInfo::EAnswered);
		TBool flagged = flaginfo.QueryFlag(TMessageFlagInfo::EFlagged);
		TBool deleted = flaginfo.QueryFlag(TMessageFlagInfo::EDeleted);
		TBool draft = flaginfo.QueryFlag(TMessageFlagInfo::EDraft);
		TBool recent = flaginfo.QueryFlag(TMessageFlagInfo::ERecent);

		// Flags in the local message
		TBool oUnread, oSeen, oAnswered, oFlagged, oDeleted, oDraft, oRecent;
		messageentry.GetIMAP4Flags(oUnread, oSeen, oAnswered, oFlagged, oDeleted, oDraft, oRecent);

		// Are we configured to update the \seen flag on the server?
		if (iImapSettings.UpdatingSeenFlags())
			{
			// Make a note to update the servers \Seen flag if CHANGED on the client
			//  and different to the servers version
			if (BoolsAreEqual(messageentry.Unread(), seen) && BoolsAreEqual(oSeen, seen))
				{
				// The read flag has changed, but not on the server.  So this must be a local change.
				if (messageentry.Unread())
					{
					folder->AppendClearSeenL(messageentry.UID());
					}					
				else
					{
					folder->AppendSetSeenL(messageentry.UID());
					}					
				}
			}

		if (BoolsAreNotEqual(oSeen, seen) || 
			BoolsAreNotEqual(oAnswered, answered) || 
			BoolsAreNotEqual(oFlagged, flagged) || 
			BoolsAreNotEqual(oDeleted, deleted) || 
			BoolsAreNotEqual(oDraft, draft) || 
			BoolsAreNotEqual(oRecent, recent) )
			{
			messageentry.SetIMAP4Flags(oUnread, seen, (answered || oAnswered), flagged, deleted, draft, recent);
			}

		// update context
		SetEntryL(iMessageMsvId);
		User::LeaveIfError(iServerEntry.ChangeEntry(messageentry));	
		}


	// Finished with the fetch body response now
	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;

	PropagateCompleteFlagL(fetchInfo->PartId(), hasBodyText, partiallyDownloaded);
	
	// increment progress counters
	++iPartsDone;
	iBytesDone+=fetchInfo->BytesFetched();

	// fetch of part complete - delete the info
	CleanupStack::PopAndDestroy(fetchInfo);
	}



/**
Propagates flags indicating message completeness up the tree structure for the 
fetched message part. Popagates the Complete and Partial-Fetch status.

@param aId             ID of the message part that has just been fetched.
@param aDoBodyText     indicates that the part is text or contains text parts
@param aPartialFetched indicates that the part is partially fetched.
*/
void CImapOpFetchBody::PropagateCompleteFlagL(TMsvId aId, TBool aDoBodyText, TBool aPartialFetched)
	{
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	
	// get the siblings of this id
	SetEntryL(aId);
	TMsvId parent = iServerEntry.Entry().Parent();

	// finish if we've managed to reach the top
	if (parent == KMsvRootIndexEntryId)
		return;

	SetEntryL(parent);

	// finish if we've reached a service
	if (iServerEntry.Entry().iType == KUidMsvServiceEntry)
		{
		return;
		}

	User::LeaveIfError(iServerEntry.GetChildren(*selection));

	TBool complete=ETrue;
	TBool bodyTextComplete=ETrue;
	TBool partiallyFetched=EFalse;

	TBool related=((TMsvEmailEntry) iServerEntry.Entry()).MessageFolderType()==EFolderTypeRelated ? 
																					ETrue:EFalse;
	for (TInt i=0; i < selection->Count(); i++)
		{
		SetEntryL((*selection)[i]);
		if (!iServerEntry.Entry().Complete())
			{
			complete=EFalse;
			if((iServerEntry.Entry().iType==KUidMsvFolderEntry) && aPartialFetched)
				{
				complete=ETrue;
				}				
			// The current part is not complete so...
			// if it is either a text part or a HTML part then the body
			// text is marked as being incomplete.
			//
			// This code means that, if present, then both the text/plain
			// and text/html alternatives need to be downloaded before
			// the body text is marked as being complete.
			if ((iServerEntry.Entry().iType == KUidMsvEmailTextEntry)
				|| (iServerEntry.Entry().iType == KUidMsvEmailHtmlEntry ) || related )
				{
				if(aPartialFetched)
					{
					complete = ETrue;
					bodyTextComplete=ETrue;
					}
				else
					bodyTextComplete=EFalse;
				}

			break;
			}
		}
	
	CleanupStack::PopAndDestroy(selection);

	// if all the siblings were complete then make the parent
	// complete and continue up.
	if (complete || ((aDoBodyText || related) && bodyTextComplete))
		{
		SetEntryL(parent);
		TMsvEmailEntry entry = iServerEntry.Entry();

		// check whether parent is complete, this will prevent us
		// checking all the messages in a real folder as they will all
		// be initialised to Complete
		if (!entry.Complete())
			{
			if (complete || ((iServerEntry.Entry().iType==KUidMsvFolderEntry) && aPartialFetched))
				{
				entry.SetComplete(ETrue);
				}
								
			if(aPartialFetched)
				{	
				if((iServerEntry.Entry().iType != KUidMsvAttachmentEntry) &&
						(iServerEntry.Entry().iType != KUidMsvEmailExternalBodyEntry))
					{
					entry.SetPartialDownloaded(ETrue);
					}
				partiallyFetched = ETrue;
				}
			else
				{
				entry.SetPartialDownloaded(EFalse);
				partiallyFetched = EFalse;
				}
				
			entry.SetBodyTextComplete(ETrue);
			User::LeaveIfError(iServerEntry.ChangeEntry(entry));
		
			PropagateCompleteFlagL(parent, related|aDoBodyText,partiallyFetched);
			}
		else if (entry.PartialDownloaded())
			{
			entry.SetPartialDownloaded(EFalse);
			User::LeaveIfError(iServerEntry.ChangeEntry(entry));
			PropagateCompleteFlagL(parent, related|aDoBodyText,partiallyFetched);
			}
		}	
	}
	

/**
Populates the passed progress object with progress information on the 
current message fetch operation.
@param aGenericProgressn - progress information object to be populated.
*/
void CImapOpFetchBody::Progress(TImap4GenericProgress& aGenericProgress)
	{
	aGenericProgress.iPartsToDo = iPartsToDo;
	aGenericProgress.iPartsDone = iPartsDone;
	aGenericProgress.iBytesToDo = iBytesToDo;
	
	// iBytesDone is the byte count of completed parts 
	// - need increase this by the parts done on the current part,
	// if a fetch is outstanding.
	TInt32 tempBytesDone = iBytesDone;
	if ((  iCurrentStep == EFetchFirstPart || iCurrentStep == EFetchNext)
		&& iFetchList.Count()>0)
		{
		tempBytesDone += (iFetchList[0])->BytesFetched();
		}
	aGenericProgress.iBytesDone = tempBytesDone;
	}


/**
Handles server error responses according to current step

@return TInt error code for completion (if error fatal)
*/
TInt CImapOpFetchBody::ProcessServerError()
	{
	switch(iCurrentStep)
		{
	case EGetBodyStructure:
	case EProcessBodyStructure:
	case EBuildFetchList:
	case EFetchFirstPart:
	case EFetchNext:
	default:
		return (iStatus.Int());
		}
	// return KErrNone;
	}

