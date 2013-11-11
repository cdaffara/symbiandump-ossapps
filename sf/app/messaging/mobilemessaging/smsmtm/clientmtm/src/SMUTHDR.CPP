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
//

#include <smuthdr.h>
#include <csmsemailfields.h>
#include <gsmuieoperations.h>

#include <gsmumsg.h>
#include <msvstore.h>	
#include <cntdb.h>
#include <gsmunonieoperations.h>
#include <gsmuelem.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <tmsvsmsentry.h>
#endif

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
#include <msvrcpt.h> 
#endif
	
const TUid KUidMsvSMSHeaderStream		= {0x10001834};
const TInt16 KMsvSmsHeaderVersion		= 1;
const TInt KSmcmRecipientsGranularity	= 8;
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
_LIT16(KComma, ",");
_LIT16(KDelimiter, ";");
#endif

/** 
Allocates and constructs a new CSmsHeader object.

Use this function to create a new SMS-SUBMIT, SMS-COMMAND, SMS-DELIVER or 
SMS-STATUS-REPORT type message.

@param	aType
The Protocol Data Unit (PDU) type for the SMS message.

@param	aText
The message text.

@return
A new CSmsHeader object.
*/
EXPORT_C CSmsHeader* CSmsHeader::NewL(CSmsPDU::TSmsPDUType aType, CEditableText& aText)
	{
	CSmsHeader* self = new(ELeave) CSmsHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aType, aText);
	CleanupStack::Pop(self);
	return self;
	}
	
/** 
Allocates and constructs a new CSmsHeader object.

Use this function to create a new SMS-SUBMIT, SMS-COMMAND, SMS-DELIVER or 
SMS-STATUS-REPORT type message. This version uses a previously connected
file server session handle.

@param	aType
The Protocol Data Unit (PDU) type for the SMS message.

@param	aText
The message text.

@param aFs
Handle to an open file server session. CSmsHeader will not close the file session.

@return
A new CSmsHeader object.
*/
EXPORT_C CSmsHeader* CSmsHeader::NewL(CSmsPDU::TSmsPDUType aType, CEditableText& aText, RFs& aFs)
	{
	CSmsHeader* self = new(ELeave) CSmsHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aType, aText, aFs);
	CleanupStack::Pop(self);
	return self;
	}

