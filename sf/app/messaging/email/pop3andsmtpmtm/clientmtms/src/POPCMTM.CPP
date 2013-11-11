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
// Client MTM for the POP3 protocol
// 
//

#include <bautils.h>	// BaflUtils
#include <mtmdef.h>		//KUidMtmQueryxxx & TMsvPartList flags

#include <barsc.h>		//RResourceFile
#include <barsread.h>
#include <msvftext.h>
#include <cemailaccounts.h>

#include "POPCMTM.H"
#include "POP3SET.H"		//CImPop3Settings
#include <imcm.rsg>
#include "MIUT_ERR.H"
#include "IMCMMAIN.H"
#include "IMCMUTIL.H"
#include <autosend.h>
#include <msvenhancesearchsortutil.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <mtmuidsdef.hrh>
#include "miut_errconsts.h"
#include "cimmessagepart.h"
#endif

#if defined (_UNICODE)
	#define KUidMsgInternetMailEditorDLL	0x10003C53  // (268450899)
#else
	#define KUidMsgInternetMailEditorDLL	0x100011AC  // (268439980)
#endif

_LIT(KMsvAutoSendExe, "Autosend.exe");
const TUid KMsvAutoSendExeUid = {0x1000A402}; //268477442

/**
@internalTechnology 
*/

EXPORT_C CPop3ClientMtm* CPop3ClientMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
	{
	CPop3ClientMtm* self = new(ELeave) CPop3ClientMtm(aRegisteredMtmDll, aMsvSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CPop3ClientMtm::CPop3ClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
: CBaseMtm(aRegisteredMtmDll, aMsvSession)
	{
	__DECLARE_NAME(_S("CPop3ClientMtm"));
	}

void CPop3ClientMtm::ConstructL()
	{
	iImPop3Settings = new (ELeave) CImPop3Settings;
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

CPop3ClientMtm::~CPop3ClientMtm()
/** Destructor. */
	{
	delete iEmailAddressFormatString;
	delete iImPop3Settings;
	delete iImPOP3GetMail;
	delete iHeader;
	}

void CPop3ClientMtm::SaveMessageL()
/** Client MTM base class function, with an empty implementation. */
	{
	}

void CPop3ClientMtm::LoadMessageL()
/** Loads the cache with the message data for the current context. */
	{
	Body().Reset();
	iHeader->Reset();
	switch (iMsvEntry->Entry().iType.iUid)
		{
		case KUidMsvServiceEntryValue:
			RestoreSettingsL();
			break;
		case KUidMsvMessageEntryValue:
			{//restore header
			if (!iMsvEntry->HasStoreL())
				return;
			
		// Get a reference to TMsvEnhanceSearchSortUtil  instance set by CMsvSearchsortOpOnHeaderBody class
		// If advanced search and sort is being performed than do not load the message header and body.
		// These are loaded when the search criteria is known in FindL()
		// For API's other than CMsvSearchsortOpOnHeaderBody-> FindInHeaderBodyL(), a call to LoadMessageL()
		// loads the body and the header.
				
			TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
			if ( searchsortutil == NULL )
				{
				// Load message Body
				CMsvStore* msvStore = iMsvEntry->ReadStoreL();
				CleanupStack::PushL(msvStore);
			
		   		if (! msvStore->IsPresentL(KUidMsgFileIMailHeader) )
		    		{
		    		CleanupStack::PopAndDestroy(); //store
		    		return;
		    		}

				//Restore the header
				iHeader->RestoreL(*msvStore);
				CleanupStack::PopAndDestroy();//store
				CImEmailMessage* message=CImEmailMessage::NewLC(*iMsvEntry);
				message->GetBodyTextL(iMsvEntry->Entry().Id(),CImEmailMessage::EThisMessageOnly,Body(),*iParaFormatLayer,*iCharFormatLayer);
				CleanupStack::PopAndDestroy();//message	
				break;
				
				}
			}
		};
	}

EXPORT_C void CPop3ClientMtm::StoreSettingsL()
/** Stores the current service settings from the object's cache in to the Central 
Repository for the current entry.

The current entry must be a service. */
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EPopcMTMNoCMsvEntrySet));
	
	// check that current context is a service entry - Assuming that StoreL() is ONLY used to write to Service Entry. True?!
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvServiceEntry,gPanic(EPopcMTMNotAServiceEntry));

	CEmailAccounts* account = CEmailAccounts::NewLC();
	TPopAccount id;
	account->GetPopAccountL(iMsvEntry->Entry().Id(), id);
	account->SavePopSettingsL(id, *iImPop3Settings);
	CleanupStack::PopAndDestroy(account);    
	}

EXPORT_C void CPop3ClientMtm::RestoreSettingsL()
/** Loads into the object's cache the service settings from the Central Repository for
the current entry.

The current entry must be a service. */
	{
	if (iMsvEntry->Entry().iType==KUidMsvServiceEntry)
		{
		CEmailAccounts* account = CEmailAccounts::NewLC();
		TPopAccount id;
		account->GetPopAccountL(iMsvEntry->Entry().Id(), id);
		account->LoadPopSettingsL(id, *iImPop3Settings);
		CleanupStack::PopAndDestroy(account);    
		}
	}

void CPop3ClientMtm::SendOnNextConnectionL()
	{
	TMsvId smtpServiceId = KMsvNullIndexEntryId;
	TMsvId entry = iMsvEntry->Entry().Id();
	if (iMsvEntry->Entry().iType.iUid==KUidMsvServiceEntryValue)
		smtpServiceId = iMsvEntry->Entry().iRelatedId;
	else if (iMsvEntry->Entry().iType.iUid==KUidMsvMessageEntryValue)
		{
		TMsvId id=iMsvEntry->Entry().iServiceId;
		iMsvEntry->SetEntryL(iMsvEntry->Entry().iServiceId);		
		smtpServiceId = iMsvEntry->Entry().iRelatedId;
		iMsvEntry->SetEntryL(id);		
		}

	iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	CMsvEntrySelection* sel = iMsvEntry->ChildrenWithMtmL(KUidMsgTypeSMTP);
	CleanupStack::PushL(sel);

	// there is no need to start the autosend operation if there are no SMTP messages
	// in the outbox

	if(sel->Count())
		{
		TBuf<20> cmdString;
		cmdString.Num(smtpServiceId, EDecimal);
			RProcess p;
			TInt error=p.Create(KMsvAutoSendExe, cmdString, TUidType(KNullUid, KNullUid, KMsvAutoSendExeUid));
			if (error==KErrNone)
				{
				p.Resume();
				p.Close();
				}
		}
	CleanupStack::PopAndDestroy(sel);
	iMsvEntry->SetEntryL(entry);
	}

