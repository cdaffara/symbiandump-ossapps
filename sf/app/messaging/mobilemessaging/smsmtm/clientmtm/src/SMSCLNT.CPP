// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// SMSCLNT.CPP
//

#include <txtrich.h>	// CRichText

#include <msvuids.h>	// KUidMsvRootEntry etc.
#include <mtmdef.hrh>	// KUidMtmQuerySupports etc.
#include <mtmdef.h>		// KMsvMessagePartRecipient	etc.
#include <msvids.h>		// KMsvGlobalOutBoxIndexEntryId etc.
#include <msvftext.h>	// CMsvFindText
#include <biodb.h>		// CBIODatabase
#include <smss.rsg>
#include <barsc.h>
#include <barsread.h>

#include "SMSCLNT.H"	// smsclntmtm
#include "SMCMMAIN.H"	// panics
#include "smscmds.h"	// ESmsMtmCommandReadServiceCenter etc.
#include "smut.h"		// TSmsUtilities
#include "SMUTHDR.H"	// CSmsHeader
#include "SMUTSET.H"	// CSmsSettings
#include <smutsimparam.h>
#include <csmsemailfields.h>
#include <csmsaccount.h>
#include "csmssendmessageoperation.h"

#include <msvenhancesearchsortutil.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "msvconsts.h"
#include <mtmuidsdef.hrh>
#include <tmsvsmsentry.h>
#endif



/**
The maximum number of SMS PDUs allowed in a concatenated SMS message.
Together with KSmcmMaxCharsInMessageConcatenated7Bit, this is the response to
the query for the KUidMtmQueryMaxTotalMsgSizeValue capability.
@see	CSmsClientMtm::QueryCapability
@see	KSmcmMaxCharsInMessageConcatenated7Bit
*/
const TInt KSmcmMaxMessageNumber=0x32;

/**
The maximum number of characters in a concatenated SMS PDU.
Together with KSmcmMaxMessageNumber, this is the response to the query for the
KUidMtmQueryMaxTotalMsgSizeValue capability.
@see	CSmsClientMtm::QueryCapability
@see	KSmcmMaxCharsInMessageConcatenated7Bit
*/
const TInt KSmcmMaxCharsInMessageConcatenated7Bit=0x99;

/**
The maximum number of characters in a non-concatenated SMS PDU.
This is the response to the query for the KUidMtmQueryMaxBodySizeValue
capability.
@see	CSmsClientMtm::QueryCapability
*/
const TInt KSmcmMaxTotalMsgSize=160;

/**
The granularity of the in-memory buffer for CRichText objects.
@see	CRichText::NewL
*/
const TInt KSmcmRichTextConstant=256;


// These default subject formats are used if the resource field has not been
// migrated to include a localised set of subject formats.
_LIT(KSmsDefaultForwardSubjectFormat,	"Fw: %S");
_LIT(KSmsDefaultReplySubjectFormat,		"Re: %S");

