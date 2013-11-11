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
// IMAP4MTM.CPP
// 
//

#include "IMPCMTM.H"
#include "MIUT_ERR.H"

#include <barsc.h>		//RResourceFile
#include <barsread.h>
#include <bautils.h>
#include <txtrich.h>
#include <msvutils.h>
#include <cemailaccounts.h>

#include "MIUTMSG.H"	//CImEmailOperation
#include "CONSYNC.H"
#include <imcm.rsg>
#include "IMCMUTIL.H"
#include <e32base.h>
#include <autosend.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "miut_errconsts.h"
#include "cimmessagepart.h"
#include "timrfc822datefield.h"
#include <mtmuidsdef.hrh>
#endif

#include <msvenhancesearchsortutil.h>

_LIT(KMsvAutoSendExe, "Autosend.exe");
const TUid KMsvAutoSendExeUid = {0x1000A402}; //268477442

#define TODO//@


//
//  Session Observer for the Imap4 Client MTM  
class CImap4ClientSessionObserver : public CBase , public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};

void CImap4ClientSessionObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}


//
//  CImap4ClientMtm methods.


/**
@internalTechnology
*/
/** IMAP4 client MTM factory function.

This function is not called directly by messaging clients. To create a client MTM
object, use CClientMtmRegistry::NewMtmL().

@param aRegisteredMtmDll MTM registry information 
@param aSession Message server session
@return New IMAP4 client MTM object
@see CClientMtmRegistry::NewMtmL()
*/
EXPORT_C CImap4ClientMtm* CImap4ClientMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aSession)
	{
	CImap4ClientMtm* self = new (ELeave) CImap4ClientMtm(aRegisteredMtmDll, aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CImap4ClientMtm::CImap4ClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aSession)
: CBaseMtm(aRegisteredMtmDll, aSession)
	{
	}

void CImap4ClientMtm::ConstructL()
	{
	iImap4ClientSessionObserver = new (ELeave) CImap4ClientSessionObserver;
	iHeader = CImHeader::NewLC();
	CleanupStack::Pop();	// iHeader

	//open the resource file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	//	Need to search for IMCM resource file on all drives -
	//	it won't necessarily be on the same one as IMPC.DLL
	//
	TFileName fileName(KImEngineResourceFile);
	MsvUtils::AddPathAndExtensionToResFileL(fileName);
	BaflUtils::NearestLanguageFile( fs, fileName );		//	change to appropriate language

	RResourceFile resourceFile;
	resourceFile.OpenL(fs,fileName);
	CleanupClosePushL(resourceFile);
	HBufC8* buf = resourceFile.AllocReadLC(EMAIL_ADDRESS_FORMATTING_STRING);
	TResourceReader reader;
	reader.SetBuffer(buf);
	iEmailAddressFormatString = (reader.ReadTPtrC()).AllocL();
	CleanupStack::PopAndDestroy(3);// resourceFile (Close resourceFile), fs
	}

CImap4ClientMtm::~CImap4ClientMtm()
/** Destructor. */
	{
	delete iImap4ClientSessionObserver;
	delete iEmailAddressFormatString;
	delete iHeader;
	delete iMsvEntrySelection;
	delete iImIMAP4GetMail;
	delete iImEmailOperation;
	}

void CImap4ClientMtm::SaveMessageL()
/** Empty implementation of base class function. 

To edit an IMAP message, use CImEmailMessage.
*/
	{
	}

EXPORT_C void CImap4ClientMtm::StoreSettingsL()
/** If the current context of the client MTM is an IMAP4 service, stores settings 
in the Central Repository. 

@panic IMCM 33 MTM has no current context (debug builds only)
@panic IMCM 44 Current context is not a service (debug builds only)
*/
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EImpcNoCMsvEntrySet));

	// check that current context is a service entry
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvServiceEntry,gPanic(EImpcMTMNotAServiceEntry));

	CEmailAccounts* account = CEmailAccounts::NewLC();
  	TImapAccount id;
	account->GetImapAccountL(iMsvEntry->Entry().Id(), id);
	account->SaveImapSettingsL(id, iImImap4Settings);
	CleanupStack::PopAndDestroy(account);    
	}

void CImap4ClientMtm::LoadMessageL()
/** Loads the cache with the message data for the current message context. 

The current context should be an IMAP service or a message. 
If the current context is an IMAP service, then the function loads the service settings
from the Central Repository. This is the same behaviour as RestoreSettingsL().
*/
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
			// These are loaded when the search criteria is known in DoFindL()
			// For API's other than CMsvSearchsortOpOnHeaderBody-> FindInHeaderBodyL(), a call to LoadMessageL()
			// loads the body and the header.
			TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
			
			if ( searchsortutil == NULL )
				{
				CMsvStore* msvStore = iMsvEntry->ReadStoreL();
				CleanupStack::PushL(msvStore);
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

EXPORT_C void CImap4ClientMtm::RestoreSettingsL()
/** If the current context of the client MTM is on an IMAP4 service, restores settings 
from the Central Repository. 

The restored settings can be accessed through the Imap4Settings() function.

@panic IMCM 33 MTM has no current context (debug builds only)
*/
    {
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EImpcNoCMsvEntrySet));

	CEmailAccounts* account = CEmailAccounts::NewLC();
  	TImapAccount id;
	account->GetImapAccountL(iMsvEntry->Entry().Id(), id);
	account->LoadImapSettingsL(id, iImImap4Settings);
	CleanupStack::PopAndDestroy(account);    
	}

CMsvOperation* CImap4ClientMtm::ReplyL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus)
/** Creates an SMTP reply message to the current message context.

@param aDestination Entry to which to assign the reply. 
@param aPartList Defines the parts that are to be copied from the original 
message into the reply.
@param aCompletionStatus The request status to be completed when the operation 
has finished.
@return If successful, this is an asynchronously completing reply operation. 
If failed, this is a completed operation, with status set to the relevant 
error code. */
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TUid messageType = KUidMsgTypeSMTP;
	return CImEmailOperation::CreateReplyL(aCompletionStatus, Session(), iMsvEntry->EntryId(), aDestination, aPartList, msvEmailTypeList, messageType);
	}

CMsvOperation* CImap4ClientMtm::ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus)
/** Creates an forwarded SMTP message from the current message context. 

@param aDestination Entry to which to assign the forwarded message.
@param aPartList Defines the parts that are to be copied from the original 
message into the forwarded message 
@param aCompletionStatus The request status to be completed when the operation 
has finished.
@return If successful, this is an asynchronously completing forward message 
operation. If failed, this is a completed operation, with status set to the 
relevant error code. */
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TUid messageType = KUidMsgTypeSMTP;
	return CImEmailOperation::CreateForwardL(aCompletionStatus, Session(), iMsvEntry->EntryId(), aDestination, aPartList, msvEmailTypeList, messageType);
   }

TUint CImap4ClientMtm::ValidateMessage(TUint aPartList)
/** Validates the current message context.

Only the message recipients and sender fields are checked for well-formed 
addresses.

@param aPartList Indicates the value of the message parts for which validation 
is requested.
@return Values of message parts. 

@panic IMCM 33 MTM has no current context (debug builds only)
*/
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EImpcNoCMsvEntrySet));
	TBool retVal=0;
	if (aPartList & KMsvMessagePartRecipient)
		{
		TBool valid = ETrue;
		// check the recipient list for valid 'addresses' - ie no digits
		for (TInt ii=0; ii < AddresseeList().Count(); ++ii)
			if ( (valid = ValidateAddress((*iAddresseeList)[ii])) == EFalse)
				continue;
		retVal += (valid ? 0 : KMsvMessagePartRecipient);
		}
	if (aPartList & KMsvMessagePartOriginator)
		{
		// check the originator field for valid 'addresses' - ie no digits
		if (!ValidateAddress(iMsvEntry->Entry().iDetails))
			retVal += KMsvMessagePartOriginator;
		}
	return retVal;
	}

TBool CImap4ClientMtm::ValidateAddress(const TPtrC& anAddress)
	{
	return iTImMessageField.ValidInternetEmailAddress(anAddress);
	}

