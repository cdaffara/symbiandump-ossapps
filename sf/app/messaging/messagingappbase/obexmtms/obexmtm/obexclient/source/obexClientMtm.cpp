// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//class include
#include <obexclientmtm.h>

//system includes
#include <e32std.h>
#include <e32base.h>
#include <txtrich.h>	// CRichText

#include <mtmuids.h>	// KUidMtmQueryCanSendMsg
#include <msvreg.h>		// CRegisteredMtmDll
#include <mtmdef.h>		// KUidMtmQueryxxx & TMsvPartList flags
#include <msvuids.h>	// KUidMsvMessageEntry
#include <obexheader.h> // CObexMtmHeader


#include <mmsvattachmentmanager.h>
#include <msvids.h>


#include <obexheaderlist.h>

//user includes
#include "obexClientMtmPanic.h"


const TInt KMaxObexPasswordLength = 30;

//
//  CObexClientMtm
//
EXPORT_C CObexClientMtm::CObexClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession, TUid aMsgTypeUid)
: CBaseMtm(aRegisteredMtmDll, aMsvSession),
iMsgTypeUid(aMsgTypeUid)
/**
 * Constructor. Initialises iMsgTypeUid with Uid provided by the derived class.
 *
 * @param aRegisteredMtmDll Registration data for MTM DLL.
 * @param aMsvSession CMsvSession of the client requesting the object.
 * @param aMsgTypeUid Uid of the message
 */
	{
	}

EXPORT_C void CObexClientMtm::ConstructL()
/**
 * Empty implementation provided for future-proofing.
 */
	{
	}

EXPORT_C CObexClientMtm::~CObexClientMtm()
/**
 * Destructor deletes FileNameExternaliser and Header
 */
	{
	delete iHeader;
	}

EXPORT_C void CObexClientMtm::SaveMessageL()
/**
 * Commits cached changes to the message store, by calling CommitChangesL. 
 *
 * @leave KErrXxx System-wide error codes if message cannot be saved.
 */
	{
	//Commit header to store. 
	CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL(store);
	iHeader->StoreL(*store);
    store->CommitL();
    CleanupStack::PopAndDestroy();
	
	CommitChangesL();

	}

EXPORT_C void CObexClientMtm::CommitChangesL()
/**
 * Commits cached changes to the message store. 
 *
 * @leave KErrXxx System-wide error codes if changes cannot be committed.
 */
	{	// OBEX messages are very simple. Outgoing data is expected to be in attachments. To save space filenames 
		// can be stored within entry (instead of making copies of the data), which will be loaded upon send.
	if(Body().DocumentLength() > 0)
		{
		User::Leave(KErrNotSupported);  // Data is not expected in the message body
		}

	// Update entry details
	TMsvEntry messageEntry=iMsvEntry->Entry();

	// since CBaseMtm::CreateMessageL creates message as in preparation and non-visible, 
	// must now make it visible and not in preparation
	messageEntry.SetVisible(ETrue);
	messageEntry.SetInPreparation(EFalse);

	messageEntry.iSize = GetAttachmentSizeL();
	messageEntry.iDescription.Set(SubjectL());

	if (AddresseeList().Count() > 0)
		messageEntry.iDetails.Set(AddresseeList()[0]);

	// Display the required send status.
	messageEntry.SetSendingState(KMsvSendStateWaiting); // Quartz are checking this flag for ir messaging for sending

	// update the contents of the message entry
	iMsvEntry->ChangeL(messageEntry);
	}


	
EXPORT_C void CObexClientMtm::CreateMessageL(TMsvId /*aServiceId*/)
/**
 * Create a new message for this MTM. This creates a new messaging entry, then
 * re-initialises the MTM's CObexMtmHeader object.
 *
 * @param aServiceId The ID of the service for which the message is to be created
 * @leave KErrXxx Standard EPOC error codes if construction of new objects fails
 */
    {
    // First create the message as normal: this creates a new entry of the current MTM type
    CBaseMtm::CreateMessageL(KMsvRootIndexEntryId);

    // Now call ContextEntrySwitched as this isn't called from CBaseMtm::CreateMessageL
    ContextEntrySwitched();

    // Now create a new iHeader
    InitialiseHeaderL();
    }


