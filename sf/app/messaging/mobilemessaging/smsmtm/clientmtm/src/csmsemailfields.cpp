// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <csmsemailfields.h>

#include <msvstore.h>

const TInt KSmsEmailAddressesArrayGranularity	= 4;

const TUid KUidMsvSmsEmailFieldsStream	= {0x10204C9D};
const TInt KMsvSmsEmailFieldsVersion	= 1;
const TUint8 KSmsEmailComma			= ',';
const TUint8 KSmsEmailAtSign		= '@';
const TUint8 KSmsEmailSpace 		= ' ';
const TUint8 KSmsEmailOpenBracket	= '(';
const TUint8 KSmsEmailCloseBracket	= ')';
const TUint8 KSmsEmailHash			= '#';
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
_LIT16(KDelimiter, ";");
_LIT16(KComma, ",");
#endif
/**
Factory constructor.

@return
An empty email fields object.
*/
EXPORT_C CSmsEmailFields* CSmsEmailFields::NewL()
	{
	CSmsEmailFields* self = new (ELeave) CSmsEmailFields();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Factory copy constructor.

@param	aEmailFields
The orginal email fields object from which a copy is made.

@return
A copy of aEmailFields.
*/
EXPORT_C CSmsEmailFields* CSmsEmailFields::NewL(const CSmsEmailFields& aEmailFields)
	{
	CSmsEmailFields* self = new (ELeave) CSmsEmailFields();
	CleanupStack::PushL(self);
	self->ConstructL(aEmailFields);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Destructor.
*/	
EXPORT_C CSmsEmailFields::~CSmsEmailFields()
	{
	Reset();
	delete iAddresses;
	}
	
/**
Resets the contents of the email fields object.
*/
EXPORT_C void CSmsEmailFields::Reset()
	{
	delete iSubject;
	iSubject = NULL;
	if( iAddresses!=NULL )
		{
		iAddresses->Reset();
		}
	}
	
/**
The number of characters in the email fields.

Calculates the number of characters that will be pre-pended to an SMS message
when SMS is set for interworking with email. The address and subject fields are
only valid if the their length is greater than zero.

The address field is a comma separated list of addresses.

It assumes that a subject will be added with the following format.

@code

	<address>(<subject>)<message>

@endcode

No validation of the contents of the fields is performed.

@return
The number of characters in the email fields.
*/
EXPORT_C TInt CSmsEmailFields::Length() const
	{
	// Get the address length and subject length
	TInt subject = Subject().Length();
	TInt address = 0;
	
	if( HasAddress() )
		{
		// Go through address list - there is at least one address...	
		address = (*iAddresses)[0].Length();
		
		TInt count = iAddresses->Count();
		for( TInt i=1; i < count; ++i )
			{
			// Any subsequent addresses are in a comma separated list - include
			// length of comma separator.
			address += (1 + (*iAddresses)[i].Length());
			}
		}
	
	if( subject > 0 )
		{
		// There is a subject - add the address and subject lengths plus the
		// two delimiters; the '(' and ')' delimiters.
		return (address + subject + 2);
		}
	if( address > 0 )
		{
		// There is no subject field, but there is an address field - add the
		// address length and the single space delimiter for the end of the 
		// address field.
		return (address + 1);
		}

	// There is neither an address nor a subject field - return zero length.
	return 0;
	}
	
/**
Indicates whether the address field contains at least one address

@return
A value of true if the address field contains at least one address, false if the
address field is empty.
*/
EXPORT_C TBool CSmsEmailFields::HasAddress() const
	{
	return (iAddresses->Count() > 0);
	}
	
/**
Composes the email fields.

The email fields are formatted into a buffer. The buffer is returned and left
on the cleanup stack.

If there is no address fields, then the function leaves with the error code 
KErrCorrupt.

The address field is a comma separate list of addresses.

If a subject field exists then the address fields is delimited by a open bracket
'(' and the subject is delimited by a close bracket ')'.

@return
A buffer contain the formatted email fields. A pointer to this buffer is left
on the cleanup stack.

@leave	KErrCorrupt
The email fields has a zero-length address field.

@internalTechnology
*/
EXPORT_C HBufC* CSmsEmailFields::ComposeLC() const
	{
	if( !HasAddress() )
		{
		User::Leave(KErrCorrupt);
		}

	HBufC* buf = HBufC::NewLC(Length());
	TPtr ptr(buf->Des());
	
	// Append the addresses - must have at least one address...
	ptr.Append((*iAddresses)[0]);
	
	// Append remaining addresses as comma separated list
	TInt count = iAddresses->Count();
	for( TInt i=1; i < count; ++i )
		{
		ptr.Append(KSmsEmailComma);
		ptr.Append((*iAddresses)[i]);
		}
	
	// The delimiter for the address depends on whether there is a subject.
	if( Subject().Length() > 0 )
		{
		// Append open bracket '(', followed by subject and then close bracket ')'.
		ptr.Append(KSmsEmailOpenBracket);
		ptr.Append(*iSubject);
		ptr.Append(KSmsEmailCloseBracket);
		}
	else
		{
		// No subject - address delimited by a space.
		ptr.Append(KSmsEmailSpace);		
		}
	return buf;
	}
	
/**
Parses the given buffer for email fields.

Supports the two address formats.

@code

	user@domain1.domain2

or

	User Name <user@domain1.domain2>

@endcode

Also, supports the two subject formats.

@code

	<address>(<subject>)<message>

or
	
	<address>##<subject>#<message>

@endcode

The parsed address and optional subject are set in the email fields object. Any
existing data in the object is deleted.

@param	aBuffer
The buffer to be parsed.

@return
If the email fields were successfully parsed the start position of the actual
body text from the start of aBuffer is returned. If the email fields could not
be parsed from aBuffer the error code KErrCorrupt is returned.

@internalComponent
*/	
TInt CSmsEmailFields::ParseL(const TDesC& aBuffer)
	{
	Reset();
	
	// The to-address or the from-address can have following two formats - 
	// 1. user@domain1.domain2
	// 2. User Name <user@domain1.domain2>
	// In the second format the <> brackets are part of the address.
	// The address is delimited by one of the following - 
	// 1. <space>
	// 2. (
	// 3. #
	// The later two cases imply that there is a subject field. These two 
	// delimiters are also mutually exclusive.
	//
	// First find the @ symbol - this will be the address.
	TPtrC buf(aBuffer);
	TInt pos = buf.Locate(KSmsEmailAtSign);
	if( pos <= 0 )
		{
		return KErrCorrupt;
		}
	
	// Found the address - search for the delimiters.
	TInt atPos = pos;
	buf.Set(buf.Mid(pos));
	pos = buf.Locate(KSmsEmailSpace);
	TInt spacePos	= pos != KErrNotFound ? pos : KMaxTInt;

	pos = buf.Locate(KSmsEmailOpenBracket);
	TInt bracketPos	= pos != KErrNotFound ? pos : KMaxTInt;

	pos = buf.Locate(KSmsEmailHash);
	TInt hashPos	= pos != KErrNotFound ? pos : KMaxTInt;
	
	TInt bodyPos = KErrNotFound;
	if( spacePos < bracketPos && spacePos < hashPos )
		{
		// There is no subject as the <space> is the first delimiter.
		pos = spacePos + atPos;
		
		// Set the start of the body text - need to move past the delimiter.
		bodyPos = pos + 1;
		}
	else 
		{
		// The delimiter is either the # or ( - this implies a subject field.
		TInt end = KErrNotFound;
		if( bracketPos < hashPos )
			{
			// Address delimited by the bracket position;
			pos = bracketPos + atPos;
			
			// Set the start of the body text - need to move past the delimiter.
			bodyPos = pos + 1;
			
			// Subject has the (subject) format - find the end of the subject
			// indicated by the closing bracket ')'. First move past delimiter.
			buf.Set(buf.Mid(bracketPos+1));
			end = buf.Locate(KSmsEmailCloseBracket);
			}
		else if( hashPos < KMaxTInt && buf[hashPos+1] == KSmsEmailHash )
			{
			// Address delimited by the hash position;
			pos = hashPos + atPos;
			
			// Set the start of the body text - need to move past the two # 
			// delimiters.
			bodyPos = pos + 2;

			// Subject has the ##subject# format - find the end of the subject
			// indicated by a final hash '#'. First move past the delimiters.
			buf.Set(buf.Mid(hashPos+2));
			end = buf.Locate(KSmsEmailHash);
			}
		if( end == KErrNotFound )
			{
			// In this case the email fields are corrupt - one of the following
			// has occured - 
			// 1. The address had no delimiter - missing <space>, # or (.
			// 2. The subject had no delimiter - missing # or ).
			// 3. The address has only a single delimiting #.
			return KErrCorrupt;
			}
		SetSubjectL(buf.Left(end));
		
		// Update the start of the body text to get passed the subject - need
		// to move past the delimiter.
		bodyPos += end + 1;
		}
	// Add the address and return the position of the start of the body text.
	AddAddressL(aBuffer.Left(pos));
	
	return bodyPos;	
	}
	
/**
Adds the address to the list of addresses.

The address to be added must have a non-zero length. A zero-length address will
not be added to the address list.

@param	aAddress
The address to be added.
*/
EXPORT_C void CSmsEmailFields::AddAddressL(const TDesC& aAddress)
	{
	if( aAddress.Length() > 0 )
		{
		iAddresses->AppendL(aAddress);
		}
	}
	
/**
Removes the specified address.

@param	aIndex
The index for the address to be remvoed.
*/
EXPORT_C void CSmsEmailFields::RemoveAddress(TInt aIndex)
	{
	iAddresses->Delete(aIndex);
	}
	
/**
The array of email addresses.

@see	MDesCArray

@return
An array of email addresses.
*/
EXPORT_C const MDesCArray& CSmsEmailFields::Addresses() const
	{
	return *iAddresses;
	}
	
/**
Sets the email subject field.

Copies the contents of subject as the email subject field. If the copy is 
unsuccessful the current contents is left unchanged.

@param	aSubject
The subject to be copied into the email field.
*/
EXPORT_C void CSmsEmailFields::SetSubjectL(const TDesC& aSubject)
	{
	HBufC* subject = aSubject.AllocL();
	delete iSubject;
	iSubject = subject;	
	}
	
/**
The subject field.

@return
The subject field.
*/
EXPORT_C const TDesC& CSmsEmailFields::Subject() const
	{
	return (iSubject) ? static_cast<const TDesC&>(*iSubject) : KNullDesC();
	}

/**
Restores the email fields.

If the store does not contain email fields data, the current email data is reset.

@param	aStore
The store to be read.

@internalComponent
*/
void CSmsEmailFields::RestoreL(CMsvStore& aStore)
	{
	if( aStore.IsPresentL(KUidMsvSmsEmailFieldsStream) )
		{
		// The email fields stream exists - internalise from the stream.
		RMsvReadStream in;
		in.OpenLC(aStore, KUidMsvSmsEmailFieldsStream);
		InternalizeL(in);
		CleanupStack::PopAndDestroy(&in);	
		}
	else
		{
		// There is no stream - reset the email fields.
		Reset();
		}
	}


/**
Stores the email fields.

The email fields data will only be stored if there is at least some address or
subject data. If there is neither address nor subject data and the store already
contains email fields data, then this existing data is removed.

@param	aStore
The store to be written.

@internalComponent
*/
void CSmsEmailFields::StoreL(CMsvStore& aStore) const
	{
	if( HasAddress() || Subject().Length() > 0 )
		{
		// The email fields have some data - store them.
		RMsvWriteStream out;	
		out.AssignLC(aStore, KUidMsvSmsEmailFieldsStream);
		ExternalizeL(out);
		out.CommitL();
		CleanupStack::PopAndDestroy(&out);
		}
	else if( aStore.IsPresentL(KUidMsvSmsEmailFieldsStream) )
		{
		// Ok, the current email fields have no data, but the email fileds 
		// stream exists - remove it.
		aStore.Remove(KUidMsvSmsEmailFieldsStream);
		}
	}
	
/**
Internalizes the email fields from the given stream.

@param	aStream
The stream to be read.
*/
void CSmsEmailFields::InternalizeL(RReadStream& aStream)
	{
	Reset();
	aStream.ReadInt16L();	// read the version - to local as causes warning in EABI...

	TCardinality cardinality;
	aStream >> cardinality;
	TInt count = cardinality;

	for( TInt i = 0; i < count; ++i )
		{
		HBufC* buf = HBufC::NewLC(aStream, KMaxTInt);
		AddAddressL(*buf);
		CleanupStack::PopAndDestroy(buf);
		}
	iSubject = HBufC::NewL(aStream, KMaxTInt);	
	}
	
/**
Externalizes the email fields into the given stream.

@param	aStream
The stream to be written.
*/
void CSmsEmailFields::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt16L(KMsvSmsEmailFieldsVersion);
	
	TInt count = iAddresses->Count();	
	aStream << TCardinality(count);
	for( TInt i = 0; i < count; ++i )
		{
		aStream << (*iAddresses)[i];
		}
	aStream << Subject();
	}

CSmsEmailFields::CSmsEmailFields()
: CBase()
	{
	}
	
void CSmsEmailFields::ConstructL()
	{
	iAddresses = new (ELeave) CDesCArrayFlat(KSmsEmailAddressesArrayGranularity); 	
	}

void CSmsEmailFields::ConstructL(const CSmsEmailFields& aEmailFields)
	{
	ConstructL();
	
	const MDesCArray& addresses = aEmailFields.Addresses();
	TInt count = addresses.MdcaCount();
	
	for( TInt i=0; i < count; ++i )
		{
		AddAddressL(addresses.MdcaPoint(i));		
		}
	iSubject = aEmailFields.Subject().AllocL();
	}

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)