void CImap4ClientMtm::SendOnNextConnectionL()
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

TMsvPartList CImap4ClientMtm::DoFindL(const TDesC& aTextToFind, TMsvPartList aPartList)
	{ 
	CImClientMTMUtils* clientMTMUtils = CImClientMTMUtils::NewL();
	CleanupStack::PushL(clientMTMUtils);

	TMsvPartList retList = KMsvMessagePartNone;
	CMsvFindText* findText = CMsvFindText::NewLC();
 
  	
	// Get a reference to TMsvEnhanceSearchSortUtil  instance set by CMsvSearchsortOpOnHeaderBody class
		
	TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
	
	if( searchsortutil != NULL )
 		{
 		// Get Advanced search sort setting flag
 		TInt32 searchsortsetting=searchsortutil->GetSearchSortSetting();
 		
 		/* If the search is on message body than Load message Body 
 		 If the search is on message header than Load the message header
 		 Also set searchsortsetting flag EMessagePartBodyLoaded or EMessagePartHeaderLoaded 
 		 in TMsvEnhanceSearchSortUtil class
 		 This ensures that the body or header is not loaded twice if 2 query options
 		 are on the body */
 		
 		if(aPartList & KMsvMessagePartBody && !(searchsortsetting & EMessagePartBodyLoaded))  	
   			{
   			CImEmailMessage* message=CImEmailMessage::NewLC(*iMsvEntry);
			message->GetBodyTextL(iMsvEntry->Entry().Id(),CImEmailMessage::EThisMessageOnly,Body(),*iParaFormatLayer,*iCharFormatLayer);
			CleanupStack::PopAndDestroy();//message
			searchsortutil->SetSearchSortSetting(EMessagePartBodyLoaded);
   			}
   		else if(!(searchsortsetting & EMessagePartHeaderLoaded))
   			{
   			// Load the header
			CMsvStore* msvStore = iMsvEntry->ReadStoreL();
			CleanupStack::PushL(msvStore);
			iHeader->RestoreL(*msvStore);
			CleanupStack::PopAndDestroy();//store
			searchsortutil->SetSearchSortSetting(EMessagePartHeaderLoaded);	
   			}
   			
   		// Issue the Find request
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
		if ((searchsortsetting & EMessagePartSort ) || ((searchsortsetting & EMessagePartSearchSort) && (searchsortsetting & EMessagePartLastQueryOption) && (retList)))
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
   			else // Header not present. Copy sort data from TMsvEntry details and description field
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
	else
		{

		// Old implementation for searching
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
	CleanupStack::PopAndDestroy(clientMTMUtils); 
	return retList;
	}

TMsvPartList CImap4ClientMtm::Find(const TDesC& aTextToFind, TMsvPartList aPartList)
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

TInt CImap4ClientMtm::QueryCapability(TUid aCapability, TInt& aResponse)
/** Queries if the MTM supports a particular capability, specified by a UID.

@param aCapability UID of capability to be queried. See MTMDEF.HRH for 
definition of UIDs. 
@param aResponse Response value. The format of the response varies according 
to the capability. 
@return KErrNone: aCapability is a recognised value and a response is returned 
KErrNotSupported: aCapability is not a recognised value 
*/
	{
	TInt error = KErrNone;
   const TInt KImap4MaxTextMessageSize = 0xFFFF;
	switch (aCapability.iUid)
		{
	case KUidMtmQueryMaxBodySizeValue:
	case KUidMtmQueryMaxTotalMsgSizeValue:
		aResponse = KImap4MaxTextMessageSize;
		break;
	case KUidMtmQuerySupportedBodyValue:
		aResponse = KMtm7BitBody + KMtm8BitBody + KMtm16BitBody + KMtmBinaryBody;
		break;
   // Supported
	case KUidMtmQueryCanReceiveMsgValue:		
   case KUidMtmQuerySupportAttachmentsValue:
   case KUidMtmQuerySupportsFolderValue:
		break;
	// Not supported
	case KUidMtmQueryOffLineAllowedValue:	
	case KUidMtmQueryCanSendMsgValue:		
	default:
		aResponse = ETrue;
		return KErrNotSupported;
		}
	return error;
	}

void CImap4ClientMtm::InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter)
/** Invokes synchronous IMAP-specific operations.

@param aFunctionId ID of the requested function. Supported IDs are #KIMAP4MTMIsConnected and #KIMAP4MTMBusy.
@param aSelection Collection of function entries to invoke.
@param aParameter Buffer containing input and output parameters. 

@panic IMCM 33 MTM has no current context (debug builds only)
@leave KErrNotSupported Function does not provide function specified by aFunctionId
@see #KIMAP4MTMIsConnected 
@see #KIMAP4MTMBusy
*/
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EImpcNoCMsvEntrySet));
	TInt error = KErrNone;
	switch (aFunctionId)
		{
	case KIMAP4MTMIsConnected:
   case KIMAP4MTMBusy:
		{
		CMsvOperationActiveSchedulerWait *wait = CMsvOperationActiveSchedulerWait::NewLC();
		CMsvOperation* operation = Session().TransferCommandL(aSelection, aFunctionId, aParameter, wait->iStatus);
		CleanupStack::PushL(operation);
		wait->iStatus = KRequestPending;
		wait->Start();
      error = operation->iStatus.Int();
		CleanupStack::PopAndDestroy(2); //operation, wait
		// return the result via aParameter as a TInt packaged into a TDes8
		TPckgBuf<TInt> resultPackage(error);
		aParameter.Copy(resultPackage);
      return;
		}
	default:
		error=KErrNotSupported;
		break;
		}
	User::LeaveIfError(error);
	}

CMsvOperation* CImap4ClientMtm::InvokeAsyncFunctionL(TInt aFunctionId,
												const CMsvEntrySelection& aSelection, 
												TDes8& aParameter, 
												TRequestStatus& aCompletionStatus)