/** 
Allocates and constructs a new CSmsHeader object.

Use this function to create a new SMS-DELIVER type message.

@param	aMessage
The SMS message encapsulation from the SMS stack.

@return
A new CSmsHeader object.
*/
EXPORT_C CSmsHeader* CSmsHeader::NewL(CSmsMessage* aMessage)
	{
	CSmsHeader* self = new(ELeave) CSmsHeader(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
*/
EXPORT_C CSmsHeader::~CSmsHeader()
	{
	iRecipients.ResetAndDestroy();
	delete iMessage;
	delete iEmailFields;
	if(iCloseFs)
		{
		iFs.Close();
		}
	}

/** 
Sets the SMS message settings for the message.

This can only be used on SMS-SUBMIT, SMS-COMMAND, SMS-DELIVER or SMS-STATUS-REPORT
type messages.

NOTE - if this is an Email over SMS message then the PID of is not changed. The
PID would have been set correctly when the email fields were set.

@param	aSmsSettings
The SMS message settings for the message

@leave	KErrNotSupported
The message PDU type was not supported, or the message conversion value was not
supported.

@panic	SMCM	0
The message PDU is not supported (debug only).
*/
EXPORT_C void CSmsHeader::SetSmsSettingsL(const CSmsMessageSettings& aSmsSettings)
	{
	__ASSERT_DEBUG( (Type()==CSmsPDU::ESmsSubmit) || (Type()==CSmsPDU::ESmsDeliver) || (Type()==CSmsPDU::ESmsStatusReport) || (Type()==CSmsPDU::ESmsCommand), Panic(ESmutPanicUnsupportedMsgType)); 

	switch(Type())
		{
		case(CSmsPDU::ESmsSubmit):
			{
			Submit().SetRejectDuplicates(aSmsSettings.RejectDuplicate());
			Submit().SetReplyPath(aSmsSettings.ReplyPath());
			
			//Check if delivery report for the last segment only is required . 
			if(aSmsSettings.LastSegmentDeliveryReport())
				{
				CSmsTPSRROperations& tpSRROperations = static_cast<CSmsTPSRROperations&>(iMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
				tpSRROperations.SetSchemeL();
				tpSRROperations.SetLastSegmentStatusReportL(ETrue);
				}
			else
				{
				Submit().SetStatusReportRequest(aSmsSettings.DeliveryReport());	
				}
			Submit().SetValidityPeriod(aSmsSettings.ValidityPeriod());
			Submit().SetValidityPeriodFormat(aSmsSettings.ValidityPeriodFormat());
			break;
			}
		case(CSmsPDU::ESmsCommand):
			{
			Command().SetStatusReportRequest(aSmsSettings.DeliveryReport());
			break;
			}
		case(CSmsPDU::ESmsDeliver):
		case(CSmsPDU::ESmsStatusReport):
			break;
		default:
			{			
			User::Leave(KErrNotSupported);
			break;
			}
		};

	if (Message().TextPresent())
		SetCanConcatenate(aSmsSettings.CanConcatenate());

	CSmsPDU& pdu=Message().SmsPDU();
	if (pdu.DataCodingSchemePresent())
		pdu.SetAlphabet(aSmsSettings.CharacterSet());
	if (pdu.ProtocolIdentifierPresent() && iEmailFields->Length() == 0 )
		{
		// The email fields object for this message is empty - therefore the
		// PID can be set. With a non-empty email fields object the PID would
		// have been set for interworking with email and MUST not be changed.
		pdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);

		// MessageConversion
		switch(aSmsSettings.MessageConversion())
			{
			case(ESmsConvPIDNone):
				{
				pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsNoTelematicDevice);
				break;
				}
			case ESmsConvFax:
			case ESmsConvX400:
			case ESmsConvPaging:
			case ESmsConvMail:
			case ESmsConvErmes:
			case ESmsConvSpeech:
				{
				pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
				pdu.SetTelematicDeviceType((TSmsProtocolIdentifier::TSmsTelematicDeviceType) aSmsSettings.MessageConversion());
				break;
				}
			default:
				{
				User::Leave(KErrNotSupported);
				break;
				}
			}; 
		}
	}

/** 
Gets the SMS message settings for the message.

This can only be used on SMS-SUBMIT type messages.

@param	aSmsSettings
The output argument with the SMS message settings.

@leave	KErrNotSupoprted
The Telematic Device type is not supported.

@panic	SMCM	0
The message PDU is not supported (debug only).
*/
EXPORT_C void CSmsHeader::GetSmsSettingsL(CSmsMessageSettings& aSmsSettings) const
	{
	__ASSERT_DEBUG( Type()==CSmsPDU::ESmsSubmit, Panic(ESmutPanicUnsupportedMsgType)); 
	switch(Type())
		{
		case(CSmsPDU::ESmsSubmit):
			{
			aSmsSettings.SetRejectDuplicate(Submit().RejectDuplicates());
			aSmsSettings.SetReplyPath(Submit().ReplyPath());
			TSmsStatusReportScheme scheme = iMessage->Scheme();
			if (scheme == ETPSRRScheme)	
				{
				aSmsSettings.SetLastSegmentDeliveryReport(ETrue);
				}
			else
				{
				aSmsSettings.SetDeliveryReport(Submit().StatusReportRequest());
				}
			aSmsSettings.SetValidityPeriod(Submit().ValidityPeriod());
			aSmsSettings.SetValidityPeriodFormat(Submit().ValidityPeriodFormat());
			aSmsSettings.SetCharacterSet(Submit().Alphabet());
		
			if (iMessage->SmsPDU().PIDType() == TSmsProtocolIdentifier::ESmsPIDTelematicInterworking)
				{
				if (iMessage->SmsPDU().TelematicDeviceIndicator() == TSmsProtocolIdentifier::ESmsNoTelematicDevice)
					{
					aSmsSettings.SetMessageConversion(ESmsConvPIDNone);
					}
				else //if (iMessage->SmsPDU().TelematicDeviceIndicator() == TSmsProtocolIdentifier::ESmsTelematicDevice)
					{
					switch (iMessage->SmsPDU().TelematicDeviceType())
						{
						case TSmsProtocolIdentifier::ESmsGroup3TeleFax:
						case TSmsProtocolIdentifier::ESmsX400MessageHandlingSystem:
						case TSmsProtocolIdentifier::ESmsNationalPagingSystem:
						case TSmsProtocolIdentifier::ESmsInternetElectronicMail:
						case TSmsProtocolIdentifier::ESmsERMES:
						case TSmsProtocolIdentifier::ESmsVoiceTelephone:
							aSmsSettings.SetMessageConversion((TSmsPIDConversion) iMessage->SmsPDU().TelematicDeviceType());
							break;
						default:
							User::Leave(KErrNotSupported);
							break;
						}
					}
				}

			aSmsSettings.SetCanConcatenate(CanConcatenate());
			break;
			}
		default:
			{
			break;
			}
		}
	}
	
/**
Sets the email fields for the message.

If the supplied email fields is not empty then the PID of the PDU of the message
is set for interworking with email.

If the supplied email fields is empty and the PID of the PDU of the message is 
set for interworking with email then the PID is set to the default value that
indicates no telematic device. If the PID was not set for interworking with 
email then it is left unchanged.

@param	aEmailFields
The email fields object. A copy is made of this object.
*/	
EXPORT_C void CSmsHeader::SetEmailFieldsL(const CSmsEmailFields& aEmailFields)
	{
	__ASSERT_DEBUG( Type()==CSmsPDU::ESmsSubmit, Panic(ESmutPanicUnsupportedMsgType)); 

	CSmsEmailFields* temp = CSmsEmailFields::NewL(aEmailFields);
	delete iEmailFields;
	iEmailFields = temp;
	
	CSmsPDU& pdu = Message().SmsPDU();
	if( pdu.ProtocolIdentifierPresent() )
		{
		if( iEmailFields->Length() > 0 )
			{
			// Set the PID for interworking with email.
			pdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
			pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
			pdu.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsInternetElectronicMail);
			}
		else
			{
			// The email fields are empty - if the PID is set for interworking
			// with email, set to indicate no telematic device.
			if(	pdu.PIDType() == TSmsProtocolIdentifier::ESmsPIDTelematicInterworking &&
				pdu.TelematicDeviceIndicator() == TSmsProtocolIdentifier::ESmsTelematicDevice &&
				pdu.TelematicDeviceType() == TSmsProtocolIdentifier::ESmsInternetElectronicMail )
				{
				pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsNoTelematicDevice);
				}
			}
		}
	}
	
