// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client MTM for the SMTP protocol
// 
//

#include <bautils.h>	// BaflUtils::NearestLanguageFile()
#include <txtrich.h>	// CRichText

#include <barsc.h>		//RResourceFile
#include <barsread.h>

#include <msvstore.h>	// CMsvStore
#include <mtmdef.h>		//KUidMtmQueryxxx & TMsvPartList flags
#include <imcm.rsg>
#include <mmsvattachmentmanager.h>
#include <cmsvmimeheaders.h>
#include <cemailaccounts.h>
#include "CImAttachmentWaiter.h"
#include "cmsvsmtpsendoperation.h"
#include "MIUTHDR.H"	//CImHeader
#include "SMTCMTM.H"
#include "MIUT_ERR.H"
#include "IMCMMAIN.H"
#include "IMCMUTIL.H"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include <mtmuidsdef.hrh>
#include "timrfc822datefield.h" 
#include "miut_errconsts.h"
#include "msvconsts.h"
#include "cimmessagepart.h"
#endif

#include <msvenhancesearchsortutil.h>

#if defined (_UNICODE)
	#define KUidMsgInternetMailEditorDLL	0x10003C53  // (268450899)
#else
	#define KUidMsgInternetMailEditorDLL	0x100011AC  // (268439980)
#endif

/**
@internalTechnology
*/

EXPORT_C CSmtpClientMtm* CSmtpClientMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
	{
	CSmtpClientMtm* self = new(ELeave) CSmtpClientMtm(aRegisteredMtmDll, aMsvSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSmtpClientMtm::CSmtpClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
	: CBaseMtm(aRegisteredMtmDll, aMsvSession)
	{
	__DECLARE_NAME(_S("CSmtpClientMtm"));
	}

void CSmtpClientMtm::ConstructL()
	{
	iWait=CMsvOperationActiveSchedulerWait::NewLC();
	CleanupStack::Pop();	// iWait

	iImSmtpSettings = new (ELeave) CImSmtpSettings;
	iHeader = CImHeader::NewLC();
	CleanupStack::Pop();	// iHeader

	//open the resource file
	RResourceFile resourceFile;
	OpenResourceFileL(resourceFile, Session().FileSession());
	CleanupClosePushL(resourceFile);

	HBufC8* buf = resourceFile.AllocReadLC(EMAIL_ADDRESS_FORMATTING_STRING);
	TResourceReader reader;
	reader.SetBuffer(buf);
	iEmailAddressFormatString = (reader.ReadTPtrC()).AllocL();
	CleanupStack::PopAndDestroy(2); // resourceFile (Close resourceFile), buf
	}

CSmtpClientMtm::~CSmtpClientMtm()
/** Destructor. */
	{
	delete iWait;
	delete iImSmtpSettings;
	delete iHeader;
	delete iSubject;
	delete iEmailAddressFormatString;
	delete iEntrySelection;
	delete iImEmailOperation;
	delete iAttachmentWaiter;
	delete iEmailMessage;
	}

void CSmtpClientMtm::ResetData()
	{
	// The client MTM owns a number of objects which are used to store data
	// from service settings and messages.
	iAddresseeList->Reset();
	iImSmtpSettings->Reset();
	
	delete iSubject;
	iSubject = NULL;

	iHeader->Reset();
	Body().Reset();	// discard old contents of CRichText object
	}

void CSmtpClientMtm::SaveMessageL()
/** Commits cached changes to the current message context to the storage controlled 
by the Message Server. */
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ESmtcMTMNoCMsvEntrySet));
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue, gPanic(ESmtcMTMNotAMessageEntry));
	
	StoreEmailMessageL();
	}

