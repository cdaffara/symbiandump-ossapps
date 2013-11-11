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
// smut.cpp
//
#include <smut.h>
#include <gsmuieoperations.h>

#include <e32std.h>
#include <gsmumsg.h>
#include <msvstd.h>
#include <cntdb.h>
#include <cntfield.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <msventry.h>
#include <msvuids.h>
#include <msvapi.h>
#include <txtetext.h>
#include <gsmubuf.h>
#include <bautils.h>
#include <barsc.h>
#include <smss.rsg>
#include <smss.hrh>
#include <msvutils.h>
#include "SMCMMAIN.H"
#include "SMUTSET.H"

// Used by ValidGsmNumber(). The characters that may appear at the start of a
// valid GSM number
_LIT(KSmsValidGsmNumberFirstChar, "+");

// Used by ValidGsmNumber(). The characters that may appear after the first
// character of a valid GSM number
_LIT(KSmsValidGsmNumberOtherChar, "#*");

const TInt KSmsValidGsmNumberMinLength = 2;

/**
Finds and returns all the Service IDs for the specified MTM.

A Service ID is the entry ID for an service-type entry. The first Service ID
for the specified MTM is returned.

If the complete set of Service IDs for the MTM is required then the caller
should provide a valid CMsvEntrySelection object in aServiceIds. The Service
Ids are appended to this object. If the complete set is not required then the
input/output argument aServiceIds should be set to NULL.

@param	aEntry
A server message entry that can be used by this function.

@param	aFirstId
An output argument with the first Service ID.

@param	aMtm
The specified MTM. This has a default value of KUidMsgTypeSMS.

@param	aServiceIds
An input/output argument with the complete selection of Service IDs.  This has
a default value of NULL.

@leave	KErrNotFound
A service entry could not be found for the specified MTM.
*/
EXPORT_C void TSmsUtilities::ServiceIdL(CMsvServerEntry& aEntry, TMsvId& aFirstId, TUid aMtm, CMsvEntrySelection* aServiceIds)
	{
	aFirstId = KMsvNullIndexEntryId;

	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(selection);

	User::LeaveIfError(aEntry.SetEntry(KMsvRootIndexEntryId));

	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	aEntry.SetSort(order);

	// Get the children on the Root Index Entry
	User::LeaveIfError(aEntry.GetChildrenWithType(KUidMsvServiceEntry, *selection));

	TInt count = selection->Count();

	// Find an entry for MTM aMtm
	for (TInt curChild = 0; curChild < count && (aFirstId == KMsvNullIndexEntryId || aServiceIds); ++curChild)
		{
		User::LeaveIfError(aEntry.SetEntry(selection->At(curChild)));
		CompareEntryL(aEntry.Entry(), aMtm, aFirstId, aServiceIds);
		}

	// Leave if no Service Entry found for MTM aMtm
	if (aFirstId == KMsvNullIndexEntryId)
		User::Leave(KErrNotFound);

	CleanupStack::PopAndDestroy(selection);
	}

/**
Finds and returns all the Service IDs for the specified MTM.

A Service ID is the entry ID for an service-type entry. The first Service ID
for the specified MTM is returned.

If the complete set of Service IDs for the MTM is required then the caller
should provide a valid CMsvEntrySelection object in aServiceIds. The Service
Ids are appended to this object. If the complete set is not required then the
input/output argument aServiceIds should be set to NULL.

@param	aSession
A message server session.

@param	aFirstId
An output argument with the first Service ID.

@param	aMtm
The specified MTM. This has a default value of KUidMsgTypeSMS.

@param	aServiceIds
An input/output argument with the complete selection of Service IDs.  This has
a default value of NULL.

@leave	KErrNotFound
A service entry could not be found for the specified MTM.
*/
EXPORT_C void TSmsUtilities::ServiceIdL(CMsvSession& aSession, TMsvId& aFirstId, TUid aMtm, CMsvEntrySelection* aServiceIds)
 	{
 	TMsvSelectionOrdering order;
 	order.SetShowInvisibleEntries(ETrue);

 	CMsvEntry* root = CMsvEntry::NewL(aSession, KMsvRootIndexEntryId, order);
 	CleanupStack::PushL(root);

 	ServiceIdL(*root, aFirstId, aMtm, aServiceIds);

 	CleanupStack::PopAndDestroy(root);
 	}

