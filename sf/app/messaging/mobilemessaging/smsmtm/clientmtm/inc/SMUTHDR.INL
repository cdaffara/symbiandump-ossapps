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
// SMUTHDR.INL
//

#include <smcmmain.h>
#include <gsmumsg.h>

/** 
Gets the message PDU for a SMS-SUBMIT type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return
The SMS-SUBMIT PDU object.

@panic	SMCM	0
The message was not a PDU of type SMS-SUBMIT (debug only).

@see	CSmsHeader::Type
*/
inline CSmsSubmit& CSmsHeader::Submit()
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsSubmit, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsSubmit&)(iMessage->SmsPDU());
	}

/** 
Gets the (const) message PDU for a SMS-SUBMIT type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return
The SMS-SUBMIT PDU const object.

@panic	SMCM	0
The message was not a PDU of type SMS-SUBMIT (debug only).

@see	CSmsHeader::Type
*/
inline const CSmsSubmit& CSmsHeader::Submit() const
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsSubmit, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsSubmit&)(iMessage->SmsPDU());
	}

/** 
Gets the message PDU for a SMS-DELIVER type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return
The SMS-DELIVER PDU object.

@panic	SMCM	0
The message was not a PDU of type SMS-DELIVER (debug only).

@see	CSmsHeader::Type
*/
inline CSmsDeliver& CSmsHeader::Deliver()
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsDeliver, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsDeliver&)(iMessage->SmsPDU());
	}

/** 
Gets the (const) message PDU for a SMS-DELIVER type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return
The SMS-DELIVER PDU const object.

@panic	SMCM	0
The message was not a PDU of type SMS-DELIVER (debug only).

@see	CSmsHeader::Type
*/
inline const CSmsDeliver& CSmsHeader::Deliver() const
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsDeliver, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsDeliver&)(iMessage->SmsPDU());
	}

/** 
Gets the message PDU for a SMS-COMMMAND type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return	The SMS-COMMMAND PDU object.

@panic	SMCM	0
The message was not a PDU of type SMS-COMMMAND (debug only).

@see	CSmsHeader::Type
*/
inline CSmsCommand& CSmsHeader::Command()
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsCommand, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsCommand&)(iMessage->SmsPDU());
	}

/** 
Gets the (const) message PDU for a SMS-COMMMAND type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return
The SMS-COMMMAND PDU const object.

@panic	SMCM	0
The message was not a PDU of type SMS-COMMMAND (debug only).

@see	CSmsHeader::Type
*/
inline const CSmsCommand& CSmsHeader::Command() const
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsCommand, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsCommand&)(iMessage->SmsPDU());
	}

/** 
Gets the message PDU for a SMS-STATUS-REPORT type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return
The SMS-STATUS-REPORT PDU object.

@panic	SMCM	0
The message was not a PDU of type SMS-STATUS-REPORT (debug only).

@see	CSmsHeader::Type
*/
inline CSmsStatusReport& CSmsHeader::StatusReport()
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsStatusReport, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsStatusReport&)(iMessage->SmsPDU());
	}

/** 
Gets the (const) message PDU for a SMS-STATUS-REPORT type message.

The message PDU type can be checked by using the CSmsHeader::Type API.

@return
The SMS-STATUS-REPORT PDU const object.

@panic	SMCM	0
The message was not a PDU of type SMS-STATUS-REPORT (debug only).

@see	CSmsHeader::Type
*/
inline const CSmsStatusReport& CSmsHeader::StatusReport() const
	{
	__ASSERT_DEBUG( Type() == CSmsPDU::ESmsStatusReport, Panic(ESmutPanicUnsupportedMsgType));
	return (CSmsStatusReport&)(iMessage->SmsPDU());
	}

/**
Gets the message PDU type.

There are six types of PDU -  SMS-DELIVER, SMS-DELIVER-REPORT, SMS-SUBMIT, 
SMS-SUBMIT-REPORT, SMS-STATUS-REPORT and SMS-COMMAND. 

There is access to only to SMS-DELIVER, SMS-SUBMIT, SMS-STATUS-REPORT and
SMS-COMMAND PDUs via the CSmsHeader encapsulation.

@return
The message PDU type.

@see	CSmsPDU::TSmsPDUType
*/
inline CSmsPDU::TSmsPDUType CSmsHeader::Type() const
	{
	return iMessage->SmsPDU().Type();
	}