void CSmtpClientMtm::StoreEmailMessageL()
	{
	__ASSERT_DEBUG(KUidMsvMessageEntryValue==iMsvEntry->Entry().iType.iUid, gPanic(ESmtcMTMNotAMessageEntry));
	CMsvStore* store = iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);

	// Save the Email header stream...
	iHeader->SetSubjectL(SubjectL());
	const TInt numberOfRecipients=iAddresseeList->Count();
	// put all addressees in the To: field...
	for (TInt n=0 ; n < numberOfRecipients ; ++n)
		{
		// validation done by ValidateMessage()
		switch(iAddresseeList->Type(n))
			{
		case EMsvRecipientTo:
			iHeader->ToRecipients().AppendL(AddresseeList()[n]);
			break;
		case EMsvRecipientCc:
			iHeader->CcRecipients().AppendL(AddresseeList()[n]);
			break;
		case EMsvRecipientBcc:
			iHeader->BccRecipients().AppendL(AddresseeList()[n]);
			break;
			}
		}

	iHeader->StoreL(*store);
	store->CommitL();

	// NB the From field will be set by server MTM when the email message is sent

	CleanupStack::PopAndDestroy();

	TMsvEntry messageEntry=iMsvEntry->Entry();
	CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*iMsvEntry);

	// now store the body text
	emailMessage->StoreBodyTextL(messageEntry.Id(), Body(), iWait->iStatus);
	iWait->Start();		// wait for the asynch operation to complete
	
	// 
	// I have stored away all the email data,
	// now update the TMsventry that represents
	// the email to ensure that it is up to date
	//
	TInt32 totalSizeOfAllAttachments = GetAttachmentSizeL(*emailMessage, messageEntry.Id());

	messageEntry.iSize = iHeader->DataSize() + Body().DocumentLength() + totalSizeOfAllAttachments;
	messageEntry.iDescription.Set(iHeader->Subject());
	messageEntry.iDate.UniversalTime();

	// fix for DEF051564 - SMTP client MTM CreateMessageL not creating message in correct state
	//
	// since CreateMessageL creates message as in preparation and non-visible, the SaveMessageL
	// must now make it visible and not in preparation
	messageEntry.SetVisible(ETrue);
	messageEntry.SetInPreparation(EFalse);

	if (iHeader->ToRecipients().Count()>0)
		messageEntry.iDetails.Set(iHeader->ToRecipients()[0]);
	else if (iHeader->CcRecipients().Count())
		messageEntry.iDetails.Set(iHeader->CcRecipients()[0]);
	else if (iHeader->BccRecipients().Count())
		messageEntry.iDetails.Set(iHeader->BccRecipients()[0]);
	//else do nothing as there are no recipients yet!

	if (totalSizeOfAllAttachments>0)
		messageEntry.SetAttachment(ETrue);

	// if there are multiple recipients then set the flag
	if ((iHeader->ToRecipients().Count() + iHeader->CcRecipients().Count() + iHeader->BccRecipients().Count()) >=2)
		messageEntry.SetMultipleRecipients(ETrue);

	// update the contents of the message entry
	iMsvEntry->ChangeL(messageEntry);

	CleanupStack::PopAndDestroy();	// emailMessage
	}

EXPORT_C void CSmtpClientMtm::StoreSettingsL()
/** Stores the current service settings from the object's cache in to the Central 
Repository for the current entry.

The current entry must be a service. */
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvServiceEntryValue, gPanic(ESmtcMTMNotAServiceEntry));

	CEmailAccounts* account = CEmailAccounts::NewLC();
  	TSmtpAccount id;
	account->GetSmtpAccountL(iMsvEntry->Entry().Id(), id);
	account->SaveSmtpSettingsL(id, *iImSmtpSettings);
	CleanupStack::PopAndDestroy(account);    
	}

EXPORT_C void CSmtpClientMtm::RestoreSettingsL()
/** Loads into the object's cache the service settings from the Central Repository
for the current entry. */
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvServiceEntryValue, gPanic(ESmtcMTMNotAServiceEntry));

	CEmailAccounts* account = CEmailAccounts::NewLC();
  	TSmtpAccount id;
	account->GetSmtpAccountL(iMsvEntry->Entry().Id(), id);
	account->LoadSmtpSettingsL(id, *iImSmtpSettings);
	CleanupStack::PopAndDestroy(account);    
	}

void CSmtpClientMtm::LoadMessageL()
/** Loads the cache with the message data for the current context. */
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ESmtcMTMNoCMsvEntrySet));
	switch (iMsvEntry->Entry().iType.iUid)
		{
		case KUidMsvServiceEntryValue:
			RestoreSettingsL();
			break;
		case KUidMsvMessageEntryValue:
			RestoreEmailMessageL();
			break;
		};
	}	

void CSmtpClientMtm::RestoreEmailMessageL()
	{

	// Get a reference to TMsvEnhanceSearchSortUtil  instance set by CMsvSearchsortOpOnHeaderBody class
	// If advanced search and sort is being performed than do not load the message header and body.
	// These are loaded when the search criteria is known in DoFindL()
	// For API's other than CMsvSearchsortOpOnHeaderBody-> FindInHeaderBodyL(), a call to LoadMessageL()
	// loads the body and the header.

	TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
	if ( searchsortutil == NULL )
		{
		CMsvStore* msvStore = iMsvEntry->ReadStoreL();
		CleanupStack::PushL(msvStore);

		// message must have a CImHeader stream...if it's not there leave as there's something wrong
		iHeader->RestoreL(*msvStore);

		TPtrC subject = iHeader->Subject();
		TPtrC to;
		if (iHeader->ToRecipients().Count())
			to.Set(iHeader->ToRecipients()[0]);
		else if (iHeader->CcRecipients().Count())
			to.Set(iHeader->CcRecipients()[0]);
		else if (iHeader->BccRecipients().Count())
			to.Set(iHeader->BccRecipients()[0]);
	//else do nothing as there are no recipients!

		SetSubjectL(subject);
		SetAddresseeListL();

		// Get the attachments and the body text...
		TMsvEntry messageEntry=iMsvEntry->Entry();

		CleanupStack::PopAndDestroy(); // msvStore
		CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*iMsvEntry);

		GetBodyTextL(*emailMessage, messageEntry.Id());
		TInt32 totalSizeOfAllAttachments = GetAttachmentSizeL(*emailMessage, messageEntry.Id());

		messageEntry.iSize = iHeader->DataSize() + Body().DocumentLength() + totalSizeOfAllAttachments;
		messageEntry.iDescription.Set(subject);
		messageEntry.iDetails.Set(to);

	// update the contents of the message entry
		iMsvEntry->ChangeL(messageEntry);
	
		CleanupStack::PopAndDestroy(); // emailMessage
	
		}
	}