/**
Static factory constuctor.

@param	aRegisteredMtmDll
The registry information for the SMS MTM.

@param	aSession
The message server session

@leave KErrArgument
Invalid data read from the SMS resource file

@internalTechnology

@see	CRegisteredMtmDll
*/
EXPORT_C CSmsClientMtm* CSmsClientMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll,CMsvSession& aSession)
	{
	CSmsClientMtm* self=new(ELeave) CSmsClientMtm(aRegisteredMtmDll,aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
*/
CSmsClientMtm::~CSmsClientMtm()
	{
	delete iServiceSettings;
	delete iSmsHeader;
	delete iEmailForwardSubjectFormat;
	delete iEmailReplySubjectFormat;
	}

void CSmsClientMtm::HandleEntryEvent(enum MMsvEntryObserver::TMsvEntryEvent /*aEvent*/,TAny* ,TAny* ,TAny* )
	{
	}

/**
Reads SMS parameters from the SIM.

It starts a new CSmsSimParamOperation, which uses the SMS Server MTM to read the 
SMS parameters from the SIM. 

If the phone is unable to supply the SMS parameters from the SIM, then the progress
information of the CSmsSimParamOperation operation will contain an error. 

@param	aObserverRequestStatus
The request status to be completed when the read operation has completed.

@return
A SIM parameter reader operation. The read SIM parameters are stored in
CSmsSimParamOperation::iSimParams. 
*/
EXPORT_C CSmsSimParamOperation* CSmsClientMtm::ReadSimParamsL(TRequestStatus& aObserverRequestStatus)
	{
	RestoreServiceAndSettingsL();

	CMsvEntry* entry = Session().GetEntryL(iServiceId);
	CleanupStack::PushL(entry);

	CSmsSimParamOperation* op = CSmsSimParamOperation::ReadSimParamsL(entry->Entry().iMtm, iServiceId, Session(), aObserverRequestStatus);

	CleanupStack::PopAndDestroy(entry);
	return op;
	}

/**
Writes SMS parameters to the SIM.

It starts a new CSmsSimParamOperation, which uses the SMS Server to write the 
specified SIM parameters to the SIM.

If the phone is unable to write the SMS parameters to the SIM, then the progress
information of the CSmsSimParamOperation operation will contain an error.

@param	aList
The SIM parameters to be written.

@param	aObserverRequestStatus
The request status to be completed when the write operation has completed.

@return
A SIM parameter writer operation.
*/
EXPORT_C CSmsSimParamOperation* CSmsClientMtm::WriteSimParamsL(const CMobilePhoneSmspList& aList, TRequestStatus& aObserverRequestStatus)
	{
	RestoreServiceAndSettingsL();

	CMsvEntry* entry = Session().GetEntryL(iServiceId);
	CleanupStack::PushL(entry);

	CSmsSimParamOperation* op = CSmsSimParamOperation::WriteSimParamsL(aList, entry->Entry().iMtm, iServiceId, Session(), aObserverRequestStatus);

	CleanupStack::PopAndDestroy(entry);

	return op;
	}

/**
Restores the SMS service ID and SMS service settings.

The SMS service ID is obtained. The service settings for the obtained ID are 
restored from the message server. The SMS service ID can be obtained using the
CSmsClientMtm::ServiceId API and the SMS settings are accessed via the two
CSmsClientMtm::ServiceSettings API overloads.

This API should be used before using any of the other APIs that required the SMS
service ID and SMS service settings to be set.

@see	CSmsClientMtm::ServiceId
@see	CSmsClientMtm::ServiceSettings
*/
EXPORT_C void CSmsClientMtm::RestoreServiceAndSettingsL()
	{
	if (iServiceSettings == NULL)
		{
		iServiceSettings = CSmsSettings::NewL();
		}

	// Let's find the right service entry!
	TSmsUtilities::ServiceIdL(Session(), iServiceId, KUidMsgTypeSMS);

	CSmsAccount* account = CSmsAccount::NewLC();
	// just v2
	account->LoadSettingsL(*iServiceSettings);
	CleanupStack::PopAndDestroy(account);    
	}


CSmsClientMtm::CSmsClientMtm(CRegisteredMtmDll& aRegisteredMtmDll,CMsvSession& aSession)
: CBaseMtm(aRegisteredMtmDll,aSession)
	{
	}

void CSmsClientMtm::ConstructL()
	{
	SwitchCurrentEntryL(KMsvRootIndexEntryId);  //  Creates rich text
	
	RFs& fileSession = Session().FileSession();

	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,Body(),fileSession);

	TRAPD(err, RestoreServiceAndSettingsL());

	if (err)
		{
		if (err != KErrNotFound)
			{
			User::Leave(err);
			}
		TMsvEntry entry;
		entry.iMtm = KUidMsgTypeSMS;
	    entry.iType = KUidMsvServiceEntry;
	    entry.SetReadOnly(EFalse);
	    entry.SetVisible(EFalse);
	    entry.iDate.UniversalTime();
	    entry.iDetails.Set(_L("Default SMS Message"));
    
    	CMsvEntry* root = Session().GetEntryL(KMsvRootIndexEntryId);
	    root->CreateL(entry);
	   	iServiceId = entry.Id();
	   	delete root;
		}

	//Initialise iRealAddressOpen and iRealAddressClose from the SMS resouce file
	RResourceFile resFile = TSmsUtilities::OpenResourceFileL(fileSession);
	CleanupClosePushL(resFile);
	TBuf<4> buf;
	TSmsUtilities::ReadResourceStringL(resFile, R_ADDRESSEE_TELEPHONE_OPEN, buf);
	if (buf.Length() > 0)
		{
		iRealAddressOpen = buf[0];
		}
	else
		{
		User::Leave(KErrArgument); 
		}
	
	TSmsUtilities::ReadResourceStringL(resFile, R_ADDRESSEE_TELEPHONE_CLOSE, buf);
	if (buf.Length() > 0)
		{
		iRealAddressClose = buf[0];
		}
	else
		{
		User::Leave(KErrArgument); 
		}
	iEmailForwardSubjectFormat	= ReadEmailSubjectFormatL(resFile, R_SMS_EMAIL_FORWARD_SUBJECT_FORMAT, KSmsDefaultForwardSubjectFormat);
	iEmailReplySubjectFormat	= ReadEmailSubjectFormatL(resFile, R_SMS_EMAIL_REPLY_SUBJECT_FORMAT, KSmsDefaultReplySubjectFormat);

	CleanupStack::PopAndDestroy(&resFile);
	}
	
HBufC* CSmsClientMtm::ReadEmailSubjectFormatL(RResourceFile& aResourceFile, TInt aResourceId, const TDesC& aDefaultFormat)
	{
	HBufC* format = NULL;
	if( aResourceFile.OwnsResourceId(aResourceId) )
		{
		HBufC8* buf = aResourceFile.AllocReadLC(aResourceId);
		TResourceReader reader;
		reader.SetBuffer(buf);
		format = (reader.ReadTPtrC()).AllocL();
		CleanupStack::PopAndDestroy(buf);	
		}
	else
		{
		format = aDefaultFormat.AllocL();
		}
	return format;
	}

CMsvOperation* CSmsClientMtm::CreateNewEntryL(TMsvEntry& aNewEntry, TMsvId aDestination,CSmsHeader& aSmsHeader,const CRichText& aBody,TRequestStatus& aCompletionStatus)
	{
	RestoreServiceAndSettingsL();

	CMsvEntry* cEntry = CMsvEntry::NewL(Session(), aDestination, TMsvSelectionOrdering());
	CleanupStack::PushL(cEntry);

	cEntry->CreateL(aNewEntry);

	const TMsvId msvid = aNewEntry.Id();
	Session().CleanupEntryPushL(msvid);

	// switch context to the new entry
	cEntry->SetEntryL(msvid);
	aNewEntry = cEntry->Entry(); //for the description and details fields

	aSmsHeader.SetSmsSettingsL(*iServiceSettings);

	// service centre was not being set 
	//If we haven't been provided with a service centre address then use the default one
	if(!aSmsHeader.ReplyPathProvided())
		{
		TInt defaultSC = iServiceSettings->DefaultServiceCenter();
		if(defaultSC > -1)
			{
			aSmsHeader.SetServiceCenterAddressL( iServiceSettings->GetServiceCenter(defaultSC).Address() );	
			}
		}

	// Check the context header to see an email message...
	if( iSmsHeader->EmailFields().Length() > 0 )
		{
		// Yep - update the message as follows
		// 1. Add the email <-> SMS gateway as the recipient of the new message.
		// 2. Set the PID for interworking with email.
		// 3. Set the entry description to be the email subject (if there is one).
		// 4. Set the entry details to be the email address (if there is one).
		DoAddRecipientL(&aSmsHeader, iSmsHeader->FromAddress(), KNullDesC);

		CSmsPDU& pdu = aSmsHeader.Message().SmsPDU();
		if( pdu.ProtocolIdentifierPresent() )
			{
			pdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
			pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
			pdu.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsInternetElectronicMail);
			}
		
		const CSmsEmailFields& fields = aSmsHeader.EmailFields();
		if( fields.Subject().Length() > 0 )
			{
			// There is a subject - use this as the description
			aNewEntry.iDescription.Set(fields.Subject());
			}
		if( fields.HasAddress() )
			{
			// There is an address - use first address as the details.
			aNewEntry.iDetails.Set(fields.Addresses().MdcaPoint(0));			
			}
		}


	CMsvStore* msvstore=cEntry->EditStoreL();
	CleanupStack::PushL(msvstore);

	aSmsHeader.StoreL(*msvstore);
	msvstore->StoreBodyTextL(aBody);
	msvstore->CommitL();

	aNewEntry.iSize = msvstore->SizeL();
	cEntry->ChangeL(aNewEntry);

	CleanupStack::PopAndDestroy(msvstore);


	TPckg<TMsvId> prog(msvid);
	CMsvOperation* op = CMsvCompletedOperation::NewL(Session(), KUidMsgTypeSMS, prog, aNewEntry.iServiceId, aCompletionStatus);

	// Now safe to pop the entry off the message server cleanup stack, and any
	// other remaining resources.
	Session().CleanupEntryPop();
	CleanupStack::PopAndDestroy(cEntry);

	return op;
	}

TBool CSmsClientMtm::ValidRecipients() const
	{
	TInt numrecipients = iSmsHeader->Recipients().Count();
	TBool valid=numrecipients;

	while (numrecipients-- && valid)
		valid=ValidNumber(iSmsHeader->Recipients()[numrecipients]->Address());

	return valid;
	}

TBool CSmsClientMtm::ValidNumber(const TDesC& aNumber) const
	{
	TBool valid=EFalse;
	for (TInt i=0; i<aNumber.Length() && !valid; i++)  //  valid if at least one number in string
		valid=(aNumber[i]>='0') && (aNumber[i]<='9');
	return valid;
	}

void CSmsClientMtm::DoAddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias)
	{
	if (aAlias.Length())
		{
		HBufC* buf=HBufC::NewLC(aAlias.Length()+aRealAddress.Length()+3);
		TPtr ptr = buf->Des();
		ptr.Copy(aAlias);
		ptr.Append(_L(" "));
		ptr.Append(iRealAddressOpen);
		ptr.Append(aRealAddress);
		ptr.Append(iRealAddressClose);
		iAddresseeList->AppendL(ptr);
		CleanupStack::PopAndDestroy(buf);
		}
	else
		iAddresseeList->AppendL(aRealAddress);
	}

void CSmsClientMtm::AddRecipientL(const TDesC& aRealAddress, const TDesC& aAlias)
	{
	DoAddRecipientL(iSmsHeader, aRealAddress, aAlias);
	}