CMsvOperation* CPop3ClientMtm::ReplyL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus)
/** Creates a reply message to the current message context.

The reply is an SMTP message.

@return	If successful, this is an asynchronously completing reply operation. 
If failed, this is a completed operation, with status set to the relevant error code. 
@param aDestination The entry to which to assign the reply 
@param aPartList Defines the parts that are to be copied from the original message into the reply
@param aCompletionStatus The request status to be completed when the operation has finished 
*/
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TUid messageType = KUidMsgTypeSMTP;
	return iImEmailOperation->CreateReplyL(aCompletionStatus, Session(), iMsvEntry->EntryId(), aDestination, aPartList, msvEmailTypeList, messageType);
	}

CMsvOperation* CPop3ClientMtm::ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus)
/** Creates a forwarded message from the current message context.

The forwarded message is an SMTP message.

@return	If successful, this is an asynchronously completing forward message operation. 
If failed, this is a completed operation, with status set to the relevant error code. 
@param aDestination The entry to which to assign the forwarded message  
@param aPartList Defines the parts that are to be copied from the original message into the forwarded message 
@param aCompletionStatus The request status to be completed when the operation has finished 
*/
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TUid messageType = KUidMsgTypeSMTP;
	return iImEmailOperation->CreateForwardL(aCompletionStatus, Session(), iMsvEntry->EntryId(), aDestination, aPartList, msvEmailTypeList, messageType);
	}

TUint CPop3ClientMtm::ValidateMessage(TMsvPartList /*aPartList*/)
	/** Client MTM base class function, with an empty implementation.
	
	@param aPartList Unused
	@return Unused */
	{
	return KMsvMessagePartRecipient;
	}

TBool CPop3ClientMtm::ValidateAddress(const TPtrC& anAddress)
	{
	return iTImMessageField.ValidInternetEmailAddress(anAddress);
	}

TMsvPartList CPop3ClientMtm::DoFindL(const TDesC& aTextToFind, TMsvPartList aPartList)
   	{
   	CImClientMTMUtils* clientMTMUtils = CImClientMTMUtils::NewL();
   	CleanupStack::PushL(clientMTMUtils);
   
   	TMsvPartList retList = KMsvMessagePartNone;
    
 	// For POP3 mail we need to search the entry as well as the header. This is because the header stream might not be present.
 	CMsvFindText* findText = CMsvFindText::NewLC();
  
  	// Get a reference to TMsvEnhanceSearchSortUtil instance set by CMsvSearchsortOpOnHeaderBody class
	
 	TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
 	
 	// searchsortuitl variable will not be NULL for Advanced Search and Sort called from CMsvSearchsortOpOnHeaderBody
 	// For the old implementation, it will be NULL
  	if( searchsortutil != NULL )
 		{
 		// Get the searchsort setting flags
 		TInt32 searchsortsetting=searchsortutil->GetSearchSortSetting();
 		
 		// The body was not loaded in LoadMessageL()
 		// If 2 search query options are on the body or on the header, than it sets EMessagePartBodyLoaded flag
 		// of searchsortsetting
 		if(aPartList & KMsvMessagePartBody && !(searchsortsetting & EMessagePartBodyLoaded))  	
   			{
   			// Restore Body
   			CImEmailMessage* message=CImEmailMessage::NewLC(*iMsvEntry);
			message->GetBodyTextL(iMsvEntry->Entry().Id(),CImEmailMessage::EThisMessageOnly,Body(),*iParaFormatLayer,*iCharFormatLayer);
			CleanupStack::PopAndDestroy();//message
			searchsortutil->SetSearchSortSetting(EMessagePartBodyLoaded);
   			}
   		else if(!(searchsortsetting & EMessagePartHeaderLoaded))
   			{
 			// Restore header
			CMsvStore* msvStore = iMsvEntry->ReadStoreL();
  			CleanupStack::PushL(msvStore);
			
		   	if (! msvStore->IsPresentL(KUidMsgFileIMailHeader) )
		    	{
		    	CleanupStack::PopAndDestroy(); //store
		    	return 0;
		    	}

			iHeader->RestoreL(*msvStore);
			CleanupStack::PopAndDestroy();//store
			searchsortutil->SetSearchSortSetting(EMessagePartHeaderLoaded);
			}
   		
   		// Issue the find request
   		clientMTMUtils->FindL(aTextToFind, Body(), *iHeader, aPartList, retList);
   		
   		/* If the search didn't succeed, than search in the details field of TMsvEntry class. It holds the From field
		   If the search is on subject, than look in description field of TMsvEntry class.	*/
   		if(!retList)
 			{
 			if (aPartList & KMsvMessagePartFrom)
 				{
 				findText->FindTextL(aTextToFind, iMsvEntry->Entry().iDetails, aPartList) ? retList|=KMsvMessagePartOriginator : retList;
 				}
	 		if (aPartList & KMsvMessagePartSubject)
	 			{
		 		findText->FindTextL(aTextToFind, iMsvEntry->Entry().iDescription, aPartList) ? retList|=KMsvMessagePartDescription : retList;
		 		}
 			}
 			
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
   			else  // Header not present only originator and description is available 
   	   			{ 
   		    	if(searchsortsetting & EMessagePartFromSort)
   		    		{
   		    		SetExtensionData((TAny*)&iMsvEntry->Entry().iDetails);
   		    		}
 				else if(searchsortsetting & EMessagePartSubjectSort)
 					{
 					SetExtensionData((TAny*)&iMsvEntry->Entry().iDescription);		
 					}
  				}
   			}
   		
   		}
   	else // Old implementation for search
   		{

		clientMTMUtils->FindL(aTextToFind, Body(), *iHeader, aPartList, retList);
   		if(!retList)
 			{
 			if (aPartList & KMsvMessagePartOriginator)
 				{
 				findText->FindTextL(aTextToFind, iMsvEntry->Entry().iDetails, aPartList) ? retList|=KMsvMessagePartOriginator : retList;
 				}
	 		if (aPartList & KMsvMessagePartDescription)
	 			{
		 		findText->FindTextL(aTextToFind, iMsvEntry->Entry().iDescription, aPartList) ? retList|=KMsvMessagePartDescription : retList;
		 		}
 			}
 		
   		}
	CleanupStack::PopAndDestroy(findText);
	CleanupStack::PopAndDestroy(); //clientMTMUtils

  	return retList;
 	}
 