void CSmtpClientMtm::GetBodyTextL(CImEmailMessage& aMessage, TMsvId aMsvId)
	{
	CRichText* messageText = CRichText::NewL(iParaFormatLayer, iCharFormatLayer);
	CleanupStack::PushL(messageText);

	aMessage.GetBodyTextL(aMsvId, CImEmailMessage::EThisMessageOnly, *messageText,*iParaFormatLayer,*iCharFormatLayer);

	Body().Reset();
	Body().AppendTakingSolePictureOwnershipL(*messageText);

	CleanupStack::PopAndDestroy();
	}

TInt32 CSmtpClientMtm::GetAttachmentSizeL(CImEmailMessage& aMessage, TMsvId aMsvId)
	{
	// Calculate the total size of all attachments associated with this message
	TInt total=0;
	aMessage.GetAttachmentsListL(iWait->iStatus, aMsvId, CImEmailMessage::EAllAttachments,CImEmailMessage::EThisMessageOnly);
	iWait->Start();		// wait for the asynch operation to complete
	TInt numAttachments=aMessage.AttachmentManager().AttachmentCount();
	for (TInt n=0 ; n<numAttachments ; ++n)
		{
		CMsvAttachment* attachment = aMessage.AttachmentManager().GetAttachmentInfoL(n);
		total+=attachment->Size();
		delete attachment;
		}
	return total;
	}

CMsvOperation* CSmtpClientMtm::ReplyL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus)
/** Creates a reply message to the current message context. 

@return	If successful, this is an asynchronously completing reply operation. 
If failed, this is a completed operation, with status set to the relevant error code. 
@param aDestination The entry to which to assign the reply 
@param aPartList Defines the parts that are to be copied from the original message into the reply 
@param aCompletionStatus The request status to be completed when the operation has finished 
*/
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TUid messageType = KUidMsgTypeSMTP;
	return CImEmailOperation::CreateReplyL(aCompletionStatus, Session(), iMsvEntry->EntryId(), aDestination, aPartList, msvEmailTypeList, messageType);
	}

CMsvOperation* CSmtpClientMtm::ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus)
/** Creates a forwarded message from the current message context. 

@return	If successful, this is an asynchronously completing forward message operation. 
If failed, this is a completed operation, with status set to the relevant error code. 		
@param aDestination The entry to which to assign the forwarded message 
@param aPartList Defines the parts that are to be copied from the original message into the forwarded message 
@param aCompletionStatus The request status to be completed when the operation has finished 
*/
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TUid messageType = KUidMsgTypeSMTP;
	return CImEmailOperation::CreateForwardL(aCompletionStatus, Session(), iMsvEntry->EntryId(), aDestination, aPartList, msvEmailTypeList, messageType);
	}

EXPORT_C void CSmtpClientMtm::SetSubjectL(const TDesC& aSubject)
/** Sets the message context's subject text.

@param aSubject Subject text */
	{
	HBufC* newSubject= aSubject.AllocL();
	delete iSubject;
	iSubject = newSubject;	
	}

EXPORT_C const TPtrC CSmtpClientMtm::SubjectL() const
/** Gets the message context's subject text.

@return Subject text */
	{
	// NB won't ever leave
	return (iSubject) ? TPtrC(*iSubject) : TPtrC();
	}

TMsvPartList CSmtpClientMtm::ValidateMessage(TMsvPartList aPartList)
/** Validates the current message context.

The addresses for the message are checked to be well-formed email addresses.

@param aPartList Indicates the message parts for which validation is requested 
@return If valid, KErrNone If invalid, identifies the invalid part(s). The 
error value is the bitmask of the TMsvPartList IDs for each invalid part */
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ESmtcMTMNoCMsvEntrySet));

	TMsvPartList failed(0);
	if (aPartList & KMsvMessagePartRecipient)
		{
		if (iAddresseeList->Count() == 0)
			failed |= KMsvMessagePartRecipient;
		else
			{
			// check the recipient list for valid 'addresses'
			for (TInt ii=0; ii < iAddresseeList->Count(); ++ii)
				{
				if (((*iAddresseeList)[ii].Length() == 0) || !ValidateAddress((*iAddresseeList)[ii]))
					{
					failed |= KMsvMessagePartRecipient;
					break;
					}
				}
			}
		}
	return failed;
	}