void CSmsClientMtm::DoAddRecipientL(CSmsHeader* aSmsHeader, const TDesC& aRealAddress, const TDesC& aAlias)
	{
	CSmsNumber* recipient=CSmsNumber::NewL();
	CleanupStack::PushL(recipient);
	if ( aRealAddress.Locate('@') != KErrNotFound )
        {
        CSmsEmailFields* emailFields = NULL;
        if ( aSmsHeader->EmailFields().Length() > 0 )
            {
            emailFields = CSmsEmailFields::NewL( aSmsHeader->EmailFields() );
            }
        else
            {
            emailFields = CSmsEmailFields::NewL();
            }
        CleanupStack::PushL( emailFields );
       
		recipient->SetEmailAddressL(aRealAddress, emailFields, aAlias);
       
        // Clears the CSmsHeaders EmailFields for non Email addresses
        aSmsHeader->SetEmailFieldsL( *emailFields );
        CleanupStack::PopAndDestroy( emailFields );
        }
	else
	    {
	    recipient->SetAddressL(aRealAddress);
	    if (aAlias.Length() > 0)
	    	{	
	        recipient->SetNameL(aAlias);
	    	}
	    }
    aSmsHeader->Recipients().AppendL(recipient);
    CleanupStack::Pop(recipient);
	}

void CSmsClientMtm::ResetHeader()
	{
	if (iSmsHeader)
		iSmsHeader->Recipients().ResetAndDestroy();

	if (iAddresseeList)
		iAddresseeList->Reset();
	}

void CSmsClientMtm::FindInBodyL(const TDesC& aTextToFind, const TMsvPartList& aFoundList, TMsvPartList& aResult)
	{
	CMsvFindText* text=CMsvFindText::NewL();
	CleanupStack::PushL(text);
	if (text->FindRichTextL(aTextToFind,Body(),aFoundList))
		aResult|=KMsvMessagePartBody;
	CleanupStack::PopAndDestroy(text);
	}

void CSmsClientMtm::FindL(const TDesC& aTextToFind, const TMsvPartList aPartList, TMsvPartList& aFoundList)
	{
	
	CMsvFindText* text=CMsvFindText::NewL();
	CleanupStack::PushL(text);

	// Enhance search and sort
  	// The setting variables are accessed by getting an instance to TMsvEnhanceSearchSortUtil  class
  	// This is accessed here by using GetExtensionData() defined in CBaseMtm which refers 
  	// to iExtensionData member variable
 	TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
		
	if(searchsortutil != NULL)
		{
		// For callees other than CMsvSearchsortOpOnHeaderBody class, searchsortutil pointer will be NULL	
		
		TInt index=0;
		TBool foundinname = EFalse;
		TBool foundinaddress = EFalse;

		// Retrieve the search sort setting flags
		TUint32 searchsortsetting=searchsortutil->GetSearchSortSetting();
		
		if(aPartList & KMsvMessagePartTo)
			{
			TInt count=iSmsHeader->Recipients().Count();
			/* 
			Search for the To field in the the SMS header
			First it looks in the Senders name
			if not found, it tries to find it in the senders numbers
			*/
			
			for(TInt i=0;i<count;i++)
				{
				if ((text->FindTextL(aTextToFind,(iSmsHeader->Recipients()).operator[](i)->Name(),aPartList)))
					{
					aFoundList|=KMsvMessagePartTo;
					foundinname = ETrue;
					index = i;
					}
				if(!foundinname)
					{
					if ((text->FindTextL(aTextToFind,(iSmsHeader->Recipients()).operator[](i)->Address(),aPartList)))
						{
						aFoundList|=KMsvMessagePartTo;
						foundinaddress = ETrue;
						index = i;
						}
					}
				}
			}
			// Search for the From field
		else if(aPartList & KMsvMessagePartFrom)
			{
			if ((text->FindTextL(aTextToFind,iSmsHeader->FromAddress(),aPartList)) 
		 	|| (text->FindTextL(aTextToFind,iMsvEntry->Entry().iDetails,aPartList)))
				{
				aFoundList|=KMsvMessagePartFrom;
				}
			}
			// Search for the Subject
		else if (aPartList & KMsvMessagePartSubject)
			{
			if (text->FindTextL(aTextToFind,iMsvEntry->Entry().iDescription,aPartList))
				{
				aFoundList|=KMsvMessagePartDescription;
				}
			}
		/* Copy the sort data if sorting is specified.
		 The operations being performed could be only be sort or it could be search and sort
		 If the operation is search and sort than copy the sort data only if the
		 search operation succeeded	*/
					
		if ((aPartList & EMessagePartSort ) || ((aPartList & EMessagePartSearchSort) && (aPartList & EMessagePartLastQueryOption) && aFoundList)) 
			{
			if (aPartList & EMessagePartToSort )
   				{
   				if(foundinname) // Copy the sort data form the senders name
   					{
   					TPtrC ptr((iSmsHeader->Recipients()).operator[](index)->Name());
   					SetExtensionData(&ptr);
   					}
   				if(foundinaddress) // Copy data from senders Address
   					{
   					TPtrC ptr((iSmsHeader->Recipients()).operator[](index)->Address());	
   					SetExtensionData(&ptr);
   					}
   				}
			else if(aPartList & EMessagePartFromSort )
   				{
   				SetExtensionData((TAny*)(iSmsHeader->FromAddress().Ptr()));
   				}
   			else if(aPartList & EMessagePartSubjectSort )
   				{
   				SetExtensionData((TAny*)&iMsvEntry->Entry().iDescription);
				}
			else 
   				// Fix for DEF124605. If a different sort field is specified than
   				// sets setting flag to EMessagePartInvalidSortField
   				{
   				searchsortutil->SetSearchSortSetting(EMessagePartInvalidSortField);
   				}
			}
		}
	else
		{
	
		// Implementation prior to PREQ1667
		if (aPartList & KMsvMessagePartRecipient) 		
			{
			TInt count=iSmsHeader->Recipients().Count();
	
			for(TInt i=0;i<count;i++)
				{
				if ((text->FindTextL(aTextToFind,(iSmsHeader->Recipients()).operator[](i)->Name(),aPartList))
			 	|| (text->FindTextL(aTextToFind,(iSmsHeader->Recipients()).operator[](i)->Address(),aPartList)))
					{
					aFoundList|=KMsvMessagePartRecipient;
					}
				}
			}
		else if(aPartList & KMsvMessagePartOriginator)
			{
			if((text->FindTextL(aTextToFind,iSmsHeader->FromAddress(),aPartList)) 
			 || (text->FindTextL(aTextToFind,iMsvEntry->Entry().iDetails,aPartList)))
				{
				aFoundList|=KMsvMessagePartOriginator;
				}
			}
		else if (aPartList & KMsvMessagePartDescription)
			{
			if (text->FindTextL(aTextToFind,iMsvEntry->Entry().iDescription,aPartList))
				{
				aFoundList|=KMsvMessagePartDescription;
				}
			}
		}
	CleanupStack::PopAndDestroy(text);
	}