/** Invokes asynchronous IMAP-specific operations.

This provides support for a large number of IMAP-specific functions, including
operations to:

- Connect and logon to a remote server
- Synchronise headers
- Subscribe to mailboxes
- Populate messages
- Copy messages
- Move messages
- Create messages
- Set offline behaviour

For details of operations, see #TImap4Cmds.

@param aFunctionId Specifies which operation to perform e.g. connect, copy 
new mail etc. The specific operations are defined by the #TImap4Cmds enumeration. 
@param aSelection A selection of messages. The use is dependant upon the command 
specified by @c aFunctionID.
@param aParameter Use is dependant upon the command specified by @c aFunctionID
@param aCompletionStatus The status when the operation completes. 
@leave KErrNotFound The selection of email to be moved or copied is empty
@leave KErrNotSupported The specified operation is not recognised
@return If successful, this is an asynchronously completing operation. If failed, 
this is a completed operation, with status set to the relevant error code. 
@panic IMCM 33 MTM has no current context (debug builds only)
@see TImap4Cmds */
	{
    __ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(EImpcNoCMsvEntrySet));
	CMsvOperation* operation = NULL;
	switch (aFunctionId)
		{
	case KIMAP4MTMConnect:
	case KIMAP4MTMConnectAndSynchronise:
	case KIMAP4MTMDisconnect:
		if (aFunctionId!=KIMAP4MTMDisconnect)
			{
			RestoreSettingsL();
			if (iImImap4Settings.AutoSendOnConnect()) //hope iImPop3Settings is restored by this stage
				{
				TRAPD(err,SendOnNextConnectionL());  //ignore the error and continue with connecting
				}
			}
		operation = (Session().TransferCommandL(aSelection, aFunctionId, aParameter, aCompletionStatus));
		break;
	case KIMAP4MTMCancelBackgroundSynchronise:
	case KIMAP4MTMStartBatch:
	case KIMAP4MTMEndBatch:
	case KIMAP4MTMBusy:
	case KIMAP4MTMSelect:
	case KIMAP4MTMSynchronise:
	case KIMAP4MTMSyncTree:
	case KIMAP4MTMSyncSubscription:
	case KIMAP4MTMFullSync:
	case KIMAP4MTMLocalSubscribe:
	case KIMAP4MTMLocalUnsubscribe:
	case KIMAP4MTMInboxNewSync:
	case KIMAP4MTMFolderFullSync:
	case KIMAP4MTMWaitForBackground:
	case KIMAP4MTMRenameFolder:
	case KIMAP4MTMUndeleteAll:
	case KIMAP4MTMCancelOffLineOperations:
	case KIMAP4MTMPopulate:
		{
		if(aFunctionId == KIMAP4MTMPopulate)
			{
			ConvertToPartialPopulate(aParameter);
			return (Session().TransferCommandL(aSelection, aFunctionId, iImap4GetPartialMailInfo, aCompletionStatus));
			}
		operation = (Session().TransferCommandL(aSelection, aFunctionId, aParameter, aCompletionStatus));
		}
		break;
	case KIMAP4MTMConnectAndSyncCompleteAfterConnect:
	case KIMAP4MTMConnectAndSyncCompleteAfterFullSync:
	case KIMAP4MTMConnectAndSyncCompleteAfterDisconnect:
		{
		TImapConnectionCompletionState connectAndSyncCompleteState = EAfterConnect;
		switch (aFunctionId)
			{
			case KIMAP4MTMConnectAndSyncCompleteAfterConnect:
				connectAndSyncCompleteState = EAfterConnect;
				break;
			case KIMAP4MTMConnectAndSyncCompleteAfterFullSync:
				connectAndSyncCompleteState = EAfterFullSync;
				break;
			case KIMAP4MTMConnectAndSyncCompleteAfterDisconnect:
				connectAndSyncCompleteState = EAfterDisconnection;
				break;
			default:
				break;
			}

		MMsvImapConnectionObserver* connectionObserver;
		TPckgC<MMsvImapConnectionObserver*> paramPack(connectionObserver);
		paramPack.Set(aParameter);
		connectionObserver = paramPack();

		operation = CImapConnectAndSyncOp::NewL(Session(),
												aSelection,
												*this,
												CActive::EPriorityStandard,
												aCompletionStatus,
												connectAndSyncCompleteState,
												connectionObserver);
		}
		break;
	case KIMAP4MTMCopyNewMailWhenAlreadyConnected:
	case KIMAP4MTMCopyAllMailWhenAlreadyConnected:
	case KIMAP4MTMCopyMailSelectionWhenAlreadyConnected:
	case KIMAP4MTMMoveNewMailWhenAlreadyConnected:
	case KIMAP4MTMMoveMailSelectionWhenAlreadyConnected:
	case KIMAP4MTMMoveAllMailWhenAlreadyConnected:
	case KIMAP4MTMPopulateNewMailWhenAlreadyConnected:
	case KIMAP4MTMPopulateAllMailWhenAlreadyConnected:
	case KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected:
		{
		delete iMsvEntrySelection;
		iMsvEntrySelection = new(ELeave) CMsvEntrySelection();
		ConvertToPartialPopulate(aParameter);
		switch (aFunctionId)
			{
			case KIMAP4MTMCopyNewMailWhenAlreadyConnected:
			case KIMAP4MTMCopyAllMailWhenAlreadyConnected:
			case KIMAP4MTMMoveNewMailWhenAlreadyConnected:
			case KIMAP4MTMMoveAllMailWhenAlreadyConnected:
			case KIMAP4MTMPopulateNewMailWhenAlreadyConnected:
			case KIMAP4MTMPopulateAllMailWhenAlreadyConnected:
				{
				FilterAllOrNewMailsL(aFunctionId,aSelection,iImap4GetPartialMailInfo);
				}
				break;
			case KIMAP4MTMCopyMailSelectionWhenAlreadyConnected:
			case KIMAP4MTMMoveMailSelectionWhenAlreadyConnected:
			case KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected:
				{
				FilterMailSelectionL(aSelection,iImap4GetPartialMailInfo);
				}
				break;
			}

		// only need to do copy/move if the selection is not empty
		if (iMsvEntrySelection->Count()>0)
			{
			return CopyMoveOrPopulateL(aFunctionId,iImap4GetPartialMailInfo,aCompletionStatus);
			}
		else
			operation = CMsvCompletedOperation::NewL(Session(), KUidMsgTypeIMAP4, KNullDesC8, iMsvEntry->Entry().iServiceId, aCompletionStatus);	
		}
		break;
	case KIMAP4MTMConnectAndCopyNewMailAndStayOnline:
	case KIMAP4MTMConnectAndCopyNewMailAndDisconnect:
	case KIMAP4MTMConnectAndCopyAllMailAndStayOnline:
	case KIMAP4MTMConnectAndCopyAllMailAndDisconnect:
	case KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline:
	case KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect:
	case KIMAP4MTMConnectAndMoveNewMailAndStayOnline:
	case KIMAP4MTMConnectAndMoveNewMailAndDisconnect:
	case KIMAP4MTMConnectAndMoveAllMailAndStayOnline:
	case KIMAP4MTMConnectAndMoveAllMailAndDisconnect:
	case KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline:
	case KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect:
	case KIMAP4MTMConnectAndPopulateNewMailAndStayOnline:
	case KIMAP4MTMConnectAndPopulateNewMailAndDisconnect:
	case KIMAP4MTMConnectAndPopulateAllMailAndStayOnline:
	case KIMAP4MTMConnectAndPopulateAllMailAndDisconnect:
	case KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline:
	case KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect:
		{
		ConvertToPartialPopulate(aParameter);
		return iImIMAP4GetMail->GetMailL(aFunctionId, *this, aSelection, iImap4GetPartialMailInfo, aCompletionStatus);
		}
	case KIMAP4MTMCreateNewEmailMessage:
	case KIMAP4MTMCreateReplyEmailMessage:
	case KIMAP4MTMCreateForwardEmailMessage:
	case KIMAP4MTMCreateForwardAsAttachmentEmailMessage:
	case KIMAP4MTMCreateReceiptEmailMessage:
		{
		TImCreateMessageOptions createMessageOptions;	
		TPckgC<TImCreateMessageOptions> paramPack(createMessageOptions);
		paramPack.Set(aParameter);
		switch (aFunctionId)
			{
		case KIMAP4MTMCreateNewEmailMessage:
			return iImEmailOperation->CreateNewL(aCompletionStatus, iMsvEntry->Session(), aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KIMAP4MTMCreateReplyEmailMessage:
			return iImEmailOperation->CreateReplyL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KIMAP4MTMCreateForwardEmailMessage:
			return iImEmailOperation->CreateForwardL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KIMAP4MTMCreateForwardAsAttachmentEmailMessage:
			return iImEmailOperation->CreateForwardAsAttachmentL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		case KIMAP4MTMCreateReceiptEmailMessage:
			return iImEmailOperation->CreateReceiptL(aCompletionStatus, iMsvEntry->Session(), aSelection[1], aSelection[0], paramPack().iMsvPartList, paramPack().iMsvEmailTypeList, paramPack().iMessageType);
		default:
			User::Leave(KErrNotSupported);
			return NULL;	// shouldn't really get here!
			}
		}
	default:
		User::Leave(KErrNotSupported);
		return NULL;
		}
	return operation;
}