EXPORT_C void CObexClientMtm::LoadMessageL()
/**
 * This function loads the header from the message store
 *
 * @leave KErrXXX system-wide error codes
 */
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EObexMtmNoCMsvEntrySet));
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue, gPanic(EObexMtmNotAMessageEntry));

	//(Re-)initialise header to appropriate CObexMtmHeader derived class
	InitialiseHeaderL();


	// INC042015:
	// catch and "pre-process" here errors in ReadStoreL.
	// Ignore the error if the leave occurs because there is no store, UNLESS the message
	// is being sent (in this case the header IS needed).
	CMsvStore* store = NULL;
	TRAPD(err, store = iMsvEntry->ReadStoreL() );
	if (err == KErrNone) // store is OK
		{
		CleanupStack::PushL(store);
		iHeader->RestoreL(*store);
		CleanupStack::PopAndDestroy();
		}
	else if ( (err == KErrNotFound) && (iMsvEntry->Entry().Parent() != KMsvGlobalOutBoxIndexEntryId) )
		{  // ignore error in this particular case: the header is not needed.
		return;
		}
	else // in all other cases, pass on the error as was done before
		{
		User::LeaveIfError(err);
		}
	}	


EXPORT_C TInt32 CObexClientMtm::GetAttachmentSizeL()
/**
 * This function calculates the size of all attachments to the message by either:
 *
 *  @li summing the sizes of each of the children or;
 *  @li reading in all of the filenames from the store, and summing their sizes.*
 *
 *  @return total size of the attachments.
 *  @leave KErrXXX system-wide error codes
 */
	{
	TInt size=0;	//Total attachment size



		// size has not yet been set, must be using the Attachment API, so use
		// the Attachment API to calculate size
		CMsvStore* store = iMsvEntry->ReadStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& manager = store->AttachmentManagerL();
		TInt attachmentCount = manager.AttachmentCount();
		for( TInt ii=0; ii<attachmentCount; ++ii )
			{
			// loop through each attachment...
			CMsvAttachment* attachmentInfo = manager.GetAttachmentInfoL(ii);
			if( attachmentInfo->Type() == CMsvAttachment::EMsvMessageEntry )
				{
				// Only EFileAttachment and ELinkedFile are supported
				User::Leave(KErrArgument);
				}
			size += attachmentInfo->Size();
			delete attachmentInfo;		
			}
		CleanupStack::PopAndDestroy(store);


	return size;
	}

//

EXPORT_C CMsvOperation* CObexClientMtm::ReplyL(TMsvId /*aReplyEntryId*/, TMsvPartList /*aPartlist*/,  TRequestStatus& /*aCompletionStatus*/)
/**
 * Unsupported
 *
 * @leave Leaves always with KErrNotSupported
 */
	{  
	User::Leave(KErrNotSupported);
	return (CMsvOperation*)NULL;
	}

EXPORT_C CMsvOperation* CObexClientMtm::ForwardL(TMsvId /*aForwardEntryId*/, TMsvPartList /*aPartList*/, TRequestStatus& /*aCompletionStatus*/)
/**
 * Unsupported
 *
 *  @leave Leaves always with KErrNotSupported
 */
	{
	User::Leave(KErrNotSupported);
	return (CMsvOperation*)NULL;
	}

EXPORT_C TUint CObexClientMtm::ValidateMessage(TMsvPartList aPartList)
/**
 * Ensure that the current message context part indicated is valid. Currently only works on KMsvMessagePartRecipient--all other parts
 * are assumed to be valid
 *
 * @param aPartList Bitmask indicating parts of message to be validated
 * @return Bitmask of any invalid parts--KErrNone otherwise
 */
	{ 
	TMsvPartList failed = KErrNone;  // No errors.

	if (!iAddresseeList)
		{
		return KMsvMessagePartRecipient;
		}

	if(aPartList & KMsvMessagePartRecipient)
		{
		// All actual recipients should have at least one character.
		for (TInt ii=0; ii < iAddresseeList->Count(); ++ii)
			{
			if((*iAddresseeList)[ii].Length() == 0)
				{
				failed |= KMsvMessagePartRecipient;
				break;
				}
			}
		}
	return failed;
	}


EXPORT_C TMsvPartList CObexClientMtm::Find(const TDesC& /*aTextToFind*/, TMsvPartList /*aPartList*/)
/**
 * Finds the given text within the specified parts of the current message context. NOT IMPLEMENTED!
 *
 * @param aTextToFind Reference to the text to search for in the specified message parts
 * @param aPartList Bitmask indicating parts of message to be searched for the text
 * @return Bitmask of any parts containing the message text
 */
	{
	return 0;
	}


EXPORT_C void CObexClientMtm::SetSubjectL(const TDesC& aSubject)
/**
 * Sets the subject of the current message context
 *
 * @param aSubject Reference to the new message subject text
 * @leave Leaves if creation of new HBufC fails
 */
	{
	iHeader->SetSubjectL(aSubject);
	}