TMsvPartList CSmsClientMtm::DoFindL(const TDesC& aTextToFind, TMsvPartList aPartList)
	{
	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));

	TMsvPartList retList = 0;

	// Enhance search and sort
  	// The setting variables are accessed by getting an instance to TMsvEnhanceSearchSortUtil  class
  	// This is accessed here by using GetExtensionData() defined in CBaseMtm which refers 
  	// to iExtensionData member variable
	
 	TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
	
	// For callees other than CMsvSearchsortOpOnHeaderBody class, searchsortutil pointer will be NULL	
	if(searchsortutil !=NULL)
		{
		TUint32 searchsortsetting=searchsortutil->GetSearchSortSetting();
		if ((aPartList & KMsvMessagePartBody) && !(searchsortsetting & EMessagePartBodyLoaded))
			{
			CMsvStore* msvstore=iMsvEntry->ReadStoreL();
			CleanupStack::PushL(msvstore);
			msvstore->RestoreBodyTextL(Body());			
			FindInBodyL(aTextToFind,aPartList,retList); // Find in body	
			CleanupStack::PopAndDestroy(msvstore);
			
			// The message body is loaded.Set the setting variable to specify that the body is loaded
			// If the next search is also on body, than it wont be loaded next time around
			
			searchsortutil->SetSearchSortSetting(EMessagePartBodyLoaded); 
			}
		else if( (aPartList & KMsvMessagePartTo ) || (aPartList & KMsvMessagePartFrom ) || (aPartList & KMsvMessagePartSubject))
			{
			// Search for To, From and Subject
			FindL(aTextToFind,aPartList,retList); // Find in header	
			}
		}
	else
		{
		// Search implementation Prior to PREQ1667
		if( (aPartList & KMsvMessagePartRecipient) || (aPartList & KMsvMessagePartOriginator) || (aPartList & KMsvMessagePartDescription))
			{
			FindL(aTextToFind,aPartList,retList); // Find in header	
			}
		else if(aPartList & KMsvMessagePartBody)
			{
			FindInBodyL(aTextToFind,aPartList,retList); // Find in body	
			}

		}
	return retList;
	}

/*
Used by ForwardL() and ReplyL() for initialising the contents of the new message
*/
void CSmsClientMtm::BasicReplyForwardEntry(TMsvEntry& aNewEntry) const
	{
	aNewEntry.iMtm = KUidMsgTypeSMS;
 	// Fix for DEF000924: Need to be able to send/cancel an sms while another is being sent
 	aNewEntry.iRelatedId = Entry().Entry().iServiceId;
 	aNewEntry.iServiceId = KMsvLocalServiceIndexEntryId;
 	// End of fix
 	aNewEntry.iType = KUidMsvMessageEntry;
	aNewEntry.iDate.UniversalTime();
	}

/*
 *	Methods from CBaseMtm
 */

/** 
Commits cached changes to the current message context to the message store.

The current context must be set to a message entry with type KUidMsvServiceEntryValue,
KUidMsvFolderEntryValue or KUidMsvMessageEntryValue. If the current context is
not set, or is set to an entry with a type that is not supported by this function
then a panic will occur.

Both the SMS service ID and SMS service settings must have been set or a panic
will occur. The CSmsClientMtm::RestoreServiceAndSettingsL API can restore both
of these items.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	4
The type of the current context is not supported.

@panic	SMCM	11
The SMS Service settings have not been set (debug only).

@panic	SMCM	12
The ID for SMS Service has not been set (debug only).

@see	CSmsClientMtm::RestoreServiceAndSettingsL
*/
void CSmsClientMtm::SaveMessageL()
	{
	__ASSERT_DEBUG(iMsvEntry,Panic(ESmscEntryNotSet));

	CMsvStore* msvstore=iMsvEntry->EditStoreL();
	CleanupStack::PushL(msvstore);
	
	TMsvEntry entry(iMsvEntry->Entry());
	TBool entryChanged = EFalse;
	
	switch (iMsvEntry->Entry().iType.iUid)
		{
		case KUidMsvServiceEntryValue:
			{
			__ASSERT_DEBUG(iServiceSettings, Panic(ESmscSettingsNotSet));
			__ASSERT_DEBUG(iServiceId, Panic(ESmscServiceIdNotSet));

			CSmsAccount* account = CSmsAccount::NewLC();
			//just v2
			account->SaveSettingsL(*iServiceSettings);
			CleanupStack::PopAndDestroy(account);    
			break;
			}
		case KUidMsvFolderEntryValue:
			{
			break;
			}
		case KUidMsvMessageEntryValue:
			{
			// since CreateMessageL creates message as in preparation and non-visible, the SaveMessageL
			// must now make it visible and not in preparation

			// If iDetails is empty, set it to be the first recipient in the header
			if ((0 >= entry.iDetails.Length()) && (iSmsHeader->Recipients().Count() > 0))
				{
				CSmsNumber* recipient = iSmsHeader->Recipients().At(0);
				if(recipient->Name().Length() > 0)
					entry.iDetails.Set(recipient->Name());
				else
					entry.iDetails.Set(recipient->Address());
				}
			
			// If iDescription is empty, set it to be the first part of the body text
			if(Body().DocumentLength()>0 && 0 >= entry.iDescription.Length())
				{
				entry.iDescription.Set(Body().Read(0,iServiceSettings->DescriptionLength()));	
				}
			entry.SetVisible(ETrue);
			entry.SetInPreparation(EFalse);	
			entry.SetMultipleRecipients(iSmsHeader->Recipients().Count() > 1);
			entryChanged = ETrue;

			iSmsHeader->StoreL(*msvstore);
			StoreBodyL(*msvstore);
			break;
			}
		default:
			Panic(ESmscUnrecognizedEntryType);
		}
	msvstore->CommitL();

 	// Set the iSize member of the TMsvEntry
 	if (iMsvEntry->HasStoreL())
 		{
 		const TInt size = msvstore->SizeL();		
 		if (entry.iSize != size)
 			{
 			entry.iSize = size; 			
 			entryChanged = ETrue;
 			}
 		}

	CleanupStack::PopAndDestroy(msvstore);

	if ( entryChanged )
		{
		// The entry info has been updated - commit to the store.		
		iMsvEntry->ChangeL(entry);
		}
	}

/**
Loads the cache with the message data for the current context.

The current context must be set to a message entry with type KUidMsvServiceEntryValue,
KUidMsvFolderEntryValue or KUidMsvMessageEntryValue. If the current context is
not set, or is set to an entry with a type that is not supported by this function
then a panic will occur.

If the current context is of type KUidMsvServiceEntryValue then the SMS service
settings are restore from the context. The SMS service settings can be accessed
using the overloaded CSmsClientMtm::ServiceSettings APIs .

If the current context is of type KUidMsvMessageEntryValue then the SMS message
object is restore from the context. This can be accessed using the overloaded 
CSmsClientMtm::SmsHeader APIs.

If the current context is of type KUidMsvFolderEntryValue then this function 
does nothing.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	4
The entry type of the current context was not recognised.

@see	CSmsClientMtm::SmsHeader
@see	CSmsClientMtm::ServiceSettings
*/
void CSmsClientMtm::LoadMessageL()
	{
	__ASSERT_DEBUG(iMsvEntry,Panic(ESmscEntryNotSet));

	CMsvStore* msvstore=iMsvEntry->ReadStoreL();
	CleanupStack::PushL(msvstore);
	switch (iMsvEntry->Entry().iType.iUid)
		{
		case KUidMsvServiceEntryValue:
			{
			RestoreServiceAndSettingsL();
			break;
			}
		case KUidMsvFolderEntryValue:
			{
			break;
			}
		case KUidMsvMessageEntryValue:
			{
			ResetHeader();
			Body().Reset();
			
			CSmsHeader* smsheader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,Body());
			CleanupStack::PushL(smsheader);
			smsheader->RestoreL(*msvstore);
			// the following part inserts the recipients from header also to the smsclient
			// the recipients are restored inside the csmsheader already
			if (smsheader->Type()==CSmsPDU::ESmsSubmit)
				{
				for (TInt i=0; i<smsheader->Recipients().Count(); ++i)
					{
					CSmsNumber* recipient=smsheader->Recipients()[i];
					DoAddAddresseeL(recipient->Address(),recipient->Name());
					}
				}
				
		// Get a reference to TMsvEnhanceSearchSortUtil  instance set by CMsvSearchsortOpOnHeaderBody class
		// If advanced search and sort is being performed than do not load the body here
		// For API's other than CMsvSearchsortOpOnHeaderBody-> FindInHeaderBodyL(), a call to LoadMessageL()
		// loads the body.
	
			TMsvEnhanceSearchSortUtil* searchsortutil = (TMsvEnhanceSearchSortUtil*)(GetExtensionData());
			if ( searchsortutil == NULL )
				{
				msvstore->RestoreBodyTextL(Body()); // Restore Body text
				}
			CleanupStack::Pop(smsheader); 
			delete iSmsHeader;
			iSmsHeader=smsheader;
			break;
			}
		default:
			Panic(ESmscUnrecognizedEntryType);
		}
	CleanupStack::PopAndDestroy(msvstore);
	}