/**
Stores the email fields in SQL DB.

The email fields data will only be stored if there is at least some address or
subject data. If there is neither address nor subject data and the store already
contains email fields data, then this existing data is removed.

@param	aStore
The store to be written.

@internalComponent
*/

void CSmsEmailFields::StoreDBL(CMsvStore& aStore) 
	{
	// This is the continuation of the header entry created 
	// in CSmsHeader::StoreDbL().
	
	if( HasAddress() || Subject().Length() > 0 )
		{
		CHeaderFields* emailOverSmsHeaderFields = new (ELeave)CHeaderFields();
		CleanupStack::PushL(emailOverSmsHeaderFields);
		emailOverSmsHeaderFields->iUid = KUidMsvSmsEmailFieldsStream;

		//----------------------EOS Info  6th Field ------------------------------	
		CFieldPair* smsdetailField = new (ELeave)CFieldPair();
		CleanupStack::PushL(smsdetailField);
		
		RBuf eosbuf;
		CleanupClosePushL(eosbuf);
		eosbuf.CreateL(GetBufSize()); 
		
		// Header version	
		eosbuf.AppendNum(KMsvSmsEmailFieldsVersion);
		eosbuf.Append(KDelimiter);
			
		// AddressCount	
		eosbuf.AppendNum(iAddresses->Count());
		eosbuf.Append(KDelimiter);

		// Addresses
		for (TInt i=0; i<iAddresses->Count(); i++)
			{
			eosbuf.Append(iAddresses->MdcaPoint(i));
			if(i<(iAddresses->MdcaCount())-1)
				eosbuf.Append(KComma);
			}
		
		eosbuf.Append(KDelimiter);
		
		// Subject
		eosbuf.Append(Subject());
		eosbuf.Append(KDelimiter);
		
		HBufC* newFrom1 = HBufC::NewL(eosbuf.Length());
		newFrom1->Des().Copy(eosbuf);
		smsdetailField->iFieldTextValue = newFrom1;
		emailOverSmsHeaderFields->iFieldPairList.AppendL(smsdetailField);
		
		eosbuf.Close();
		CleanupStack::Pop();  // Rbuf
		CleanupStack::Pop(smsdetailField);

		
		TMsvWriteStore storeWriter(aStore);
		storeWriter.AssignL(emailOverSmsHeaderFields);
		storeWriter.CommitL();
		
		CleanupStack::Pop(emailOverSmsHeaderFields);
		
		}
	else if( aStore.IsPresentL(KUidMsvSmsEmailFieldsStream) )
		{
		
		CHeaderFields* emailOverSmsHeaderFields = new (ELeave)CHeaderFields();
		CleanupStack::PushL(emailOverSmsHeaderFields);
		emailOverSmsHeaderFields->iUid = KUidMsvSmsEmailFieldsStream;

			
		CFieldPair* smsdetailField = new (ELeave)CFieldPair();
		CleanupStack::PushL(smsdetailField);
		smsdetailField->iFieldTextValue = KNullDesC().AllocL();
		emailOverSmsHeaderFields->iFieldPairList.AppendL(smsdetailField);
		CleanupStack::Pop(smsdetailField);

		TMsvWriteStore storeWriter(aStore);
		storeWriter.AssignL(emailOverSmsHeaderFields);
		storeWriter.CommitL();

		
		// Ok, the current email fields have no data, but the email fileds 
		// stream exists - remove it.
		aStore.Remove(KUidMsvSmsEmailFieldsStream);
		
		CleanupStack::Pop(emailOverSmsHeaderFields);
		}
	}