/**
Sets the email fields for a reply message.

The address field is copied from the specified email fields. The subject field 
is formatted according to the specified subject format using the subject from 
the specified email fields.

The PID of the PDU of the message is set for interworking with email.

@param	aEmailFields
The email fields object.

@param	aReplySubjectFormat
The format of the subject field.

@internalComponent
*/
void CSmsHeader::SetReplyEmailFieldsL(const CSmsEmailFields& aEmailFields, const TDesC& aReplySubjectFormat)
	{
	// Create the reply email fields - copy address and then format the subject,
	// if one exists according to the reply prefix.
	CSmsEmailFields* temp = CSmsEmailFields::NewL();
	CleanupStack::PushL(temp);
	temp->AddAddressL(aEmailFields.Addresses().MdcaPoint(0));
	
	SetEmailReplyForwardSubjectL(temp, aEmailFields.Subject(), aReplySubjectFormat);

	CleanupStack::Pop(temp);
	delete iEmailFields;
	iEmailFields = temp;

	// Set the PID for interworking with email.
	CSmsPDU& pdu = Message().SmsPDU();
	if( pdu.ProtocolIdentifierPresent() )
		{
		pdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
		pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
		pdu.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsInternetElectronicMail);
		}
	}

/**
Sets the email fields for a forward message.

The address field is left empty. The subject field is formatted according to the
specified subject format using the subject from the specified email fields.

The PID of the PDU of the message is set for interworking with email.

@param	aEmailFields
The email fields object.

@param	aForwardSubjectFormat
The format of the subject field.

@internalComponent
*/
void CSmsHeader::SetForwardEmailFieldsL(const CSmsEmailFields& aEmailFields, const TDesC& aForwardSubjectFormat)
	{
	// Create the reply email fields - copy address and then format the subject,
	// if one exists according to the reply prefix.
	CSmsEmailFields* temp = CSmsEmailFields::NewL();
	CleanupStack::PushL(temp);
	
	SetEmailReplyForwardSubjectL(temp, aEmailFields.Subject(), aForwardSubjectFormat);

	CleanupStack::Pop(temp);
	delete iEmailFields;
	iEmailFields = temp;

	// Set the PID for interworking with email.
	CSmsPDU& pdu = Message().SmsPDU();
	if( pdu.ProtocolIdentifierPresent() )
		{
		pdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
		pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
		pdu.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsInternetElectronicMail);
		}
	}
	