/** 
Validates the current message context.

The addresses for the message are checked to be well-formed SMS addresses.

The current context must be set. If the current context is not set then a panic
will occur.

@param	aPartList
Indicates the message parts for which the validation has been requested.

@return
If valid KErrNone is returned. If invalid, the invalid parts of the message are
identified. The return error value is the bitmask of the TMsvPartList IDs for
each invalid part

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	6
The addressee list and recipient list do not match (debug only).
*/
TMsvPartList CSmsClientMtm::ValidateMessage(TMsvPartList aPartList)
	{
	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));
	__ASSERT_DEBUG(iSmsHeader->Recipients().Count()==iAddresseeList->Count(), Panic(ESmscRecpAddresseeMiscount));

	TMsvPartList partlist=TMsvPartList(0);
	if (aPartList&KMsvMessagePartRecipient)
		if (!ValidRecipients())  
			partlist|=KMsvMessagePartRecipient;

	return partlist;
	}

/**
Searches the specified message part(s) for the plain-text version of the text 
to be found.

@param	aTextToFind
The plain-text version of the text to be found. 

@param	aPartList
Indicates the message parts which should be searched. 

@return
If the text was not found, or searching is unsupported, 0. If the text was found,
a bitmask of the TMsvPartList IDs for each part in which the text was present.
*/
TMsvPartList CSmsClientMtm::Find(const TDesC& aTextToFind,TMsvPartList aPartList)
	{
	TMsvPartList retList = 0;
	TRAP_IGNORE(retList = DoFindL(aTextToFind, aPartList));
	return retList;
	}

/**
Creates a new SMS message as a reply to the current message context.

The SMS in the current context must be of type SMS-DELIVER, indicating a mobile
terminated message.

The current context must be set to a message entry with type KUidMsvMessageEntryValue.
If the current context is not set, or is set to an entry not of type
KUidMsvMessageEntryValuethen a panic will occur.

The new SMS message has the recipient set to the sender of the message in the
current context. It has a type SMS-SUBMIT, indicating a mobile originated meesage.
If the SMS service settings CSmsSettings::ReplyQuoted is true then the reply
message will contain the body of the original message, including any EMS elements.

@param	aDestination
The message entry (folder) under which to create the new message.

@param	aPartList
Ignored.

@param	aCompletionStatus
The request status to be completed when the operation has finished.

@return
An operation object. If the relpy email is successful created, the operation will
complete with KErrNone. The operation's progress information is a TPckg<TMsvId>,
containing the TMsvId of the new SMS message. If the reply message fails to be 
created, the operation completes with the relevant error code.

@leave	KErrNotSupported
The current context does not have a PDU type of SMS-DELIVER.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	2
The current context was not of type KUidMsvMessageEntry (debug only).
*/
CMsvOperation* CSmsClientMtm::ReplyL(TMsvId aDestination,TMsvPartList /*aPartList*/, TRequestStatus& aCompletionStatus)
	{
	LoadMessageL();

	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvMessageEntry, Panic(ESmscWrongContextType));

	if (iSmsHeader->Type()!=CSmsPDU::ESmsDeliver)
		{
		User::Leave(KErrNotSupported);
		}

	CRichText* body = CRichText::NewL(iParaFormatLayer, iCharFormatLayer,CEditableText::EFlatStorage,KSmcmRichTextConstant);
	CleanupStack::PushL(body);

	CSmsHeader* smsheader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,Body());
	CleanupStack::PushL(smsheader);

	// Check to see if we are copying the contents of the original message
	if(iServiceSettings->ReplyQuoted() && iMsvEntry->Entry().iBioType==NULL)
		{
		// Copy original rich text
		body->AppendTakingSolePictureOwnershipL(Body());
		// Copy ems elements from original message to new message
		iSmsHeader->Message().CopyEmsElementsL(smsheader->Message());
		}
		
	if( iSmsHeader->EmailFields().Length() > 0 )
		{
		// The context message is an email message - set the email fields for
		// the new message.
		smsheader->SetReplyEmailFieldsL(iSmsHeader->EmailFields(), *iEmailReplySubjectFormat);
		}
	else
		{
		// Get the reply to address if there is one else the from address.
		DoAddRecipientL(smsheader, iSmsHeader->ReplyAddressL(), iMsvEntry->Entry().iDetails);
		}

	if(iSmsHeader->Deliver().ReplyPath())
		{
		smsheader->Submit().SetServiceCenterAddressL(iSmsHeader->Deliver().ServiceCenterAddress());
		smsheader->SetReplyPathProvided(ETrue);
		}
	else smsheader->SetReplyPathProvided(EFalse);
	
	TMsvEntry entry;
	BasicReplyForwardEntry(entry);

	CMsvOperation* operation = CreateNewEntryL(entry, aDestination, *smsheader, *body, aCompletionStatus);

	CleanupStack::PopAndDestroy(2, body);
	return operation;
	}