TBool CSmtpClientMtm::ValidateAddress(const TPtrC& anAddress)
	{
	return iTImMessageField.ValidInternetEmailAddress(anAddress);
	}

TMsvPartList CSmtpClientMtm::DoFindL(const TDesC& aTextToFind, TMsvPartList aPartList)
	{
	CImClientMTMUtils* clientMTMUtils = CImClientMTMUtils::NewL();
	CleanupStack::PushL(clientMTMUtils);

	TMsvPartList retList = KMsvMessagePartNone;
	
	// Get a reference to TMsvEnhanceSearchSortUtil instance set by CMsvSearchsortOpOnHeaderBody class	
	TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
	
	// searchsortuitl variable will not be NULL for Advanced Search and Sort called from CMsvSearchsortOpOnHeaderBody
 	// For the old implementation, it will be NULL
	if(searchsortutil != NULL)
		{
		// Get the searchsort setting flags
		TUint32 searchsortsetting=searchsortutil->GetSearchSortSetting();
		
		// The body was not loaded in LoadMessageL()
 		// If 2 search query options are on the body or on the header, than it sets EMessagePartBodyLoaded flag
 		// or EMessagePartHeaderLoaded of searchsortsetting
		if(aPartList & KMsvMessagePartBody && !(searchsortsetting & EMessagePartBodyLoaded))  	
   			{
   			// Restore the body
   			
			Body().Reset();	
			// Get the attachments and the body text...
			TMsvEntry messageEntry=iMsvEntry->Entry();
			CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*iMsvEntry);
			GetBodyTextL(*emailMessage, messageEntry.Id());
			CleanupStack::PopAndDestroy(); // emailMessage
			searchsortutil->SetSearchSortSetting(EMessagePartBodyLoaded);
			}
		else if (!(searchsortsetting & EMessagePartHeaderLoaded))
			{
			// Restore the header
			CMsvStore* msvStore = iMsvEntry->ReadStoreL();
			CleanupStack::PushL(msvStore);

			// message must have a CImHeader stream...if it's not there leave as there's something wrong
			iHeader->RestoreL(*msvStore);

			TPtrC subject = iHeader->Subject();
			TPtrC to;
			if (iHeader->ToRecipients().Count())
				to.Set(iHeader->ToRecipients()[0]);
			else if (iHeader->CcRecipients().Count())
				to.Set(iHeader->CcRecipients()[0]);
			else if (iHeader->BccRecipients().Count())
				to.Set(iHeader->BccRecipients()[0]);
			//else do nothing as there are no recipients!

			SetSubjectL(subject);
			SetAddresseeListL();

			// Get the attachments and the body text...
			TMsvEntry messageEntry=iMsvEntry->Entry();

			CleanupStack::PopAndDestroy(); // msvStore
			searchsortutil->SetSearchSortSetting(EMessagePartHeaderLoaded);
			}
			
		// Issue a request to FindL
		clientMTMUtils->FindL(aTextToFind, Body(), *iHeader, aPartList, retList);
		
		/* Copy the sort data if sorting is specified.
		 The operations being performed could be only be sort or it could be search and sort
		 If the operation is search and sort than copy the sort data only if the
		 search operation succeeded	*/
		 
		if ((searchsortsetting & EMessagePartSort ) || (((searchsortsetting & EMessagePartSearchSort) && (searchsortsetting & EMessagePartLastQueryOption) && (retList))))
   			{
   			/* Copy the data to be sorted from the header stream.
   			   This done by setting iExtensionData to point to the field being copied */
   			   
	   		if (iHeader)
   				{
   				if (searchsortsetting & EMessagePartToSort )
   					{
   					SetExtensionData((TAny*)&iHeader->ToRecipients());
	   				}
   				else if(searchsortsetting & EMessagePartCcSort)
   					{
   					SetExtensionData((TAny*)&iHeader->CcRecipients());
   					}
   				else if(searchsortsetting & EMessagePartBccSort)
 	 	  			{
    				SetExtensionData((TAny*)&iHeader->BccRecipients());
    				}
    			else if(searchsortsetting & EMessagePartFromSort)
    				{
    				SetExtensionData((TAny*)(iHeader->From().Ptr()));
    				}
				else if(searchsortsetting & EMessagePartSubjectSort)
					{
					SetExtensionData((TAny*)(iHeader->Subject().Ptr()));
					}
    			}
   			}
		}
	else
		{

		clientMTMUtils->FindL(aTextToFind, Body(), *iHeader, aPartList, retList);	
		}
    CleanupStack::PopAndDestroy(clientMTMUtils); 
	return retList;
	}