void CSmsHeader::SetEmailReplyForwardSubjectL(CSmsEmailFields* aEmailFields, const TDesC& aSubject, const TDesC& aSubjectFormat)
	{
	TInt length = aSubject.Length();
	if( length > 0 )
		{
		// Prevent a chain of the format string appearing in the subject - check
		// to see if the format string is already the prefix of the subject.
		TInt formatPosInSubject = KErrNotFound;

		_LIT(KFormatStringSpecifier, "%S");
		TInt formatPos = aSubjectFormat.Find(KFormatStringSpecifier);
		if( formatPos > 0 )
			{
			// Look in current subject for the format string - avoid the '%'.
			formatPosInSubject = aSubject.FindF(aSubjectFormat.Left(formatPos - 1));
			}
		
		// Need to format the string if the format string is not at the start of
		// the current subject.
		if( formatPosInSubject != 0 )
			{
			// Create a buffer large enough to hold re-formated subject - need
			// to subtract two from the prefix length (the %S).
			length += aSubjectFormat.Length() - 2;
			HBufC* buf = HBufC::NewLC(length);
			TPtr ptr(buf->Des());
			
			// Format the reply subject and set in the email fields.
			ptr.Format(aSubjectFormat, &aSubject);
			aEmailFields->SetSubjectL(*buf);
			
			CleanupStack::PopAndDestroy(buf);
			}
		else
			{
			// Subject already contains the format string - use it.
			aEmailFields->SetSubjectL(aSubject);
			}
		}
	}
	
/**
The email fields object for this message.

@return
The email fields object for this message.
*/	
EXPORT_C const CSmsEmailFields& CSmsHeader::EmailFields() const
	{
	return *iEmailFields;
	}

/** 
Internalises the object from the specified stream.

@param	aStream
The stream to be read from.
*/
EXPORT_C void CSmsHeader::InternalizeL(RMsvReadStream& aStream)
	{
#if (!defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)

	aStream.ReadInt16L(); //version. Not used yet
	
	iRecipients.ResetAndDestroy();
	TInt count=aStream.ReadInt32L();
	while(count--)
		{
		CSmsNumber* recipient=CSmsNumber::NewL();  
		CleanupStack::PushL(recipient);
		recipient->InternalizeL(aStream);
		iRecipients.AppendL(recipient);
		CleanupStack::Pop(recipient);
		}

	iFlags = aStream.ReadUint32L();
	iBioMsgIdType = (TBioMsgIdType) aStream.ReadInt8L();
	iMessage->InternalizeWithoutBufferL(aStream);
#else
	iMessage->InternalizeWithoutBufferL(aStream);
#endif
	}

/**
Externalises the object to the specified stream.

@param	aStream
The stream to be writen to.
*/
EXPORT_C void CSmsHeader::ExternalizeL(RMsvWriteStream& aStream) const
	{
#if (!defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)

	aStream.WriteInt16L(KMsvSmsHeaderVersion);

	TInt count=iRecipients.Count();
	aStream.WriteInt32L(count);

	for (TInt i=0; i<count; i++)
		iRecipients[i]->ExternalizeL(aStream);

	aStream.WriteUint32L(iFlags);
	aStream.WriteInt8L(iBioMsgIdType);
	iMessage->ExternalizeWithoutBufferL(aStream); 
#else
	iMessage->ExternalizeWithoutBufferL(aStream); 	
#endif 	
	}

/**
Restores the object from the message entry store.

The SMS object is restored from the KUidMsvSMSHeaderStream in the supplied store.
 
@param	aStore
The store from which the object is restored.
 
@leave	KErrNotFound
The stream KUidMsvSMSHeaderStream does not exist in aStore.
*/
EXPORT_C void CSmsHeader::RestoreL(CMsvStore& aStore)
	{
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	ReStoreDBL(aStore);
	RMsvReadStream in;
	in.OpenLC(aStore,KUidMsvSMSHeaderStream);
	InternalizeL(in);
	CleanupStack::PopAndDestroy(&in);
#else

	RMsvReadStream in;
	in.OpenLC(aStore,KUidMsvSMSHeaderStream);
	InternalizeL(in);
	CleanupStack::PopAndDestroy(&in);
	
	iEmailFields->RestoreL(aStore);
	
#endif 
	}


/**
Stores the object in the message entry store.

The object is written to the KUidMsvSMSHeaderStream stream. Any previous content
is overwritten.

@param	aStore
The store to hold the stream into which the object is written.
*/
EXPORT_C void CSmsHeader::StoreL(CMsvStore& aStore) const
	{
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	StoreDbL(aStore);
	RMsvWriteStream out;
	out.AssignLC(aStore,KUidMsvSMSHeaderStream);
	ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy(&out);

#else
	RMsvWriteStream out;
	out.AssignLC(aStore,KUidMsvSMSHeaderStream);
	ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy(&out);
	
	iEmailFields->StoreL(aStore);
#endif
	}