TMsvPartList CPop3ClientMtm::Find(const TDesC& aTextToFind, TMsvPartList aPartList)
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

void CPop3ClientMtm::AddAddresseeL(const TDesC& aRealAddress)
/** Adds an addressee for the current context.

@param aRealAddress String representing an address to be added to the list 
for the current message */
	{
	iAddresseeList->AppendL(aRealAddress);
	}

void CPop3ClientMtm::AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias)
/** Adds an addressee with an alias for the current context.

@param aRealAddress String representing an address to be added to the list 
for the current message 
@param aAlias Alias information */
	{
	HBufC* emailAddress = HBufC::NewLC(aRealAddress.Length()+aAlias.Length()+iEmailAddressFormatString->Length()-4);
	emailAddress->Des().Format(*iEmailAddressFormatString,&aAlias,&aRealAddress);
	iAddresseeList->AppendL(aRealAddress);
	CleanupStack::PopAndDestroy();	// emailAddress
	}

void CPop3ClientMtm::RemoveAddressee(TInt aIndex)
/** Removes an address from the current address list.

@param aIndex Index of address to be removed */
	{
	if (iAddresseeList->Count() > aIndex)
		iAddresseeList->Delete(aIndex);
	}

void CPop3ClientMtm::ContextEntrySwitched()
	{
	iImPop3Settings->Reset();
	iHeader->Reset();
	Body().Reset();	
	}

TInt CPop3ClientMtm::QueryCapability(TUid aCapability, TInt& aResponse)
/** Queries if the MTM supports a particular capability, specified by a UID.

@param aCapability UID of capability to be queried 
@param aResponse Response value. The format of the response varies according 
to the capability. 
@return KErrNone: aCapability is a recognised value and a response is returned 
KErrNotSupported: aCapability is not a recognised value */
	{
	TInt error = KErrNone;
	// Supported
	switch (aCapability.iUid)
		{
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
	case KUidMtmQuerySupportAttachmentsValue:
	case KUidMtmQueryCanReceiveMsgValue:		
		break;
	// All others - Not Supported:
	default:
		error = KErrNotSupported;
		}
	return error;
	}

EXPORT_C void CPop3ClientMtm::SetSettingsL(const CImPop3Settings& aSettings)
/** Copies the specified service settings to the cached service settings.

@param aSettings New service settings */
	{
	iImPop3Settings->CopyL(aSettings);
	}

EXPORT_C const CImPop3Settings& CPop3ClientMtm::Settings() const
/** Gets the current cached service settings.

@return The current cached service settings */
	{
	return *iImPop3Settings;
	}

void CPop3ClientMtm::InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter)
/** Invokes synchronous POP3-specific operations.

@param aFunctionId ID of the requested operation. The only supported operation 
is KPOP3MTMIsConnected.
@param aSelection Selection of message entries. This is used if the operation 
requires message entries to work on. 
@param aParameter Buffer containing input and output parameters. The format 
of this is specific to the operation. */
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EPopcMTMNoCMsvEntrySet));

	TInt error = KErrNone;
	switch (aFunctionId)
		{
		case KPOP3MTMIsConnected:
			{
			CMsvOperationActiveSchedulerWait* synchOperation;
			synchOperation=CMsvOperationActiveSchedulerWait::NewLC();
			CMsvOperation* operation = Session().TransferCommandL(aSelection, KPOP3MTMIsConnected, aParameter, synchOperation->iStatus);
			synchOperation->Start();
			CleanupStack::PushL(operation);
			error = operation->iStatus.Int();
			CleanupStack::PopAndDestroy(2);	// synchOperation, operation

			TPop3Progress progress;
			progress.iErrorCode = error;

			TPckgBuf<TPop3Progress> resultPackage(progress);
			aParameter.Copy(resultPackage);
			return;
			}
		default:
			error=KErrNotSupported;
			__ASSERT_DEBUG(EFalse,gPanic(EPopcUnknownSyncFunction));
		}

	User::LeaveIfError(error);
	}

CMsvOperation* CPop3ClientMtm::InvokeAsyncFunctionL(TInt aFunctionId,
													const CMsvEntrySelection& aSelection,
												    TDes8& aParameter, 
												    TRequestStatus& aCompletionStatus)