/**
Finds and returns all the Service IDs for the specified MTM.

A Service ID is the entry ID for an service-type entry. The first Service ID
for the specified MTM is returned.

If the complete set of Service IDs for the MTM is required then the caller
should provide a valid CMsvEntrySelection object in aServiceIds. The Service
Ids are appended to this object. If the complete set is not required then the
input/output argument aServiceIds should be set to NULL.

@param	aEntry
A message entry that can be used by this function.

@param	aFirstId
An output argument with the first Service ID.

@param	aMtm
The specified MTM. This has a default value of KUidMsgTypeSMS.

@param	aServiceIds
An input/output argument with the complete selection of Service IDs.  This has
a default value of NULL.

@leave	KErrNotFound
A service entry could not be found for the specified MTM.
*/
EXPORT_C void TSmsUtilities::ServiceIdL(CMsvEntry& aEntry, TMsvId& aFirstId, TUid aMtm, CMsvEntrySelection* aServiceIds)
	{
	aFirstId = KMsvNullIndexEntryId;

 	TMsvSelectionOrdering order(aEntry.SortType());
 	if (!order.ShowInvisibleEntries())
 		{
 		order.SetShowInvisibleEntries(ETrue);
 		aEntry.SetSortTypeL(order);
 		}

   	aEntry.SetEntryL(KMsvRootIndexEntryId);
 	const TInt count = aEntry.Count();

	// Find an entry for MTM aMtm
	for (TInt curChild = 0; curChild < count && (aFirstId == KMsvNullIndexEntryId || aServiceIds != NULL); ++curChild)
		{
		CompareEntryL(aEntry[curChild], aMtm, aFirstId, aServiceIds);
		}

	// Leave if no Service Entry found for MTM aMtm
	if (aFirstId == KMsvNullIndexEntryId)
		User::Leave(KErrNotFound);
	}

/**
Populates a message index.

The input data is used to set the fields in supplied message index. The affected
fields are the entry type, MTM, entry date, Service ID and error fields. The
date field is set from the time information in the aMessage argument.

This function can be used as part of the process when creating SMS messages in
the message store.

@param	aEntry
An input/output argument with the index entry to populate.

@param	aMessage
The SMS message object for the index entry.

@param	aServiceId
The Service ID for the message.

@param	aMtm
The specified MTM. This has a default value of KUidMsgTypeSMS.
*/
EXPORT_C void TSmsUtilities::PopulateMsgEntry(TMsvEntry& aEntry, const CSmsMessage& aMessage, TMsvId aServiceId, TUid aMtm)
	{
	aEntry.iType		= KUidMsvMessageEntry;
	aEntry.iMtm			= aMtm;
	aEntry.iDate		= aMessage.Time();
	aEntry.iServiceId	= aServiceId;
	aEntry.iError		= KErrNone;
	}

/**
Populates a message index.

The input data is used to set the fields in supplied message index. The affected
fields are the entry type, MTM, entry date, Service ID and error fields. The
date field is either set from the time information in the aMessage argument or from
the service center timestamp in the PDU depending on the associated SMS setting.

This function can be used as part of the process when creating SMS messages in
the message store.

@param	aEntry
An input/output argument with the index entry to populate.

@param	aMessage
The SMS message object for the index entry.

@param	aServiceId
The Service ID for the message.

@param	aSettings
The settings for the SMS account.

@param	aMtm
The specified MTM. This has a default value of KUidMsgTypeSMS.
*/
EXPORT_C void TSmsUtilities::PopulateMsgEntry(TMsvEntry& aEntry, const CSmsMessage& aMessage, TMsvId aServiceId, const CSmsSettings& aSettings, TUid aMtm)
	{
	TSmsUtilities::PopulateMsgEntry(aEntry, aMessage, aServiceId, aMtm);

	if (aSettings.UseServiceCenterTimeStampForDate())
		{
		const CSmsPDU& pdu = aMessage.SmsPDU();

		TTime time = 0;
		TInt gmtOffset = 0;

		if (pdu.Type() == CSmsPDU::ESmsDeliver)
			{
			CSmsDeliver& smsDeliver =
				const_cast<CSmsDeliver&>(static_cast<const CSmsDeliver&>(pdu));
			smsDeliver.ServiceCenterTimeStamp(time, gmtOffset);
			}
		else if (pdu.Type() == CSmsPDU::ESmsStatusReport)
			{
			CSmsStatusReport& smsStatusReport =
				const_cast<CSmsStatusReport&>(static_cast<const CSmsStatusReport&>(pdu));
			smsStatusReport.ServiceCenterTimeStamp(time, gmtOffset);
			}

		if (time > 0)
			{
			aEntry.iDate = time;
			}
		}
	}

