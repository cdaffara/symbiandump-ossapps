// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MIUTRSLV.CPP
//

#include "MIUTRSLV.H"
#include <msvuids.h>
#include "MIUTMSG.H"
#include "MIUT_ERR.H"

#include <cmsvmimeheaders.h>
#include <mmsvattachmentmanager.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cimmessagepart.h"
#include "miut_errconsts.h"
#include "timrfc822datefield.h"
#endif

_LIT(KImMhtmlCidString, "cid");
_LIT8(KImMhtmlStartString, "start");


// This class implements the URI resolving process described in RFC 2110.
// Also see RFC 2112 and 1808.

// MHTML messages may contain multiple HTML body parts. Each of these body parts
// is able to reference another body part within the same multipart/related
// structure. These references are called URI's.

// When an MHTML message is parsed by the message engine, the HTML body of each
// part is stored as a file. The following classes are a means of finding the
// file name of the body part that corresponds to a given a URI.

CImMhtmlUriResolver* CImMhtmlUriResolver::NewL(CMsvEntry& aEntry)
	{
	CImMhtmlUriResolver* self = NewLC(aEntry);
	CleanupStack::Pop(); // self
	return self;
	}

CImMhtmlUriResolver* CImMhtmlUriResolver::NewLC(CMsvEntry& aEntry)
	{
	CImMhtmlUriResolver* self = new (ELeave) CImMhtmlUriResolver(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
void CImMhtmlUriResolver::ResolveL(const TDesC& aURI, const TDesC& aHtmlBase, TMsvId aCurrentPartId,TBool aFileNameOnly, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iResolved = EFalse;
	delete iUri;
	iUri = CImMhtmlUri::NewL(aURI);
	iBodyPartId = aCurrentPartId;
	iCurrentEntry.SetEntryL(aCurrentPartId);
	iState = EImResolvingUri;
	
	iTryingThisMessage = EFalse;
	iFileNameOnly = aFileNameOnly;
	ResolveL(aHtmlBase, aCurrentPartId);
	}


void CImMhtmlUriResolver::ResolveL(const TDesC& aHtmlBase, TMsvId aCurrentPartId)
	{

	if (!(iUri->IsAbsolute()))
	// Use the aHtmlBase parameter as a base to make an absolute URI from the given relative one.
	// Try and find a match for this URI.
		{
		if (aHtmlBase.Length() != 0)
			{
			CImMhtmlUri* baseUri = CImMhtmlUri::NewLC(aHtmlBase);
			if (baseUri->IsAbsolute())
				{
				iUri->MakeAbsoluteL(*baseUri);
				}
			CleanupStack::PopAndDestroy(); // baseUri
			}
		}

	if (!(iUri->IsAbsolute()))
	// Use the Content-location value that is associated with the current part as a base.
	// Use this base to make an absolute URI from the given one and then search for it.
		{
		HBufC* contentLocation = GetContentLocationL(aCurrentPartId);
		if (contentLocation)
			{
			CleanupStack::PushL(contentLocation);
			CImMhtmlUri* baseUri = CImMhtmlUri::NewLC(contentLocation->Des());
			if (baseUri->IsAbsolute())
				{
				iUri->MakeAbsoluteL(*baseUri);
				}
			CleanupStack::PopAndDestroy(2); // baseUri, contentLocation
			}
		}
		
	iStatus=KRequestPending;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CImMhtmlUriResolver::FindFirstL(TMsvId aRootMessageId,TBool aFileNameOnly, TRequestStatus &aStatus)
	{
	Queue(aStatus);
	iMhtmlFirstPageFinder->FindL(aRootMessageId,aFileNameOnly, iStatus);
	iFileNameOnly = aFileNameOnly;
	iState = EImFindingFirstUri;
	SetActive();
	}

CImMhtmlUriResolver::~CImMhtmlUriResolver()
	{
	iFile.Close();
	delete iUri;
	delete iMhtmlFileFinder;
	delete iMhtmlFirstPageFinder;
	iUriCacheArray.ResetAndDestroy();
	}

	
TInt CImMhtmlUriResolver::FileHandle(RFile& aFile) const	
	{
	if(iResolved)
		{
		// someone requested for the file handle,
		// pass the handle and create a empty file handle
		aFile = iFile;
		iFile=RFile();
		return KErrNone;	
		}
	else
		{
		return KErrNotFound;	
		}	
	}

HBufC* CImMhtmlUriResolver::FileNameL() const
	{
	if (iResolved)
		{
		HBufC* fileName = HBufC::NewL(iFileName.Length());
		(*fileName) = iFileName;
		return fileName;
		}
	else
		{
		return iUri ? iUri->TextL(ETrue) : 0;
		}
	}

TMsvId CImMhtmlUriResolver::LinkedEntryId() const
	{
	return iLinkedEntryId;
	}

void CImMhtmlUriResolver::ConstructL()
	{
	CActiveScheduler::Add(this);
	iMhtmlFileFinder = CImMhtmlFileFinder::NewL(iCurrentEntry,iUriCacheArray);
	iMhtmlFirstPageFinder = CImMhtmlFirstPageFinder::NewL(iCurrentEntry);
	}

void CImMhtmlUriResolver::DoRunL()
	{
	TBool matchUri = EFalse;

	if ((iState == EImFindingFile)
		&& (!iMhtmlFileFinder->MatchFound())
		&& (iTryingThisMessage))
		// Fix for MIME::lite.
		// The body part could not be found using the thismessage: scheme,
		// so look for a content location with the original unresolved URI.
		{
		matchUri = ETrue;
		iTryingThisMessage = EFalse;
		
		CImMhtmlUri* unresolvedUri = CImMhtmlUri::NewLC(iUri->OriginalUriText()->Des());
		delete iUri;
		iUri = unresolvedUri;
		CleanupStack::Pop(); // unresolvedUri
		}

	if (EImResolvingUri == iState)
		{
		if (!(iUri->IsAbsolute()))
			{
			iCurrentEntry.SetEntryL(iCurrentEntry.Entry().Parent());

			if ((KUidMsvMessageEntry != iCurrentEntry.Entry().iType)
				&& (KUidMsvFolderEntry != iCurrentEntry.Entry().iType))
			// If there are no parent entries then use 'thismessage:/' as the base.
				{
				_LIT(KDefaultBase, "thismessage:/");
				CImMhtmlUri* defaultBase = CImMhtmlUri::NewLC(KDefaultBase);
				iUri->MakeAbsoluteL(*defaultBase);
				CleanupStack::PopAndDestroy(); // defaultBase
				matchUri = ETrue;
				iTryingThisMessage = ETrue;
				}
			// If a valid base is still needed to complete the uri then try and find it in the parent headers.
			else
				{
				ResolveL(KNullDesC ,iCurrentEntry.Entry().Id());
				iState=EImTryingWithoutResolve;
				}
			}
		else
			{
			matchUri = ETrue;
			}
		}
	else if (iState == EImTryingWithoutResolve)
		{
		matchUri = ETrue;
		}


	if (matchUri)
		{
		if(CheckCacheForLinkedEntryL())
			{
			if (iStatus == KImcmHTMLPartNotPopulated)					
				{
				User::Leave(KImcmHTMLPartNotPopulated);	
				}
			}	
		else
			{
			// Search the appropriate structures for the file that matches the absolute URI.
			TBool cidUri = iUri->CompareScheme(KImMhtmlCidString);
			// Do not include the scheme if the URI is a content-id ('cid:').
			HBufC* uriText = iUri->TextL(!cidUri);
	
			CleanupStack::PushL(uriText);
			iMhtmlFileFinder->FindL(uriText->Des(), iBodyPartId,iFileNameOnly, iStatus);
			CleanupStack::PopAndDestroy(); // uriText
			iState = EImFindingFile;
			SetActive();
			}
		
		}
	}
/** 
Searches in cache for a URl that need to be resolved. If URL found in cache then it implies that URL 
has been resolved in previous look ups (Each entry of cache contains a URL and its corresponding Linked Entry)
@return True if cache contains URI else false
*/
TBool CImMhtmlUriResolver::CheckCacheForLinkedEntryL()
	{	
	TInt count = iUriCacheArray.Count();
	if(count)
		{
		CImCacheUriEntry* cacheEntry = NULL;
		for(TInt i=0; i < count; ++i)
			{
			cacheEntry = iUriCacheArray[i];
			if((iUri->OriginalUriText()->Des().Compare(cacheEntry->GetContentLocation()->Des()) == 0 ) || (iUri->OriginalUriText()->Des().Compare(cacheEntry->GetContentId()->Des()) == 0 ))
				{
				iLinkedEntryId = cacheEntry->GetUriEntry();
				iCurrentEntry.SetEntryL(iLinkedEntryId);
				CMsvStore* store = iCurrentEntry.ReadStoreL();
				CleanupStack::PushL(store);
				MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
				// emails have one attachment per attachment entry
				if(attachmentMgr.AttachmentCount())
					{
					if(iFileNameOnly)
						{
						CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
						iFileName = attachment->FilePath();
						delete attachment;
						}
					else 
						{
						iFile = attachmentMgr.GetAttachmentFileL(0);	
						}		
					}
				CleanupStack::PopAndDestroy(store);

			// Check that the file exists. If it doesn't then complete with an error.
			if (!iCurrentEntry.Entry().Complete())
					{
					iStatus = KImcmHTMLPartNotPopulated;
					}
				iResolved = ETrue;	//need to constract the uriresolver entry.
				return ETrue;
				}
			}
		}
	return EFalse;
	}


void CImMhtmlUriResolver::DoComplete(TInt& aStatus)
	{
	if (EImFindingFirstUri == iState)
		{
		if ((iStatus == KErrNone) || (iStatus == KImcmHTMLPartNotPopulated))
			{
			if(iFileNameOnly)	
				{
				iMhtmlFirstPageFinder->Result(iFileName, iLinkedEntryId);			
				}
			else
				{
				iMhtmlFirstPageFinder->Result(iFile,iLinkedEntryId);
				}
			iResolved = ETrue;
			}
		}
	else if (EImFindingFile == iState)
		{
		if ((iMhtmlFileFinder->MatchFound()) || (iStatus == KImcmHTMLPartNotPopulated))
			{
			if(iFileNameOnly)	
				{
				iMhtmlFileFinder->Result(iFileName, iLinkedEntryId);					
				}
			else
				{
				iMhtmlFileFinder->Result(iFile,iLinkedEntryId);
				}
			
			iResolved = ETrue;
			}
		}
	
	if ((!iResolved) && (aStatus != KImcmHTMLPartNotPopulated))
		{
		aStatus = KErrNotFound;
		}
	}

void CImMhtmlUriResolver::DoCancel()
	{
	switch (iState)
		{
		case EImFindingFile:
			iMhtmlFileFinder->Cancel();
			break;
		case EImFindingFirstUri:
			iMhtmlFirstPageFinder->Cancel();
			break;
		default:
			break;
		}
	CMsgActive::DoCancel();
	}

CImMhtmlUriResolver::CImMhtmlUriResolver(CMsvEntry& aEntry) :  CMsgActive(EPriorityStandard), iCurrentEntry(aEntry)
	{

	}

HBufC* CImMhtmlUriResolver::GetContentLocationL(TMsvId aEntryId)
	{
// Get the Content-location value from the mime header of the specified entry.
	iCurrentEntry.SetEntryL(aEntryId);

	HBufC* result = 0;

	if(iCurrentEntry.HasStoreL())
		{
		CMsvStore* entryStore = iCurrentEntry.ReadStoreL();
		CleanupStack::PushL(entryStore);
		if (entryStore->IsPresentL(KUidMsgFileMimeHeader))
			{
			CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
			mimeHeader->RestoreL(*entryStore);
			TPtrC16 contentLocationPtr = mimeHeader->ContentLocation();
			result = HBufC::NewL(contentLocationPtr.Length());
			(*result) = contentLocationPtr;
			CleanupStack::PopAndDestroy(); // mimeHeader
			}
		CleanupStack::PopAndDestroy(); // entryStore
		}
	return result;
	}

CImMhtmlChildEntrySearcher* CImMhtmlChildEntrySearcher::NewL(CMsvEntry& aEntry, RPointerArray<CImCacheUriEntry>& aUriCacheUriArray)
	{
	CImMhtmlChildEntrySearcher* self = new (ELeave) CImMhtmlChildEntrySearcher(aEntry, aUriCacheUriArray);
	CActiveScheduler::Add(self);
	return self;
	}

void CImMhtmlChildEntrySearcher::StartL(TMsvId aEntry, TDesC& aUri, TRequestStatus& aStatus)
	{

	delete iUri;
	iUri = 0;
	iUri = HBufC::NewL(aUri.Length());
	(*iUri) = aUri;

	delete iChildEntries;
	iChildEntries = 0;
	iEntry.SetEntryL(aEntry);
	iChildEntries = iEntry.ChildrenL();
 	TInt index = iChildEntries->Count();
 	while (index--)
 		{
 		if (iEntry.ChildDataL((*iChildEntries)[index]).iType == KUidMsvFolderEntry)
 			iChildEntries->Delete(index);
 		}
	iChildEntryIndex = 0;

	iFound = EFalse;

	if (iChildEntries->Count() != 0)
		{
		CheckCurrentEntryL();
		Queue(aStatus);
		}
	else
		{
		Queue(aStatus);
		iStatus=KRequestPending;
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

CImMhtmlChildEntrySearcher::~CImMhtmlChildEntrySearcher()
	{
	delete iUri;
	delete iChildEntries;
	}

TBool CImMhtmlChildEntrySearcher::SearchResult(TMsvId& aSearchResultEntry)
	{
	if (iFound)
		{
		aSearchResultEntry = (*iChildEntries)[iChildEntryIndex];
		}
	return iFound;
	}

void CImMhtmlChildEntrySearcher::DoRunL()
	{
	if (!iFound)
		{
		iChildEntryIndex++;
		if (iChildEntryIndex < iChildEntries->Count())
			{
			CheckCurrentEntryL();
			}
		}
	}
	
	
void CImMhtmlChildEntrySearcher::DoCancel()
	{
	CMsgActive::DoCancel();
	}	

void CImMhtmlChildEntrySearcher::DoComplete(TInt& )
	{

	}

/** 
Search for a TMsvId entry in cache. If found there is no need to open 
store to get content-location, content-id of that entry because cache contains that information
@return index of the cache if cache contains URI else 0
*/
TInt CImMhtmlChildEntrySearcher::CheckCacheForEntryId(TMsvId aId)
{
	TInt count = iUriCacheArray.Count();
	if(count)
		{
		CImCacheUriEntry* cacheEntry = NULL;
		for(TInt i=0; i < count; ++i)
			{
			cacheEntry = iUriCacheArray[i];
			if(cacheEntry->GetUriEntry() == aId)
			return i;
			}
		}
	return KErrNotFound;
}

void CImMhtmlChildEntrySearcher::CheckCurrentEntryL()
//Compares the search URI with the content-location and content-id fields of each child entry in cache if not found then
// compares the search URI with the content-location and content-id fields of each child entry from store.
// It sets iFound if either content field in the current entry matches the URI.
	{
	TInt index = CheckCacheForEntryId((*iChildEntries)[iChildEntryIndex]);
	if( index != KErrNotFound )
		{
		CImCacheUriEntry* uriEntry = iUriCacheArray[index];
		iFound = CheckContentDetailsL(uriEntry->GetContentLocation()->Des(),uriEntry->GetContentId()->Des());
		}
	else
		{
		iEntry.SetEntryL((*iChildEntries)[iChildEntryIndex]);
		if (iEntry.HasStoreL() && (iEntry.Entry().iType != KUidMsvFolderEntry))
			{
			CMsvStore* entryStore = iEntry.ReadStoreL();
			CleanupStack::PushL(entryStore);
			// Get the content-location and content-id values for the current entry.
			if (entryStore->IsPresentL(KUidMsgFileMimeHeader))
				{
				CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
				mimeHeader->RestoreL(*entryStore);
				TPtrC16 contentLocationPtr = mimeHeader->ContentLocation();
				TPtrC8 contentIdPtr = mimeHeader->ContentID();
				// Copy the 8 bit contentId string into a 16 bit wide descriptor.
				TInt contentIdLength = contentIdPtr.Length();
				HBufC* contentId = HBufC::NewL(contentIdLength);
				CleanupStack::PushL(contentId);
				TInt contentIdCopyCounter = 0;
				TPtr copyOfContentId = contentId->Des();
				for (contentIdCopyCounter = 0; contentIdCopyCounter < contentIdLength; contentIdCopyCounter++)
					{
					copyOfContentId.Append(contentIdPtr[contentIdCopyCounter]);
					}

				// Compare the content-location and content-id values to the search URI string. 
				iFound = CheckContentDetailsL(contentLocationPtr, copyOfContentId);
				//Add entry to cache. So that for each new URI lookup, first 
				//look in the cache to see URI has been loaded during a previous lookup.
				CImCacheUriEntry* uriEntry  = CImCacheUriEntry::NewL(contentLocationPtr,copyOfContentId,iEntry.EntryId());
				if(iUriCacheArray.Append(uriEntry) != KErrNone)
					{
					delete uriEntry;
					} 
				CleanupStack::PopAndDestroy(2); // contentId, mimeHeader
				}
			CleanupStack::PopAndDestroy(); // entryStore
			}
		}
	iStatus=KRequestPending;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}


TBool CImMhtmlChildEntrySearcher::CheckContentDetailsL(const TDesC& aContentLocation, const TDesC& aContentId)
// Compare the URI to the given content-location and content-id
	{
	TBool matchFound = EFalse;

	if ((iUri->Des()).CompareF(aContentLocation) == 0)
		{
		matchFound = ETrue;
		}
	else if ((iUri->Des()).CompareF(aContentId) == 0)
		{
		matchFound = ETrue;
		}

	return matchFound;
	}

CImMhtmlChildEntrySearcher::CImMhtmlChildEntrySearcher(CMsvEntry &aEntry, RPointerArray<CImCacheUriEntry>& aUriCacheUriArray) : CMsgActive(EPriorityStandard), iEntry(aEntry),	iUriCacheArray(aUriCacheUriArray)
	{
	}






CImMhtmlFileFinder* CImMhtmlFileFinder::NewL(CMsvEntry& aEntry, RPointerArray<CImCacheUriEntry>& aUriCacheUriArray)
	{
	CImMhtmlFileFinder* self = new (ELeave) CImMhtmlFileFinder(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL(aUriCacheUriArray);
	CleanupStack::Pop(); // self
	return self;
	}

void CImMhtmlFileFinder::FindL(const TDesC& aUri, TMsvId aBodyPartId, TBool aFileNameOnly,TRequestStatus &aStatus)
// Recursively search the parent to find a match for the given content-ID or content-location.
	{
	Queue(aStatus);

	delete iUri;
	iUri = 0;
	iUri = HBufC::NewL(aUri.Length());
	(*iUri) = aUri;

	iCurrentBodyPartId = aBodyPartId;
	iMatchFound = EFalse;
	
	iFileNameOnly = aFileNameOnly;
	DoFindL();
	}

void CImMhtmlFileFinder::DoFindL()
	{
	iCurrentEntry.SetEntryL(iCurrentBodyPartId);

	TPtr uri = iUri->Des();
	iChildEntrySearcher->StartL((iCurrentEntry.Entry()).Parent(), uri, iStatus);
	SetActive();
	}
	
void CImMhtmlFileFinder::Result(RFile& aFile, TMsvId& aEntryId) const
	{
	aFile= iFile;
	iFile = RFile();
	aEntryId = iLinkedEntryId;	
	}

void CImMhtmlFileFinder::Result(TFileName& aFileName, TMsvId& aEntryId) const
	{
	aFileName = iFileName;
	aEntryId = iLinkedEntryId;
	}

TBool CImMhtmlFileFinder::MatchFound() const
	{
	return iMatchFound;
	}

CImMhtmlFileFinder::~CImMhtmlFileFinder()
	{
	iFile.Close();
	delete iUri;
	delete iChildEntrySearcher;
	}

CImMhtmlFileFinder::CImMhtmlFileFinder(CMsvEntry &aEntry) : CMsgActive(EPriorityStandard), iCurrentEntry(aEntry)
	{

	}


void CImMhtmlFileFinder::ConstructL(RPointerArray<CImCacheUriEntry>& aUriCacheUriArray)
	{
	CActiveScheduler::Add(this);
	iChildEntrySearcher = CImMhtmlChildEntrySearcher::NewL(iCurrentEntry, aUriCacheUriArray);
	}

void CImMhtmlFileFinder::DoRunL()
	{
	if (iStatus == KErrNone)
		{
		TMsvId matchingEntry;
		iMatchFound = iChildEntrySearcher->SearchResult(matchingEntry);
		if (!iMatchFound)
			{
			// If no match for the URI is found under the current entry then search the parent.
			iCurrentEntry.SetEntryL(iCurrentBodyPartId);
			iCurrentBodyPartId = iCurrentEntry.Entry().Parent();
			DoFindL();
			}
		else
			{
			// If a match is found the set the result values.
			iLinkedEntryId = matchingEntry;
			iCurrentEntry.SetEntryL(iLinkedEntryId);
			CMsvStore* store = iCurrentEntry.ReadStoreL();
			CleanupStack::PushL(store);
			MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
			// emails have one attachment per attachment entry
			if(attachmentMgr.AttachmentCount())
				{
				if(iFileNameOnly)
					{
					CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
					iFileName = attachment->FilePath();
					delete attachment;
					}
				else 
					{
					iFile = attachmentMgr.GetAttachmentFileL(0);	
					}		
				}
			CleanupStack::PopAndDestroy(store);

			// Check that the file exists. If it doesn't then complete with an error.
			if (!iCurrentEntry.Entry().Complete())
				{
				iStatus = KImcmHTMLPartNotPopulated;
				}
			}
		}
	}

void CImMhtmlFileFinder::DoCancel()
	{
	iChildEntrySearcher->Cancel();
	CMsgActive::DoCancel();
	}

void CImMhtmlFileFinder::DoComplete(TInt& aStatus)
	{
	if (iStatus == KImcmHTMLPartNotPopulated)
		{
		aStatus = KImcmHTMLPartNotPopulated;
		}
	else if (aStatus == KErrNotFound)
		{
		aStatus = KErrNone;
		}
	}






CImMhtmlFirstPageFinder* CImMhtmlFirstPageFinder::NewL(CMsvEntry& aEntry)
	{
	CImMhtmlFirstPageFinder* self = new (ELeave) CImMhtmlFirstPageFinder(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CImMhtmlFirstPageFinder::FindL(TMsvId aRootMessage,TBool aFileNameOnly, TRequestStatus& aStatus)
// Find the first html part of the specified message.
// Use the Id function to retrieve the result.
	{
	Queue(aStatus);

	iNextUnrelatedEntryStack->Reset();
	iRelatedDepthCounter = 0;

	delete iEntryStack;
	iEntryStack = 0;
	iCurrentEntry.SetEntryL(aRootMessage);
	iEntryStack = iCurrentEntry.ChildrenL();
	TKeyArrayFix sortKey(0, ECmpTInt32);
	iEntryStack->Sort(sortKey);

	// The order of the list must be reversed because items are taken from the end.
	TInt startCounter = 0;
	TInt endCounter = iEntryStack->Count() - 1;
	TMsvId savedId;
	while (startCounter < endCounter)
		{
		savedId = (*iEntryStack)[startCounter];
		(*iEntryStack)[startCounter] = (*iEntryStack)[endCounter];
		(*iEntryStack)[endCounter] = savedId;
		startCounter++;
		endCounter--;
		}

	iHtmlPartFound = EFalse;
	iUnrelatedHtmlPartFound = EFalse;

	SetStartParameterL();

	DoFindL();
	iFileNameOnly = aFileNameOnly;
	}

void CImMhtmlFirstPageFinder::SetStartParameterL()
	{
	CMsvStore* store = NULL;
	if(iCurrentEntry.HasStoreL())
		{
		store = iCurrentEntry.ReadStoreL();
		CleanupStack::PushL(store);
	
		delete iStartParameter;
		iStartParameter = 0;
		if (store->IsPresentL(KUidMsgFileMimeHeader))
			{
			CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
			TRAPD(err, mimeHeader->RestoreL(*store));
			if (KErrNotFound == err)
				{
				err = KImcmInvalidMessageStructure;
				}
			User::LeaveIfError(err);
			const CDesC8Array& contentTypeParams = mimeHeader->ContentTypeParams();
			TInt startLength = ((TDesC&)KImMhtmlStartString).Length();
			TInt compareResult = 1;
			// Search for the start parameter.
			TInt paramIndex = contentTypeParams.Count();
			while ((paramIndex > 0) && (compareResult != 0))
				{
				paramIndex--;
				compareResult = ((contentTypeParams[paramIndex]).Left(startLength)).CompareF(KImMhtmlStartString);
				}

			if (compareResult == 0)
			// The 'start' parameter has been found, now the value needs to be extracted.
				{
				TPtrC8 startValue = contentTypeParams[paramIndex].Right(contentTypeParams[paramIndex].Length() - startLength);
				
				// Remove any leading whitespace and a single '=' character.
				TBool equalsFound = EFalse;
				TBool firstValueCharacterFound = EFalse;
				while ((startValue.Length() > 0) && (!firstValueCharacterFound))
					{
					if ((startValue[0] == '=') && (!equalsFound))
						{
						equalsFound = ETrue;
						startValue.Set(startValue.Right(startValue.Length() - 1));
						}
					else if (startValue[0] == ' ')
						{
						startValue.Set(startValue.Right(startValue.Length() - 1));
						}
					else
						{
						firstValueCharacterFound = ETrue;
						}
					}

				// Remove the quotes if they exist.
				if (startValue.Length() > 1)
					{
					if ((startValue[0] == '"') && (startValue[startValue.Length() - 1] == '"'))
						{
						startValue.Set(startValue.Mid(1, startValue.Length() - 2));
						}
					}

				// Keep a copy of the unquoted start parameter.
				delete iStartParameter;
				iStartParameter = 0;
				iStartParameter = HBufC8::NewL(startValue.Length());
				(*iStartParameter) = startValue;
				}
			CleanupStack::PopAndDestroy(); // mimeHeader
			}
		CleanupStack::PopAndDestroy(); // store
		}
		else
		{
			iStatus=KErrNotFound;		
		}
	}

void CImMhtmlFirstPageFinder::DoFindL()
	{
	// Get the entry details from the item at the head of the stack.
	if (iEntryStack->Count() > 0)
		{
		iCurrentEntry.SetEntryL((*iEntryStack)[iEntryStack->Count() - 1] );

		// Check if the current entry is at the top of the unrelated stack
		// If it is then we are no longer under the related structure.
		if (iNextUnrelatedEntryStack->Count() != 0)
			{
			if (iCurrentEntry.Entry().Id() == (*iNextUnrelatedEntryStack)[iNextUnrelatedEntryStack->Count() - 1])
				{
				iRelatedDepthCounter--;
				iNextUnrelatedEntryStack->ResizeL(iNextUnrelatedEntryStack->Count() - 1);
				}				
			}

		// Pop the entry from the stack.
		iEntryStack->ResizeL(iEntryStack->Count() - 1);

		// If the current entry is a related type then increase the related depth counter
		// We need to know if we are under a related entry in order to recognise the first HTML part

		TUid entryType = iCurrentEntry.Entry().iType;
		if (EFolderTypeRelated == ((TMsvEmailEntry)iCurrentEntry.Entry()).MessageFolderType())
			{
			iRelatedDepthCounter++;
			if (iEntryStack->Count() != 0)
				{
				iNextUnrelatedEntryStack->AppendL((*iEntryStack)[iEntryStack->Count() - 1]);
				}
			}

		if (KUidMsvEmailHtmlEntry == entryType)
			{
			if (iStartParameter != 0)
				{
				CMsvStore* store = iCurrentEntry.ReadStoreL();
				CleanupStack::PushL(store);
				if (store->IsPresentL(KUidMsgFileMimeHeader))
					{
					CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
					mimeHeader->RestoreL(*store);
					TPtrC8 contentId = mimeHeader->ContentID();
					if ((iStartParameter->Des()).CompareF(contentId) == 0)
						// If the start parameter matches the content-id then the first HTML part
						// has been found.
						{
						iHtmlPartFound = ETrue;
						iFirstHtmlPartId = iCurrentEntry.Entry().Id();
						}
					CleanupStack::PopAndDestroy(); // mimeHeader
					}
				CleanupStack::PopAndDestroy(); // store
				}
			else
				{
				if (iRelatedDepthCounter > 0)
					// This entry is under a related structure (either directly or indirectly)
					// so it is the one we are after.
					// HTML parts that are not under a related structure are normal attachments
					// and NOT part of an MHTML message.
					{
					iHtmlPartFound = ETrue;
					iFirstHtmlPartId = iCurrentEntry.Entry().Id();
					}
				else
					{
					if ((!iHtmlPartFound) && (!iUnrelatedHtmlPartFound))
						{
						iUnrelatedHtmlPartFound = ETrue;
						iFirstUnrelatedHtmlPartId = iCurrentEntry.Entry().Id();
						}
					}
				}
			}

		if (KUidMsvFolderEntry == entryType)
			{
			// Push the sorted child items on to the stack.
			CMsvEntrySelection* childEntries = iCurrentEntry.ChildrenL();
			CleanupStack::PushL(childEntries);
			TKeyArrayFix sortKey(0, ECmpTInt32);
			childEntries->Sort(sortKey);

			// The entries need to be added in reverse order because they are retrieved from a stack.
			TInt childEntriesCounter = childEntries->Count();
			while (childEntriesCounter > 0)
				{
				childEntriesCounter--;
 				// Ignore any entries that weren't folders or HTML entries.
 				if ((iCurrentEntry.ChildDataL((*childEntries)[childEntriesCounter]).iType == KUidMsvFolderEntry)
 					|| (iCurrentEntry.ChildDataL((*childEntries)[childEntriesCounter]).iType == KUidMsvEmailHtmlEntry))
 					{
 					iEntryStack->AppendL((*childEntries)[childEntriesCounter]);
 					}
  				}
			CleanupStack::PopAndDestroy(childEntries); 
			}

		}

	TRequestStatus* status = &iStatus;
	iStatus=KRequestPending;
	SetActive();
	if ((!iCurrentEntry.Entry().Complete()) && (iHtmlPartFound))
		{
		User::RequestComplete(status, KImcmHTMLPartNotPopulated);
		}
	else
		{
		User::RequestComplete(status, KErrNone);
		}
	}

void CImMhtmlFirstPageFinder::Result(RFile& aFile, TMsvId& aEntryId) const
	{
	aFile = iFile;
	iFile = RFile();
	aEntryId = iFirstHtmlPartId;	
	}

void CImMhtmlFirstPageFinder::Result(TFileName& aFileName, TMsvId& aEntryId) const
	{
	aFileName = iFileName;
	aEntryId = iFirstHtmlPartId;	
	}

CImMhtmlFirstPageFinder::~CImMhtmlFirstPageFinder()
	{
	iFile.Close();
	delete iEntryStack;
	delete iStartParameter;
	delete iNextUnrelatedEntryStack;
	}

CImMhtmlFirstPageFinder::CImMhtmlFirstPageFinder(CMsvEntry& aEntry) : CMsgActive(EPriorityStandard), iCurrentEntry(aEntry)
	{
	}

void CImMhtmlFirstPageFinder::ConstructL()
	{
	CActiveScheduler::Add(this);
	iNextUnrelatedEntryStack = new (ELeave) CMsvEntrySelection;
	}

void CImMhtmlFirstPageFinder::DoRunL()
	{
	// We are done if we could not find the parent structure, or if we have found an HTML part.
	if (iStatus == KErrNone)
		{
		if ((iEntryStack->Count() != 0) && (!iHtmlPartFound))
			{
			DoFindL();
			}
		else
			{
			if (iUnrelatedHtmlPartFound)
				{
				iHtmlPartFound = ETrue;
				iFirstHtmlPartId = iFirstUnrelatedHtmlPartId;
				}
			}
		}

	if (iHtmlPartFound && !IsActive())
		{
		iCurrentEntry.SetEntryL(iFirstHtmlPartId);
		CMsvStore* store = iCurrentEntry.ReadStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
		// get the file handle to pass it to clients who request for it using
		// function CImEmailMessage::GetUniversalResourceIdentifierL()
		if(attachmentMgr.AttachmentCount())
			{
			if(iFileNameOnly)
				{
				CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
				iFileName = attachment->FilePath();
				delete attachment;
				}
			else 
				{
				iFile = attachmentMgr.GetAttachmentFileL(0);	
				}	
			}
		CleanupStack::PopAndDestroy(store); 
		}
	}

void CImMhtmlFirstPageFinder::DoComplete(TInt& aStatus)
	{
	if (!iHtmlPartFound)
		{
		aStatus = KErrNotFound;
		}
	else
		{
		aStatus = KErrNone;
		}
	}

void CImMhtmlFirstPageFinder::DoCancel()
	{
		CMsgActive::DoCancel();
	}


// The following class implements a class for parsing and combining URI's.
// See RFC 1808 for more details.

CImMhtmlUri* CImMhtmlUri::NewL(const TDesC& aUriText)
	{
	CImMhtmlUri* self = NewLC(aUriText);
	CleanupStack::Pop(); // self
	return self;
	}

CImMhtmlUri* CImMhtmlUri::NewLC(const TDesC& aUriText)
	{
	CImMhtmlUri* self = new (ELeave) CImMhtmlUri();
	CleanupStack::PushL(self);
	self->ConstructL(aUriText);
	return self;
	}

void CImMhtmlUri::ConstructL(const TDesC& aUriText)
	{
	iUriText = HBufC::NewL(aUriText.Length());
	(*iUriText) = aUriText;
	iDirectoryPath = new (ELeave) CDesCArrayFlat(8);
	ParseL();
	}

void CImMhtmlUri::MakeAbsoluteL(const CImMhtmlUri& aBaseUri)
	{
	TBool done = EFalse;
	
	if (iScheme == 0)
		{
		if (aBaseUri.iScheme)
			{
			iScheme = HBufC::NewL(aBaseUri.iScheme->Length());
			(*iScheme) = aBaseUri.iScheme->Des();
			}
		}
	else
		{
		done = ETrue;
		}
	
	if ((iNetLoc == 0) && (!done))
		{
		if (aBaseUri.iNetLoc)
			{
			iNetLoc = HBufC::NewL(aBaseUri.iNetLoc->Length());
			(*iNetLoc) = aBaseUri.iNetLoc->Des();
			}
		}
	else
		{
		done = ETrue;
		}

	if ((!iAbsolutePath) && (!done))
		{
		// Try to add any directories from the base before hand.
		TInt count = aBaseUri.iDirectoryPath->Count();

		if (iDirectoryPath->Count() != 0)
			{
			done = ETrue;
			if (aBaseUri.iContainsFileName)
				{
				count--;
				}
			}
		else
			{
			iContainsFileName = aBaseUri.iContainsFileName;
			}

		while (count > 0)
			{
			count--;
			iDirectoryPath->InsertL(0, (*(aBaseUri.iDirectoryPath))[count]);
			}
		}
	else
		{
		done = ETrue;
		}

	if ((iParameters == 0) && (!done))
		{
		if (aBaseUri.iParameters)
			{
			iParameters = HBufC::NewL(aBaseUri.iParameters->Length());
			(*iParameters) = aBaseUri.iParameters->Des();
			}
		}
	else
		{
		done = ETrue;
		}

	if ((iQuery == 0) && (!done))
		{
		if (aBaseUri.iQuery)
			{
			iQuery = HBufC::NewL(aBaseUri.iQuery->Length());
			(*iQuery) = aBaseUri.iQuery->Des();
			}
		}
	else
		{
		done = ETrue;
		}

	if ((iFragment == 0) && (!done))
		{
		if (aBaseUri.iFragment)
			{
			iFragment = HBufC::NewL(aBaseUri.iFragment->Length());
			(*iFragment) = aBaseUri.iFragment->Des();
			}
		}

	Normalise();
	}

HBufC* CImMhtmlUri::TextL(TBool aIncludeScheme)
	{
	// First calculate the size of the required HBufC
	TInt length = 0;
	
	if ((iScheme) && (aIncludeScheme))
		{
		length += (iScheme->Des()).Length() + 1;
		}

	if (iNetLoc)
		{
		length += (iNetLoc->Des()).Length() + 2;
		}

	if (iParameters)
		{
		length += (iParameters->Des()).Length() + 1;
		}

	if (iQuery)
		{
		length += (iQuery->Des()).Length() + 1;
		}

	if (iFragment)
		{
		length += (iFragment->Des()).Length() + 1;
		}

	if (iDirectoryPath != 0)
		{
		TInt counter = iDirectoryPath->Count();
		while (counter > 0)
			{
			counter--;
			length += ((*iDirectoryPath)[counter]).Length();
			length++;
			}
		}

	if (iAbsolutePath != 0)
		{
		length++;
		}

	if (!iContainsFileName)
		{
		length++;
		}

	HBufC* text = HBufC::NewL(length);

	if ((iScheme) && (aIncludeScheme))
		{
		(text->Des()).Append(iScheme->Des());
		(text->Des()).Append(TChar(':'));
		}

	if (iNetLoc)
		{
		(text->Des()).Append(TChar('/'));
		(text->Des()).Append(TChar('/'));
		(text->Des()).Append(iNetLoc->Des());
		}

	if( iDirectoryPath != 0 )
		{
		if (((iAbsolutePath) || (iNetLoc)) && (iDirectoryPath->Count()))
			{
			(text->Des()).Append(TChar('/'));
			}

		TInt counter = 0;
		TInt count = iDirectoryPath->Count();
		while (counter < count)
			{
			(text->Des()).Append((*iDirectoryPath)[counter]);

			counter++;

			if ((counter < count) || (!iContainsFileName))
				{
				(text->Des()).Append(TChar('/'));
				}
			}
		}

	if (iParameters)
		{
		(text->Des()).Append(TChar(';'));
		(text->Des()).Append(iParameters->Des());
		}

	if (iQuery)
		{
		(text->Des()).Append(TChar('?'));
		(text->Des()).Append(iQuery->Des());
		}

	if (iFragment)
		{
		(text->Des()).Append(TChar('#'));
		(text->Des()).Append(iFragment->Des());
		}

	return text;
	}


TBool CImMhtmlUri::IsAbsolute() const
	{
	return (iScheme == 0) ? EFalse : ETrue;
	}


TBool CImMhtmlUri::CompareScheme(const TDesC& aScheme) const
// Returns ETrue if the given descriptor matches the scheme of this URI, returns EFalse otherwise.
	{
	if (iScheme)
		{
		return (aScheme.CompareF(iScheme->Des()) == 0);
		}
	else
		{
		return EFalse;
		}
	}


HBufC* CImMhtmlUri::OriginalUriText()
	{
	return iUriText;
	}


CImMhtmlUri::~CImMhtmlUri()
	{
	delete iDirectoryPath;

	delete iUriText;
	delete iScheme;
	delete iNetLoc;
	delete iParameters;
	delete iQuery;
	delete iFragment;
	}

CImMhtmlUri::CImMhtmlUri()
	{

	}

void CImMhtmlUri::ParseL()
	{
	TInt textIndex = 0;
	TInt textCount = iUriText->Length();

	TInt stringStart = 0;
	TInt stringLength = 0;
	TImUriDelimiterType delimiterType;

	iState = EParsingFirstString;

	while (textIndex <= textCount)
		{
		delimiterType = Delimiter(textIndex);

		if (EImUriNotDelimiter != delimiterType)
			{
			switch (iState)
				{
				case EParsingFirstString:
					switch(delimiterType)
						{
						case EImUriScheme:
							if (stringLength)
								{
								delete iScheme;
								iScheme = NULL;
								iScheme = HBufC::NewL(stringLength);
								(*iScheme) = iUriText->Mid(stringStart, stringLength);
								}
							ChangeState(delimiterType);
							break;
						case EImUriPath:
							if ((0 == stringLength)
								&& (0 == stringStart))
								{
								iAbsolutePath = ETrue;
								}
						case EImUriParameter:
						case EImUriQuery:
						case EImUriFragment:
						case EImUriEnd:
							if (stringLength)
								{
								iDirectoryPath->AppendL(iUriText->Mid(stringStart, stringLength));
								iContainsFileName = (EImUriPath != delimiterType);
								}
							ChangeState(delimiterType);
							break;
						case EImUriNetLoc:
							stringLength = 0;
							stringStart = 0;
							ChangeState(delimiterType);
						default:
							break;
						};
					break;
				case EParsingScheme:
					if (stringLength)
						{
						delete iScheme;
						iScheme = NULL;
						iScheme = HBufC::NewL(stringLength);
						(*iScheme) = iUriText->Mid(stringStart, stringLength);
						}
					ChangeState(delimiterType);
					break;
				case EParsingNetLoc:
					if (stringLength)
						{
						delete iNetLoc;
						iNetLoc = NULL;
						iNetLoc = HBufC::NewL(stringLength);
						(*iNetLoc) = iUriText->Mid(stringStart, stringLength);
						}
					ChangeState(delimiterType);
					break;
				case EParsingPath:
					if (stringLength)
						{
						iDirectoryPath->AppendL(iUriText->Mid(stringStart, stringLength));
						}
					if ((delimiterType!=EImUriPath) && (stringLength != 0))
						{
						iContainsFileName = ETrue;
						}
					ChangeState(delimiterType);
					break;
				case EParsingParameter:
					if (stringLength)
						{
						// If more than one parameters are there
						if(iParameters)
							{
							TInt previousParameterLength = (iParameters->Des()).Length() + 1;
							iParameters = iParameters->ReAlloc(previousParameterLength + stringLength);
							//Append ";" between the parameters
							(iParameters->Des()).Append(TChar(';'));
							HBufC* tempParameter = HBufC::NewL(stringLength);
							*tempParameter = iUriText->Mid(stringStart, stringLength);
							(iParameters->Des()).Append(tempParameter->Des());
							delete tempParameter;
							}
						// If all the parameters has been processed or only one parameter element
						else
							{
							iParameters = HBufC::NewL(stringLength);
							(*iParameters) = iUriText->Mid(stringStart, stringLength);
							}
						}
					ChangeState(delimiterType);
					break;
				case EParsingQuery:
					if (stringLength)
						{
						delete iQuery;
						iQuery = NULL;
						iQuery = HBufC::NewL(stringLength);
						(*iQuery) = iUriText->Mid(stringStart, stringLength);
						}
					ChangeState(delimiterType);
					break;
				case EParsingFragment:
					if (stringLength)
						{
						delete iFragment;
						iFragment = NULL;
						iFragment = HBufC::NewL(stringLength);
						(*iFragment) = iUriText->Mid(stringStart, stringLength);
						}
					ChangeState(delimiterType);
					break;
				}

			stringStart = textIndex + 1;
			stringLength = 0;
			}
		else
			{
			stringLength++;
			}

		textIndex++;
		}
	}

void CImMhtmlUri::Normalise()
	{
	_LIT(KImSinglePeriod, ".");
	_LIT(KImDoublePeriod, "..");

	// Remove any single '.' character at the end of the path as this will have replaced a file name if it were present.
	if (iDirectoryPath->Count() != 0)
		{
		if ((*iDirectoryPath)[iDirectoryPath->Count() - 1] == KImSinglePeriod)
			{
			// Remove the filename.
			iContainsFileName = EFalse;
			iDirectoryPath->Delete(iDirectoryPath->Count() - 1);
			}
		}


	TBool directoryRemoved = ETrue;
	TInt count;
	TInt counter;
	while (directoryRemoved)
		{
		directoryRemoved = EFalse;
		count = iDirectoryPath->Count();
		counter = 0;
		while ((counter < count) && (!directoryRemoved))
			{
			if ((*iDirectoryPath)[counter] == KImDoublePeriod)
				{
				if (counter > 0)
					{
					directoryRemoved = ETrue;
					iDirectoryPath->Delete(counter - 1, 2);
					if (counter == count - 1)
						{
						iContainsFileName = EFalse;
						}
					}
				}
			else if ((*iDirectoryPath)[counter] == KImSinglePeriod)
				{
				iDirectoryPath->Delete(counter);
				directoryRemoved = ETrue;
				}
			counter++;
			}
		}
	}

CImMhtmlUri::TImUriDelimiterType CImMhtmlUri::Delimiter(TInt& aIndex)
	{
	TImUriDelimiterType delimiterType = EImUriNotDelimiter;

	if (aIndex >= iUriText->Length())
		{
		return EImUriEnd;
		}

	TChar currentCharacter = (*iUriText)[aIndex];

	if ((iState == EParsingFirstString)
		&& (TChar(':') == currentCharacter))
		{
		return EImUriScheme;
		}

	if (((iState == EParsingFirstString)
		|| (iState == EParsingNetLoc)
		|| (iState == EParsingPath))
		&& (TChar('/') == currentCharacter))
		{
		// Path or NetLoc, so check the next character if there is one.
		if (aIndex + 1 < iUriText->Length())
			{
			if (TChar('/') == TChar((*iUriText)[aIndex+1]))
				{
				aIndex++;
				return EImUriNetLoc;
				}
			else
				{
				return EImUriPath;
				}
			}
		else
			{
			return EImUriPath;
			}
		}

	if ((iState != EParsingQuery)
		&& (iState != EParsingFragment)
		&& (TChar(';') == currentCharacter))
		{
		return EImUriParameter;
		}

	if ((iState != EParsingFragment)
		&& (TChar('?') == currentCharacter))
		{
		return EImUriQuery;
		}

	if (TChar('#') == currentCharacter)
		{
		return EImUriFragment;
		}

	return delimiterType;
	}


void CImMhtmlUri::ChangeState(TImUriDelimiterType aDelimiterType)
	{
	switch (aDelimiterType)
		{
		case EImUriScheme:
			iState = EParsingFirstString;
			break;
		case EImUriNetLoc:
			iState = EParsingNetLoc;
			break;
		case EImUriPath:
			iState = EParsingPath;
			break;
		case EImUriParameter:
			iState = EParsingParameter;
			break;
		case EImUriQuery:
			iState = EParsingQuery;
			break;
		case EImUriFragment:
			iState = EParsingFragment;
			break;
		case EImUriEnd:
			iState = EParsingFirstString;
			break;
		default:
			break;
		}
	}

//CImCacheUriEntry
CImCacheUriEntry* CImCacheUriEntry::NewL(const TDesC& aContentLocation, const TDesC& aContentId,TMsvId aEntry)
	{
	CImCacheUriEntry* self = new (ELeave) CImCacheUriEntry();
	CleanupStack::PushL(self);
	self->ConstructL(aContentLocation,aContentId,aEntry);
	CleanupStack::Pop();
	return self;
	}
	
void CImCacheUriEntry::ConstructL(const TDesC& aContentLocation, const TDesC& aContentId,TMsvId aEntry)
	{
	iContentLocation = aContentLocation.AllocL();
	iContentId = aContentId.AllocL();
	iUriEntry = aEntry;
	}
/**
Constructor
*/
CImCacheUriEntry::CImCacheUriEntry()
	{
	}
/**
Destructor
*/
CImCacheUriEntry::~CImCacheUriEntry()
	{
	delete iContentLocation;
	delete iContentId;
	}
/**
@return URI Contentlocation
*/
HBufC* CImCacheUriEntry::GetContentLocation()
	{
	return (iContentLocation);
	}
/**
@return ContentId
*/
HBufC* CImCacheUriEntry::GetContentId()
	{
	return (iContentId);
	}
/**
@return URI Entry Id
*/
TMsvId CImCacheUriEntry::GetUriEntry()
	{
	return iUriEntry;
	}