/** 
Creates a SMS forwarded message from the current message context.

The SMS in the current context must be of type SMS-DELIVER, a mobile terminated 
message, or SMS-SUBMIT, a mobile originated message. 

The current context must be set to a message entry with type KUidMsvMessageEntryValue.
If the current context is not set, or is set to an entry not of type
KUidMsvMessageEntryValuethen a panic will occur.

The new SMS message will have type SMS-SUBMIT, a mobile originated message. The 
recipient is left blank. The contents that are copied to the new message include
the message body, iBioType and description. Also, any EMS components are also
copied.

@param	aDestination
The message entry (folder) under which to create the new message.

@param	aPartList
Ignored.

@param	aCompletionStatus
The request status to be completed when the operation has finished.

@return
An operation object. If the forward email is successful created, the operation
will complete with KErrNone. The operation's progress information is a 
TPckg<TMsvId>, containing the TMsvId of the new SMS message. If the forward 
message fails to be created, the operation completes with the relevant error code.

@leave	KErrNotSupported
The current context has a PDU type that is neither SMS-SUBMIT nor SMS-DELIVER.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	2
The current context was not of type KUidMsvMessageEntry (debug only).
*/
CMsvOperation* CSmsClientMtm::ForwardL(TMsvId aDestination,TMsvPartList /*aPartList*/, TRequestStatus& aCompletionStatus)
	{
	LoadMessageL();

	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvMessageEntry, Panic(ESmscWrongContextType));

	// Only forwarding of Submitted or Delivered Messages is supported - ie only
	// SMS messages that either been sent or received. 
	CSmsPDU::TSmsPDUType smsType = iSmsHeader->Type();
	if (smsType!=CSmsPDU::ESmsSubmit && smsType!=CSmsPDU::ESmsDeliver)
		User::Leave(KErrNotSupported);

	CRichText* body = CRichText::NewL(iParaFormatLayer, iCharFormatLayer,CEditableText::EFlatStorage,KSmcmRichTextConstant);
	CleanupStack::PushL(body);
	body->AppendTakingSolePictureOwnershipL(Body());

	CSmsHeader* smsheader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,Body());
	CleanupStack::PushL(smsheader);

	smsheader->SetReplyPathProvided(EFalse);

	// Copy any EMS elements
	iSmsHeader->Message().CopyEmsElementsL(smsheader->Message());
	
	if( iSmsHeader->EmailFields().Length() > 0 )
		{
		// The context message is an email message - set the email fields for
		// the new message.
		smsheader->SetForwardEmailFieldsL(iSmsHeader->EmailFields(), *iEmailForwardSubjectFormat);
		}
	
	// Set the contents of the new TMsvEntry
	const TMsvEntry& originalEntry = Entry().Entry();
	TMsvEntry entry;
	BasicReplyForwardEntry(entry);
	entry.iBioType = originalEntry.iBioType; // added since 6.1
	entry.iDescription.Set(originalEntry.iDescription); // added since 6.1		

	CMsvOperation* operation = CreateNewEntryL(entry, aDestination,*smsheader,*body,aCompletionStatus);

	CleanupStack::PopAndDestroy(2, body);
	return operation;
	}

/**
Adds an addressee to the current context.

The current context must be an SMS messge of type SMS-SUBMIT, indicating a 
mobile originated message. The address is added to the recipient list for the 
message.

For SMS an address is a telephone number. There is no validation done on the 
input argument aRealAddress to ensure that it is a valid telephone number.

The TSmsUtilities::GetDetails API can be used prior to this API to find the if
a unique alias exists in the contacts database for this particular telephone 
number.

The current context must be set to a message entry with type KUidMsvMessageEntryValue.
If the current context is set to an entry not of type KUidMsvMessageEntryValuethen
a panic will occur.

The SMS MTM maintains an addressee list that matches the recipient list in the 
SMS message header object. If the two list cannot be maintained together then
this function will leave and both lists are unchanged.

@param	aRealAddress
A string representing the address to be added to the recipient list for the
current context.

@leave	KErrNotSupported
The current contxt PDU type is not SMS-SUBMIT.

@panic	SMCM	2
The current context was not of type KUidMsvMessageEntry (debug only).

@panic	SMCM	6
The addressee list and recipient list do not match (debug only).
*/
void CSmsClientMtm::AddAddresseeL(const TDesC& aRealAddress)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvMessageEntry, Panic(ESmscWrongContextType));
	if (iSmsHeader->Type()!=CSmsPDU::ESmsSubmit)
		User::Leave(KErrNotSupported);
	__ASSERT_DEBUG(iAddresseeList->Count()==iSmsHeader->Recipients().Count(),Panic(ESmscRecpAddresseeMiscount));
	TPtr ptr(NULL,0);
	DoAddAddresseeL(aRealAddress,ptr);
	TRAPD(ret,AddRecipientL(aRealAddress,ptr));
	if (ret!=KErrNone)
		{
		iAddresseeList->Delete(iAddresseeList->Count()-1);
		User::Leave(ret);
		}
	}

/** 
Adds an addressee with an alias to the current context.

The SMS message in the current context must be of type SMS-SUBMIT, indicating a
mobile originated message. The address is added to the recipient list for the 
message.

For SMS an address is a telephone number. There is no validation done on the 
input argument aRealAddress to ensure that it is a valid telephone number.

The TSmsUtilities::GetDetails API can be used prior to this API to find the if
a unique alias exists in the contacts database for this particular telephone 
number.

The current context must be set to a message entry with type KUidMsvMessageEntryValue.
If the current context is set to an entry not of type KUidMsvMessageEntryValuethen
a panic will occur.

The SMS MTM maintains an addressee list that matches the recipient list in the 
SMS message header object. If the two list cannot be maintained together then
this function will leave and both lists are unchanged.

@param	aRealAddress
A string representing the address to be added to the recipient list for the 
current message.

@param	aAlias
The alias information for the address.

@leave	KErrNotSupported
The current contxt PDU type is not SMS-SUBMIT.

@panic	SMCM	2
The current context was not of type KUidMsvMessageEntry (debug only).

@panic	SMCM	6
The addressee list and recipient list do not match (debug only).
*/
void CSmsClientMtm::AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvMessageEntry, Panic(ESmscWrongContextType));
	if (iSmsHeader->Type()!=CSmsPDU::ESmsSubmit)
		User::Leave(KErrNotSupported);
	__ASSERT_DEBUG(iAddresseeList->Count()==iSmsHeader->Recipients().Count(),Panic(ESmscRecpAddresseeMiscount));
	DoAddAddresseeL(aRealAddress,aAlias);
	TRAPD(ret,AddRecipientL(aRealAddress,aAlias));
	if (ret!=KErrNone)
		{
		iAddresseeList->Delete(iAddresseeList->Count()-1);
		User::Leave(ret);
		}
	}

/** 
Removes a recipient from the current address list.

The recipient to be removed is specified by its index in the list of recipients.
This function can only be used when the current context is a mobile-originated
message (is a SMS-SUBMIT type message).

The index must be valid or a panic will occur. A valid index is not negative and
does not exceed the upper bounds of the list.

The current context must be set to a message entry with type KUidMsvMessageEntryValue.
If the current context is set to an entry not of type KUidMsvMessageEntryValuethen
a panic will occur.

The SMS MTM maintains an addressee list that matches the recipient list in the 
SMS message header object. The address is removed from both lists.

@param aIndex
The index of recipient to be removed.

@panic	SMCM	0
The current contxt PDU type is not SMS-SUBMIT (debug only).

@panic	SMCM	2
The current context was not a mesasge type context (debug only).

@panic	SMCM	5
The specified index exceeds the recipient list range (debug only).

@panic	SMCM	6
The addressee list and recipient list do not match (debug only).
*/
void CSmsClientMtm::RemoveAddressee(TInt aIndex)
	{
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvMessageEntry, Panic(ESmscWrongContextType));
	__ASSERT_DEBUG(iSmsHeader->Type() == CSmsPDU::ESmsSubmit, Panic(ESmutPanicUnsupportedMsgType));
	__ASSERT_DEBUG((aIndex>=0) && (aIndex<iAddresseeList->Count()),Panic(ESmscAddresseeIndexOutOfRange));
	__ASSERT_DEBUG(iAddresseeList->Count()==iSmsHeader->Recipients().Count(),Panic(ESmscRecpAddresseeMiscount));
	delete iSmsHeader->Recipients()[aIndex];
	iSmsHeader->Recipients().Delete(aIndex);
	iAddresseeList->Delete(aIndex);
	}