/**
Check  whether a CMsvStore contains an SMS header stream.

@param aStore The store.
@return ETrue if there is a   SMS header
*/
EXPORT_C TBool CSmsHeader::ContainsSmsHeaderL(const CMsvStore& aStore)
    {
    return aStore.IsPresentL(KUidMsvSMSHeaderStream);
    }

CSmsHeader::CSmsHeader(CSmsMessage* aSmsMessage)
:	iRecipients(KSmcmRecipientsGranularity),
	iMessage(aSmsMessage),
	iFlags(ESmsHeaderNoFlags),
	iBioMsgIdType(EBioMsgIdNbs),
	iCloseFs(ETrue)
	{
	}

void CSmsHeader::ConstructL(CSmsPDU::TSmsPDUType aType, CEditableText& aText)
	{
	User::LeaveIfError(iFs.Connect());
	iMessage=CSmsMessage::NewL(iFs, aType, CSmsEditorBuffer::NewL(aText), EFalse);
	iEmailFields = CSmsEmailFields::NewL();
	}
	
void CSmsHeader::ConstructL(CSmsPDU::TSmsPDUType aType, CEditableText& aText, RFs& aFs)
	{
	iFs=aFs;
	iCloseFs = EFalse;
	iMessage=CSmsMessage::NewL(iFs, aType, CSmsEditorBuffer::NewL(aText), EFalse);
	iEmailFields = CSmsEmailFields::NewL();
	}

void CSmsHeader::ConstructL()
	{
	iEmailFields = CSmsEmailFields::NewL();

	// Check the PID of the SMS PDU to see if set for interworking with email.
	CSmsPDU& pdu = Message().SmsPDU();
	if( pdu.ProtocolIdentifierPresent() &&
		pdu.PIDType() == TSmsProtocolIdentifier::ESmsPIDTelematicInterworking &&
		pdu.TelematicDeviceIndicator() == TSmsProtocolIdentifier::ESmsTelematicDevice &&
		pdu.TelematicDeviceType() == TSmsProtocolIdentifier::ESmsInternetElectronicMail )
		{
		__ASSERT_DEBUG( pdu.Type() == CSmsPDU::ESmsDeliver, User::Invariant() );
		
		// Ok, this is an email SMS - parse out the address and the optional 
		// subject fields.
		CSmsBufferBase& message = Message().Buffer();
		TInt length = message.Length();
		HBufC* buf = HBufC::NewLC(length);
		TPtr bufPtr(buf->Des());
		message.Extract(bufPtr, 0, length);
		
		TInt end = iEmailFields->ParseL(*buf);
		
		if( end > 0 )
			{
			TPtrC body((*buf).Mid(end));
			message.Reset();
			message.InsertL(0, body);
			}
		CleanupStack::PopAndDestroy(buf);
		}
	}

void CSmsHeader::SetCanConcatenate(TBool aCanConcatenate)
	{
	iFlags = (iFlags & ~ESmsHeaderCanConcatenate) | (aCanConcatenate ? ESmsHeaderCanConcatenate : ESmsHeaderNoFlags);
	}

TBool CSmsHeader::CanConcatenate() const
	{
	return iFlags & ESmsHeaderCanConcatenate;
	}

/**
Gets the summary information for the specified acknowledgement.

If the specified acknowledgement is not recognised, then this function returns
the default value of TMsvSmsEntryAckSummary::ENoAckSummary.
nothing.

@param
aAckType	The requested acknowledgement summary.

@return
The acknowledgement summary information.

@see
TMsvSmsEntry::TMsvSmsEntryAckSummary
*/
EXPORT_C TMsvSmsEntry::TMsvSmsEntryAckSummary TMsvSmsEntry::AckSummary(TSmsAckType aAckType) const
	{
	TMsvSmsEntryAckSummary summary = ENoAckSummary;
	switch( aAckType )
		{
	case ESmsAckTypeDelivery:
		summary = static_cast<TMsvSmsEntryAckSummary>((iMtmData2 & EMsvSmsEntryDeliveryAckSummary) >> EMsvSmsEntryDeliveryAckSummaryShift);
		break;
	default:
		// Use default summary - fail gracefully.
		break;
		}
	return summary;
	}