/**
To gett he Buffer size.
*/	
	
TInt CSmsEmailFields::GetBufSize()
	{
	TInt size  = 0;
	
	size += sizeof(KMsvSmsEmailFieldsVersion) + 1;  // 1 is for Delimiter
	size += Subject().Length() + 1 ;
	size += sizeof(iAddresses->Count()) + 1 ;
	
	for(TInt i=0; i<iAddresses->Count(); i++)
		{
		size += iAddresses->MdcaPoint(i).Length() + 1;  	// Extra byte for Comma
		}

	return size ;
	
	}



/**
Retrives the email fields from SQL DB.

If the store does not contain email fields data, the current email data is reset.

@param	aStore
The store to be read.

@internalComponent
*/

	
void CSmsEmailFields::ReStoreDBL(CMsvStore& aStore)
	{
	Reset();
	CHeaderFields* rcvEmailHeaderRow = NULL;
	TMsvReadStore storeReader(aStore, KUidMsvSmsEmailFieldsStream);
	TRAPD(err, storeReader.ReadL(rcvEmailHeaderRow));
	
	if(KErrNotFound == err)
		{
		return;
		}
	
	TInt i = 0;  //SMS Email over SMS 	
	HBufC* 	eosheaderinfo = rcvEmailHeaderRow->iFieldPairList[i++]->iFieldTextValue ;
	TPtr16 eosheaderPtr = (eosheaderinfo->Des());	

	//Parsing the eos string 
	TInt firstSemiColonPos = eosheaderPtr.Locate(';');
	TInt lastSemiColonPos = eosheaderPtr.LocateReverse(';');

	RPointerArray<HBufC16> eosinfoData;
	CleanupClosePushL(eosinfoData);
	TInt ii = 0 ;

	do
		{
		TPtrC16 str1 = eosheaderPtr.Left(firstSemiColonPos); // First data
		HBufC16* tt = str1.AllocLC();
		firstSemiColonPos++;
		eosinfoData.AppendL(tt); 
		CleanupStack::Pop();
		
		if(firstSemiColonPos != lastSemiColonPos)
			{
			eosheaderPtr = (eosheaderPtr.Mid(firstSemiColonPos,eosheaderPtr.Length()-(firstSemiColonPos) ));
			firstSemiColonPos = eosheaderPtr.Locate(';');
			lastSemiColonPos = eosheaderPtr.LocateReverse(';');
			if(firstSemiColonPos == lastSemiColonPos)
				{
				TPtrC16 str1 = eosheaderPtr.Left(eosheaderPtr.Length() -1); // Last data is Subject . End with Semicolon. -1 is to remove semicolon.
				HBufC16* tt = str1.AllocLC();
				eosinfoData.AppendL(tt);
				CleanupStack::Pop(); 
				}
			}
		else
			{
			//Empty field.
			HBufC16* tt = KNullDesC().AllocLC();
			eosinfoData.AppendL(tt);
			CleanupStack::Pop(); 
			}
		
		ii++;	
		}while(firstSemiColonPos != lastSemiColonPos);

	TInt16 headerversion = ConvertToTInt(*eosinfoData[0]);
	
	TInt count = ConvertToTInt(*eosinfoData[1]);
	
	if(count > 0)
		{
		_LIT16(KSemicolon, ";");
		HBufC16* addressesList = eosinfoData[2]->Des().AllocLC();
		addressesList = addressesList->ReAllocL(addressesList->Length()+2);
		TPtr16 addressesListPtr = (addressesList->Des());
		addressesListPtr.Append(KSemicolon);
		GetRecipientListL(addressesListPtr);
		CleanupStack::PopAndDestroy();
		}
	else
		{
		AddAddressL(KNullDesC16);
		}
	
	iSubject = eosinfoData[3]->Des().AllocL();
	
	
	eosinfoData.ResetAndDestroy();
	CleanupStack::PopAndDestroy();   //RPointerArray	
	}

 /* Convert a String to an Integer.
 * @param aStr A string to make Integer.
 * @return TInt A integer value 
 */