/**
Get the SMS message recipient/sender details.

The recipient/sender telephone number is extracted from the supplied message
object. If the recipient/sender telephone number appears uniquely in the contacts
database then the family name and given name contact details are set into the
output argument aDetails in the format specified by the resource item
R_SENDER_NAME_FORMAT. The buffer limit specified by aMaxLength is observed.

If there is not a unique contact entry for the recipient/sender telephone number
then aDetails will contain the orginally telephone number.

@param	aFs
A connected file server session.

@param	aMessage
The message object with the recipient/sender telephone number.

@param	aDetails
The output argument to contain the message details.

@param	aMaxLength
The maximum length of the supplied buffer in aDetails.

@return
KErrNotSupported if the message is not of type SMS-SUBMIT, SMS-DELIVER or SMS-STATUS-REPORT.
KErrArgument if the telephone number is invalid.
KErrNotFound if a contact could not be found.
KErrAlreadyExists if more than one contact entry found.
KErrNone if details is obtained successfully.
*/
EXPORT_C TInt TSmsUtilities::GetDetails(RFs& aFs, const CSmsMessage& aMessage, TDes& aDetails, TInt aMaxLength)
	{
	__ASSERT_DEBUG( aMaxLength <= aDetails.MaxLength(), User::Invariant() );

	if (aMaxLength > aDetails.MaxLength())
		{
		aMaxLength = aDetails.MaxLength();
		}

	aDetails.Zero();

	TPtrC fromAddress;

	switch (aMessage.SmsPDU().Type())
		{
		case CSmsPDU::ESmsSubmit:
		case CSmsPDU::ESmsDeliver:
		case CSmsPDU::ESmsStatusReport:
			fromAddress.Set(aMessage.SmsPDU().ToFromAddress());
			break;
		default:
			return KErrNotSupported;
		}

	return GetDetails(aFs, fromAddress, aDetails, aMaxLength);
	}

/**
Get the SMS message recipient/sender details.

The recipient/sender telephone number is searched for in the contacts database.
If a unique match is found then the family name and given name contact details
are set into the output argument aDetails in the format specified by the
resource item R_SENDER_NAME_FORMAT. The buffer limit specified by aMaxLength is
observed.

If a unique match is not found or the supplied telephone number is invalid, then
aDetails will contain the orginally telephone number.

@param	aFs
A connected file server session.

@param	aFromAddress
The recipient/sender telephone number.

@param	aDetails
The output argument to contain the message details.

@param	aMaxLength
The maximum length of the supplied buffer in aDetails.

@return
KErrArgument if aFromAddress is not a valid GSM number.
KErrNotFound if a contact could not be found.
KErrAlreadyExists if more than one contact entry found.
KErrNone if details is obtained successfully.
*/
EXPORT_C TInt TSmsUtilities::GetDetails(RFs& /*aFs*/, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength)
	{
	__ASSERT_DEBUG( aMaxLength <= aDetails.MaxLength(), User::Invariant() );

	if (aMaxLength > aDetails.MaxLength())
		{
		aMaxLength = aDetails.MaxLength();
		}

	if ( (aDetails.Length() == 0) )
		{
		if (aFromAddress.Length() <= aMaxLength)
			{
			aDetails = aFromAddress;
			aDetails.Trim();
			}
		else
			{
			// Truncate aFromAddress so that it fits into aDetails.
			aDetails = aFromAddress.Left(aMaxLength);
			aDetails.Trim();
			}
		}

	return KErrNone;
	}