/**
Gets the (const) message recipients.

@return
The const array of message recipients 
*/
inline const CArrayPtrFlat<CSmsNumber>& CSmsHeader::Recipients() const
	{
	return iRecipients;
	}

/** 
Gets the (const) message recipients.

@return
The array of message recipients 
*/
inline CArrayPtrFlat<CSmsNumber>& CSmsHeader::Recipients()
	{
	return iRecipients;
	}

/** 
Gets the SMS stack representation of the message.

@return
The SMS stack representation of the message

@see	CSmsMessage
*/
inline CSmsMessage& CSmsHeader::Message()
	{
	return *iMessage;
	}
	
/** 
Gets the (const) SMS stack representation of the message.

@return
The const SMS stack representation of the message 
*/
inline const CSmsMessage& CSmsHeader::Message() const
	{
	return *iMessage;
	}

/**
Tests if the message contains a reply path.

@return
A value of True if the message contains a reply path.
*/
inline TBool CSmsHeader::ReplyPathProvided() const
	{
	return iFlags & ESmsHeaderReplyPathProvided;
	}

/** 
Set the message flag that indicates whether it contains a reply path.

@param	aReplyPathProvided
A value of True to set the flag.
*/
inline void CSmsHeader::SetReplyPathProvided(TBool aReplyPathProvided)
	{
	iFlags = (iFlags & ~ESmsHeaderReplyPathProvided) | (aReplyPathProvided ? ESmsHeaderReplyPathProvided : ESmsHeaderNoFlags);
	}

/**
Sets the originator address.

@param	aAddress
The originator address
*/
inline void CSmsHeader::SetFromAddressL(const TDesC& aAddress)
	{
	Message().SmsPDU().SetToFromAddressL(aAddress);
	}

/** 
Gets the originator address.

Only valid for SMS-DELIVER and SMS-SUBMIT type messages. With any other message
types the returned originator address is empty.

@return
The originator address.
*/
inline TPtrC CSmsHeader::FromAddress() const
	{
	switch(Type())
		{
		case(CSmsPDU::ESmsSubmit):
			return Submit().ToFromAddress();
		case(CSmsPDU::ESmsDeliver):
			return Deliver().ToFromAddress();
		default: 
			return TPtrC(0,0);
		}
	}

/** 
Gets the message's service center address.

This can only be used on SMS-SUBMIT type messages. The message PDU type can be
checked by using the CSmsHeader::Type API.

@return
The Service Center address.

@panic	SMCM	0
The message was not a PDU of type SMS-SUBMIT (debug only).

@see	CSmsHeader::Type
*/
inline TPtrC CSmsHeader::ServiceCenterAddress() const
	{
	return Submit().ServiceCenterAddress();
	}

/**
Sets the message's service center address.

This can only be used on SMS-SUBMIT type messages. The message PDU type can be
checked by using the CSmsHeader::Type API.


@param	aAddress
The Service Center address

@panic	SMCM	0
The message was not a PDU of type SMS-SUBMIT (debug only).

@see	CSmsHeader::Type
*/
inline void CSmsHeader::SetServiceCenterAddressL(const TDesC& aAddress)
	{
	Submit().SetServiceCenterAddressL(aAddress);
	}

/** 
Gets the message's BIO message type identifier.

For non-BIO messages, this is EBioMsgIdNbs.

@return
The BIO message type identifier 
*/
inline TBioMsgIdType CSmsHeader::BioMsgIdType() const
	{
	return iBioMsgIdType;
	}

/** 
Sets the message's BIO message type identifier.

@param	aBioMsgIdType
The BIO message type identifier
*/
inline void CSmsHeader::SetBioMsgIdType(TBioMsgIdType aBioMsgIdType)
	{
	iBioMsgIdType = aBioMsgIdType;
	}