TInt CSmsEmailFields::ConvertToTInt(TDesC16& aStr)
	{
	TLex str(aStr);
	TInt32 string;
	str.Val(string);
	return string;
	}

/**
To parse the recipient address list.
*/

void CSmsEmailFields::GetRecipientListL(TDesC16& aStr)
	{
	TPtrC16 recvstr = aStr;
	
	if(KErrNotFound != recvstr.Locate(',') )
		{ // It has multipule recipient list is :-  example@nokia.com, example@nokia.com,example@nokia.com;
		TInt startPos = recvstr.Locate(',') ;
		while(1)
			{
			TPtrC16 partstrx = recvstr.Left(startPos);
			AddAddressL(partstrx);
			startPos++;
			recvstr.Set(recvstr.Mid(startPos, recvstr.Length()- startPos));
		
			startPos = recvstr.Locate(',');
			if(startPos == KErrNotFound)
				{
				TPtrC16 partstrxy;
				partstrxy.Set(recvstr.Mid(0,recvstr.Length()-1));
				AddAddressL(partstrxy);
				break;
				}
			}
		
		}
	else  // It has one recipient  list is like this->  example@nokia.com;
		{
		TInt lastpos = recvstr.Locate(';');	
		TPtrC16 partstrx = recvstr.Left(lastpos);
		AddAddressL(partstrx);
		}
	}
#endif