TMsvPartList CSmtpClientMtm::Find(const TDesC& aTextToFind, TMsvPartList aPartList)
/** Searches the specified message part(s) for the plain-text version of the text 
to be found.

@param aTextToFind The plain-text version of the text to be found. 
@param aPartList Indicates the message parts which should be searched. 
@return If the text was not found, or searching is unsupported, 0. If the text 
was found, a bitmask of the TMsvPartList IDs for each part in which the text 
was present. */
	{
 	TMsvPartList retList = KMsvMessagePartNone;
 	TRAPD(ret, retList = DoFindL(aTextToFind, aPartList));
   	return retList;
	}

void CSmtpClientMtm::AddAddresseeL(const TDesC& aRealAddress)
/** Adds an addressee for the current context.

@param aRealAddress String representing an address to be added to the list 
for the current message */
	{
	iAddresseeList->AppendL(EMsvRecipientTo, aRealAddress);
	}

void CSmtpClientMtm::SetAddresseeListL()
	{
	// fill in the addressee list from the contents of iHeader
	// copy all of the recipient addresses into the addressee list

	iAddresseeList->Reset();
	TInt numberOfRecipients=iHeader->ToRecipients().Count();
	TInt n;
	for (n=0 ; n < numberOfRecipients ; ++n)
		{
		TPtrC addressWithoutAlias = iTImMessageField.GetValidInternetEmailAddressFromString(iHeader->ToRecipients()[n]);
		AddAddresseeL(EMsvRecipientTo, addressWithoutAlias);
		}

	numberOfRecipients=iHeader->CcRecipients().Count();
	for (n=0 ; n < numberOfRecipients ; ++n)
		{
		TPtrC addressWithoutAlias = iTImMessageField.GetValidInternetEmailAddressFromString(iHeader->CcRecipients()[n]);
		AddAddresseeL(EMsvRecipientCc, addressWithoutAlias);
		}

	// there shouldn't be any Bcc: addresses in an incoming email
	// but add them anyway?
	numberOfRecipients=iHeader->BccRecipients().Count();
	for (n=0 ; n < numberOfRecipients ; ++n)
		{
		TPtrC addressWithoutAlias = iTImMessageField.GetValidInternetEmailAddressFromString(iHeader->BccRecipients()[n]);
		AddAddresseeL(EMsvRecipientBcc, addressWithoutAlias);
		}
	}

void CSmtpClientMtm::AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias)
/** Adds an addressee with an alias for the current context.

@param aRealAddress String representing an address to be added to the list 
for the current message 
@param aAlias Alias information */
	{
	HBufC* emailAddress = HBufC::NewLC(aRealAddress.Length()+aAlias.Length()+iEmailAddressFormatString->Length()-4);
	emailAddress->Des().Format(*iEmailAddressFormatString,&aAlias,&aRealAddress);
	iAddresseeList->AppendL(EMsvRecipientTo, *emailAddress);
	CleanupStack::PopAndDestroy();	// emailAddress
	}

void CSmtpClientMtm::RemoveAddressee(TInt aIndex)
/** Removes an address from the current address list.

@param aIndex Index of address to be removed */
	{
	if (iAddresseeList->Count() > aIndex)
		iAddresseeList->Delete(aIndex);
	}

void CSmtpClientMtm::AddAddresseeL(TMsvRecipientType aType, const TDesC& aRealAddress)
	{
	iAddresseeList->AppendL(aType, aRealAddress);
	}

void CSmtpClientMtm::AddAddresseeL(TMsvRecipientType aType, const TDesC& aRealAddress, const TDesC& aAlias)
	{
	HBufC* emailAddress = HBufC::NewLC(aRealAddress.Length()+aAlias.Length()+iEmailAddressFormatString->Length()-4);
	emailAddress->Des().Format(*iEmailAddressFormatString,&aAlias,&aRealAddress);
	iAddresseeList->AppendL(aType, *emailAddress);
	CleanupStack::PopAndDestroy(emailAddress);
	}
	
void CSmtpClientMtm::ContextEntrySwitched()
	{
	ResetData();
	}

TInt CSmtpClientMtm::QueryCapability(TUid aCapability, TInt& aResponse)
/** Queries if the MTM supports a particular capability, specified by a UID.

@param aCapability UID of capability to be queried 
@param aResponse Response value. The format of the response varies according 
to the capability. 
@return KErrNone: aCapability is a recognised value and a response is returned 
KErrNotSupported: aCapability is not a recognised value */
	{
	TInt error = KErrNone;
	switch (aCapability.iUid)
		{
	// Supported:
	case KUidMtmQueryMaxBodySizeValue:
	case KUidMtmQueryMaxTotalMsgSizeValue:
		aResponse = KMaxTInt;
		break;
	case KUidMsvMtmQueryEditorUidValue:
		aResponse = KUidMsgInternetMailEditorDLL;
		break;
	case KUidMtmQuerySupportedBodyValue:
		aResponse = KMtm7BitBody + KMtm8BitBody + KMtm16BitBody + KMtmBinaryBody;
		break;
	case KUidMtmQuerySupportSubjectValue:
	case KUidMtmQuerySupportAttachmentsValue:
	case KUidMtmQueryCanSendMsgValue:
	case KUidMtmQuerySupportsRecipientTypeValue:
	case KUidMtmQuerySendAsMessageSendSupportValue:
		break;
	// All others - Not Supported:
	default:
		error = KErrNotSupported;
		}
	return error;
	}