/** Converts the full download information to partial download informations with parameters 
for partial download information set to  defaults. This package will later be interpreted by 
server to check whether it is for full download or partial download.

@param aParameter Contains information about full fetch of a message or
partial fetch of a message .
*/
void CImap4ClientMtm::ConvertToPartialPopulate(TDes8& aParameter)
	{
	if(aParameter.Length() == sizeof(TImImap4GetPartialMailInfo))
		{
		iImap4GetPartialMailInfo.Copy(aParameter);
		}
	else
		{
		TImImap4GetMailInfo imap4GetMailInfo;	
		TPckgC<TImImap4GetMailInfo> paramPack(imap4GetMailInfo);
		paramPack.Set(aParameter);
		// Copy TImImap4GetMailInfo information into TImImap4GetPartialMailInfo
		TImImap4GetPartialMailInfo imap4GetPartialMailInfo;
		imap4GetPartialMailInfo.iGetMailBodyParts = paramPack().iGetMailBodyParts;
		imap4GetPartialMailInfo.iMaxEmailSize = paramPack().iMaxEmailSize;
		imap4GetPartialMailInfo.iDestinationFolder = paramPack().iDestinationFolder;

		// Set the remaining members to default so that the server can check 
		// if these are defaults, then this package is for TImImap4GetMailInfo
		imap4GetPartialMailInfo.iTotalSizeLimit	= KMaxTInt;
		imap4GetPartialMailInfo.iBodyTextSizeLimit = KMaxTInt;
		imap4GetPartialMailInfo.iAttachmentSizeLimit = KMaxTInt;
		imap4GetPartialMailInfo.iPartialMailOptions = ENoSizeLimits;
		
		iImap4GetPartialMailInfo = imap4GetPartialMailInfo;
		}
	}

/** Returns whether aParameter contains information for partial fetch of full fetch

@param aParameter Contains information about full fetch of a message or
partial fetch of a message .

@return ETrue if aParamter contains information for partial fetch
else EFalse indicates it has information for full download of a message
*/
TBool CImap4ClientMtm::IsPartialPopulate(TDes8& aParameter)
	{
	TBool isPartialPopulate = EFalse;
	if(aParameter.Length() == sizeof(TImImap4GetPartialMailInfo))
		{
		TImImap4GetPartialMailInfo iImapGetPartialMailInfo;
		TPckgC<TImImap4GetPartialMailInfo>	pkgPartial(iImapGetPartialMailInfo);
		pkgPartial.Set(aParameter);

		if(pkgPartial().iPartialMailOptions == ENoSizeLimits &&
			pkgPartial().iTotalSizeLimit == KMaxTInt &&
			pkgPartial().iBodyTextSizeLimit == KMaxTInt && 
			pkgPartial().iAttachmentSizeLimit == KMaxTInt && 
			(pkgPartial().iGetMailBodyParts == EGetImap4EmailHeaders || 
			pkgPartial().iGetMailBodyParts == EGetImap4EmailBodyText ||
			pkgPartial().iGetMailBodyParts == EGetImap4EmailBodyTextAndAttachments ||
			pkgPartial().iGetMailBodyParts == EGetImap4EmailAttachments ||
			pkgPartial().iGetMailBodyParts == EGetImap4EmailBodyAlternativeText))
			{
			isPartialPopulate = EFalse;
			}
		else
			{
			isPartialPopulate = ETrue;
			}
		}
	else
		{
		isPartialPopulate = EFalse;
		}
	return isPartialPopulate;
	}

/** Copies or moves the selection only if the selection is not empty

@param aFunctionId Specifies which operation to perform e.g. copy new mail, Move or Populate.
The specific operations are defined by the TImap4Cmds enumeration. 
@param aParameter Use is dependant upon the command specified by aFunctionID
@param aCompletionStatus The status when the operation completes. 
@return If successful, this is an asynchronously completing operation. If failed, 
this is a completed operation, with status set to the relevant error code. 
*/
CMsvOperation* CImap4ClientMtm::CopyMoveOrPopulateL(TInt aFunctionId,TDes8& aParameter,TRequestStatus& aCompletionStatus)
	{
	// set entry to the parent of the selection of messages
	// assumes the selection of messages have the same parent i.e. in the same folder
	CMsvOperation* operation = NULL;
	TMsvEmailEntry entry;
	TMsvId service = KMsvNullIndexEntryId;
	iMsvEntry->Session().GetEntry((*iMsvEntrySelection)[0], service, entry);
	iMsvEntry->SetEntryL(entry.Parent());
	
	// Unpackage TImImap4GetPartialMailInfo
	TImImap4GetPartialMailInfo imap4GetMailInfo;	
	TPckgC<TImImap4GetPartialMailInfo> paramPack(imap4GetMailInfo);
	paramPack.Set(aParameter);	

	if ((aFunctionId == KIMAP4MTMCopyNewMailWhenAlreadyConnected) ||
		(aFunctionId == KIMAP4MTMCopyMailSelectionWhenAlreadyConnected) ||
		(aFunctionId == KIMAP4MTMCopyAllMailWhenAlreadyConnected))
		{
		return iMsvEntry->CopyL(*iMsvEntrySelection, paramPack().iDestinationFolder, aCompletionStatus);
		}
	else if ((aFunctionId == KIMAP4MTMMoveNewMailWhenAlreadyConnected) ||
		(aFunctionId == KIMAP4MTMMoveMailSelectionWhenAlreadyConnected) ||
		(aFunctionId == KIMAP4MTMMoveAllMailWhenAlreadyConnected))
		{
		return iMsvEntry->MoveL(*iMsvEntrySelection, paramPack().iDestinationFolder, aCompletionStatus);
		}
	else //KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected,KIMAP4MTMPopulateNewMailWhenAlreadyConnected,KIMAP4MTMPopulateAllMailWhenAlreadyConnected
		{
		operation = Session().TransferCommandL(*iMsvEntrySelection, KIMAP4MTMPopulate, paramPack, aCompletionStatus);		
		return operation;
		}
	}