/**
Sets the summary information for the specified acknowlwdgement.

If the specified acknowledgement is not recognised, then this function does
nothing.

@param
aAckType	The acknowledgement summary to be set.

@param
aAckSummary	The summary information.

@see
TMsvSmsEntry::TMsvSmsEntryAckSummary
*/
EXPORT_C void TMsvSmsEntry::SetAckSummary(TSmsAckType aAckType, TMsvSmsEntryAckSummary aAckSummary)
	{
	switch( aAckType )
		{
	case ESmsAckTypeDelivery:
		iMtmData2 = (iMtmData2 & ~EMsvSmsEntryDeliveryAckSummary) | ((aAckSummary << EMsvSmsEntryDeliveryAckSummaryShift) & EMsvSmsEntryDeliveryAckSummary);
		break;
	default:
		// Do nothing - fail gracefully.
		break;
		}
	}

/**
Gets the message log ID and a flag indicating if it is valid.

The returned flag indicates whether the message log ID is valid. If it is valid
then the message has only a single recipient that is pending a status report. In
this case the returned message log ID refers to the recipient that is pending.

If it is not valid then the message has more than one recipient that is pending 
a status report. The message log ID should is not valid and should not be used.

@param
aMessageId	An output argument for the message log ID.

@return
A flag indicating whether the message log ID aMessageId is valid or not.
*/
EXPORT_C TBool TMsvSmsEntry::MessageId(TInt32& aMessageId) const
	{
	aMessageId = iMtmData3;
	return iMtmData2 & EMsvSmsMessageValid;
	}

/**
Sets the message log ID and a flag indicating if it is valid.

If the message has only a single recipient that is pending a status report then
the message log ID for that recipient can be set in the index. The flag must be
set to true.

If the message has more than one recipient that is pending a status report then
the message log ID must not be used. The flag must be set to false.

This functionality cannot be used with bio-messages. This function will panic if
used with a bio-message.

@param
aMessageId	The message log ID. Should only be used if aIsValid is true. It has
			a default value of zero.

@param
aIsValid	A flag indicating if the message log ID is valid. This should have
			a value or true if the message has only a single recipient pending
			a status report.
			
@panic	USER	0
This message is a bio-message and therefore the message ID field cannot be used.
*/
EXPORT_C void TMsvSmsEntry::SetMessageId(TInt32 aMessageId, TBool aIsValid)
	{
	__ASSERT_ALWAYS( iBioType == 0, User::Invariant() );

	aIsValid ? (iMtmData2 |= EMsvSmsMessageValid) : (iMtmData2 &= ~EMsvSmsMessageValid);
	iMtmData3 = aMessageId;
	}

/** 
Gets the reply to address if it exists, otherwise calls FromAddress().

Only valid for SMS-DELIVER type messages. 
@return
The reply to address if it exists, otherwise the originator address as returned by FromAddress().
*/	
TPtrC CSmsHeader::ReplyAddressL() const
	{
	
	if(Type()==CSmsPDU::ESmsDeliver)
		{
		//Check for a reply address field
		CSmsReplyAddressOperations& operations = STATIC_CAST(CSmsReplyAddressOperations&,Message().GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));
		if(!operations.ContainsReplyAddressIEL())
			{
			//if there is no reply to field call FromAddress()
			return FromAddress();	
			}	
		//return the reply to address
		HBufC* replyAddressHBuf=operations.GetReplyAddressL();
		return replyAddressHBuf->Des();			
		}	
	else
		{
		return FromAddress();		
		}
	}

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)

/**
Stores the object in the message entry store in SQL DB.

The object is written to the KUidMsvSMSHeaderStream stream. Any previous content
is overwritten.

@param	aStore
The store to hold the stream into which the object is written.
*/