EXPORT_C const TPtrC CObexClientMtm::SubjectL() const
/**
 * Returns the subject of the current message context. Will not leave.
 *
 * @return const TPtrC representation of the Subject, or an empty TPtrC if the subject hasn't been set
 *  @leave Never
 */
	{
	return iHeader->Subject();
	}


EXPORT_C void CObexClientMtm::AddAddresseeL(const TDesC& aRealAddress)
/**
 * Adds addressee to the addressee "list". Note that this MUST be a TDesC containing binary data representing
 * the TSockAddress of the addressee, usually as a TDesC16. This function will then package the data directly into 
 * 8 bit data.
 * NB: Only ONE addressee is supported
 *
 * @param aRealAddress Reference to the address of the recipient
 * @leave KErrXXX if address cannot be appended
 * @leave KErrAlreadyExists if the Addressee "list" already contains an entry
 */
	{
	if (iAddresseeList->Count()==0)
		{
		//list is empty--OK to add addressee
		iAddresseeList->AppendL(aRealAddress);

		}
	else
		{
		//list already contains an entry--leave with KErrAlreadyExists
		User::Leave(KErrAlreadyExists);
		}
	}

EXPORT_C void CObexClientMtm::AddAddresseeL(const TDesC& aRealAddress, const TDesC& /*aAlias*/)

/**
 * Adds addressee to the addressee "list". Note that this MUST be a TDesC containing binary data representing
 * the TSockAddress of the addressee, usually as a TDesC16. This function will then package the data directly into 
 * 8 bit data.
 * NB: Only ONE addressee is supported
 *
 * @param aRealAddress Reference to the address of the recipient
 * @param aAlias Reference to the alias of the recipient--ignored in this implementation
 * @leave KErrXXX if address cannot be appended
 * @leave KErrAlreadyExists if the Addressee "list" already contains an entry
 */
	{
	AddAddresseeL(aRealAddress); 
	}

EXPORT_C void CObexClientMtm::RemoveAddressee(TInt /*aIndex*/)
/**
 * Removes addressee at index aIndex from the addressee "list".
 *
 * @param aIndex zero-based index of the addressee (ignored since "there can be only one").
 */
	{
	if (iAddresseeList->Count() > 0)	//since only one addressee is supported anyhow
		{
		iAddresseeList->Delete(0);
		}
	}

EXPORT_C void CObexClientMtm::ContextEntrySwitched()
/**
 * Resets the MTM when the context is switched.
 * Must be called by the derived class's ContextEntrySwitched prior to constructing a new header.
 */
	{
	iAddresseeList->Reset();
	delete iHeader;		//will be reconstructed in LoadMessageL()
	iHeader = NULL;
	}


EXPORT_C TInt CObexClientMtm::QueryCapability(TUid aCapability, TInt& aResponse)
/**
 * Gives the capability of the MTM. 
 *
 * @param aCapability capability to be queried
 * @param aResponse capability dependent return value.
 * @return KErrNone if the capability is supported, or KErrNotSupported if not.
 */
	{
	TInt error = KErrNone;
	switch (aCapability.iUid)
		{
		case KUidMtmQueryMaxBodySizeValue:
		case KUidMtmQueryMaxTotalMsgSizeValue:
			aResponse = KMaxTInt;
			break;
		case KUidMtmQuerySupportAttachmentsValue:
		case KUidMtmQuerySupportSubjectValue:
		case KUidMtmQueryCanSendMsgValue:
		case KUidMtmQuerySupportsBioMsgValue:
		case KUidMtmQuerySendAsMessageSendSupportValue:
			break;
		case KUidMtmQueryMaxRecipientCountValue:
			aResponse = 1;
			break;
		case KUidMtmQuerySupportedBodyValue:
			aResponse = KMtm7BitBody + KMtm8BitBody + KMtm16BitBody + KMtmBinaryBody;
			break;
		case KUidMsvMtmQueryEditorUidValue: // Return NULL for editor value because there is no OBEX editor.
			break;
		default:
			return KErrNotSupported;
		}
	return error;
	}

/**
Obex based MTMs do not support message entry attachments. This method will leave with
KErrNotSupported.
@param aAttachmentId This is not supported.
@leave KErrNotSupported This method is not supported.
*/
EXPORT_C void CObexClientMtm::CreateMessageAttachmentL(TMsvId /*aAttachmentId*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
No functionality is provided by this method. Leaves with KErrNotSupported.
@param aFunctionId Unused.
@param aSelection Unused.
@param aParameter Unused.
@leave KErrNotSupported
*/
EXPORT_C void CObexClientMtm::InvokeSyncFunctionL(TInt /*aFunctionId*/, const CMsvEntrySelection& /*aSelection*/, TDes8& /*aParameter*/)
	{
	User::Leave(KErrNotSupported);
	}