/** Filter the selection of messages for all mails or new mails

@param aFunctionId Specifies which operation to perform e.g. connect, copy 
new mail etc. The specific operations are defined by the TImap4Cmds enumeration. 
@param aSelection A selection of messages. The use is dependant upon the command 
specified by aFunctionID.
@param aParameter Use is dependant upon the command specified by aFunctionID . 
This can have information for full fetch or partial fetch of a message. 
*/
void CImap4ClientMtm::FilterAllOrNewMailsL(TInt aFunctionId,	const CMsvEntrySelection& aSelection,TDes8& aParameter)
	{
	iMsvEntry->SetEntryL(aSelection[1]); // set entry to the folder from which to copy/move
	// get a selection of messages
	CMsvEntrySelection* msvEntrySelection = iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	CleanupStack::PushL(msvEntrySelection);
	TMsvId messageId;
	// get a selection of messages which are new - i.e have new flag set.
	//	from the selection of messages under the service
	for (TInt i=0; i<msvEntrySelection->Count(); i++)
		{
		messageId = (*msvEntrySelection)[i];
		TMsvEmailEntry entry;
		TMsvId service = KMsvNullIndexEntryId;
		iMsvEntry->Session().GetEntry(messageId, service, entry);

		TImImap4GetPartialMailInfo imap4GetPartialMailInfo;	
		TPckgC<TImImap4GetPartialMailInfo> paramPack(imap4GetPartialMailInfo);
		paramPack.Set(aParameter);
			
        TBool isComplete = ! ((entry.Complete() && entry.PartialDownloaded()) ||
							 	(!entry.Complete() && 
							 		(!entry.BodyTextComplete() 
							 		|| (paramPack().iGetMailBodyParts ==  EGetImap4EmailBodyTextAndAttachments)) 
							 		   && !entry.PartialDownloaded()));
		if ((aFunctionId == KIMAP4MTMCopyNewMailWhenAlreadyConnected) ||
			(aFunctionId == KIMAP4MTMMoveNewMailWhenAlreadyConnected) ||
			(aFunctionId == KIMAP4MTMPopulateNewMailWhenAlreadyConnected))
			{

			if(aFunctionId == KIMAP4MTMPopulateNewMailWhenAlreadyConnected && IsPartialPopulate(aParameter) && !isComplete)
				{
				if ((entry.iType == KUidMsvMessageEntry) && (entry.New()))
					iMsvEntrySelection->AppendL(messageId);
				}
			else
				{
				if ((entry.iType == KUidMsvMessageEntry) && (entry.New()) && (entry.iSize <= paramPack().iMaxEmailSize))
					{
					if ((isComplete && aFunctionId == KIMAP4MTMPopulateNewMailWhenAlreadyConnected) ||
					 	(entry.Parent() == paramPack().iDestinationFolder && (aFunctionId == KIMAP4MTMMoveNewMailWhenAlreadyConnected || isComplete)))
						{
						// Selected entries are filtered from the original list if this is a 
						// populate operation and the message is complete.
						//
						// Note that a copy to local operation where the destination folder is
						// the same as the source folder is equivalent to a populate operation
						// as the contents of the message are fetched to the local mirror folder
						// and hence complete messages are removed from the list in this case.
						//
						// Move operations where the source and destination folder are the same
						// are not supported and are also filtered from the original list.
						}
					else
						{
						iMsvEntrySelection->AppendL(messageId);
						}
					}
				}
			}
		else // KIMAP4MTMCopyAllMailWhenAlreadyConnected, KIMAP4MTMMoveAllMailWhenAlreadyConnected, KIMAP4MTMPopulateAllMailWhenAlreadyConnected
			{
			
			if(aFunctionId == KIMAP4MTMPopulateAllMailWhenAlreadyConnected && IsPartialPopulate(aParameter) && !isComplete)
				{
				if ((entry.iType == KUidMsvMessageEntry) && !entry.Complete())
					iMsvEntrySelection->AppendL(messageId);
				}
			else
				{
				if ((entry.iType == KUidMsvMessageEntry) && (entry.iSize <= paramPack().iMaxEmailSize))
					{
					if ((isComplete && aFunctionId == KIMAP4MTMPopulateAllMailWhenAlreadyConnected) ||
					 (entry.Parent() == paramPack().iDestinationFolder && (aFunctionId == KIMAP4MTMMoveAllMailWhenAlreadyConnected || isComplete)))
						{
						// Selected entries are filtered from the original list if this is a 
						// populate operation and the message is complete.
						//
						// Note that a copy to local operation where the destination folder is
						// the same as the source folder is equivalent to a populate operation
						// as the contents of the message are fetched to the local mirror folder
						// and hence complete messages are removed from the list in this case.
						//
						// Move operations where the source and destination folder are the same
						// are not supported and are also filtered from the original list.
						}
					else
						{
						iMsvEntrySelection->AppendL(messageId);
						}
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(); //msvEntrySelection;
	}	

/** Filters the mails selected for partial fetch or full fetch of messages

@param aSelection A selection of messages. The use is dependant upon the command 
specified by aFunctionID.
@param aParameter Use is dependant upon the command specified by aFunctionID . 
This can have information for full fetch or partial fetch of a message. 
*/
void CImap4ClientMtm::FilterMailSelectionL(const CMsvEntrySelection& aSelection,TDes8& aParameter)
	{
	iMsvEntry->SetEntryL(aSelection[0]); // set entry to service
	TMsvId messageId;
	// get the selection of messages from aSelection (note first element of the
	// CMsvEntrySelection is the service which is then followed by any messages
	for (TInt i=0; i<aSelection.Count(); i++)
		{
		messageId = (aSelection)[i];
		if (i>0) // since 1st element is the service
			{
			TMsvEmailEntry entry;
			TMsvId service = KMsvNullIndexEntryId;			
			User::LeaveIfError(iMsvEntry->Session().GetEntry(messageId, service, entry));
			TImImap4GetPartialMailInfo imap4GetPartialMailInfo;	
			TPckgC<TImImap4GetPartialMailInfo> paramPack(imap4GetPartialMailInfo);
			paramPack.Set(aParameter);
			
			  TBool isComplete = ! ((entry.Complete() && entry.PartialDownloaded()) ||
								 	(!entry.Complete() && 
								 		(!entry.BodyTextComplete() 
					 					|| (paramPack().iGetMailBodyParts ==  EGetImap4EmailBodyTextAndAttachments)) 
					 		   			   && !entry.PartialDownloaded()));

			if(IsPartialPopulate(aParameter) && !isComplete)
				{
				if ((entry.iType == KUidMsvMessageEntry))
					iMsvEntrySelection->AppendL(messageId);	
				}
			else
				{

				if ((entry.iType == KUidMsvMessageEntry) && (entry.iSize <= paramPack().iMaxEmailSize))
					{
					if(entry.Parent() == paramPack().iDestinationFolder && isComplete)
						{
						// do not append if this is a populate operation and the message is complete
						}
					else
						{
						iMsvEntrySelection->AppendL(messageId);			
						}
					}
				}
			}
		}
	}

void CImap4ClientMtm::HandleEntryEvent(TMsvEntryEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{    
	}

//
// CImImap4Settings wrapper functions
EXPORT_C void CImap4ClientMtm::SetImap4SettingsL(const CImImap4Settings& aSettings)
/** Sets the IMAP4 email service settings.

This can be called at any time to override any current email settings (if 
they exist). However, new settings are only used by the MTM if they have been 
stored and then a new IMAP4 email session has been started: i.e. settings 
cannot be changed in mid-session.

@param aSettings New IMAP4 service settings */
    {
    iImImap4Settings.CopyL(aSettings);
    }

EXPORT_C const CImImap4Settings& CImap4ClientMtm::Imap4Settings() const
/** Gets the IMAP4 email service settings currently being used by the client MTM, 
if they exist. 

If no settings exist, then an empty settings object will be returned by this 
function. Settings will only exist if a prior call has been made to the RestoreL() 
function when the client MTM was positioned over an IMAP4 service in the message 
store.

@return IMAP4 service settings */
    {
    return iImImap4Settings;
    }

//
// inherited from MUndoOffLine. This method is responsible for undoing
// an offline operation.
/**
This function is not supported.

@param aDeleted Unused
@param aFolderId Unused
*/
void CImap4ClientMtm::UndoOffLineChangesL(const CImOffLineOperation& /*aDeleted*/, TMsvId /*aFolderId*/)
//
// This method receives an offline operation struct the user wishes to cancel.
// The offline operation will already have been removed from the store of the
// folder. All that needs to be done is actually undo the operation itself.
// For instance, if an offline operation moved a message to another folder, this
// is the time to move the message back.
// This is probably best done by theserver dll, as the move has to be done without
// doing it on the remote server.
//
    {
    TODO//

    }

//
// Returning a list of all the offline operations for a service entry.

/**
@internalTechnology
*/

EXPORT_C CImOperationQueueList* CImap4ClientMtm::QueueListL(CMsvEntry& aServiceEntry)
	{
	return CImOperationQueueList::NewL(aServiceEntry, this);
	}

void CImap4ClientMtm::AddAddresseeL(const TDesC& aRealAddress) 
/** Message address function required by the base class, but not used for IMAP. 

To modify message address information, use the address functions in CImHeader.

@param aRealAddress Address. */
    {
    __ASSERT_DEBUG(iAddresseeList != NULL, gPanic(EImpcNoAddresseeList));
    iAddresseeList->AppendL(aRealAddress);
    }

void CImap4ClientMtm::AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias) 
/** Message address function required by the base class, but not used for IMAP. 

To modify message address information, use the address functions in CImHeader.

@param aRealAddress Address.
@param aAlias Alias information. */
    {
	 __ASSERT_DEBUG(iAddresseeList != NULL, gPanic(EImpcNoAddresseeList));
	HBufC* emailAddress = HBufC::NewLC(aRealAddress.Length()+aAlias.Length()+iEmailAddressFormatString->Length()-4);
	emailAddress->Des().Format(*iEmailAddressFormatString,&aAlias,&aRealAddress);
	iAddresseeList->AppendL(aRealAddress);
	CleanupStack::PopAndDestroy();	// emailAddress
	}

void CImap4ClientMtm::RemoveAddressee(TInt aIndex) 
/** Message address function required by the base class, but not used for IMAP. 

To modify message address information, use the address functions in CImHeader.

@param aIndex Index of address to be removed. */
    {
    __ASSERT_DEBUG(iAddresseeList != NULL, gPanic(EImpcNoAddresseeList));
    iAddresseeList->Delete(aIndex);
    }

void CImap4ClientMtm::ContextEntrySwitched() 
    {
    } 


// Attachment functions to support the SendAs API

/** Unsupported client MTM base class function.

To modify message attachments, use CImEmailMessage.

@param aFilePath Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
@leave KErrNotSupported Function not supported.
 */
EXPORT_C void CImap4ClientMtm::AddAttachmentL(const TDesC& /*aFilePath*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

/** Unsupported client MTM base class function.

To modify message attachments, use CImEmailMessage.

@param aFile Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
@leave KErrNotSupported Function not supported.
 */	
EXPORT_C void CImap4ClientMtm::AddAttachmentL(RFile& /*aFile*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/** Unsupported client MTM base class function.

To modify message attachments, use CImEmailMessage.

@param aFilePath Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
@leave KErrNotSupported Function not supported.
 */
EXPORT_C void CImap4ClientMtm::AddLinkedAttachmentL(const TDesC& /*aFilePath*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/** Unsupported client MTM base class function.

To modify message attachments, use CImEmailMessage.

@param aAttachmentId Unused
@param aStatus Unused
@leave KErrNotSupported Function not supported.
 */
EXPORT_C void CImap4ClientMtm::AddEntryAsAttachmentL(TMsvId /*aAttachmentId*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/** Unsupported client MTM base class function.

To modify message attachments, use CImEmailMessage.

@param aFileName Unused
@param aAttachmentFile Unused
@param aMimeType Unused
@param aCharset Unused
@param aStatus Unused
@leave KErrNotSupported Function not supported.
 */
EXPORT_C void CImap4ClientMtm::CreateAttachmentL(const TDesC& /*aFileName*/, RFile& /*aAttachmentFile*/, const TDesC8& /*aMimeType*/, TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CImap4ClientMtm::CreateMessageL(TMsvId /*aServiceId*/)
/** Unsupported client MTM base class function.

To create a new message, use CImEmailOperation.

@param aServiceId Unused 
@leave KErrNotSupported Function not supported.
*/
	{
	User::Leave(KErrNotSupported);
	}

/** Unsupported client MTM base class function.

@return KErrNotSupported Function not supported.
@leave KErrNotSupported Function not supported.
*/
EXPORT_C TMsvId CImap4ClientMtm::DefaultServiceL() const
	{
	User::Leave(KErrNotSupported);
	return KErrNotSupported;
	}
	
/** Unsupported client MTM base class function.

@leave KErrNotSupported Function not supported.
*/
EXPORT_C void CImap4ClientMtm::RemoveDefaultServiceL()
	{
	User::Leave(KErrNotSupported);
	}

/** Unsupported client MTM base class function.

@param aService Unused
@leave KErrNotSupported Function not supported.
*/
EXPORT_C void CImap4ClientMtm::ChangeDefaultServiceL(const TMsvId& /*aService*/)
	{
	User::Leave(KErrNotSupported);
	}


/*
//	Imap4GetMail
*/
EXPORT_C CMsvOperation* CImImap4GetMail::GetMailL(TInt aFunctionId, CImap4ClientMtm& aImap4ClientMtm, const CMsvEntrySelection& aMsvEntrySelection, TDes8& aImap4GetMailInfo, TRequestStatus& aObserverRequestStatus)
/** Creates and begins a new IMAP4 get mail operation.

Note that the function should be, though is not, marked as static. The workaround
is to call the function using a NULL CImImap4GetMail pointer:
@code
CImImap4GetMail* gm = NULL;
CMsvOperation* gmOp = gm->GetMailL(id, mtm, sel, info, status);
@endcode

Alternatively, instead of using this class, call 
CImap4ClientMtm::InvokeAsyncFunctionL() directly. Any functionality
accessible through this class can also be accessed through that function. 

@param aFunctionId Type of operation to perform, a TImap4Cmds value. Permitted commands are:

Copy commands:

- #KIMAP4MTMConnectAndCopyNewMailAndStayOnline
- #KIMAP4MTMConnectAndCopyNewMailAndDisconnect
- #KIMAP4MTMConnectAndCopyAllMailAndStayOnline
- #KIMAP4MTMConnectAndCopyAllMailAndDisconnect
- #KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline
- #KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect

Move commands:

- #KIMAP4MTMConnectAndMoveNewMailAndStayOnline
- #KIMAP4MTMConnectAndMoveNewMailAndDisconnect
- #KIMAP4MTMConnectAndMoveAllMailAndStayOnline
- #KIMAP4MTMConnectAndMoveAllMailAndDisconnect
- #KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline
- #KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect

Populate commands:

- #KIMAP4MTMConnectAndPopulateNewMailAndStayOnline
- #KIMAP4MTMConnectAndPopulateNewMailAndDisconnect
- #KIMAP4MTMConnectAndPopulateAllMailAndStayOnline
- #KIMAP4MTMConnectAndPopulateAllMailAndDisconnect
- #KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline
- #KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect

@param aImap4ClientMtm A reference to the IMAP4 Client MTM that wants to perform 
the Get Mail operation.
@param aMsvEntrySelection A selection of messages that need to be copied/moved 
to a local folder. The first entry in this selection MUST be the service. 
Other IDs can be appended to the selection depending on which operation is 
to be performed. For the 'copy new', 'copy all', 'move new', 'move all', 'populate 
new', 'populate all' situations, the folder from which the messages are to 
be copied, moved or populated should be appended after the service ID. For 
all the other situations (i.e. copying, moving or populating a selection of 
messages), the selection of messages should be appended after the service 
ID. Please note that the selection of messages MUST have the same parent i.e. 
MUST be in the same folder.
@param aImap4GetMailInfo A packaged TImImap4GetMailInfo object storing the 
maximum message size, the destination folder ID, and what message parts are 
required. For populate commands, this can be a packaged TImImap4GetPartialMailInfo 
object, specifying separate size limits for body text and attachments.
@param aObserverRequestStatus The status to be completed when the get mail 
operation has completed.
@return The new message operation object through which the get operation can 
be controlled.

@see TImap4Cmds
@see TImImap4GetMailInfo 
@see TImImap4GetPartialMailInfo
*/
	{
	CImImap4GetMail* self = new(ELeave) CImImap4GetMail(aImap4ClientMtm.Session(), aImap4ClientMtm, aObserverRequestStatus);
	CleanupStack::PushL(self);
	self->ConstructL(aFunctionId, aMsvEntrySelection, aImap4GetMailInfo);
	CleanupStack::Pop(); //self
	return self;
	}

CImImap4GetMail::~CImImap4GetMail()
/** Destructor. */
	{
	Cancel();
	delete iMsvEntrySelection;
	delete iMsvOperation;
	}

const TDesC8& CImImap4GetMail::FinalProgress()
/** Gets information about a completed operation.

@return Packaged TImap4GenericProgress holding progress information. 
@panic IMCM 30 Operation has not completed
@see TImap4GenericProgress */
	{
	__ASSERT_ALWAYS(!IsActive(), gPanic(EMiutActiveInFinalProgress));

	const TDesC8& opProgress = iMsvOperation->FinalProgress();

	if (opProgress.Length())
		{
		// if an error was encountered during the Get New Mail operation then need to 
		// return this as part of the final progresse.  If no error was encoutered then
		// return iProgress
		TImap4GenericProgress prog;	
		TPckgC<TImap4GenericProgress> paramPack(prog);
		paramPack.Set(opProgress);
		TImap4GenericProgress progress=paramPack();
		if (iErrorProgress.iErrorCode != KErrNone)
			{
			progress.iMsgsToDo = iErrorProgress.iMsgsToDo;
			progress.iBytesDone = iErrorProgress.iBytesDone;
			progress.iErrorCode = iErrorProgress.iErrorCode;
			progress.iImap4SubStateProgress = iErrorProgress.iImap4SubStateProgress;
			}
		else 
			progress.iImap4SubStateProgress = progress.iState;

		switch(iCommand)
			{
			case KIMAP4MTMConnectAndCopyNewMailAndStayOnline:
			case KIMAP4MTMConnectAndCopyNewMailAndDisconnect:
				progress.iState= TImap4GenericProgress::ECopyNewMail;
				break;
			case KIMAP4MTMConnectAndMoveNewMailAndStayOnline:
			case KIMAP4MTMConnectAndMoveNewMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EMoveNewMail;
				break;
			case KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline:
			case KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect:
				progress.iState = TImap4GenericProgress::ECopyMailSelection;
				break;
			case KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline:
			case KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect:
				progress.iState = TImap4GenericProgress::EMoveMailSelection;
				break;
			case KIMAP4MTMConnectAndCopyAllMailAndStayOnline:
			case KIMAP4MTMConnectAndCopyAllMailAndDisconnect:
				progress.iState = TImap4GenericProgress::ECopyAllMail;
				break;
			case KIMAP4MTMConnectAndMoveAllMailAndStayOnline:
			case KIMAP4MTMConnectAndMoveAllMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EMoveAllMail;
				break;
			case KIMAP4MTMConnectAndPopulateNewMailAndStayOnline:
			case KIMAP4MTMConnectAndPopulateNewMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EPopulateNewMail;
				break;
			case KIMAP4MTMConnectAndPopulateAllMailAndStayOnline:
			case KIMAP4MTMConnectAndPopulateAllMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EPopulateAllMail;
				break;
			case KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline:
			case KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect:
				progress.iState = TImap4GenericProgress::EPopulateMailSelection;
				break;
			}
		
		iProgressBuf = progress;
		}
	return iProgressBuf;
	}

void CImImap4GetMail::ResetProgress()
	{
	switch(iCommand)
		{
		case KIMAP4MTMConnectAndCopyNewMailAndStayOnline:
		case KIMAP4MTMConnectAndCopyNewMailAndDisconnect:
			iProgress.iState = TImap4GenericProgress::ECopyNewMail;
			break;
		case KIMAP4MTMConnectAndMoveNewMailAndStayOnline:
		case KIMAP4MTMConnectAndMoveNewMailAndDisconnect:
			iProgress.iState = TImap4GenericProgress::EMoveNewMail;
			break;
		case KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline:
		case KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect:
			iProgress.iState = TImap4GenericProgress::ECopyMailSelection;
			break;
		case KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline:
		case KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect:
			iProgress.iState = TImap4GenericProgress::EMoveMailSelection;
			break;
		case KIMAP4MTMConnectAndCopyAllMailAndStayOnline:
		case KIMAP4MTMConnectAndCopyAllMailAndDisconnect:
			iProgress.iState = TImap4GenericProgress::ECopyAllMail;
			break;
		case KIMAP4MTMConnectAndMoveAllMailAndStayOnline:
		case KIMAP4MTMConnectAndMoveAllMailAndDisconnect:
			iProgress.iState = TImap4GenericProgress::EMoveAllMail;
			break;
		case KIMAP4MTMConnectAndPopulateNewMailAndStayOnline:
		case KIMAP4MTMConnectAndPopulateNewMailAndDisconnect:
			iProgress.iState = TImap4GenericProgress::EPopulateNewMail;
			break;
		case KIMAP4MTMConnectAndPopulateAllMailAndStayOnline:
		case KIMAP4MTMConnectAndPopulateAllMailAndDisconnect:
			iProgress.iState = TImap4GenericProgress::EPopulateAllMail;
			break;
		case KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline:
		case KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect:
			iProgress.iState = TImap4GenericProgress::EPopulateMailSelection;
			break;
		}
	iProgress.iMsgsToDo = 0;
	iProgress.iBytesDone = 0;
	iProgress.iErrorCode = KErrNone;
	}

const TDesC8& CImImap4GetMail::ProgressL()
/** Gets information on the progress of the operation. 

@return Packaged TImap4GenericProgress holding progress information. 
@see TImap4GenericProgress */
	{
	const TDesC8& opProgress = iMsvOperation->ProgressL();

	if (opProgress.Length())
		{
		// need to get Sub operation progress and put this into iImap4SubStateProgress
		TImap4GenericProgress prog;	
		TPckgC<TImap4GenericProgress> paramPack(prog);
		paramPack.Set(opProgress);
		TImap4GenericProgress progress=paramPack();	

		progress.iImap4SubStateProgress = progress.iState;
		switch(iCommand)
			{
			case KIMAP4MTMConnectAndCopyNewMailAndStayOnline:
			case KIMAP4MTMConnectAndCopyNewMailAndDisconnect:
				progress.iState = TImap4GenericProgress::ECopyNewMail;
				break;
			case KIMAP4MTMConnectAndMoveNewMailAndStayOnline:
			case KIMAP4MTMConnectAndMoveNewMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EMoveNewMail;
				break;
			case KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline:
			case KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect:
				progress.iState = TImap4GenericProgress::ECopyMailSelection;
				break;
			case KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline:
			case KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect:
				progress.iState = TImap4GenericProgress::EMoveMailSelection;
				break;
			case KIMAP4MTMConnectAndCopyAllMailAndStayOnline:
			case KIMAP4MTMConnectAndCopyAllMailAndDisconnect:
				progress.iState = TImap4GenericProgress::ECopyAllMail;
				break;
			case KIMAP4MTMConnectAndMoveAllMailAndStayOnline:
			case KIMAP4MTMConnectAndMoveAllMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EMoveAllMail;
				break;
			case KIMAP4MTMConnectAndPopulateNewMailAndStayOnline:
			case KIMAP4MTMConnectAndPopulateNewMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EPopulateNewMail;
				break;
			case KIMAP4MTMConnectAndPopulateAllMailAndStayOnline:
			case KIMAP4MTMConnectAndPopulateAllMailAndDisconnect:
				progress.iState = TImap4GenericProgress::EPopulateAllMail;
				break;
			case KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline:
			case KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect:
				progress.iState = TImap4GenericProgress::EPopulateMailSelection;
				break;
			}
		iProgressBuf = progress;
		}
	return iProgressBuf;
	}

void CImImap4GetMail::StoreProgressL()
	{
	const TDesC8& opProgress = iMsvOperation->ProgressL();

	if (opProgress.Length())
		{
		TImap4GenericProgress prog;	
		TPckgC<TImap4GenericProgress> paramPack(prog);
		paramPack.Set(opProgress);
		TImap4GenericProgress progress=paramPack();	

		iProgress.iMsgsToDo = progress.iMsgsToDo;
		iProgress.iBytesDone = progress.iBytesDone;
		iProgress.iState = progress.iState;
		iProgress.iImap4SubStateProgress = progress.iState;
		
		// only write the error code if no error has previously been set
		if ((iProgress.iErrorCode == KErrNone) && (iErrorProgress.iErrorCode == KErrNone))
			{
			if (iStatus.Int() != KErrNone)				
				iProgress.iErrorCode = iStatus.Int();
			else
				iProgress.iErrorCode = progress.iErrorCode;
			}
		}
	}

void CImImap4GetMail::DoCancel()
	{
	iMsvOperation->Cancel();
	TRequestStatus* st = &iObserverRequestStatus;
	User::RequestComplete(st, KErrCancel);
	}

CImImap4GetMail::CImImap4GetMail(CMsvSession& aMsvSession, CImap4ClientMtm& aImap4ClientMtm, TRequestStatus& aObserverRequestStatus)
	: CMsvOperation(aMsvSession, EPriorityStandard, aObserverRequestStatus),
	iImap4ClientMtm(aImap4ClientMtm)
	{
	}

void CImImap4GetMail::ConstructL(TInt aFunctionId, const CMsvEntrySelection& aMsvEntrySelection, TDes8& aImap4GetMailInfo)
	{
	iMsvEntrySelection = aMsvEntrySelection.CopyL();
	
	iImap4GetPartialMailInfo.Copy(aImap4GetMailInfo);
	iCommand = aFunctionId;

	CActiveScheduler::Add(this);
	ResetProgress();
	iState = EConnectToMailbox;
	ChangeStateL();
	iObserverRequestStatus = KRequestPending;
	SetActive();
	}

void CImImap4GetMail::Complete()
	{
	// complete the observer with error
	TRequestStatus* status=&iObserverRequestStatus;
	User::RequestComplete(status,KErrNone);
	}

void CImImap4GetMail::SelectAndChangeToNextStateL()
	{
	SelectNextStateL();
	ChangeStateL();
	}

void CImImap4GetMail::RunL()
	{
	TInt error = KErrNone;
	if (iState != EFinished)
		{
		// store progress if connecting, copying/moving new/all/selected messages.
		if ((iState == EConnectToMailbox) || (iState == ECopyNewMessages) ||
			(iState == ECopyAllMessages) ||	(iState == ECopyMessageSelection) ||
			(iState == EMoveNewMessages) || (iState == EMoveAllMessages) ||
			(iState == EMoveMessageSelection) || (iState == EPopulateNewMessages) || 
			(iState == EPopulateAllMessages) ||	(iState == EPopulateMessageSelection))
			StoreProgressL();
		if (iProgress.iErrorCode != KErrNone)
			{
			// There has been an error in the previous operation - remember the error.
			iErrorProgress.iMsgsToDo = iProgress.iMsgsToDo;
			iErrorProgress.iMsgsDone = iProgress.iMsgsDone;
			iErrorProgress.iErrorCode = iProgress.iErrorCode;

			// reset progress error code
			iProgress.iErrorCode = KErrNone;

			// update the state that the error occured.
			switch (iState)
				{
				case EConnectToMailbox:
					iErrorProgress.iImap4SubStateProgress = TImap4GenericProgress::EConnecting;
					Complete();
					return;
				case ECopyNewMessages:
				case ECopyAllMessages:
				case ECopyMessageSelection:
					iErrorProgress.iImap4SubStateProgress = TImap4GenericProgress::ECopying;
					// The next state is disconnect so continue!
					break;
				case EMoveNewMessages:
				case EMoveAllMessages:
				case EMoveMessageSelection:
					iErrorProgress.iImap4SubStateProgress = TImap4GenericProgress::EMoving;
					// The next state is disconnect so continue!
					break;
				case EPopulateNewMessages:
				case EPopulateAllMessages:
				case EPopulateMessageSelection:
					iErrorProgress.iImap4SubStateProgress = TImap4GenericProgress::EFetching;
					// The next state is disconnect so continue!
					break;
				case EDisconnectFromMailbox:
					iErrorProgress.iImap4SubStateProgress = TImap4GenericProgress::EDisconnecting;
					Complete();
					return;
				default:
					Complete();
					return;
				}
			}
		TRAP(error, SelectAndChangeToNextStateL());
		if ((error == KErrNone) && (iState != EFinished))
			SetActive();
		else if (error != KErrNone)
			{
			// if iState == EFinished, then we don't need to complete - done previously
			TRequestStatus* st = &iObserverRequestStatus;
			User::RequestComplete(st, error);
			}
		}
	}

void CImImap4GetMail::SelectNextStateL()
	{
	switch (iState)
		{
		case EConnectToMailbox:
			if (iProgress.iErrorCode == KErrNone)
				{
				switch(iCommand)
					{
					case KIMAP4MTMConnectAndCopyNewMailAndStayOnline:
					case KIMAP4MTMConnectAndCopyNewMailAndDisconnect:
						iState = ECopyNewMessages;
						break;
					case KIMAP4MTMConnectAndMoveNewMailAndStayOnline:
					case KIMAP4MTMConnectAndMoveNewMailAndDisconnect:
						iState = EMoveNewMessages;
						break;
					case KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline:
					case KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect:
						iState = ECopyMessageSelection;
						break;
					case KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline:
					case KIMAP4MTMConnectAndMoveMailSelectionAndDisconnect:
						iState = EMoveMessageSelection;
						break;
					case KIMAP4MTMConnectAndCopyAllMailAndStayOnline:
					case KIMAP4MTMConnectAndCopyAllMailAndDisconnect:
						iState = ECopyAllMessages;
						break;
					case KIMAP4MTMConnectAndMoveAllMailAndStayOnline:
					case KIMAP4MTMConnectAndMoveAllMailAndDisconnect:
						iState = EMoveAllMessages;
						break;
					case KIMAP4MTMConnectAndPopulateNewMailAndStayOnline:
					case KIMAP4MTMConnectAndPopulateNewMailAndDisconnect:
						iState = EPopulateNewMessages;
						break;
					case KIMAP4MTMConnectAndPopulateAllMailAndStayOnline:
					case KIMAP4MTMConnectAndPopulateAllMailAndDisconnect:
						iState = EPopulateAllMessages;
						break;
					case KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline:
					case KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect:
						iState = EPopulateMessageSelection;
						break;
					}
				}
			else 
				iState = EDisconnectFromMailbox;
			break;
		case ECopyNewMessages:
		case ECopyAllMessages:
		case ECopyMessageSelection:
		case EMoveNewMessages:
		case EMoveAllMessages:
		case EMoveMessageSelection:
		case EPopulateNewMessages:
		case EPopulateAllMessages:
		case EPopulateMessageSelection:
			if ((iCommand == KIMAP4MTMConnectAndCopyNewMailAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndCopyAllMailAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndMoveNewMailAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndMoveAllMailAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndMoveMailSelectionAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndPopulateNewMailAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndPopulateAllMailAndStayOnline) ||
				(iCommand == KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline))
				{
				iState = EFinished;
				}
			else
				iState = EDisconnectFromMailbox;
			break;
		case EDisconnectFromMailbox:
			iState = EFinished;
			break;
		default:
			User::LeaveIfError(KErrNotSupported);
			break;
		}
	}

void CImImap4GetMail::ChangeStateL()
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
		case EPopulateNewMessages:
			PopulateNewMessagesL();
			break;
		case EPopulateAllMessages:
			PopulateAllMessagesL();
			break;
		case EPopulateMessageSelection:
			PopulateMessageSelectionL();
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

void CImImap4GetMail::RequestComplete(TInt aError)
	{
	iStatus = KRequestPending;
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

void CImImap4GetMail::ConnectToMailboxL()
	{
	ResetProgress();
	iProgress.iImap4SubStateProgress = TImap4GenericProgress::EConnecting;
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMConnect, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
	}

void CImImap4GetMail::CopyMoveNewMessagesL(TBool aCopy)
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	if (aCopy)
		{
		iProgress.iImap4SubStateProgress = TImap4GenericProgress::ECopying;
		iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMCopyNewMailWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
		}
	else
		{
		iProgress.iImap4SubStateProgress = TImap4GenericProgress::EMoving;
		iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMMoveNewMailWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
		}
	}

void CImImap4GetMail::CopyMoveMessageSelectionL(TBool aCopy)
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	if (aCopy)
		{
		iProgress.iImap4SubStateProgress = TImap4GenericProgress::ECopying;
		iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMCopyMailSelectionWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
		}
	else
		{
		iProgress.iImap4SubStateProgress = TImap4GenericProgress::EMoving;
		iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMMoveMailSelectionWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
		}
	}

void CImImap4GetMail::CopyMoveAllMessagesL(TBool aCopy)
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	if (aCopy)
		{
		iProgress.iImap4SubStateProgress = TImap4GenericProgress::ECopying;
		iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMCopyAllMailWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
		}
	else
		{
		iProgress.iImap4SubStateProgress = TImap4GenericProgress::EMoving;
		iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMMoveAllMailWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
		}
	}

void CImImap4GetMail::PopulateNewMessagesL()
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	iProgress.iImap4SubStateProgress = TImap4GenericProgress::EFetching;
	iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMPopulateNewMailWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
	}

void CImImap4GetMail::PopulateAllMessagesL()
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	iProgress.iImap4SubStateProgress = TImap4GenericProgress::EFetching;
	iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMPopulateAllMailWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
	}

void CImImap4GetMail::PopulateMessageSelectionL()
	{
	ResetProgress();
	delete iMsvOperation;
	iMsvOperation=NULL;
	iProgress.iImap4SubStateProgress = TImap4GenericProgress::EFetching;
	iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
	}

void CImImap4GetMail::DisconnectFromMailboxL()
	{
	//don't reset progress so that no. of messages copied are still remembered
	iProgress.iImap4SubStateProgress = TImap4GenericProgress::EDisconnecting;
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iImap4ClientMtm.InvokeAsyncFunctionL(KIMAP4MTMDisconnect, *iMsvEntrySelection, iImap4GetPartialMailInfo, iStatus);
	}

