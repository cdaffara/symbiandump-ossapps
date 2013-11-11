// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimapenvelope.h"


_LIT8(KTxtFormatAddressPlain, "%S@%S");
const TInt KFormatCharsAddressPlain = 4; // %S%S

_LIT8(KTxtFormatAddressWithName, "\"%S\" <%S@%S>");
const TInt KFormatCharsAddressWithName = 6; // %S%S%S

CImapEnvelope* CImapEnvelope::NewL()
// static method
// There is no non-trivial construction required, so no need for a ConstructL()
	{
	return new(ELeave)CImapEnvelope();
	}

CImapEnvelope::CImapEnvelope()
	{}

CImapEnvelope::~CImapEnvelope()
	{
	iEnvFrom.Close();
	iEnvSender.Close();
	iEnvReplyTo.Close();
	iEnvTo.Close();
	iEnvCc.Close();
	iEnvBcc.Close();
	}

/**
Returns the date field
@return the date field
*/
EXPORT_C const TDesC8& CImapEnvelope::EnvDate()
	{
	return iEnvDate;
	}
/**
Returns the subject field
@return the subject field
*/
EXPORT_C const TDesC8& CImapEnvelope::EnvSubject()
	{
	return iEnvSubject;
	}
/**
Returns the array of "from" addresses structures
@return a reference to the array of "from" addresses structures
*/
EXPORT_C const CImapEnvelope::RArrayTAddress& CImapEnvelope::EnvFrom()
	{
	return iEnvFrom;
	}
/**
Returns the array of "sender" addresses structures
@return a reference to the array of "sender" addresses structures
*/
EXPORT_C const CImapEnvelope::RArrayTAddress& CImapEnvelope::EnvSender()
	{
	return iEnvSender;
	}
/**
Returns the array of "reply to" addresses structures
@return a reference to the array of "reply to" addresses structures
*/
EXPORT_C const CImapEnvelope::RArrayTAddress& CImapEnvelope::EnvReplyTo()
	{
	return iEnvReplyTo;
	}
/**
Returns the array of "to" addresses structures
@return a reference to the array of "to" addresses structures
*/
EXPORT_C const CImapEnvelope::RArrayTAddress& CImapEnvelope::EnvTo()
	{
	return iEnvTo;
	}
/**
Returns the array of "carbon copy" addresses structures
@return a reference to the array of "cc" addresses structures
*/
EXPORT_C const CImapEnvelope::RArrayTAddress& CImapEnvelope::EnvCc()
	{
	return iEnvCc;
	}
/**
Returns the array of "blind carbon copy" addresses structures
@return a reference to the array of "bcc" addresses structures
*/
EXPORT_C const CImapEnvelope::RArrayTAddress& CImapEnvelope::EnvBcc()
	{
	return iEnvBcc;
	}
/**
Returns the "in reply to" field
@return the "in reply to" field
*/
EXPORT_C const TDesC8& CImapEnvelope::EnvInReplyTo()
	{
	return iEnvInReplyTo;
	}
/**
Returns the "message id" field
@return the "message id" field
*/
EXPORT_C const TDesC8& CImapEnvelope::EnvMessageId()
	{
	return iEnvMessageId;
	}

/**
Sets the value of the date field
@param aEnvDate the date field
*/
void CImapEnvelope::SetEnvDate(const TDesC8& aEnvDate)
	{
	iEnvDate.Set(aEnvDate);
	}
/**
Sets the value of the subject field
@param aEnvSubject the subject field
*/
void CImapEnvelope::SetEnvSubject(const TDesC8& aEnvSubject)
	{
	iEnvSubject.Set(aEnvSubject);
	}
/**
Appends an address structure to the list of from addresses
@param aAddress the address structure that is to be added to the list of from addresses
*/
void CImapEnvelope::AppendEnvFromL(const TAddress& aAddress)
	{
	iEnvFrom.AppendL(aAddress);
	}
/**
Appends an address structure to the list of sender addresses
@param aAddress the address structure that is to be added to the list of sender addresses
*/
void CImapEnvelope::AppendEnvSenderL(const TAddress& aAddress)
	{
	iEnvSender.AppendL(aAddress);
	}