/**
Queries if the MTM supports the capability specified by the supplied UID.

@param	aCapability
The UID of capability to be queried.

@param	aResponse
An output argument for the response value. The format of the response varies
according to the capability. 

@return
A value of KErrNone if the specified capability is recognised and a response
is returned. KErrNotSupported indicates that the capability is not recognised.

@see	mtmuids.h
*/
TInt CSmsClientMtm::QueryCapability(TUid aCapability, TInt& aResponse)
	{
	TInt error=KErrNone;
	aResponse=ETrue;
	switch (aCapability.iUid)
		{
		case KUidMtmQueryMaxBodySizeValue:
			{
			aResponse=KSmcmMaxTotalMsgSize;
			break;
			}
		case KUidMtmQueryMaxTotalMsgSizeValue:
			{
			aResponse = KSmcmMaxMessageNumber*KSmcmMaxCharsInMessageConcatenated7Bit;
			break;
			}
		case KUidMtmQuerySupportedBodyValue:
			{
			aResponse = KMtm7BitBody + KMtm8BitBody + KMtm16BitBody;
			break;
			}
		case KUidMtmQueryCanSendMsgValue:
		case KUidMtmQueryCanReceiveMsgValue:
		case KUidMtmQuerySupportsBioMsgValue:
		case KUidMtmQuerySupportsSchedulingValue:
		case KUidMtmQuerySendAsMessageSendSupportValue:
			break;
		case KUidMtmQueryMaxRecipientCountValue:
			{
			aResponse=KErrNotFound;	// not limited
			break;
			}
		case KUidMsvMtmQueryEditorUidValue:
			{
			aResponse=KUidMsgSmsEditorAppVal;
			break;
			}
		case KUidMtmQuerySupportAttachmentsValue:
		case KUidMtmQueryOffLineAllowedValue:
		case KUidMtmQuerySupportSubjectValue:
		case KUidMtmQuerySendAsRequiresRenderedImageValue:
		case KUidMtmQuerySendAsRenderingUidValue:
		case KUidMtmQuerySupportsFolderValue:
		case KUidMtmQuerySupportsRecipientTypeValue:
		default:
			error=KErrNotSupported;
		}
	return error;
	}

/**
Unused for SMS.

This function should not be used as it will cause a panic.

@param aFunctionId
Unused.

@param aSelection
Unused.

@param aParameter
Unused. 

@panic	SMCM	3
This command is not supported in SMS and should not be used.

@internalComponent
*/
void CSmsClientMtm::InvokeSyncFunctionL(TInt /*aFunctionId*/,const CMsvEntrySelection& /*aSelection*/, TDes8& /*aParameter*/)
	{
	Panic(ESmscPanicUnexpectedCommand);
	}

/**
Invokes asynchronous SMS-specific operations.

The specified operation is performed by the SMS server MTM.

If the requested command is not supported then a panic will occur. The list of
supported commands is given by TSmsMtmCommand.

The current context must be set. If the current context is not set then a panic
will occur.

@param	aFunctionId
The ID of the operation to perform. The specific operations are defined by the
TSmsMtmCommand enumeration.

@param	aSelection
A selection of messages, the use of which is specific to the selected operation.

@param	aParameter
A packaged object, the use of which is specific to the selected operation.

@param	aCompletionStatus
The request status to be notified when the operation completes. 

@return
An object encapsulating the requested operation if the specified operation was
successfully started. If the operation failed to be started an completing 
operation is returned with its status set to the relevant error code.

@see	TSmsMtmCommand 

@panic	SMCM	1
The message entry has not been set (debug only).

@panic	SMCM	3
The specified operation is not supported in SMS.
*/
CMsvOperation* CSmsClientMtm::InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter, TRequestStatus& aCompletionStatus)
	{
	__ASSERT_DEBUG(iMsvEntry,Panic(ESmscEntryNotSet));
	switch (aFunctionId)
		{
		case KMTMStandardFunctionsSendMessage:
			// perform a regular send with standardised progress information for SendAs2
			return CSmsSendMessageOperation::NewL(Session(), aSelection, aParameter, aCompletionStatus);
		case ESmsMtmCommandScheduleCopy:
		case ESmsMtmCommandScheduleMove:
		case ESmsMtmCommandDeleteSchedule:
		case ESmsMtmCommandCheckSchedule:
		case ESmsMtmCommandSendScheduledCopy:
		case ESmsMtmCommandSendScheduledMove:
		case ESmsMtmCommandEnumeratePhoneStores:
		case ESmsMtmCommandDeleteFromPhoneStore:
		case ESmsMtmCommandCopyFromPhoneStore:
		case ESmsMtmCommandMoveFromPhoneStore:
		case ESmsMtmCommandCopyToPhoneStore:
		case ESmsMtmCommandMoveToPhoneStore:
			break;
		default:
			Panic(ESmscPanicUnexpectedCommand);
		}
	return Session().TransferCommandL(aSelection,aFunctionId,(TDesC8&)aParameter, aCompletionStatus);
	}


/**
Informs the MTM that the context's BIO field is being changed.

The change is as a result of a call to CSendAs::SetBioTypeL(). This function is
used by CSendAs::CreateMessageL to create a new outgoing SMS message

@param	aBioTypeUid
The new value for the BIO field.

@panic	SMCM	10
The BIO field contains the wrong value (debug only).
*/
EXPORT_C void CSmsClientMtm::BioTypeChangedL(TUid aBioTypeUid)
	{
	// Looks up the default send bearer and sets the
	// character encoding of the Submit() accordingly.
	RestoreServiceAndSettingsL();

	CBIODatabase* bdb = CBIODatabase::NewLC(Session().FileSession());
	TBioMsgId bioMsgId;
	bdb->GetDefaultSendBearerL(aBioTypeUid, bioMsgId);

	CSmsSettings* settings = CSmsSettings::NewL();
	CleanupStack::PushL(settings);

	settings->CopyL(*iServiceSettings);

	iSmsHeader->SetBioMsgIdType(bioMsgId.iType);

	TSmsDataCodingScheme::TSmsAlphabet alpha = TSmsDataCodingScheme::ESmsAlphabet8Bit;

	// Set the SMS Data Encoding Scheme
	switch (bioMsgId.iType)
		{
		case EBioMsgIdNbs:
			{
			switch (bioMsgId.iCharacterSet.iUid)
				{
//				case KCharacterSetIdentifierIso88591:
				case KCharacterSetIdentifierCodePage1252:
					{
					alpha = TSmsDataCodingScheme::ESmsAlphabet8Bit;
					break;
					}
				case KCharacterSetIdentifierSms7Bit:
					{
					alpha = TSmsDataCodingScheme::ESmsAlphabet7Bit;
					break;
					}
				default:
					{
					alpha = TSmsDataCodingScheme::ESmsAlphabetUCS2;
					break;
					}
				}
			break;
			}
		case EBioMsgIdWap:
		case EBioMsgIdWapSecure:
		default:
			{
			//ESmsAlphabet8Bit;
			break;
			}
		};

	settings->SetCharacterSet(alpha);
	iSmsHeader->SetSmsSettingsL(*settings);

	// service centre was not being set 
	TInt defaultSC = iServiceSettings->DefaultServiceCenter();
	if(defaultSC > -1)
		{
		iSmsHeader->SetServiceCenterAddressL( iServiceSettings->GetServiceCenter(defaultSC).Address() );	
		}

	__ASSERT_DEBUG(iSmsHeader->BioMsgIdType() == bioMsgId.iType, Panic(ESmscBioMsgIdTypeError));

	CleanupStack::PopAndDestroy(2, bdb);
	}