EXPORT_C void CSmtpClientMtm::SetSettingsL(const CImSmtpSettings& aSettings)
/** Copies the specified service settings to the cached service settings.

@param aSettings New service settings */
	{
	iImSmtpSettings->CopyL(aSettings);
	}

EXPORT_C const CImSmtpSettings& CSmtpClientMtm::Settings() const
/** Gets the current cached service settings.

@return The current cached service settings */
	{
	__ASSERT_DEBUG(iImSmtpSettings!=NULL, User::Invariant());
	return *iImSmtpSettings;
	}

void CSmtpClientMtm::InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter)
/** Invokes a synchronous SMTP-specific operation.

@param aFunctionId Specifies which operation to perform. The only valid ID is KSMTPMTMIsConnected.
@param aSelection A selection of messages for the operation.
@param aParameter Not used
*/
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ESmtcMTMNoCMsvEntrySet));

	TInt error = KErrNone;
	switch (aFunctionId)
		{
		case KSMTPMTMIsConnected:
			{
			TPckgBuf<TImSmtpProgress> progress;
			Session().TransferCommandL(aSelection, KSMTPMTMIsConnected, aParameter, progress);
			aParameter.Copy(progress);
			return;
			}
		default:
			error=KErrNotSupported;
			__ASSERT_DEBUG(EFalse,gPanic(ESmtcUnknownSyncFunction));
		}

	User::LeaveIfError(error);
	}

CMsvOperation* CSmtpClientMtm::InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection,
													TDes8& aParameter, TRequestStatus& aCompletionStatus)
/** Invokes asynchronous SMTP-specific operations.

@param aFunctionId Specifies which operation to perform e.g. connect, copy 
new mail etc. The specific operations are defined by the TSmtpCmds enumeration. 
@param aSelection A selection of messages that need to be copied/moved to a 
local folder. The first entry in this selection MUST be the service. 
@param aParameter Not used
@param aCompletionStatus The status when the operation completes. 
@leave KErrNotFound The selection of email to be moved or copied is empty
@leave KErrNotSupported The specified operation is not recognised
@return If successful, this is an asynchronously completing operation. If failed, 
this is a completed operation, with status set to the relevant error code. 
@see TSmtpCmds */
	{
	switch(aFunctionId)
		{
	case KMTMStandardFunctionsSendMessage:
		return CMsvSmtpProgressOperation::NewL(Session(), aSelection, KSMTPMTMSendOnNextConnection, aParameter, aCompletionStatus);
	case KSMTPMTMSendOnNextConnection:
		return (Session().TransferCommandL(aSelection, aFunctionId, aParameter, aCompletionStatus));
	case KSMTPMTMCreateNewEmailMessage:
	case KSMTPMTMCreateReplyEmailMessage:
	case KSMTPMTMCreateForwardEmailMessage:
	case KSMTPMTMCreateForwardAsAttachmentEmailMessage:
	case KSMTPMTMCreateReceiptEmailMessage:
		{
		TImCreateMessageOptions createMessageOptions;	
		TPckgC<TImCreateMessageOptions> paramPack(createMessageOptions);
		paramPack.Set(aParameter);
		switch (aFunctionId)
			{
		case KSMTPMTMCreateNewEmailMessage:
			return iImEmailOperation->CreateNewL(aCompletionStatus, iMsvEntry->Session(), aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KSMTPMTMCreateReplyEmailMessage:
			return iImEmailOperation->CreateReplyL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KSMTPMTMCreateForwardEmailMessage:
			return iImEmailOperation->CreateForwardL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KSMTPMTMCreateForwardAsAttachmentEmailMessage:
			return iImEmailOperation->CreateForwardAsAttachmentL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KSMTPMTMCreateReceiptEmailMessage:
			return iImEmailOperation->CreateReceiptL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
			}
		}
	break;
	default:
		User::Leave(KErrNotSupported);
		};
	return NULL;
	}

// Attachment functions to support the SendAs API

EXPORT_C void CSmtpClientMtm::AddAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue, gPanic(ESmtcMTMNotAMessageEntry));

	if( iAttachmentWaiter == NULL )
		{
		iAttachmentWaiter = CImAttachmentWaiter::NewL();
		}	

	if (iEmailMessage == NULL)
		{		
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}
	else if (iEmailMessage->EmailEntryId() != iMsvEntry->EntryId())
		{
		delete iEmailMessage;
		iEmailMessage = NULL;
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}

	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	
	attachmentInfo->SetMimeTypeL(aMimeType);
	TParse fileNameParser;
	User::LeaveIfError(fileNameParser.Set(aFilePath, NULL, NULL));
	attachmentInfo->SetAttachmentNameL(fileNameParser.NameAndExt());
	if( aCharset!=0 )
		{
		CMsvMimeHeaders* headers = CMsvMimeHeaders::NewLC();
		headers->SetMimeCharset(aCharset);
		headers->StoreL(*attachmentInfo);
		CleanupStack::PopAndDestroy(headers);
		}

	iEmailMessage->AttachmentManager().AddAttachmentL(aFilePath, attachmentInfo, iAttachmentWaiter->iStatus);
	CleanupStack::Pop(attachmentInfo);// ownership passed to attachment manager
	
	iAttachmentWaiter->StartWaitingL(aStatus, iEmailMessage, EFalse);
	}
	