void CSmsHeader::StoreDbL(CMsvStore& aStore) const
	{
	_LIT(KDetails,"");
	
	CHeaderFields* smsHeaderFields = new(ELeave) CHeaderFields();
	CleanupStack::PushL(smsHeaderFields);
	smsHeaderFields->iUid = KUidMsvSMSHeaderStream;
	
	// 1. Header version field.
	CFieldPair* smsHeaderVersionfield = new(ELeave) CFieldPair();
	CleanupStack::PushL(smsHeaderVersionfield);
	smsHeaderVersionfield->iFieldNumValue = KMsvSmsHeaderVersion;
	smsHeaderFields->iFieldPairList.AppendL(smsHeaderVersionfield);
	CleanupStack::Pop(smsHeaderVersionfield);
	
	// 2. Recipient count field.
	CFieldPair* smsRecipientCountField = new(ELeave) CFieldPair();
	CleanupStack::PushL(smsRecipientCountField);
	smsRecipientCountField->iFieldNumValue = iRecipients.Count();
	smsHeaderFields->iFieldPairList.AppendL(smsRecipientCountField);
	CleanupStack::Pop(smsRecipientCountField);
	
	// 3. Recipients field.
	TInt size = 0;	
	for (TInt i=0; i < iRecipients.Count(); i++)
		{
		size += sizeof(CMsvRecipient::TRecipientStatus);
		size += 16;		// CMsvRecipient::iError (4), iRetries(4) and iTime (8)
		size += Align4(iRecipients[i]->Address().Size());	// CSmsNumber::iNumber
		size += Align4(iRecipients[i]->Name().Size());		// CSmsNumber::iName
		size += sizeof(TLogId);								// CSmsNumber::iLogId
		size += sizeof(CSmsNumber::TSmsAckStatus);			// CSmsNumber::iDeliveryStatus
		size += 10;											// For delimiters
		}

	RBuf recipients;
	CleanupClosePushL(recipients);
	recipients.CreateL(size);

	for (TInt i=0; i<iRecipients.Count(); i++)
		{		
		recipients.AppendNum(iRecipients[i]->Status()); //0
		recipients.Append(KComma);
		recipients.AppendNum(iRecipients[i]->Error()); //1
		recipients.Append(KComma);
		recipients.AppendNum(iRecipients[i]->Retries()); //2
		recipients.Append(KComma);
		recipients.AppendNum(iRecipients[i]->Time().Int64()); //3
		recipients.Append(KComma);
	
		recipients.Append(iRecipients[i]->Address()); //4
		recipients.Append(KComma);
		recipients.Append(iRecipients[i]->Name()); //5
		recipients.Append(KComma);
		recipients.AppendNum(iRecipients[i]->LogId()); //6
		recipients.Append(KComma);
		recipients.AppendNum(iRecipients[i]->AckStatus(ESmsAckTypeDelivery)); //7
		recipients.Append(KDelimiter);
		}
		
	CFieldPair* smsRecipientsField = new(ELeave) CFieldPair();
	CleanupStack::PushL(smsRecipientsField);
	smsRecipientsField->iFieldTextValue = recipients.AllocL();
	smsHeaderFields->iFieldPairList.AppendL(smsRecipientsField);
				
	CleanupStack::Pop(smsRecipientsField);	// smsRecipientsField
	CleanupStack::PopAndDestroy();			// recipients

	// 4. SMS Flag
	CFieldPair* smsSmsFlags = new(ELeave) CFieldPair();
	CleanupStack::PushL(smsSmsFlags);
	smsSmsFlags->iFieldNumValue = iFlags;
	smsHeaderFields->iFieldPairList.AppendL(smsSmsFlags);
	CleanupStack::Pop(smsSmsFlags);
	
	// 5. BIO Msg Id
	CFieldPair* smsBioMsgIdType = new(ELeave) CFieldPair();
	CleanupStack::PushL(smsBioMsgIdType);
	smsBioMsgIdType->iFieldNumValue = iBioMsgIdType;
	smsHeaderFields->iFieldPairList.AppendL(smsBioMsgIdType);
	CleanupStack::Pop(smsBioMsgIdType);
	
			
	CFieldPair* smsdetailField = new (ELeave)CFieldPair();
	CleanupStack::PushL(smsdetailField);
	smsdetailField->iFieldName =  KDetails().AllocL();
	smsdetailField->iFieldType = ETextField;
	smsdetailField->iFieldTextValue = KNullDesC().AllocL();
	smsHeaderFields->iFieldPairList.AppendL(smsdetailField);
	CleanupStack::Pop(smsdetailField);

	TMsvWriteStore storeWriter(aStore);
	storeWriter.AssignL(smsHeaderFields);
	storeWriter.CommitL();
	
	CleanupStack::Pop(smsHeaderFields);
	
	// 6.  SMS-Email UID
	// 7.  SMS-EMail HeaderVersion
	// 8.  Subject
	// 9.  AddressCount
	// 10. Addresses

	iEmailFields->StoreDBL(aStore);	
	
	}

/**
Restores the object from the message entry store in SQL DB.

The SMS object is restored from the KUidMsvSMSHeaderStream in the supplied store.
 
@param	aStore
The store from which the object is restored.
 
@leave	KErrNotFound
The stream KUidMsvSMSHeaderStream does not exist in aStore.
*/	
	
