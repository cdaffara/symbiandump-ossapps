// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// tmsvsmsentry.h
//
/**
 * @file 
 * @publishedPartner
 * @released
 */

#ifndef __TMSVSMSENTRY_H__
#define __TMSVSMSENTRY_H__

#include <gsmuelem.h>
#include <smutset.h>

/**
A specialisation of the message server index class for SMS message entries.

It provides accessers for SMS message specific data, for instance the class type
of the SMS message.

@publishedPartner
@released
*/
class TMsvSmsEntry : public TMsvEntry
	{
public:

/**
Defines a set flags used to access message entry data specific to an SMS message.

@publishedPartner
@released

*/
	enum TMsvSmsEntryFlags
		{
/**
Clears the entire set of SMS flags.
*/
		EMsvSmsEntryClearFlag				= 0x00000000,
/**
Mask for the protocol identifier data.
*/
		EMsvSmsEntryProtocolIdentifier		= 0x000000FF,
/**
Mask for the User Prompt Indicator flag.
*/
		EMsvSmsEntryUserPromptIndicator 	= 0x00000100,
/**
Mask for the SMS class data.
*/
		EMsvSmsEntryClass					= 0x00000E00,
/**
Mask for the flag indicating whether the class data has been defined.
*/
		EMsvSmsEntryClassDefined			= 0x00001000,
/**
Mask for the flag indicating whether the message ID is valid. 
*/
		EMsvSmsMessageValid					= 0x00002000,
/**
Mask for the delivery acknowledgement information.
*/
		EMsvSmsEntryDeliveryAckSummary		= 0x0001C000,
/**
Parameter defining the number of bits to be shifted in order for the SMS class
data to be at the LSB of the data block.
*/
		EMsvSmsEntryClassShift				= 9,
/**
Parameter defining the number of bits to be shifted in order for the delivery
acknowlwdgement information to be at the LSB of the data block.
*/
		EMsvSmsEntryDeliveryAckSummaryShift	= 14
		};

/**
Defines the summary acknowledgement information.

This information indicates whether the SMS message is not supplying a summary 
for an acknowledgement, is still expecting acknowledgments or it has received 
all expected acknowledgements.

If all acknowledgements have been received the summary indicates whether all 
the recipients have successfully acknowledged the message, all failed or there
was a mixture of successful and failed acknowledgements from the recipients. 
*/
	enum TMsvSmsEntryAckSummary
		{
/**
No summary information is being formed.
*/
		ENoAckSummary			= 0,
/**
The message is waiting for acknowledgements to be received for all recipients
of this message. Some recipients may have received their acknowledgements but
there are still some recipients that have not.
*/
		EPendingAcks,
/**
The summary indicates that the message was successfully acknowledged by all recipients.
*/
		EAllSuccessful,
/**
The summary indicates that the message failed to be acknowledged by all recipients.
*/
		EAllFailed,
/**
The summary indicates a mixture of successful and failed acknowledgements from 
the recipients of the message. All recipients of this message have received 
their acknowledgements.
*/
		EMixed
		};

public:
	inline TMsvSmsEntry();
	inline TMsvSmsEntry(const TMsvEntry& aEntry);

	inline void SetUserPromptIndicator(TBool aUPI);
	inline TBool UserPromptIndicator() const;

	inline TUint8 ProtocolIdentifier() const;
	inline void SetProtocolIdentifier(TSmsProtocolIdentifier aPID);
	inline void SetProtocolIdentifier(TUint8 aPID);

	inline TBool Class(TSmsDataCodingScheme::TSmsClass& aClass) const;
	inline void SetClass(TBool aClassDefined,TSmsDataCodingScheme::TSmsClass aClass = TSmsDataCodingScheme::ESmsClass0);

	IMPORT_C TMsvSmsEntryAckSummary AckSummary(TSmsAckType aAckType) const;
	IMPORT_C void SetAckSummary(TSmsAckType aAckType, TMsvSmsEntryAckSummary aAckSummary);

	IMPORT_C TBool MessageId(TInt32& aMessageId) const;
	IMPORT_C void SetMessageId(TInt32 aMessageId, TBool aIsValid);
	};

/**
The UID that indentifies the SMS message editor application.

This is the response to the query for the KUidMsvMtmQueryEditorUidValue
capability.

@see	CSmsClientMtm::QueryCapability

@publishedPartner
@released
*/
const TInt KUidMsgSmsEditorAppVal=0x1000163f;



#include <tmsvsmsentry.inl>

#endif //__TMSVSMSENTRY_H__