EXPORT_C void CSmtpClientMtm::AddAttachmentL(RFile& aFile, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue, gPanic(ESmtcMTMNotAMessageEntry));

	if( iAttachmentWaiter == NULL )
		{
		iAttachmentWaiter = CImAttachmentWaiter::NewL();
		}
	
	if (iEmailMessage == NULL)
		{		
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}
	else if (iEmailMessage->EmailEntryId() != iMsvEntry->EntryId())
		{
		delete iEmailMessage;
		iEmailMessage = NULL;
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}

	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	
	attachmentInfo->SetMimeTypeL(aMimeType);
	TFileName fileName;	
	aFile.Name(fileName);
	attachmentInfo->SetAttachmentNameL(fileName);
	if( aCharset!=0 )
		{
		CMsvMimeHeaders* headers = CMsvMimeHeaders::NewLC();
		headers->SetMimeCharset(aCharset);
		headers->StoreL(*attachmentInfo);
		CleanupStack::PopAndDestroy(headers);
		}
	
	iEmailMessage->AttachmentManager().AddAttachmentL(aFile, attachmentInfo, iAttachmentWaiter->iStatus);
	CleanupStack::Pop(attachmentInfo);// ownership passed to attachment manager	

	iAttachmentWaiter->StartWaitingL(aStatus, iEmailMessage, EFalse);
	}
	
EXPORT_C void CSmtpClientMtm::AddLinkedAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue, gPanic(ESmtcMTMNotAMessageEntry));

	if( iAttachmentWaiter == NULL )
		{
		iAttachmentWaiter = CImAttachmentWaiter::NewL();
		}

	if (iEmailMessage == NULL)
		{		
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}
	else if (iEmailMessage->EmailEntryId() != iMsvEntry->EntryId())
		{
		delete iEmailMessage;
		iEmailMessage = NULL;
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}
	
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvLinkedFile);
	CleanupStack::PushL(attachmentInfo);
	
	attachmentInfo->SetMimeTypeL(aMimeType);
	
	TParse fileNameParser;
	User::LeaveIfError(fileNameParser.Set(aFilePath, NULL, NULL));
	attachmentInfo->SetAttachmentNameL(fileNameParser.NameAndExt());

	if( aCharset!=0 )
		{
		CMsvMimeHeaders* headers = CMsvMimeHeaders::NewLC();
		headers->SetMimeCharset(aCharset);
		headers->StoreL(*attachmentInfo);
		CleanupStack::PopAndDestroy(headers);
		}
	
	iEmailMessage->AttachmentManager().AddLinkedAttachmentL(aFilePath, attachmentInfo, iAttachmentWaiter->iStatus);
	CleanupStack::Pop(attachmentInfo);// ownership passed to attachment manager	

	iAttachmentWaiter->StartWaitingL(aStatus, iEmailMessage, EFalse);
	}
	
EXPORT_C void CSmtpClientMtm::AddEntryAsAttachmentL(TMsvId aAttachmentId, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue, gPanic(ESmtcMTMNotAMessageEntry));

	if( iAttachmentWaiter == NULL )
		{
		iAttachmentWaiter = CImAttachmentWaiter::NewL();
		}

	if (iEmailMessage == NULL)
		{		
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}
	else if (iEmailMessage->EmailEntryId() != iMsvEntry->EntryId())
		{
		delete iEmailMessage;
		iEmailMessage = NULL;
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}
	
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvMessageEntry);
	CleanupStack::PushL(attachmentInfo);

	iEmailMessage->AttachmentManager().AddEntryAsAttachmentL(aAttachmentId, attachmentInfo, iAttachmentWaiter->iStatus);
	CleanupStack::Pop(attachmentInfo);// ownership passed to attachment manager	
	
	iAttachmentWaiter->StartWaitingL(aStatus, iEmailMessage, EFalse);
	}