/** 
Creates a new outgoing SMS message entry as a child of the current context.

Used by CSendAs::CreateMessageL to create an outgoing SMS message.

@param	aServiceId
The ID of the service to own the entry (not used).

@see	CSendAs::CreateMessageL
*/
EXPORT_C void CSmsClientMtm::CreateMessageL(TMsvId /*aServiceId*/)
	{
	RestoreServiceAndSettingsL();

 	// Fix for DEF000924: Need to be able to send/cancel an sms while another 
	// is being sent create an invisible blank entry.
 	TMsvEntry entry;
 	entry.iType = KUidMsvMessageEntry;
 	entry.iRelatedId = iServiceId;
 	entry.iServiceId = KMsvLocalServiceIndexEntryId;
 	entry.iMtm = Type();
 	entry.SetVisible(EFalse);
 	entry.SetInPreparation(ETrue);
 	
 	// Fix for DEF061945: Need to initialise the iDate member of the new entry
 	entry.iDate.UniversalTime();
 	// store entry in folder
 	iMsvEntry->CreateL(entry);
 	SwitchCurrentEntryL(entry.Id());
 	// End of fix

	delete iSmsHeader;
	iSmsHeader=NULL;
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,Body());
	iSmsHeader->SetSmsSettingsL(*iServiceSettings);

	// service centre was not being set 
	TInt defaultSC = iServiceSettings->DefaultServiceCenter();
	if(defaultSC > -1)
		{
		if (iServiceSettings->ServiceCenterCount() > defaultSC )
			{
			iSmsHeader->SetServiceCenterAddressL( iServiceSettings->GetServiceCenter(defaultSC).Address() );	
			}
		}
 	}



/** 
Creates a new outgoing SMS message entry as a child of the current context.
Used by CSendAs::CreateMessageL to create an outgoing SMS message.

@param	
The entry with the required fields set.

*/
EXPORT_C void CSmsClientMtm::CreateMessageL(TMsvEntry& aEntry)
	{
	RestoreServiceAndSettingsL();

 	aEntry.SetVisible(EFalse);
 	aEntry.SetInPreparation(ETrue);
 	aEntry.iDate.UniversalTime();
 	
 	// store entry in folder
 	iMsvEntry->CreateL(aEntry);
 	SwitchCurrentEntryL(aEntry.Id());
 	// End of fix

	delete iSmsHeader;
	iSmsHeader=NULL;
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,Body());
	iSmsHeader->SetSmsSettingsL(*iServiceSettings);

	// service centre was not being set 
	TInt defaultSC = iServiceSettings->DefaultServiceCenter();
	if(defaultSC > -1)
		{
		if (iServiceSettings->ServiceCenterCount() > defaultSC )
			{
			iSmsHeader->SetServiceCenterAddressL( iServiceSettings->GetServiceCenter(defaultSC).Address() );	
			}
		}
 	}

/**
Resets the current SMS message.

The recipient list in the SMS message header object is emptied. As the SMS MTM 
maintains an addressee list that matches the recipient list in the SMS message 
header object, this addressee list is also emptied.
*/
void CSmsClientMtm::ContextEntrySwitched()
	{
	ResetHeader();
	}

/** 
This method is from CBaseMtm, The default implementation is available in CBaseMtm. 
The implementation of this function assumes that the new service for setting the 
charset encoding value for a SMS message is supported.
TAny* is equivalent to void*.
@param	a0				 			The collective parameters of TAny*
@param	a1				 			The collective parameters of TAny*,Charset encoding value is actually extracted from a1.
@param aExtensionId 	 			Uid of the extension interface.
@return KErrNone					If charset is changed successfully in SMS settings for a message.
@return KErrExtensionNotSupported 	If the message is other than SMS.					
@return Other 			 			Standard system-wide error codes.
*/

EXPORT_C TInt CSmsClientMtm::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	TInt ret=KErrNone;
	switch(aExtensionId)
		{
		case KUIDCharacterSet:
			{
	    	TSmsDataCodingScheme::TSmsAlphabet tsmsAlphabet = (TSmsDataCodingScheme::TSmsAlphabet)(TUint)*(TUint*)a1;
	    	TRAP(ret,SetMessageCharacterSetL(tsmsAlphabet));
			return ret;
			}
		default:
			{
			// Chain to base class
			return CBaseMtm::Extension_(aExtensionId, a0, a1);
			}
		}
	}

/**
This is the actual functionality for setting the character encoding value other than 7-bit ASCII.
TSmsDataCodingScheme is used for collecting the encoding value for setting the charset for a message.
This setting is for per-message basis.
@param aTsmsAlphabet 	 			Enum of Sms datacoding scheme for the character encoding value.
@return void.
*/
   
void CSmsClientMtm::SetMessageCharacterSetL(TSmsDataCodingScheme::TSmsAlphabet aTsmsAlphabet)
	{
	CSmsSettings* settings = CSmsSettings::NewL();
	CleanupStack::PushL(settings);

	settings->CopyL(*iServiceSettings);
	settings->SetCharacterSet(aTsmsAlphabet);
	iSmsHeader->SetSmsSettingsL(*settings);
	CleanupStack::PopAndDestroy(settings);
	}

/** 
Commits cached changes to the current message context to the message store.

The current context must be set to a message entry with type KUidMsvServiceEntryValue,
KUidMsvFolderEntryValue or KUidMsvMessageEntryValue. If the current context is
not set, or is set to an entry with a type that is not supported by this function
then a panic will occur.

Both the SMS service ID and SMS service settings must have been set or a panic
will occur. The CSmsClientMtm::RestoreServiceAndSettingsL API can restore both
of these items.

@panic  SMCM    1
The current context has not been set (debug only).

@panic  SMCM    4
The type of the current context is not supported.

@panic  SMCM    11
The SMS Service settings have not been set (debug only).

@panic  SMCM    12
The ID for SMS Service has not been set (debug only).

@see    CSmsClientMtm::RestoreServiceAndSettingsL
*/
EXPORT_C void CSmsClientMtm::SaveMessageL(CMsvStore& aEditStore, TMsvEntry& aEntry)
    {
    switch (aEntry.iType.iUid)
        {
        case KUidMsvServiceEntryValue:
            {
            __ASSERT_DEBUG(iServiceSettings, Panic(ESmscSettingsNotSet));
            __ASSERT_DEBUG(iServiceId, Panic(ESmscServiceIdNotSet));
            CSmsAccount* account = CSmsAccount::NewLC();
            account->SaveSettingsL(*iServiceSettings);
            CleanupStack::PopAndDestroy(account);    
            break;
            }
        case KUidMsvFolderEntryValue:
            {
            break;
            }
        case KUidMsvMessageEntryValue:
            {
            if ((0 >= aEntry.iDetails.Length()) && (iSmsHeader->Recipients().Count() > 0))
                {
                CSmsNumber* recipient = iSmsHeader->Recipients().At(0);
                if(recipient->Name().Length() > 0)
                    aEntry.iDetails.Set(recipient->Name());
                else
                    aEntry.iDetails.Set(recipient->Address());
                }
            if(Body().DocumentLength()>0 && 0 >= aEntry.iDescription.Length())
                {
                aEntry.iDescription.Set(Body().Read(0,iServiceSettings->DescriptionLength()));   
                }
            aEntry.SetVisible(ETrue);
            aEntry.SetInPreparation(EFalse);           
            aEntry.SetMultipleRecipients(iSmsHeader->Recipients().Count() > 1);
            iSmsHeader->StoreL(aEditStore);
            StoreBodyL(aEditStore);
            break;
            }
        default:
            Panic(ESmscUnrecognizedEntryType);
        }
    
    const TInt size = aEditStore.SizeL();        
    if (aEntry.iSize != size)
        {
        aEntry.iSize = size;             
        }
    }