/**
Appends an address structure to the list of "reply to" addresses
@param aAddress the address structure that is to be added to the list of "reply to" addresses
*/
void CImapEnvelope::AppendEnvReplyToL(const TAddress& aAddress)
	{
	iEnvReplyTo.AppendL(aAddress);
	}
/**
Appends an address structure to the list of "to" addresses
@param aAddress the address structure that is to be added to the list of "to" addresses
*/
void CImapEnvelope::AppendEnvToL(const TAddress& aAddress)
	{
	iEnvTo.AppendL(aAddress);
	}
/**
Appends an address structure to the list of "carbon copy" addresses
@param aAddress the address structure that is to be added to the list of "cc" addresses
*/
void CImapEnvelope::AppendEnvCcL(const TAddress& aAddress)
	{
	iEnvCc.AppendL(aAddress);
	}
/**
Appends an address structure to the list of "blind carbon copy" addresses
@param aAddress the address structure that is to be added to the list of "bcc" addresses
*/
void CImapEnvelope::AppendEnvBccL(const TAddress& aAddress)
	{
	iEnvBcc.AppendL(aAddress);
	}
/**
Sets the value of the "in reply to" field
@param aEnvInReplyTo the "in reply to" field
*/
void CImapEnvelope::SetEnvInReplyTo(const TDesC8& aEnvInReplyTo)
	{
	iEnvInReplyTo.Set(aEnvInReplyTo);
	}
/**
Sets the value of the "message id" field
@param aEnvMessageId the "message id" field
*/
void CImapEnvelope::SetEnvMessageId(const TDesC8& aEnvMessageId)
	{
	iEnvMessageId.Set(aEnvMessageId);
	}


/**
Sets the value of the name field
@param aName the name field
*/
void CImapEnvelope::TAddress::SetName(const TDesC8& aName)
	{
	iName.Set(aName);
	}
/**
Sets the value of the routing information field.  This is known as addr-adl in RFC3501
@param aAdl the routing information field
*/
void CImapEnvelope::TAddress::SetAdl(const TDesC8& aAdl)
	{
	iAdl.Set(aAdl);
	}
/**
Sets the value of the mailbox field
@param aMailbox the mailbox field
*/
void CImapEnvelope::TAddress::SetMailbox(const TDesC8& aMailbox)
	{
	iMailbox.Set(aMailbox);
	}
/**
Sets the value of the host field
@param aHost the host field
*/
void CImapEnvelope::TAddress::SetHost(const TDesC8& aHost)
	{
	iHost.Set(aHost);
	}

/**
Constructs an email address string from the constituent fields of the address.
If a name is present, then the string will be of the form
	"name" <mailbox@host>
Otherwise it will be of the form
	mailbox@host
The "adl" (routing) field is not included in the address string.
@return a heap string containing the email address.  Ownership of this string is transferred to the caller.
*/
EXPORT_C HBufC16* CImapEnvelope::TAddress::CreateAddressStringL() const
	{
	// Each part of the address is provided as an 8-bit value,
	// So build up the address as an 8-bit string
	
	HBufC8* address8 = NULL;
	
	if (iName.Length() == 0)
		{
		// Use plain address: mailbox@host
		TInt length = KTxtFormatAddressPlain().Length() - KFormatCharsAddressPlain + iMailbox.Length() + iHost.Length();
		
		address8 = HBufC8::NewLC(length);
		address8->Des().Format(KTxtFormatAddressPlain, &iMailbox, &iHost);		
		}
	else
		{
		// Use address with name: "name" <mailbox@host>
		TInt length = KTxtFormatAddressWithName().Length() - KFormatCharsAddressWithName + iName.Length() + iMailbox.Length() + iHost.Length();
		
		address8 = HBufC8::NewLC(length);
		address8->Des().Format(KTxtFormatAddressWithName, &iName, &iMailbox, &iHost);
		}
		
	// Now do a single conversion up to 16 bits - by padding each byte with a zero.
	// No need to put address16 on the cleanup stack, as there are no leaving methods after it has been created.
	HBufC16* address16 = HBufC16::NewL(address8->Length());
	address16->Des().Copy(*address8);
		
	CleanupStack::PopAndDestroy(address8);
	
	return address16;
	}