/** Invokes asynchronous POP3-specific operations.

@param aFunctionId Specifies which operation to perform e.g. connect, copy 
new mail etc. The specific operations are defined by the TPop3Cmds enumeration. 

@param aSelection A selection of messages that need to be copied/moved to a 
local folder. The first entry in this selection MUST be the service. 
@param aParameter Contains information such as the destination folder ID for 
Get Mail operations. For get mail type functionality, this information must 
be packaged as a TImPop3GetMailInfo package buffer. 
@param aCompletionStatus The status when the operation completes. 
@leave KErrNotFound The selection of email to be moved or copied is empty
@leave KErrNotSupported The specified operation is not recognised
@return If successful, this is an asynchronously completing operation. If failed, 
this is a completed operation, with status set to the relevant error code. 
@see TImPop3GetMailInfo
@see TPop3Cmds */
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EPopcMTMNoCMsvEntrySet));

	

	switch(aFunctionId)
		{
		case KPOP3MTMConnect:
		case KPOP3MTMDisconnect:
		case KPOP3MTMCancelOfflineOperations:
			if (aFunctionId==KPOP3MTMConnect)
				{
				RestoreSettingsL();
				if (iImPop3Settings->AutoSendOnConnect()) 
					{
					TRAPD(err,SendOnNextConnectionL());  //ignore the error and continue with connecting
					}
				}
			return (Session().TransferCommandL(aSelection, aFunctionId, aParameter, aCompletionStatus));
		case KPOP3MTMCopyNewMailWhenAlreadyConnected:
		case KPOP3MTMMoveNewMailWhenAlreadyConnected:
		case KPOP3MTMCopyMailSelectionWhenAlreadyConnected:
		case KPOP3MTMMoveMailSelectionWhenAlreadyConnected:
		case KPOP3MTMCopyAllMailWhenAlreadyConnected:
		case KPOP3MTMMoveAllMailWhenAlreadyConnected:
			{
			__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvServiceEntryValue, gPanic(EPopcMTMNotAServiceEntry));
			TPckgBuf<TImPop3GetMailInfo> optionsBuf;
			optionsBuf.Copy(aParameter);
			const TImPop3GetMailInfo& options=optionsBuf();

			CMsvEntrySelection *entrySelection=NULL;
			TBool newMailOnly=EFalse;			
			switch (aFunctionId)
				{
				case KPOP3MTMCopyNewMailWhenAlreadyConnected:
				case KPOP3MTMMoveNewMailWhenAlreadyConnected:
					newMailOnly=ETrue;
				case KPOP3MTMCopyAllMailWhenAlreadyConnected:
				case KPOP3MTMMoveAllMailWhenAlreadyConnected:
					{
					entrySelection=FilteredChildrenLC(newMailOnly,options.iMaxEmailSize);
					}
					break;
				case KPOP3MTMCopyMailSelectionWhenAlreadyConnected:
				case KPOP3MTMMoveMailSelectionWhenAlreadyConnected:
					{
					entrySelection=FilteredSelectionLC(options.iMaxEmailSize,aSelection);
					}
					break;
				default:
					User::Leave(KErrNotSupported);
				}
			// only need to do copy/move if the selection is not empty
			CMsvOperation *op=NULL;
			if (entrySelection->Count()>0)
				{
				if ((aFunctionId == KPOP3MTMCopyNewMailWhenAlreadyConnected) ||
					(aFunctionId == KPOP3MTMCopyMailSelectionWhenAlreadyConnected) ||
					(aFunctionId == KPOP3MTMCopyAllMailWhenAlreadyConnected))
					{
					op=iMsvEntry->CopyL(*entrySelection, options.iDestinationFolder, aCompletionStatus);
					}
				else if ((aFunctionId == KPOP3MTMMoveNewMailWhenAlreadyConnected) ||
					(aFunctionId == KPOP3MTMMoveMailSelectionWhenAlreadyConnected) ||
					(aFunctionId == KPOP3MTMMoveAllMailWhenAlreadyConnected))
					{
					op=iMsvEntry->MoveL(*entrySelection, options.iDestinationFolder, aCompletionStatus);
					}
				}
			else
				op=CMsvCompletedOperation::NewL(Session(), KUidMsgTypePOP3, KNullDesC8, iMsvEntry->Entry().iServiceId, aCompletionStatus);
			CleanupStack::PopAndDestroy(entrySelection);
			return op;
			}

		case KPOP3MTMPopulate:
		case KPOP3MTMPopulateNew:
		case KPOP3MTMPopulateAll:
			{
			__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid==KUidMsvServiceEntryValue, gPanic(EPopcMTMNotAServiceEntry));
			TImPop3PopulateOptions options;
			TImPop3PopulateOptions::UnpackL(aParameter,options);

			CMsvEntrySelection *entrySelection;
			if(aFunctionId==KPOP3MTMPopulateNew || aFunctionId==KPOP3MTMPopulateAll)
				{
				entrySelection=FilteredChildrenLC(aFunctionId==KPOP3MTMPopulateNew,options.MaxEmailSize());				
				}
			else
				{
				entrySelection=FilteredSelectionLC(options.MaxEmailSize(),aSelection);
				}

			CMsvOperation *op=NULL;
			if (entrySelection->Count()>0)
				{
				entrySelection->InsertL(0,aSelection[0]);
				op=(Session().TransferCommandL(*entrySelection, KPOP3MTMPopulate, aParameter, aCompletionStatus));
				}
			else
				op=CMsvCompletedOperation::NewL(Session(), KUidMsgTypePOP3, KNullDesC8, iMsvEntry->Entry().iServiceId, aCompletionStatus);
			
			CleanupStack::PopAndDestroy(entrySelection);
			return op;
			}		

		case KPOP3MTMConnectAndCopyNewMailAndStayOnline:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndCopyNewMailAndStayOnline);
		case KPOP3MTMConnectAndCopyNewMailAndDisconnect:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndCopyNewMailAndDisconnect);
		case KPOP3MTMConnectAndMoveNewMailAndStayOnline:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndMoveNewMailAndStayOnline);
		case KPOP3MTMConnectAndMoveNewMailAndDisconnect:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndMoveNewMailAndDisconnect);
		case KPOP3MTMConnectAndCopyMailSelectionAndStayOnline:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndCopyMailSelectionAndStayOnline);
		case KPOP3MTMConnectAndCopyMailSelectionAndDisconnect:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndCopyMailSelectionAndDisconnect);
		case KPOP3MTMConnectAndMoveMailSelectionAndStayOnline:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndMoveMailSelectionAndStayOnline);
		case KPOP3MTMConnectAndMoveMailSelectionAndDisconnect:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndMoveMailSelectionAndDisconnect);
		case KPOP3MTMConnectAndCopyAllMailAndStayOnline:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndCopyAllMailAndStayOnline);
		case KPOP3MTMConnectAndCopyAllMailAndDisconnect:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndCopyAllMailAndDisconnect);
		case KPOP3MTMConnectAndMoveAllMailAndStayOnline:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndMoveAllMailAndStayOnline);
		case KPOP3MTMConnectAndMoveAllMailAndDisconnect:
			return iImPOP3GetMail->GetMailL(*this, aCompletionStatus, aSelection, aParameter, CImPOP3GetMail::EConnectAndMoveAllMailAndDisconnect);
		case KPOP3MTMCreateNewEmailMessage:
		case KPOP3MTMCreateReplyEmailMessage:
		case KPOP3MTMCreateForwardEmailMessage:
		case KPOP3MTMCreateForwardAsAttachmentEmailMessage:
		case KPOP3MTMCreateReceiptEmailMessage:
			{
			TImCreateMessageOptions createMessageOptions;	
			TPckgC<TImCreateMessageOptions> paramPack(createMessageOptions);
			paramPack.Set(aParameter);
			switch (aFunctionId)
				{
			case KPOP3MTMCreateNewEmailMessage:
				return iImEmailOperation->CreateNewL(aCompletionStatus, iMsvEntry->Session(), aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
			case KPOP3MTMCreateReplyEmailMessage:
				return iImEmailOperation->CreateReplyL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
			case KPOP3MTMCreateForwardEmailMessage:
				return iImEmailOperation->CreateForwardL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
			case KPOP3MTMCreateForwardAsAttachmentEmailMessage:
				return iImEmailOperation->CreateForwardAsAttachmentL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
			case KPOP3MTMCreateReceiptEmailMessage:
				return iImEmailOperation->CreateReceiptL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
			default:
				User::Leave(KErrNotSupported);
				return NULL;	// shouldn't really get here!
				}
			}
		default:
			User::Leave(KErrNotSupported);
			return NULL;	// shouldn't really get here!
		}
	}