void CSmsHeader::ReStoreDBL(CMsvStore& aStore)	
	{
	
	CHeaderFields* rcvHeaderRow = NULL;
	TMsvReadStore storeReader(aStore, KUidMsvSMSHeaderStream);
	storeReader.LoadL(rcvHeaderRow);

	
	iRecipients.ResetAndDestroy();
	
	TInt i = 0;
	CFieldPair* rcvHeader = rcvHeaderRow->iFieldPairList[i];

	TInt16 headerversion = rcvHeaderRow->iFieldPairList[i++]->iFieldNumValue; // header version.
	TInt count = rcvHeaderRow->iFieldPairList[i++]->iFieldNumValue;
	
	HBufC* 	receipientList = rcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	CleanupStack::PushL(receipientList);
	
	TPtrC receipientListPtr = receipientList->Des();
	GetRecipientL(receipientListPtr);

	iFlags  = rcvHeaderRow->iFieldPairList[i++]->iFieldNumValue;
	iBioMsgIdType = (TBioMsgIdType)rcvHeaderRow->iFieldPairList[i++]->iFieldNumValue ;

	CleanupStack::PopAndDestroy(receipientList);
	
	iEmailFields->ReStoreDBL(aStore);
	
	}

/**
Get the recipient strring and Create the recipient.

@param : aRecipientStr A TPtrC16.
@return None.
*/
void CSmsHeader::GetRecipientL(TDesC16& aRecipientStrList)
	{
	TPtrC16 aRecipientStr = aRecipientStrList.Left(aRecipientStrList.Length());
	
	if(aRecipientStr.Length()>0)
		{
		RArray<TPtrC16> smsNumberData;
		TInt startPos = 0;
		TInt firstSemiColonPos = aRecipientStr.Locate(';');
		TInt lastSemiColonPos = aRecipientStr.LocateReverse(';');
				
		do
			{
			CSmsNumber* recipientNumber = CSmsNumber::NewL();
			CleanupStack::PushL(recipientNumber);

			TPtrC16 str1 = aRecipientStr.Left(firstSemiColonPos+1); // First recipient
			startPos = str1.Locate(',') ;
			
			while(startPos != KErrNotFound )
				{
				TPtrC16 str2 = str1.Left(startPos);
				smsNumberData.AppendL(str2);
				
				startPos++;
				str1.Set(str1.Mid(startPos, str1.Length()- startPos));
			
				startPos = str1.Locate(',');
				if(startPos == KErrNotFound)
					{
					TPtrC16 str3;
					str3.Set(str1.Mid(0,str1.Length()-1));
					smsNumberData.Append(str3);
					break;
					}
					
				}
		
			recipientNumber->SetStatus((CMsvRecipient::TRecipientStatus)ConvertToTInt(smsNumberData[0]));
			recipientNumber->SetError(ConvertToTInt(smsNumberData[1]));
			recipientNumber->SetRetries(ConvertToTInt(smsNumberData[2]));
			TInt64 time = ConvertToTInt(smsNumberData[3]);
			recipientNumber->SetTimeValue(time);
			
			//CSmsNumber	
			recipientNumber->SetAddressL(smsNumberData[4]);
			recipientNumber->SetNameL(smsNumberData[5]);
			
			TLex logId(smsNumberData[6]);
			TInt32 logIdNum;
			logId.Val(logIdNum);
			recipientNumber->SetLogId(logIdNum);	
			
			TLex ackStatus(smsNumberData[7]);
			TInt32 ackStatusNum;
			recipientNumber->SetAckStatus( (TSmsAckType) ackStatus.Val(ackStatusNum), CSmsNumber::ENoAckRequested);
				
			iRecipients.AppendL(recipientNumber);
			CleanupStack::Pop(recipientNumber);
			smsNumberData.Reset();
			
			if(firstSemiColonPos != lastSemiColonPos)
				{
				aRecipientStr.Set(aRecipientStr.Mid(firstSemiColonPos+1,aRecipientStr.Length()-firstSemiColonPos-1));
				firstSemiColonPos = aRecipientStr.Locate(';');
				lastSemiColonPos = aRecipientStr.LocateReverse(';');
				}
			else	
				{
				break;	
				}
								
			}while(1);
		
		smsNumberData.Close();				
		}		
	}


/**
 * Convert a String to an Integer.
 * @param aStr A string to make Integer.
 * @return TInt A integer value 
 */
 TInt CSmsHeader::ConvertToTInt(TDesC16& aStr)
	{
	TLex str(aStr);
	TInt32 string;
	str.Val(string);
	return string;
	}

#endif