/**
Get the SMS message description.

If the message is Special Message Indication SMS then the description will
contain the appropriate localised text for the indication.

If the message is a Status Report then the description will contain the
appropriate localised text for a Status Report.

If the message is a standard text message the description will contain the
beginning section of the SMS message text itself.

In all cases the buffer limit specified by aMaxLength is observered.

@param	aMessage
The SMS message object.

@param	aDescription
The output argument for the description.

@param	aMaxLength
The maximum length of the supplied buffer in aDescription.

@return
An error code if the localised text for a SMS-STATUS-REPORT message could not be
obtained. Otherwise KErrNone is returned.
*/
EXPORT_C TInt TSmsUtilities::GetDescription(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength)
	{
	__ASSERT_DEBUG( aMaxLength <= aDescription.MaxLength(), User::Invariant() );

	if (aMaxLength > aDescription.MaxLength())
		{
		aMaxLength = aDescription.MaxLength();
		}

	aDescription.Zero();

	TBool gotDescription = EFalse;
	TRAPD(err, gotDescription = DoGetDescriptionL(aMessage, aDescription, aMaxLength));
	if(err != KErrNone || !gotDescription)
		ExtractDescriptionFromMessage(aMessage, aDescription, aMaxLength);

	return KErrNone;
	}

/**
Opens and returns the SMS client MTM resource file.

It is the responsibility of the caller to ensure that the resource file is
closed once it is no longer required.

@param	aFs
A connected file server session.

@return
The opened resource file.
*/
EXPORT_C RResourceFile TSmsUtilities::OpenResourceFileL(RFs& aFs)
	{
	TFileName fileName(KSmsResourceFile);
	MsvUtils::AddPathAndExtensionToResFileL(fileName);
 	BaflUtils::NearestLanguageFile(aFs, fileName);

	RResourceFile resFile;
	resFile.OpenL(aFs, fileName);
	return resFile;
	}

/**
Reads the resource specified by aResourceId from the supplied resource file.

The resource is returned in the output argument aString. The supplied resource
file must be open or this function will leave.

@param	aResourceFile
The opened resource file to read the resource from.

@param	aResourceId
The ID of the resource that is required.

@param	aString
An output buffer into which the read resource is placed.

@leave	KErrOverflow
The length of the resource string is greater than the maximum allowed.
*/
EXPORT_C void TSmsUtilities::ReadResourceStringL(RResourceFile aResourceFile, TInt aResourceId, TDes& aString)

	{
	HBufC8* buf = aResourceFile.AllocReadLC(aResourceId);
	TResourceReader reader;
	reader.SetBuffer(buf);
	
	TPtrC resString = reader.ReadTPtrC();

	if (resString.Length() <= aString.MaxLength())
		{ 
		aString.Copy(resString);
		}
	else
		{
		User::Leave(KErrOverflow);
		}

	CleanupStack::PopAndDestroy(buf);
	}

void TSmsUtilities::CompareEntryL(const TMsvEntry& aEntry, TUid aMtm, TMsvId& aFirstId, CMsvEntrySelection* aServiceIds)
	{
	if (aEntry.iType == KUidMsvServiceEntry && aEntry.iMtm == aMtm)
		{
		const TMsvId id = aEntry.Id();

		if (aFirstId == KMsvNullIndexEntryId)
			aFirstId = id;

		if (aServiceIds)
			aServiceIds->AppendL(id);
		}
	}

TBool TSmsUtilities::ValidGsmNumber(const TDesC& aTelephone)
	{
	// Returns ETrue if
	//	aTelephone is not zero length; and
	//	aTelephone[0] is a digit or "+"; and
	//	aTelephone[1..] is a digit or "*" or "#".
	//	aTelephone has at least 2 valid characters

	//Note: Returns EFalse if aTelephone contains any alpha character
	//      All spaces are ignored

	const TInt length = aTelephone.Length();

	if (length < KSmsValidGsmNumberMinLength)
		return EFalse;

	TPtrC KFirstChar(KSmsValidGsmNumberFirstChar);
	TPtrC KOtherChar(KSmsValidGsmNumberOtherChar);
	TBool validTel = ETrue;
	TInt validCharsFound = 0; //must be >= KSmsValidGsmNumberMinLength by the end

	const TText* first = aTelephone.Ptr(); //Points to the first character in aTelephone
	const TText* last = first + length - 1; //Points to the last character in aTelephone

	//Check each character in the telephone number
	while (validTel && first <= last)
		{
		TChar ch(*first);

		if (!ch.IsSpace()) //ignore whitespace
			{
			if (!ch.IsDigit())
				{
				//Need to create TPtrC because TDesC::Find() does not take a TChar
				TPtrC telCharacter(first, 1);

				if (validCharsFound)
					{
					//Check the remaining characters of the telephone number
					validTel = (KOtherChar.Find(telCharacter) != KErrNotFound);
					}
				else //validCharsFound == 0
					{
					//Check the first character of the telephone number
					validTel = (KFirstChar.Find(telCharacter) != KErrNotFound);
					}
				}

			validCharsFound++;
			}

		first++; //move to the next character
		}

	return validTel && validCharsFound >= KSmsValidGsmNumberMinLength;
	}