// Attachment functions to support the SendAs API

// Attachment functions to support the SendAs API

/** Unsupported client MTM base class function.

@param aFilePath Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
 */
EXPORT_C void CPop3ClientMtm::AddAttachmentL(const TDesC& /*aFilePath*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

/** Unsupported client MTM base class function.

@param aFile Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
 */	
EXPORT_C void CPop3ClientMtm::AddAttachmentL(RFile& /*aFile*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/** Unsupported client MTM base class function.

@param aFilePath Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
 */
EXPORT_C void CPop3ClientMtm::AddLinkedAttachmentL(const TDesC& /*aFilePath*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/** Unsupported client MTM base class function.

@param aAttachmentId Unused
@param aStatus Unused
 */
EXPORT_C void CPop3ClientMtm::AddEntryAsAttachmentL(TMsvId /*aAttachmentId*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/** Unsupported client MTM base class function.

@param aFileName Unused
@param aAttachmentFile Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
 */
EXPORT_C void CPop3ClientMtm::CreateAttachmentL(const TDesC& /*aFileName*/, RFile& /*aAttachmentFile*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CPop3ClientMtm::CreateMessageL(TMsvId /*aServiceId*/)
/** Unsupported client MTM base class function.

@param aServiceId Unused */
	{
	User::Leave(KErrNotSupported);
	}

CMsvEntrySelection* CPop3ClientMtm::FilteredChildrenLC(TBool aNewOnly, TInt aMaxEmailSize)
	{
	CMsvEntrySelection *selection= new (ELeave) CMsvEntrySelection;
	CleanupDeletePushL(selection);
	
	TInt count = iMsvEntry->Count();
	// get a selection of messages which are new - i.e have new flag set
	// and those which are smaller than iMaxEmailSize
	for (TInt i=0; i<count; ++i)
		{
		const TMsvEmailEntry &entry = (*iMsvEntry)[i];
		if (entry.iType==KUidMsvMessageEntry)
			{
			if ((!aNewOnly || entry.New()) &&
				(entry.iSize <= aMaxEmailSize))
				selection->AppendL(entry.Id());
			}
		}
	return selection;
	}

CMsvEntrySelection* CPop3ClientMtm::FilteredSelectionLC(TInt aMaxEmailSize, const CMsvEntrySelection& aSelection)
	{
	CMsvEntrySelection *selection= new (ELeave) CMsvEntrySelection;
	CleanupDeletePushL(selection);
	TInt count = aSelection.Count();
	
	for (TInt i=1; i<count; ++i)
		{
		const TMsvEmailEntry &entry = iMsvEntry->ChildDataL((aSelection)[i]);
		if ((entry.iType==KUidMsvMessageEntry)&&(entry.iSize <= aMaxEmailSize))
			{
			selection->AppendL(entry.Id());
			}
		}
	return selection;
	}

/**
Gets the default POP service.

@return
The default service

@leave 
KErrNotFound If default service setting does not exist.
*/
EXPORT_C TMsvId CPop3ClientMtm::DefaultServiceL() const
	{
	User::Leave(KErrNotSupported);
	return KErrNotSupported;
	}

/**
Removes the default POP service.
*/	
EXPORT_C void CPop3ClientMtm::RemoveDefaultServiceL()
	{
	User::Leave(KErrNotSupported);
	}

/**
Sets the default POP service.

@param	aService
The default service
*/	
EXPORT_C void CPop3ClientMtm::ChangeDefaultServiceL(const TMsvId& /*aService*/)
	{
	User::Leave(KErrNotSupported);
	}


EXPORT_C CMsvOperation* CImPOP3GetMail::GetMailL(CPop3ClientMtm& aPop3ClientMtm, TRequestStatus& aObserverRequestStatus, const CMsvEntrySelection& aMsvEntrySelection, TDes8& aPop3GetMailInfo, TImPOP3GetMailType aPOP3GetMailType)
/** Creates and begins a new POP3 get mail operation.

@param aPop3ClientMtm A reference to the POP3 Client MTM that wants to perform 
the Get Mail operation.
@param aObserverRequestStatus The status to be completed when the get mail 
operation has completed.
@param aMsvEntrySelection A selection of messages that need to be copied/moved 
to a local folder. The first entry in this selection MUST be the service.
@param aPop3GetMailInfo A packaged TImPop3GetMailInfo object, which sets the 
maximum message size and the destination folder ID.
@param aPOP3GetMailType Type of operation to perform
@return The new CImPOP3GetMail object through which the get operation can be 
controlled. */
	{
	CImPOP3GetMail* self = new(ELeave) CImPOP3GetMail(aPop3ClientMtm.Session(), aObserverRequestStatus, aPop3ClientMtm, aPOP3GetMailType);
	CleanupStack::PushL(self);
	self->ConstructL(aMsvEntrySelection, aPop3GetMailInfo);
	CleanupStack::Pop(); //self
	return self;
	}

EXPORT_C CImPOP3GetMail::~CImPOP3GetMail()
/** Destructor. */
	{
	Cancel();
	delete iMsvEntrySelection;
	delete iMsvOperation;
	}

EXPORT_C const TDesC8& CImPOP3GetMail::FinalProgress()
/** Gets information about a completed operation.

@return Packaged TPop3Progress holding progress information. 
@see TPop3Progress */
	{
	__ASSERT_ALWAYS(!IsActive(), gPanic(EMiutActiveInFinalProgress));

	const TDesC8& opProgress = iMsvOperation->FinalProgress();

	if (opProgress.Length())
		{
		// if an error was encountered during the Get New Mail operation then need to 
		// return this as part of the final progresse.  If no error was encoutered then
		// return iProgress
		TPop3Progress prog;	
		TPckgC<TPop3Progress> paramPack(prog);
		paramPack.Set(opProgress);
		TPop3Progress progress=paramPack();
		if (iErrorProgress.iErrorCode != KErrNone)
			{
			progress.iTotalMsgs = iErrorProgress.iTotalMsgs;
			progress.iMsgsToProcess = iErrorProgress.iMsgsToProcess;
			progress.iBytesDone = iErrorProgress.iBytesDone;
			progress.iTotalBytes = iErrorProgress.iTotalBytes;
			progress.iErrorCode = iErrorProgress.iErrorCode;
			progress.iPop3SubStateProgress = iErrorProgress.iPop3SubStateProgress;
			}
		else 
			progress.iPop3SubStateProgress = progress.iPop3Progress;

		switch(iPOP3GetMailType)
			{
			case EConnectAndCopyNewMailAndStayOnline:
			case EConnectAndCopyNewMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopCopyNewMail;
				break;
			case EConnectAndMoveNewMailAndStayOnline:
			case EConnectAndMoveNewMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopMoveNewMail;
				break;
			case EConnectAndCopyMailSelectionAndStayOnline:
			case EConnectAndCopyMailSelectionAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopCopyMailSelection;
				break;
			case EConnectAndMoveMailSelectionAndStayOnline:
			case EConnectAndMoveMailSelectionAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopMoveMailSelection;
				break;
			case EConnectAndCopyAllMailAndStayOnline:
			case EConnectAndCopyAllMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopCopyAllMail;
				break;
			case EConnectAndMoveAllMailAndStayOnline:
			case EConnectAndMoveAllMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopMoveAllMail;
				break;
			}
		
		iProgressBuf = progress;
		}
	return iProgressBuf;
	}

void CImPOP3GetMail::ResetProgress()
	{
	switch(iPOP3GetMailType)
		{
		case EConnectAndCopyNewMailAndStayOnline:
		case EConnectAndCopyNewMailAndDisconnect:
			iProgress.iPop3Progress = TPop3Progress::EPopCopyNewMail;
			break;
		case EConnectAndMoveNewMailAndStayOnline:
		case EConnectAndMoveNewMailAndDisconnect:
			iProgress.iPop3Progress = TPop3Progress::EPopMoveNewMail;
			break;
		case EConnectAndCopyMailSelectionAndStayOnline:
		case EConnectAndCopyMailSelectionAndDisconnect:
			iProgress.iPop3Progress = TPop3Progress::EPopCopyMailSelection;
			break;
		case EConnectAndMoveMailSelectionAndStayOnline:
		case EConnectAndMoveMailSelectionAndDisconnect:
			iProgress.iPop3Progress = TPop3Progress::EPopMoveMailSelection;
			break;
		case EConnectAndCopyAllMailAndStayOnline:
		case EConnectAndCopyAllMailAndDisconnect:
			iProgress.iPop3Progress = TPop3Progress::EPopCopyAllMail;
			break;
		case EConnectAndMoveAllMailAndStayOnline:
		case EConnectAndMoveAllMailAndDisconnect:
			iProgress.iPop3Progress = TPop3Progress::EPopMoveAllMail;
		}
	iProgress.iTotalMsgs = 0;
	iProgress.iMsgsToProcess = 0;
	iProgress.iBytesDone = 0;
	iProgress.iTotalBytes = 0;
	iProgress.iErrorCode = KErrNone;
	}

const TDesC8& CImPOP3GetMail::ProgressL()
/** Gets information on the progress of the operation. 

@return Packaged TPop3Progress holding progress information. 
@see TPop3Progress */
	{
	const TDesC8& opProgress = iMsvOperation->ProgressL();

	if (opProgress.Length())
		{
		// need to get Sub operation progress and put this into iPop3SubStateProgress
		TPop3Progress prog;	
		TPckgC<TPop3Progress> paramPack(prog);
		paramPack.Set(opProgress);
		TPop3Progress progress=paramPack();	

		progress.iPop3SubStateProgress = progress.iPop3Progress;
		switch(iPOP3GetMailType)
			{
			case EConnectAndCopyNewMailAndStayOnline:
			case EConnectAndCopyNewMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopCopyNewMail;
				break;
			case EConnectAndMoveNewMailAndStayOnline:
			case EConnectAndMoveNewMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopMoveNewMail;
				break;
			case EConnectAndCopyMailSelectionAndStayOnline:
			case EConnectAndCopyMailSelectionAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopCopyMailSelection;
				break;
			case EConnectAndMoveMailSelectionAndStayOnline:
			case EConnectAndMoveMailSelectionAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopMoveMailSelection;
				break;
			case EConnectAndCopyAllMailAndStayOnline:
			case EConnectAndCopyAllMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopCopyAllMail;
				break;
			case EConnectAndMoveAllMailAndStayOnline:
			case EConnectAndMoveAllMailAndDisconnect:
				progress.iPop3Progress = TPop3Progress::EPopMoveAllMail;
			}
		iProgressBuf = progress;
		}
	return iProgressBuf;
	}

void CImPOP3GetMail::StoreProgressL()
	{
	const TDesC8& opProgress = iMsvOperation->ProgressL();

	if (opProgress.Length())
		{
		TPop3Progress prog;	
		TPckgC<TPop3Progress> paramPack(prog);
		paramPack.Set(opProgress);
		TPop3Progress progress=paramPack();	

		iProgress.iTotalMsgs = progress.iTotalMsgs;
		iProgress.iMsgsToProcess = progress.iMsgsToProcess;
		iProgress.iBytesDone = progress.iBytesDone;
		iProgress.iTotalBytes = progress.iTotalBytes;
		iProgress.iPop3SubStateProgress = progress.iPop3Progress;
		
		// only write the error code if no error has previously been set
		if ((iProgress.iErrorCode == KErrNone) && (iErrorProgress.iErrorCode == KErrNone))
			{
			if (iStatus.Int() != KErrNone)				
				iProgress.iErrorCode = iStatus.Int();
			else
				iProgress.iErrorCode = progress.iErrorCode;
			}
		iProgressBuf = progress;
		}
	}

void CImPOP3GetMail::DoCancel()
	{
	iMsvOperation->Cancel();
	TRequestStatus* st = &iObserverRequestStatus;
	User::RequestComplete(st, KErrCancel);
	}

CImPOP3GetMail::CImPOP3GetMail(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus, CPop3ClientMtm& aPop3ClientMtm, TImPOP3GetMailType aPOP3GetMailType)
	: CMsvOperation(aMsvSession, EPriorityStandard, aObserverRequestStatus),
	iPop3ClientMtm(aPop3ClientMtm),
	iPOP3GetMailType(aPOP3GetMailType)
	{
	__DECLARE_NAME(_S("CImPOP3GetMail"));
	}

void CImPOP3GetMail::ConstructL(const CMsvEntrySelection& aMsvEntrySelection, TDes8& aPop3GetMailInfo)
	{
	iMsvEntrySelection = aMsvEntrySelection.CopyL();
	
	iPop3GetMailInfo.Copy(aPop3GetMailInfo);

	CActiveScheduler::Add(this);
	ResetProgress();
	iState = EConnectToMailbox;
	ChangeStateL();
	iObserverRequestStatus = KRequestPending;
	SetActive();
	}

void CImPOP3GetMail::Complete()
	{
	// complete the observer with error
	TRequestStatus* status=&iObserverRequestStatus;
	User::RequestComplete(status,KErrNone);
	}

void CImPOP3GetMail::SelectAndChangeToNextStateL()
	{
	SelectNextStateL();
	ChangeStateL();
	}

void CImPOP3GetMail::RunL()
	{
	TInt error = KErrNone;
	if (iState != EFinished)
		{
		// store progress if connecting, copying/moving new/all/selected messages.
		if ((iState == EConnectToMailbox) || (iState == ECopyNewMessages) ||
			(iState == ECopyAllMessages) ||	(iState == ECopyMessageSelection) ||
			(iState == EMoveNewMessages) || (iState == EMoveAllMessages) ||
			(iState == EMoveMessageSelection))
			{
			StoreProgressL();
			}
			
		if (iProgress.iErrorCode != KErrNone)
			{
			// There has been an error in the previous operation - remember the error.
			iErrorProgress.iTotalMsgs = iProgress.iTotalMsgs;
			iErrorProgress.iMsgsToProcess = iProgress.iMsgsToProcess;
			iErrorProgress.iBytesDone = iProgress.iBytesDone;
			iErrorProgress.iTotalBytes = iProgress.iTotalBytes;
			iErrorProgress.iErrorCode = iProgress.iErrorCode;

			// reset progress error code
			iProgress.iErrorCode = KErrNone;

			// update the state that the error occured.
			switch (iState)
				{
				case EConnectToMailbox:
					iErrorProgress.iPop3SubStateProgress = TPop3Progress::EPopConnecting;
					Complete();
					return;
				case ECopyNewMessages:
				case ECopyAllMessages:
				case ECopyMessageSelection:
					iErrorProgress.iPop3SubStateProgress = TPop3Progress::EPopCopying;
					// The next state is disconnect so continue!
					break;
				case EMoveNewMessages:
				case EMoveAllMessages:
				case EMoveMessageSelection:
					iErrorProgress.iPop3SubStateProgress = TPop3Progress::EPopMoving;
					// The next state is disconnect so continue!
					break;
				case EDisconnectFromMailbox:
					iErrorProgress.iPop3SubStateProgress = TPop3Progress::EPopDisconnecting;
					Complete();
					return;
				default:
					Complete();
					return;
				}
			}
			
			else if (iProgress.iPop3SubStateProgress == TPop3Progress::EPopDisconnected)
			{
			// Connection has dropped, so complete the operation
			Complete();
			return;
			}				
		}
		
		TRAP(error, SelectAndChangeToNextStateL());
		if ((error == KErrNone) && (iState != EFinished))
			{
			SetActive();
			}			
		else if (error != KErrNone)
			{
			// if iState == EFinished, then we don't need to complete - done previously
			TRequestStatus* st = &iObserverRequestStatus;
			User::RequestComplete(st, error);
			}
	}

void CImPOP3GetMail::SelectNextStateL()
	{
	switch (iState)
		{
		case EConnectToMailbox:
			if (iProgress.iErrorCode == KErrNone)
				{
				switch(iPOP3GetMailType)
					{
					case EConnectAndCopyNewMailAndStayOnline:
					case EConnectAndCopyNewMailAndDisconnect:
						iState = ECopyNewMessages;
						break;
					case EConnectAndMoveNewMailAndStayOnline:
					case EConnectAndMoveNewMailAndDisconnect:
						iState = EMoveNewMessages;
						break;
					case EConnectAndCopyMailSelectionAndStayOnline:
					case EConnectAndCopyMailSelectionAndDisconnect:
						iState = ECopyMessageSelection;
						break;
					case EConnectAndMoveMailSelectionAndStayOnline:
					case EConnectAndMoveMailSelectionAndDisconnect:
						iState = EMoveMessageSelection;
						break;
					case EConnectAndCopyAllMailAndStayOnline:
					case EConnectAndCopyAllMailAndDisconnect:
						iState = ECopyAllMessages;
						break;
					case EConnectAndMoveAllMailAndStayOnline:
					case EConnectAndMoveAllMailAndDisconnect:
						iState = EMoveAllMessages;
						break;
					}
				}
			else 
				iState = EDisconnectFromMailbox;
			break;
		case ECopyNewMessages:
		case EMoveNewMessages:
		case ECopyMessageSelection:
		case EMoveMessageSelection:
		case ECopyAllMessages:
		case EMoveAllMessages:
			if ((iPOP3GetMailType == EConnectAndCopyNewMailAndStayOnline) ||
				(iPOP3GetMailType == EConnectAndMoveNewMailAndStayOnline) ||
				(iPOP3GetMailType == EConnectAndCopyMailSelectionAndStayOnline) ||
				(iPOP3GetMailType == EConnectAndMoveMailSelectionAndStayOnline) ||
				(iPOP3GetMailType == EConnectAndCopyAllMailAndStayOnline) ||
				(iPOP3GetMailType == EConnectAndMoveAllMailAndStayOnline))
				{
				iState = EFinished;
				}
			else
				if ((iProgress.iErrorCode == KErrNone) && (iErrorProgress.iErrorCode == KErrNone) || (iProgress.iErrorCode == KErrDiskFull))
					{
   					iState = EDisconnectFromMailbox;
					}
				else
					// The POPS MTM will already have disconnected if it has completed with an error
					{
					iState = EFinished;
				}
			break;
		case EDisconnectFromMailbox:
			iState = EFinished;
			break;
		default:
			User::LeaveIfError(KErrNotSupported);
			break;
		}
	}

void CImPOP3GetMail::ChangeStateL()
	{
	switch (iState)
		{
		case EConnectToMailbox:
			ConnectToMailboxL();
			break;
		case ECopyNewMessages:
			CopyMoveNewMessagesL(ETrue);
			break;
		case EMoveNewMessages:
			CopyMoveNewMessagesL(EFalse);
			break;
		case ECopyMessageSelection:
			CopyMoveMessageSelectionL(ETrue);
			break;
		case EMoveMessageSelection:
			CopyMoveMessageSelectionL(EFalse);
			break;
		case ECopyAllMessages:
			CopyMoveAllMessagesL(ETrue);
			break;
		case EMoveAllMessages:
			CopyMoveAllMessagesL(EFalse);
			break;
		case EDisconnectFromMailbox:
			DisconnectFromMailboxL();
			break;
		case EFinished:
			{
			TRequestStatus* status=&iObserverRequestStatus;
			User::RequestComplete(status,KErrNone);
			}
		}
	}

void CImPOP3GetMail::RequestComplete(TInt aError)
	{
	iStatus = KRequestPending;
	TRequestStatus* status=&iStatus;
	iStatus=KRequestPending;
	User::RequestComplete(status,aError);
	}

void CImPOP3GetMail::ConnectToMailboxL()
	{
	ResetProgress();
	iProgress.iPop3SubStateProgress = TPop3Progress::EPopConnecting;
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMConnect, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
	}

void CImPOP3GetMail::CopyMoveNewMessagesL(TBool aCopy)
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	if (aCopy)
		{
		iProgress.iPop3SubStateProgress = TPop3Progress::EPopCopying;
		iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMCopyNewMailWhenAlreadyConnected, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
		}
	else
		{
		iProgress.iPop3SubStateProgress = TPop3Progress::EPopMoving;
		iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMMoveNewMailWhenAlreadyConnected, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
		}
	}

void CImPOP3GetMail::CopyMoveMessageSelectionL(TBool aCopy)
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	if (aCopy)
		{
		iProgress.iPop3SubStateProgress = TPop3Progress::EPopCopying;
		iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMCopyMailSelectionWhenAlreadyConnected, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
		}
	else
		{
		iProgress.iPop3SubStateProgress = TPop3Progress::EPopMoving;
		iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMMoveMailSelectionWhenAlreadyConnected, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
		}
	}

void CImPOP3GetMail::CopyMoveAllMessagesL(TBool aCopy)
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	if (aCopy)
		{
		iProgress.iPop3SubStateProgress = TPop3Progress::EPopCopying;
		iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMCopyAllMailWhenAlreadyConnected, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
		}
	else
		{
		iProgress.iPop3SubStateProgress = TPop3Progress::EPopMoving;
		iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMMoveAllMailWhenAlreadyConnected, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
		}
	}

void CImPOP3GetMail::DisconnectFromMailboxL()
	{
	//don't reset progress so that no. of messages copied are still remembered
	iProgress.iPop3SubStateProgress = TPop3Progress::EPopDisconnecting;
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iPop3ClientMtm.InvokeAsyncFunctionL(KPOP3MTMDisconnect, *iMsvEntrySelection, iPop3GetMailInfo, iStatus);
	}