EXPORT_C void CSmtpClientMtm::CreateAttachmentL(const TDesC& aFileName, RFile& aAttachmentFile, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue, gPanic(ESmtcMTMNotAMessageEntry));
	
	if( iAttachmentWaiter == NULL )
		{
		iAttachmentWaiter = CImAttachmentWaiter::NewL();
		}

	if (iEmailMessage == NULL)
		{		
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}
	else if (iEmailMessage->EmailEntryId() != iMsvEntry->EntryId())
		{
		delete iEmailMessage;
		iEmailMessage = NULL;
		iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);
		}

	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);

	attachmentInfo->SetAttachmentNameL(aFileName);
	attachmentInfo->SetMimeTypeL(aMimeType);
	
	if( aCharset!=0 )
		{
		CMsvMimeHeaders* headers = CMsvMimeHeaders::NewLC();
		headers->SetMimeCharset(aCharset);
		headers->StoreL(*attachmentInfo);
		CleanupStack::PopAndDestroy(headers);
		}

	iEmailMessage->AttachmentManager().CreateAttachmentL(aFileName, aAttachmentFile, attachmentInfo, iAttachmentWaiter->iStatus);
	CleanupStack::Pop(attachmentInfo);// ownership passed to attachment manager	
	
	iAttachmentWaiter->StartWaitingL(aStatus, iEmailMessage, EFalse);
	}


EXPORT_C void CSmtpClientMtm::CreateMessageL(TMsvId aServiceId)
/** Creates a new message entry as a child of the current context.

@param aServiceId ID of the service to own the entry. */
	{
	// fix for DEF051564 - SMTP client MTM CreateMessageL not creating message in correct state
	TMsvEmailTypeList emailTypeList = KMsvEmailTypeListInvisibleMessage | KMsvEmailTypeListMessageInPreparation;

	CEmailAccounts* account = CEmailAccounts::NewLC();
	
	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);
  	TSmtpAccount id;
	account->GetSmtpAccountL(aServiceId, id);
	account->LoadSmtpSettingsL(id, *smtpSettings);
	
	switch (smtpSettings->BodyEncoding())
		{
		case EMsgOutboxMHTMLAsMIME:
		case EMsgOutboxMHTMLAlternativeAsMIME:
			emailTypeList |= KMsvEmailTypeListMHTMLMessage;
			break;
		case EMsgOutboxDefault:
		case EMsgOutboxNoAlgorithm:
		case EMsgOutboxMIME:
			break;
		}
	CleanupStack::PopAndDestroy(2, account); // smtpSettings, account
	
	// Now invoke the create new mail operation.
	// Note that it is wrapped up to make the asynchronous call synchronous.
	CMsvOperationActiveSchedulerWait* waiter=CMsvOperationActiveSchedulerWait::NewLC();
	CImEmailOperation* createNewMailOp = CImEmailOperation::CreateNewL(waiter->iStatus, iMsvEntry->Session(), iMsvEntry->Entry().Id(), aServiceId, KMsvMessagePartBody|KMsvMessagePartAttachments, emailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(createNewMailOp);
	waiter->Start();

	// The the entry is expected to be set to the new message.
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = createNewMailOp->ProgressL();	
	paramPack.Set(progBuf);
	TMsvId messageId = paramPack();
	iMsvEntry->SetEntryL(messageId);
	
	CMsvStore* store = iMsvEntry->ReadStoreL();
	CleanupStack::PushL(store);
	iHeader->RestoreL(*store);

	CleanupStack::PopAndDestroy(3, waiter); // waiter, createNewMailOp, store	
	}
	
/**
Gets the default SMTP service.

@return
The default service

@leave 
KErrNotFound If default service setting does not exist.
*/
EXPORT_C TMsvId CSmtpClientMtm::DefaultServiceL() const
	{
	// Get default service Id from CenRep
	CEmailAccounts* account = CEmailAccounts::NewLC();
	TSmtpAccount id;
	TInt error = account->DefaultSmtpAccountL(id);
	if (error == KErrNotFound)
		{
		User::Leave(error);
		}
	
	CleanupStack::PopAndDestroy(account); 	   
	return id.iSmtpService;		
	}

/**
Removes the default SMTP service.
*/		
EXPORT_C void CSmtpClientMtm::RemoveDefaultServiceL()
	{
	User::Leave(KErrNotSupported);
	}

/**
Sets the default SMTP service.

@param	aService
The default service
*/		
EXPORT_C void CSmtpClientMtm::ChangeDefaultServiceL(const TMsvId& aService)
	{
	CEmailAccounts* account = CEmailAccounts::NewLC();
	TSmtpAccount id;
	account->GetSmtpAccountL(aService, id);
	account->SetDefaultSmtpAccountL(id);
	CleanupStack::PopAndDestroy(account); 	   		
	}
	
/**
Cancels the current attachment operation.
*/	
EXPORT_C void CSmtpClientMtm::CancelAttachmentOperation()
	{
 	if ( iAttachmentWaiter)
		 {
		 iAttachmentWaiter->Cancel();
		 }

	if ( iWait )
		 {
		 iWait->Cancel();
		 }
	}
	