TBool TSmsUtilities::DoGetDescriptionL(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength)
// this function returns EFalse if aMessage has no special message indication data and is not an SMS_STATUS_REPORT,
// i.e. more needs to be done to extract the description from the message
// otherwise returns ETrue
	{
	TInt resourceId = 0;
	TBuf<KSmsDescriptionLength> format;
	TSmsMessageIndicationType messageIndicationType;
	TExtendedSmsIndicationType extendedType;
	TSmsMessageProfileType messageProfileType;
	TBool toStore=EFalse;
	TUint totalIndicationCount=0;
	TUint totalMessageCount=0;	
	
	//check if the messae contains an enhanced voice mail indication
	CSmsEnhancedVoiceMailOperations& enhancedVoiceMailOperations = STATIC_CAST(CSmsEnhancedVoiceMailOperations&,aMessage.GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));
	
	if(enhancedVoiceMailOperations.ContainsEnhancedVoiceMailIEL())
		{
		//get a copy of the indication
		CEnhancedVoiceMailBoxInformation* retrievedNotification=enhancedVoiceMailOperations.CopyEnhancedVoiceMailIEL();
		TVoiceMailInfoType typeInfo=retrievedNotification->Type();
		//check its type
		if(typeInfo==EGsmSmsVoiceMailNotification)
			{
			//increment the indication count
			++totalIndicationCount;
			resourceId = R_MESSAGE_INDICATION_ENHANCED_VOICEMAIL_ONE;	
			}
		
		TUint8 messageCount=retrievedNotification->NumberOfVoiceMessages();
		//add the message count to the running total
		totalMessageCount+=messageCount;
		//if there is more that one message of this type then set the resouce id to 'many'
		if(messageCount!=1)
			{
			++resourceId;	
			}
		
		delete retrievedNotification;
		}  
		
	//check for special message waiting indications
	CSmsSpecialSMSMessageOperations& operations = STATIC_CAST(CSmsSpecialSMSMessageOperations&,aMessage.GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	TUint specialMessageIndicationCount=operations.GetCountOfSpecialMessageIndicationsL();	
	
	if(specialMessageIndicationCount!=0)
		{
		//add special message indications to out indication count
		totalIndicationCount+=specialMessageIndicationCount;	
		
		if(totalIndicationCount>1) 
			{
			//set the resource id to R_MESSAGE_INDICATION_OTHER_ONE
			resourceId = R_MESSAGE_INDICATION_OTHER_ONE;
			//get the total number of messages from the indicatations
			TUint8 messageCount=0;
			for(TInt loopCount=0;loopCount<specialMessageIndicationCount;loopCount++)
				{
				operations.GetMessageIndicationIEL(loopCount,toStore,messageIndicationType,extendedType,messageProfileType,messageCount);
				totalMessageCount+=messageCount;						
				}
			}
		else
			{
			//there is only one indication, get it's type and the number of messages it holds.
			TUint8 messageCount=0;
			operations.GetMessageIndicationIEL(0,toStore,messageIndicationType,
											extendedType,messageProfileType,messageCount);	
		
			//add the message count to the running total
			totalMessageCount+=messageCount;
		
			switch (messageIndicationType)
				{
				case EGsmSmsVoiceMessageWaiting:
					resourceId = R_MESSAGE_INDICATION_VOICEMAIL_ONE;
					break;
				
				case EGsmSmsFaxMessageWaiting:
					resourceId = R_MESSAGE_INDICATION_FAX_ONE;
					break;
				
				case EGsmSmsElectronicMailMessageWaiting:
					resourceId = R_MESSAGE_INDICATION_EMAIL_ONE;
					break;
					
				case EGsmSmsExtendedMessageTypeWaiting:
					//get the extended indications type
					if(extendedType==EGsmSmsVideoMessageWaiting)
						{
						resourceId = R_MESSAGE_INDICATION_VIDEOMESSAGE_ONE;	
						}
					else
						{
						resourceId = R_MESSAGE_INDICATION_OTHER_ONE;	
						}
					break;
					
				default:
					resourceId = R_MESSAGE_INDICATION_OTHER_ONE;
					break;
				}	
			}
		//if there is more that one message waiting append 'many' to the id.
		if(totalMessageCount!=1)
			{
			resourceId++;	
			}
		}

	const CSmsPDU& smsPDU= aMessage.SmsPDU();
	// If no Special Msg Indication found in the User Data,
	// then check the DataCodingScheme.
	if (totalIndicationCount==0 && smsPDU.DataCodingSchemePresent())
		{
		TInt bits7to4 = smsPDU.Bits7To4();

		switch (bits7to4)
			{
			case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndication7Bit:
			case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2:
				{
				if (smsPDU.IndicationState() == TSmsDataCodingScheme::ESmsIndicationActive)
					{
					totalIndicationCount = 1;

					switch (smsPDU.IndicationType())
						{
						case TSmsDataCodingScheme::ESmsVoicemailMessageWaiting:
							resourceId = R_MESSAGE_INDICATION_VOICEMAIL_ONE;
							break;
						case TSmsDataCodingScheme::ESmsFaxMessageWaiting:
							resourceId = R_MESSAGE_INDICATION_FAX_ONE;
							break;
						case TSmsDataCodingScheme::ESmsElectronicMailMessageWaiting:
							resourceId = R_MESSAGE_INDICATION_EMAIL_ONE;
							break;
						case TSmsDataCodingScheme::ESmsFaxOtherMessageWaiting:
						default:
							resourceId = R_MESSAGE_INDICATION_OTHER_ONE;
							break;
						} //end switch
					} //end if
				} //end case
			default:
				{
				break; //do nothing
				}
			}
		}
	
	if (totalIndicationCount!=0)
		{
		//Special message found.
		//Look up the resourceID in the SMSS resource file

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		RResourceFile resFile = OpenResourceFileL(fs);
		CleanupClosePushL(resFile);
		ReadResourceStringL(resFile, resourceId, format);
		CleanupStack::PopAndDestroy(2, &fs);

		if (totalMessageCount == 1)
			{
			if (format.Length() <= aMaxLength)
				{
				aDescription = format;
				}
			else
				{
				// Truncate format so that it fits into aDescription.
				aDescription = format.Left(aMaxLength);
				}
			}
		else if (format.Length() < aMaxLength)
			{
			aDescription.Zero();
			aDescription.AppendFormat(format, totalMessageCount);
			}
		return ETrue;
		}
	else
		{
		if(aMessage.Type() == CSmsPDU::ESmsStatusReport)
			{
			// for SMS_STATUS_REPORT messages, if we cannot read the string in, then
			// we do not attempt to extract the description from the message: return EFalse
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			RResourceFile resFile = OpenResourceFileL(fs);
			CleanupClosePushL(resFile);
			ReadResourceStringL(resFile, R_TYPE_STATUS_REPORT, aDescription);
			CleanupStack::PopAndDestroy(2, &fs);
			return ETrue;
			}
		else
			{
			return EFalse;
			}
		}
	}


void TSmsUtilities::ExtractDescriptionFromMessage(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength)
	{
	if(aMessage.Type() != CSmsPDU::ESmsStatusReport)
		{
		aMessage.Buffer().Extract(aDescription, 0, Min(aMaxLength, aMessage.Buffer().Length()));

		TInt length = aDescription.Length();

		//replace paragraphs with spaces.
		while(length--)
			{
			TText& text = aDescription[length];
			const TChar ch(text);
			if (ch.IsSpace() || text == CEditableText::EParagraphDelimiter)
				text = ' ';
			}

		aDescription.TrimAll(); //removes leading trailing and multiple internal whitespace (spaces, line feeds etc)
		}
	}

void TSmsUtilities::Replace(const TDesC& aOld, const TDesC& aNew, TDes& aString)
	{
	TInt find = aString.Find(aOld);

	if (find != KErrNotFound)
		{
		aString.Replace(find, aOld.Length(), aNew);
		}
	}
