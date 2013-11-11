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
// MIUTHDR.CPP
//

#include "MIUTHDR.H"
#include <msvstd.h>
#include <msvstore.h>	// CMsvStore
#include "MIUT_ERR.H"
#include "MIUTCONV.H"
#include <imcvcodc.h>
#include "cimencodedheader.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"
#include "cimconvertheader.h"
#include "miut_errconsts.h"
#endif

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
_LIT16(KComma, ",");
_LIT16(KDelimiter, ";");
#endif

const TInt KArrayGranularity = 16;

GLDEF_C TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize)
	{
	if (aString.Length() < aMaxSize)
		return aString;
	else
		return aString.Left(aMaxSize);
	}

GLDEF_C TPtrC16 LimitStringSize(const TPtrC16& aString, TInt aMaxSize)
	{
	if (aString.Length() < aMaxSize)
		return aString;
	else
		return aString.Left(aMaxSize);
	}

/**
// Ex/Internalize functions are not used at the moment, but have been left here since it is 
// very likely they will be needed in future. They are commented out to keep arm5 happy.

//----------------------------------------------------------------------------------------
LOCAL_C void ExternalizeL(const CArrayFix<TImHeaderEncodingInfo>& anArray,
								RWriteStream& aStream)
//----------------------------------------------------------------------------------------
	{
	TInt count;
	count=anArray.Count();

	aStream << TCardinality(count);		// compressed value
	for (TInt ii=count-1 ; ii>=0 ; --ii)
		{
		TImHeaderEncodingInfo info(anArray[ii]);
		info.ExternalizeL(aStream);
		}
	}
	
//----------------------------------------------------------------------------------------
LOCAL_C void InternalizeL(CArrayFix<TImHeaderEncodingInfo>& anArray, RReadStream& aStream)
//----------------------------------------------------------------------------------------
	{
	TCardinality card;
	aStream >> card;
	TInt count=card;
	anArray.Reset();
	for (TInt ii=0; ii<count ; ++ii)
		{
		TImHeaderEncodingInfo info;
		info.InternalizeL(aStream);
		anArray.InsertL(0,info);
		}
	}
*/	

#if defined (_UNICODE)

void ExternalizeL(const CDesC16Array& anArray,RWriteStream& aStream)
	{
	// Used in CImHeader and CImEncodedHeader
	TInt count=anArray.Count();
	aStream << TCardinality(count);		// compressed value
	for (TInt ii=0;ii<count;++ii)
		aStream << LimitStringSize(anArray[ii], KMaxImHeaderStringLength);
	}

void InternalizeL(CDesC16Array& anArray,RReadStream& aStream)
	{
	TCardinality card;
	aStream >> card;
	TInt count=card;
	anArray.Reset();
	for (TInt ii=0;ii<count;++ii)
		{
		HBufC16* buf=HBufC16::NewLC(aStream,KMaxImHeaderStringLength);
		anArray.CArrayFixBase::InsertL(ii,&buf);
		CleanupStack::Pop();
		}
	}

#endif

void ExternalizeL(const CDesC8Array& anArray,RWriteStream& aStream)
	{
	// Used in CImHeader , CImMimeHeader & CImMimePartData
	TInt count=anArray.Count();
	aStream << TCardinality(count);		// compressed value
	for (TInt ii=0;ii<count;++ii)
		aStream << LimitStringSize(anArray[ii], KMaxImHeaderStringLength);
	}

void InternalizeL(CDesC8Array& anArray,RReadStream& aStream)
	{
	TCardinality card;
	aStream >> card;
	TInt count=card;
	anArray.Reset();
	for (TInt ii=0;ii<count;++ii)
		{
		HBufC8* buf=HBufC8::NewLC(aStream,KMaxImHeaderStringLength);
		anArray.CArrayFixBase::InsertL(ii,&buf);
		CleanupStack::Pop();
		}
	}



//****************************************************************************************
//              Class TImHeaderEncodingInfo Functions
//****************************************************************************************

//----------------------------------------------------------------------------------------
EXPORT_C TImHeaderEncodingInfo::TImHeaderEncodingInfo() :  
		iType(ENoEncoding), iAddSpace(EFalse), iCharsetUid(KUidMsvCharsetNone)
//----------------------------------------------------------------------------------------
/** Default constructor. */
	{
	}

//----------------------------------------------------------------------------------------
EXPORT_C TImHeaderEncodingInfo::TImHeaderEncodingInfo(const TImHeaderEncodingInfo& aFrom) 
//----------------------------------------------------------------------------------------
/** Copy constructor.

@param aFrom Object to copy */
	{
	SetField( aFrom.Field() );
	SetOffset( aFrom.Offset() );
	SetLength( aFrom.Length() );
	SetEncodingType( aFrom.EncodingType() );
	SetArrayValue( aFrom.ArrayValue() );
	SetCharsetUid(aFrom.CharsetUid());
	SetAddSpace( aFrom.AddSpace() ); 
	SetEncodedLength( aFrom.EncodedLength() ); 
	}

//----------------------------------------------------------------------------------------
EXPORT_C void TImHeaderEncodingInfo::ExternalizeL( RWriteStream& aWriteStream ) const
//----------------------------------------------------------------------------------------
/** Externalises the object to the specified stream.

@param aWriteStream Stream to write to */
	{
	aWriteStream.WriteUint16L( Field() );
	aWriteStream.WriteUint16L( Offset() );
	aWriteStream.WriteUint16L( Length() );
	aWriteStream.WriteUint16L( EncodingType() );
	aWriteStream.WriteUint16L( ArrayValue() );
	aWriteStream.WriteUint32L( CharsetUid() );
	aWriteStream.WriteUint16L( AddSpace() );
	aWriteStream.WriteUint8L( EncodedLength() );
	}


//----------------------------------------------------------------------------------------
EXPORT_C void TImHeaderEncodingInfo::InternalizeL( RReadStream& aReadStream )
//----------------------------------------------------------------------------------------
/** Internalises the object from the specified stream.

@param aReadStream Stream to read from */
	{
	SetField( (TFieldList) aReadStream.ReadUint16L());
	SetOffset( aReadStream.ReadUint16L());
	SetLength( aReadStream.ReadUint16L());
	SetEncodingType( (TEncodingType) aReadStream.ReadUint16L());
	SetArrayValue( aReadStream.ReadUint16L());
	SetCharsetUid( aReadStream.ReadUint32L());
	SetAddSpace( aReadStream.ReadUint16L());
	SetEncodedLength( aReadStream.ReadUint8L());
	}

//****************************************************************************************
//              Class  CImHeader  Functions
//****************************************************************************************


CImHeader::CImHeader()
	{
	}

EXPORT_C CImHeader* CImHeader::NewLC()
/** Allocates and creates a new CImHeader object, leaving the object on the cleanup 
stack.

@return New CImHeader object */
	{
	// standard two-phase construction stuff...
	CImHeader* self = new (ELeave) CImHeader;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
	
void CImHeader::ConstructL()
	{
	iTo  = new (ELeave)CDesCArrayFlat( KArrayGranularity );
	iCc  = new (ELeave)CDesCArrayFlat( KArrayGranularity );	
	iBcc = new (ELeave)CDesCArrayFlat( KArrayGranularity );
	iEncodingInfo = new (ELeave) CArrayFixFlat<TImHeaderEncodingInfo>(KArrayGranularity);
	iResentTo  = new (ELeave)CDesCArrayFlat( KArrayGranularity );
	iResentCc  = new (ELeave)CDesCArrayFlat( KArrayGranularity );	
	iResentBcc = new (ELeave)CDesCArrayFlat( KArrayGranularity );
	iEncodedHeader = NULL;
	Reset();
	}

EXPORT_C CImHeader::~CImHeader()
/** Destructor. */
	{
	Reset();
	delete iTo;
	delete iCc;
	delete iBcc;
	delete iEncodingInfo;
	delete iResentTo;
	delete iResentCc;
	delete iResentBcc;
	delete iEncodedHeader;
	}

EXPORT_C void CImHeader::Reset()
/** Resets all header field values. */
	{
	iVersion = KImHeaderVersion;
	delete iFrom;
	iFrom = NULL;

	delete iSubject;
	iSubject = NULL;
	
	delete iImMsgId;
	iImMsgId = NULL;

	delete iReplyTo;
	iReplyTo = NULL;

	delete iReceipt;
	iReceipt = NULL;

	if (iTo)
		iTo->Reset();
	if (iCc)
		iCc->Reset();
	if (iBcc)
		iBcc->Reset();
	if (iEncodingInfo)
		iEncodingInfo->Reset();

	delete iResentFrom;
	iResentFrom=NULL;

	delete iResentMsgId;
	iResentMsgId=NULL;

	delete iInReplyTo;
	iInReplyTo=NULL;
	
	if (iResentTo)
		iResentTo->Reset();
	if (iResentCc)
		iResentCc->Reset();
	if (iResentBcc)
		iResentBcc->Reset();

	iBodyEncoding = EMsgOutboxMIME;

if (iEncodedHeader)
		iEncodedHeader->Reset();
	}

EXPORT_C const TPtrC CImHeader::Subject() const
/** Gets the "Subject" header field.

@return Field value */
	{
	return iSubject ? TPtrC(*iSubject) : TPtrC();
	}

EXPORT_C const TPtrC8 CImHeader::ImMsgId() const
/** Gets the "MessageId" header field.

@return Field value */
	{
	return iImMsgId ? TPtrC8(*iImMsgId) : TPtrC8();
	}

EXPORT_C const TPtrC CImHeader::From() const
/** Gets the "From" header field. 

This consists of an address and (possibly) an alias.

@return Field value */
	{
	return iFrom ? TPtrC(*iFrom) : TPtrC();
	}

EXPORT_C const TPtrC CImHeader::ReceiptAddress() const
/** Gets the "Receipt" header field. 

@return Field value */
	{
	return iReceipt ? TPtrC(*iReceipt) : TPtrC();
	}

EXPORT_C const TPtrC CImHeader::ReplyTo() const
/** Gets the "ReplyTo" header field. 

@return Field value */
	{
	return iReplyTo ? TPtrC(*iReplyTo) : TPtrC();
	}

EXPORT_C TUint CImHeader::Charset() const
/** Gets the character set to use when sending the message header.

If set, this overrides the default system character set for sending
the header. 

Character set and encoding options can also be set on a per header field
basis using TImHeaderEncodingInfo objects. See EncodingInfo().

@return Identifier for the character set. Character sets idenitifiers
are defined by the character conversion API in charconv.h. 
@see SetCharset()
@see EncodingInfo()
*/
	{
	return i822HeaderCharset;
	}

EXPORT_C void CImHeader::SetSubjectL( const TDesC8& aSubject )
/** Sets the "Subject" header field. 

@param aSubject Field value */
	{
#if defined (_UNICODE)
	HBufC* newSubject = HBufC::NewL(aSubject.Length());
	newSubject->Des().Copy(aSubject);
# else
	HBufC* newSubject = aSubject.Alloc();
#endif
	delete iSubject;
	iSubject = newSubject;
	}


EXPORT_C void CImHeader::SetImMsgIdL( const TDesC8& aImMsgId )
/** Sets the "MessageId" header field.

@param aImMsgId Field value
*/
	{
	HBufC8* newImMsgId = aImMsgId.Alloc();
	delete iImMsgId;
	iImMsgId = newImMsgId;
	}

EXPORT_C void CImHeader::SetFromL( const TDesC8& aFrom )
/** Sets the "From" header field. 

@param aFrom Field value */
	{
#if defined (_UNICODE)
	HBufC* newFrom = HBufC::NewL(aFrom.Length());
	newFrom->Des().Copy(aFrom);
# else
	HBufC* newFrom = aFrom.Alloc();
#endif
	delete iFrom;
	iFrom = newFrom;
	}


EXPORT_C void CImHeader::SetReplyToL( const TDesC8& aReplyTo )
/** Sets the "ReplyTo" header field. 

@param aReplyTo Field value */
	{
#if defined (_UNICODE)
	HBufC* newReplyTo = HBufC::NewL(aReplyTo.Length());
	newReplyTo->Des().Copy(aReplyTo);
# else
	HBufC* newReplyTo = aReplyTo.Alloc();
#endif
	delete iReplyTo;
	iReplyTo = newReplyTo;
	}


EXPORT_C void CImHeader::SetReceiptAddressL( const TDesC8& aReceipt )
/** Sets the "Receipt" header field. 

@param Field value
*/
	{
#if defined (_UNICODE)
	HBufC* newReceipt = HBufC::NewL(aReceipt.Length());
	newReceipt->Des().Copy(aReceipt);
# else
	HBufC* newReceipt = aReceipt.Alloc();
#endif
	delete iReceipt;
	iReceipt = newReceipt;
	}

EXPORT_C void CImHeader::ExternalizeL( RMsvWriteStream& aWriteStream ) const
/** Externalises the settings to a specified stream.

@param aWriteStream Stream to write to */
	{
	aWriteStream.WriteUint16L(Version());	// This MUST be the 1st item written into the stream

	aWriteStream << LimitStringSize(ReceiptAddress(), KMaxImHeaderStringLength);
	aWriteStream << LimitStringSize(ImMsgId(), KMaxImHeaderStringLength);
	aWriteStream << LimitStringSize(From(), KMaxImHeaderStringLength);
	aWriteStream << LimitStringSize(ReplyTo(), KMaxImHeaderStringLength);
	aWriteStream << LimitStringSize(Subject(), KMaxImHeaderStringLength);
	aWriteStream.WriteUint32L( iRemoteSize );

	aWriteStream << ToRecipients();
	aWriteStream << CcRecipients();
	aWriteStream << BccRecipients();
	aWriteStream << EncodingInfo();

	aWriteStream << LimitStringSize(ResentMsgId(), KMaxImHeaderStringLength);
	aWriteStream << LimitStringSize(ResentFrom(), KMaxImHeaderStringLength);
	aWriteStream << ResentToRecipients();
	aWriteStream << ResentCcRecipients();
	aWriteStream << ResentBccRecipients();
	aWriteStream << LimitStringSize(InReplyTo(), KMaxImHeaderStringLength);
	aWriteStream.WriteInt8L(BodyEncoding());
	aWriteStream.WriteUint32L(Charset());
	}


EXPORT_C void CImHeader::InternalizeL( RMsvReadStream& aReadStream )
/** Internalises the settings from a specified stream.

@param aReadStream Stream to read from */
	{
	Reset();

	iVersion = aReadStream.ReadUint16L();
	__ASSERT_DEBUG (iVersion == KImHeaderVersion, gPanic(EMiutBadStreamVersion));

	iReceipt = HBufC::NewL( aReadStream, KMaxImHeaderStringLength );
	iImMsgId = HBufC8::NewL( aReadStream, KMaxImHeaderStringLength );
	iFrom = HBufC::NewL( aReadStream, KMaxImHeaderStringLength );
	iReplyTo = HBufC::NewL( aReadStream, KMaxImHeaderStringLength );
	iSubject = HBufC::NewL( aReadStream, KMaxImHeaderStringLength );
	iRemoteSize=aReadStream.ReadUint32L();

	aReadStream >> ToRecipients();
	aReadStream >> CcRecipients();
	aReadStream >> BccRecipients();
	aReadStream >> EncodingInfo();

	iResentMsgId = HBufC8::NewL( aReadStream, KMaxImHeaderStringLength );
	iResentFrom = HBufC::NewL( aReadStream, KMaxImHeaderStringLength );
	aReadStream >> ResentToRecipients();
	aReadStream >> ResentCcRecipients();
	aReadStream >> ResentBccRecipients();
	iInReplyTo = HBufC8::NewL( aReadStream, KMaxImHeaderStringLength );
	iBodyEncoding = (TMsgOutboxBodyEncoding)aReadStream.ReadInt8L();
	i822HeaderCharset = aReadStream.ReadUint32L();
	}


EXPORT_C TInt CImHeader::DataSize()
/** Gets the combined length of all the field values stored. 

@return Combined length */
	{
	TInt headerSize = 2; // CRLF at end of header
	// speed-up by getting 'this' as a local variable...
	const CImHeader* self=this;
	headerSize+=self->From().Length();
	headerSize+=self->ReplyTo().Length();
	headerSize+=self->Subject().Length();
	headerSize+=self->ReceiptAddress().Length();
	headerSize+=sizeof(TTime);
	headerSize+=self->ImMsgId().Length();
	headerSize+=sizeof(TUid);				// size of Charset
// not added headerSize+=sizeof(TUint16); // iVersion

	TInt ii;
	CDesCArray& toArray = *self->iTo; 
	for ( ii = 0; ii < toArray.Count(); ii++)
		{
		headerSize+=toArray[ii].Length();
		}

	CDesCArray& ccArray = *self->iCc;
	for ( ii = 0; ii < ccArray.Count(); ii++)
		{
		headerSize+=ccArray[ii].Length(); 
		}

	CDesCArray& bccArray = *self->iBcc;
	for ( ii = 0; ii < bccArray.Count(); ii++)
		{
		headerSize+=bccArray[ii].Length();
		}

	if (iEncodedHeader)
		headerSize += iEncodedHeader->DataSize();

	return headerSize;
	}

EXPORT_C void CImHeader::StoreL( CMsvStore& aMessageStore ) const
/** Stores, but does not commit, settings to a specified message store.
	@param aMessageStore Message store to write to */
	{
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	
	if(aMessageStore.IsDbStore())
		{
		StoreDBL(aMessageStore);
		}
	else	
		{
		RMsvWriteStream out;
		out.AssignLC( aMessageStore, KUidMsgFileIMailHeader ); // pushes 'out' to the stack
		ExternalizeL(out);
		out.CommitL();
		CleanupStack::PopAndDestroy();
	if (iEncodedHeader)
		iEncodedHeader->StoreL(aMessageStore);	
		}
	
	
#else
	RMsvWriteStream out;
	out.AssignLC( aMessageStore, KUidMsgFileIMailHeader ); // pushes 'out' to the stack
	ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy();

	if (iEncodedHeader)
		iEncodedHeader->StoreL(aMessageStore);
#endif
	}


EXPORT_C void CImHeader::StoreWithoutCommitL( CMsvStore& aMessageStore ) const
/** Stores, but does not commit, settings to a specified message store.

@param aMessageStore Message store to write to */
	{
	StoreL(aMessageStore);
	}

EXPORT_C void CImHeader::RestoreL( CMsvStore& aMessageStore )
/** Restores settings from a specified message store.
@param aMessageStore Message store to read from */
	{
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	if(aMessageStore.IsDbStore())
		{
		ReStoreDBL(aMessageStore);
		}
	else
		{
		RMsvReadStream in;
		in.OpenLC( aMessageStore, KUidMsgFileIMailHeader ); // pushes 'in' to the stack
		InternalizeL(in);
		in.Close();// make sure we close the stream
		CleanupStack::PopAndDestroy();
		// Check if a stream for the encoded header exists.
		if (aMessageStore.IsPresentL(KUidMsgFileIMailEncodedHeader)) 
			{
			if (!iEncodedHeader)
				{
				iEncodedHeader = CImEncodedHeader::NewL();
				}
			iEncodedHeader->RestoreL(aMessageStore);
			}
		}
#else
	RMsvReadStream in;
	in.OpenLC( aMessageStore, KUidMsgFileIMailHeader ); // pushes 'in' to the stack
	InternalizeL(in);
	in.Close();// make sure we close the stream
	CleanupStack::PopAndDestroy();

	// Check if a stream for the encoded header exists.
	if (aMessageStore.IsPresentL(KUidMsgFileIMailEncodedHeader)) 
		{
		if (!iEncodedHeader)
			iEncodedHeader = CImEncodedHeader::NewL();
		iEncodedHeader->RestoreL(aMessageStore);
		}
#endif
	}


EXPORT_C void CImHeader::AddEncodingInfoL(TImHeaderEncodingInfo& aInfo)
/** Adds header field encoding information.

@param aInfo Header field encoding information */
	{
	iEncodingInfo->InsertL(0, aInfo);
	}

//-------------------------------------------------------------------------------------
//----------------------Used for forwarding/replying to an email ----------------------
/**
@deprecated
*/
EXPORT_C const TPtrC8 CImHeader::ResentMsgId() const
	{
	return iResentMsgId ? TPtrC8(*iResentMsgId) : TPtrC8();
	}

/**
@deprecated
*/
EXPORT_C const TPtrC CImHeader::ResentFrom() const
	{
	return iResentFrom ? TPtrC(*iResentFrom) : TPtrC();
	}

/** Gets the "In Reply To" header field.

For reply messages, this field stores the ID of the message to 
which this is a reply. It is set by CImHeader::CreateReplyL().

@return "In Reply To" header field
*/
EXPORT_C const TPtrC8 CImHeader::InReplyTo() const 
	{
	return iInReplyTo ? TPtrC8(*iInReplyTo) : TPtrC8();
	}

/**
@deprecated
*/
EXPORT_C void CImHeader::SetResentMsgIdL( const TDesC8& aResentMsgId)
{
	HBufC8* newResentMsgId = aResentMsgId.AllocL();
    delete iResentMsgId;
	iResentMsgId = newResentMsgId;
}

/**
@deprecated
*/
EXPORT_C void CImHeader::SetResentFromL( const TDesC& aResentFrom)
	{
	HBufC* newResentFrom = aResentFrom.AllocL();
    delete iResentFrom;
    iResentFrom = newResentFrom;
    }

/** Sets the "In Reply To" header field.

For reply messages, this field stores the ID of the message to 
which this is a reply.

@param aInReplyTo "In Reply To" header field
*/
EXPORT_C void CImHeader::SetInReplyToL( const TDesC8& aInReplyTo)
	{
	HBufC8* newInReplyTo = aInReplyTo.AllocL();
	delete iInReplyTo;
	iInReplyTo = newInReplyTo;
	}

void CImHeader::FormatSubjectL(CImHeader& aCImHeader, TDesC& aFormatString)
	{
	//only add to the subject line when there isn't a re: fwd: in the subject
	if (aFormatString.Length())
		{
		TInt formatPos=aFormatString.Find(KMiutFormatString);
		TInt foundInSubj=KErrNotFound;
		if (formatPos>0)
			foundInSubj=Subject().FindF(aFormatString.Left(formatPos-1)); // -1 to avoid % in the string
		//append only when it is not in the beginning of the subject line or not found.
		if (foundInSubj==KErrNotFound || foundInSubj)
			{
			HBufC* subject = HBufC::NewLC(Subject().Length() + aFormatString.Length());
			TPtr subjectPtr = subject->Des();
			subjectPtr.Format(TRefByValue<const TDesC>(aFormatString), iSubject);
			aCImHeader.SetSubjectL(subjectPtr);
			CleanupStack::PopAndDestroy(subject);
			return;
			}
		}
	aCImHeader.SetSubjectL(Subject());
	}

EXPORT_C TInt CImHeader::CreateForwardL(CImHeader& aCImHeader, TDesC& aSubjectField)
/** Populates a new forward header. 

The subject line passed in is used to construct the forward subject field value. This is then stored in the new header. 

@return	System wide error code
@param aCImHeader Header to populate
@param aSubjectField Subject line
*/
	{
	TInt error = KErrNone;
	aCImHeader.Reset();
	FormatSubjectL(aCImHeader,aSubjectField);
 
	// Copy the Encoding info to the new Header
 	for (TInt index = 0; index<iEncodingInfo->Count(); index++)
 		{
 		// If this Encoding Info is for the subject field, then make sure
 		// the new Encoding Info stores the New Subject Length
 		TImHeaderEncodingInfo encodingInfo = iEncodingInfo->At(index);
 		if (encodingInfo.Field() == TImHeaderEncodingInfo::ESubject)
 			{
 			TInt newSubjLength = aCImHeader.Subject().Length();
 			encodingInfo.SetLength(newSubjLength);
 			}
 		aCImHeader.AddEncodingInfoL(encodingInfo);
 		}

	return error;
	}

TBool CImHeader::IsRecipientPresent(CImHeader& aCImHeader, TPtrC16 newRecipient)
/** checks if the recipient already exists in "Cc" or "To" list

@return ETrue if duplicate exists, else EFalse
@param aCImHeader Contains "To" and "Cc" lists
@param newRecipient Recipient to be checked for duplicate
*/
	{
	
	TInt toCount = aCImHeader.ToRecipients().Count();
	for(TInt ii=0; ii<toCount; ++ii)
		{
		if((aCImHeader.ToRecipients()[ii].FindF(newRecipient)) >= 0)
			{
			return ETrue;
			}
		}

	TInt ccCount = aCImHeader.CcRecipients().Count();
	for(TInt jj=0; jj<ccCount; ++jj)
		{
		if((aCImHeader.CcRecipients()[jj].FindF(newRecipient)) >= 0)
			{
			return ETrue;
			}
		}

	return EFalse;
	}

/** Populates a new Reply header. 

The subject line passed in is used to construct the Reply subject field value. This is then stored in the new header. 

@return	System wide error code
@param aCImHeader Header to populate
@param aSubjectField Subject line
@param aReplyTo Reply-to address flag
*/
EXPORT_C TInt CImHeader::CreateReplyL(CImHeader& aCImHeader, TReplyTo aReplyTo, TDesC& aSubjectField)
	{
	aCImHeader.Reset();
	TInt error = KErrNone;
	if ((ResentFrom().Length() > 0) || (ResentToRecipients().Count() > 0))
		{
		switch (aReplyTo)
			{
		case EOriginator:
			// This may not go to the originator!
			// If the "Reply-To" field exists, then the reply  should go to the 
			//	addresses indicated in that field and not to the address(es) 
			//	indicated in the "From" field.
			// If there is a "From" field, but no "Reply-To" field, the reply 
			//	should be sent to the address(es) indicated in the "From" field.
			if (ReplyTo().Length() > 0)
				aCImHeader.ToRecipients().AppendL(ReplyTo());
			else
				aCImHeader.ToRecipients().AppendL(From());
			break;
		case ESender:
			aCImHeader.ToRecipients().AppendL(ResentFrom());
			break;
		case EAll:
			aCImHeader.ToRecipients().AppendL(From());	// not sure if this is needed but added from completeness!
			if(!IsRecipientPresent(aCImHeader, ResentFrom()))
				{
				aCImHeader.ToRecipients().AppendL(ResentFrom());
				}
			// don't need a break here - add To: and Cc: as well
		case ERecipients:
			TInt ii;
			for (ii=0; ii<ResentToRecipients().Count(); ii++)
				{
				if(!IsRecipientPresent(aCImHeader, ResentToRecipients()[ii]))
					{
					aCImHeader.ToRecipients().AppendL(ResentToRecipients()[ii]);
					}
				}
			for (ii=0; ii<ResentCcRecipients().Count(); ii++)
				{
				if(!IsRecipientPresent(aCImHeader, ResentCcRecipients()[ii]))
					{
					aCImHeader.CcRecipients().AppendL(ResentCcRecipients()[ii]);
					}
				}
			// Bcc recipients discarded as there should not be other Bcc recipients
			break;
			}
		}
	else
		{
		switch (aReplyTo)
			{
		case EOriginator:
		case ESender:
			if (ReplyTo().Length() > 0)
				aCImHeader.ToRecipients().AppendL(ReplyTo());
			else
				aCImHeader.ToRecipients().AppendL(From());
			break;
		case EAll:
			if (ReplyTo().Length()>0)
				aCImHeader.ToRecipients().AppendL(ReplyTo());
			else
				aCImHeader.ToRecipients().AppendL(From());
			TInt jj, count;	
			count = ToRecipients().Count();
			for (jj=0; jj<count; jj++)
				{
				if(!IsRecipientPresent(aCImHeader, ToRecipients()[jj]))
					{
					aCImHeader.CcRecipients().AppendL(ToRecipients()[jj]);
					}
				}
			count = CcRecipients().Count();	
			for (jj=0; jj<count; jj++)
				{
				if(!IsRecipientPresent(aCImHeader, CcRecipients()[jj]))
					{
					aCImHeader.CcRecipients().AppendL(CcRecipients()[jj]);
					}
				}
            break;
		case ERecipients:
			TInt ii;
			for (ii=0; ii<ToRecipients().Count(); ii++)
				{
				if(!IsRecipientPresent(aCImHeader, ToRecipients()[ii]))
					{
					aCImHeader.ToRecipients().AppendL(ToRecipients()[ii]);
					}
				}
			for (ii=0; ii<CcRecipients().Count(); ii++)
				{
				if(!IsRecipientPresent(aCImHeader, CcRecipients()[ii]))
					{
					aCImHeader.CcRecipients().AppendL(CcRecipients()[ii]);
					}
				}
			// Bcc recipients discarded as there should not be other Bcc recipients
			break;
			}
		}
		
	if (ImMsgId().Length()>0)
		aCImHeader.SetInReplyToL(ImMsgId());

	FormatSubjectL(aCImHeader,aSubjectField);
	return error;
	}

EXPORT_C void CImHeader::CreateReceiptL(CImHeader& aCImHeader, TDesC& aSubjectField)
/** Populates a Receipt email header.

@param aCImHeader Header to populate
@param aSubjectField Subject line
*/
	{
	aCImHeader.Reset();
	if (ReceiptAddress().Length())
		aCImHeader.ToRecipients().AppendL(ReceiptAddress());
	else
		aCImHeader.ToRecipients().AppendL(From());
	aCImHeader.SetInReplyToL(ImMsgId());
	FormatSubjectL(aCImHeader,aSubjectField);
	}

#if defined (_UNICODE)

EXPORT_C void CImHeader::SetSubjectL( const TDesC16& aSubject )
/** Sets the "Subject" header field. 

@param aSubject Field value */
	{
	HBufC16* newSubject = aSubject.AllocL(); 
	delete iSubject;
	iSubject = newSubject;
	}

EXPORT_C void CImHeader::SetFromL( const TDesC16& aFrom )
/** Sets the "From" header field. 

@param aFrom Field value */
	{
	HBufC16* newFrom = aFrom.AllocL();
	delete iFrom;
	iFrom = newFrom;
	}

EXPORT_C void CImHeader::SetReplyToL( const TDesC16& aReplyTo )
/** Sets the "ReplyTo" header field. 

@param aReplyTo Field value */
	{
	HBufC16* newReplyTo = aReplyTo.AllocL();
	delete iReplyTo;
	iReplyTo= newReplyTo;
	}

EXPORT_C void CImHeader::SetReceiptAddressL( const TDesC16& aReceipt )
/** Sets the "Receipt" header field. 

@param aReceipt "Receipt" header field
*/
	{
	HBufC16* newReceipt = aReceipt.AllocL();
	delete iReceipt;
	iReceipt = newReceipt;
	}
#endif

EXPORT_C void CImHeader::SetCharset(const TUint aCharset)
/** Sets the character set to use when sending the message header.

This setting overrides the default system character set for sending
the header. 

Character set and encoding options can also be set on a per header field
basis using TImHeaderEncodingInfo objects. See AddEncodingInfoL().

@param aCharset Identifier for the character set. Character sets idenitifiers
are defined by the character conversion API in charconv.h. 
@see Charset()
@see AddEncodingInfoL()
*/
	{
	i822HeaderCharset=aCharset;
	}

EXPORT_C TMsgOutboxBodyEncoding CImHeader::BodyEncoding() const
/** Gets the method of encoding the body of the email message.

The default value (EMsgOutboxMIME) is set so that text parts of the message
are sent as MIME multipart/alternative text/html parts, and are encoded using
UTF-8.
	
@see TMsgOutboxBodyEncoding

@return Method of encoding.
*/
	{
	return iBodyEncoding;
	}

/** Sets the method of encoding the body of the email message.

The default value (EMsgOutboxMIME) is set so that text parts of the message
are sent as MIME multipart/alternative text/html parts, and are encoded using
UTF-8.
	
@see TMsgOutboxBodyEncoding

@param aMessageBodyEncoding Method of encoding
*/
EXPORT_C void CImHeader::SetBodyEncoding(TMsgOutboxBodyEncoding aMessageBodyEncoding)
	{
	iBodyEncoding = aMessageBodyEncoding;
	}

//------------------------------------------------------------------------------

EXPORT_C void CImHeader::ReDecodeL(RFs& aFS)
/**
Decodes the original message data into the CImHeader fields using the override
character set. 

8 bit data MUST be decoded using the normal method initially before this method
is called.

@param aFS
A file server session handle.

@leave KErrNotSupported
Encoded header information has not been saved.
*/
	{
	if (!iEncodedHeader)
		User::Leave(KErrNotSupported);

	CCnvCharacterSetConverter* charsetConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(charsetConverter);

	CImConvertCharconv* charConv = 
		CImConvertCharconv::NewL(*charsetConverter, aFS);
	CleanupStack::PushL(charConv);

	CImConvertHeader* convertHeader = CImConvertHeader::NewL(*charConv);
	CleanupStack::PushL(convertHeader);
	
	convertHeader->SetOverrideCharset(iEncodedHeader->DecodeCharset());

	// Copy our fields with 8 bit data from CImEncodedHeader
	iEncodedHeader->CopyToHeaderL(*this);
	convertHeader->SetMessageType(ETrue);
	convertHeader->DecodeAllHeaderFieldsL(*this);

	CleanupStack::PopAndDestroy(3, charsetConverter);
	}

//------------------------------------------------------------------------------

EXPORT_C TUint CImHeader::OverrideCharset() const
/**
Returns the character set to be used when calling ReDecodeL.
*/
	{
	return (iEncodedHeader ? (iEncodedHeader->DecodeCharset()) : 0);
	}

//------------------------------------------------------------------------------

EXPORT_C void CImHeader::SetOverrideCharset(const TUint aCharset)
/**
Sets the character set to be used when calling ReDecodeL.

@param aCharset
The new character set.
*/
	{
	if (iEncodedHeader)
		iEncodedHeader->SetDecodeCharset(aCharset);
	}

//------------------------------------------------------------------------------

EXPORT_C void CImHeader::SaveEncodedHeadersL()
/**
Saves the 8 bit data in supported fields into a CImEncodedHeader. This allows
later redecoding with different character sets.

@internalTechnology
*/
	{
	if (!iEncodedHeader)
		iEncodedHeader = CImEncodedHeader::NewL();
	iEncodedHeader->CopyFromHeaderL(*this);
	}

//------------------------------------------------------------------------------
// Friend function of CImHeader, CImEncodedHeader
void CopyArrayL(const CDesCArray& aSource, CDesCArray& aDestination)
	{
	TInt count = aSource.Count();

	aDestination.Reset();
	while (count--)
		{
		TPtrC descriptor = aSource[count];
		if (descriptor.Length() == 0)
			continue;
		aDestination.AppendL(descriptor);
		}
	}

//------------------------------------------------------------------------------

//
//
//
CImMimeHeader::CImMimeHeader()
	{
	__DECLARE_NAME(_S("CImMimeHeader"));
	}

EXPORT_C CImMimeHeader* CImMimeHeader::NewLC()
/** Allocates and creates a new CImMimeHeader object, leaving the object on the 
cleanup stack.

@return New CImMimeHeader object */
	{
	// standard two-phase construction stuff...
	CImMimeHeader* self = new (ELeave) CImMimeHeader;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

EXPORT_C CImMimeHeader* CImMimeHeader::NewL()
/** Allocates and creates a new CImMimeHeader object.

@return New CImMimeHeader object */
	{
	CImMimeHeader* self = CImMimeHeader::NewLC();
	CleanupStack::Pop();
	return self;
	}

void CImMimeHeader::ConstructL()
	{
	iContentTypeParams  = new (ELeave)CDesC8ArrayFlat( 8 );
	iContentDispositionParams  = new (ELeave)CDesC8ArrayFlat( 8 );	
	iXTypeParams  = new (ELeave)CDesC8ArrayFlat( 8 );	
	Reset();
	}

EXPORT_C CImMimeHeader::~CImMimeHeader()
/** Destructor. */
	{
	delete iRelativePath;

	delete iContentType;
	delete iContentSubType;
	delete iContentDisposition;

	delete iContentDescription;

	delete iContentBase;
	delete iContentLocation;
	delete iContentID;

	// delete arrays
	delete iContentTypeParams;
	delete iContentDispositionParams;
	delete iXTypeParams;

	}

EXPORT_C void CImMimeHeader::InternalizeL( RMsvReadStream& aReadStream )
/** Internalises the settings from a specified stream.

@param aReadStream Stream to read from */
	{
	Reset();

	iVersion = aReadStream.ReadUint16L();
	__ASSERT_DEBUG (iVersion==KImMimeHeaderVersion, gPanic(EMiutBadStreamVersion));

	iRelativePath = HBufC8::NewL( aReadStream, KMaxImMimeFieldLength );

	iContentType = HBufC8::NewL( aReadStream, KMaxImMimeFieldLength );
	iContentSubType = HBufC8::NewL( aReadStream, KMaxImMimeFieldLength );

	iContentTransferEncoding = (TImEncodingType) aReadStream.ReadInt8L();
	iContentDisposition = HBufC8::NewL( aReadStream, KMaxImMimeFieldLength );

	iContentDescription = HBufC8::NewL( aReadStream, KMaxImMimeFieldLength );

	iContentBase = HBufC8::NewL( aReadStream, KMaxImMimeFieldLength );
	iContentLocation = HBufC16::NewL( aReadStream, KMaxImMimeFieldLength );
	iContentID = HBufC8::NewL( aReadStream, KMaxImMimeFieldLength );
	//
	aReadStream >> ContentTypeParams();
	aReadStream >> ContentDispositionParams();
	aReadStream >> XTypeParams();
	iMimeCharset = aReadStream.ReadUint32L();
	}


EXPORT_C void CImMimeHeader::ExternalizeL( RMsvWriteStream& aWriteStream ) const
/** Externalises the settings to a specified stream.

@param aWriteStream Stream to write to */
	{
	aWriteStream.WriteUint16L(Version());	// This MUST be the 1st item written into the stream

	aWriteStream << LimitStringSize(RelativePath(), KMaxImMimeFieldLength);
	//
	aWriteStream << LimitStringSize(ContentType(), KMaxImMimeFieldLength);
	aWriteStream << LimitStringSize(ContentSubType(), KMaxImMimeFieldLength);
	//
	aWriteStream.WriteInt8L(ContentTransferEncoding());
	aWriteStream << LimitStringSize(ContentDisposition(), KMaxImMimeFieldLength);

	aWriteStream << LimitStringSize(ContentDescription(), KMaxImMimeFieldLength);

	aWriteStream << LimitStringSize(ContentBase(), KMaxImMimeFieldLength);
	aWriteStream << LimitStringSize(ContentLocation(), KMaxImMimeFieldLength);
	aWriteStream << LimitStringSize(ContentID(), KMaxImMimeFieldLength);
	//
	aWriteStream << ContentTypeParams();
	aWriteStream << ContentDispositionParams();
	aWriteStream << XTypeParams();
	aWriteStream.WriteUint32L(MimeCharset());
	}



EXPORT_C void CImMimeHeader::Reset()
/** Resets all header field values. */
	{
	iVersion = KImMimeHeaderVersion;
	iContentTransferEncoding = EEncodingTypeUnknown;

	delete iRelativePath;
	iRelativePath=NULL;

	delete iContentType;
	iContentType=NULL;

	delete iContentSubType;
	iContentSubType=NULL;

	delete iContentDisposition;
	iContentDisposition=NULL;

	delete iContentDescription;
	iContentDescription=NULL;

	delete iContentBase;
	iContentBase=NULL;

	delete iContentLocation;
	iContentLocation=NULL;

	delete iContentID;
	iContentID=NULL;

	iContentTypeParams->Reset();
	iContentDispositionParams->Reset();
	iXTypeParams->Reset();
	}


EXPORT_C void CImMimeHeader::RestoreL( CMsvStore& aMessageStore )
/** Restores settings from a specified message store.

@param aMessageStore Message store to read from */
	{
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	if(aMessageStore.IsDbStore())
		{
		ReStoreMimeDBL(aMessageStore);
		}
	else
		{
		RMsvReadStream in;
		in.OpenLC( aMessageStore, KUidMsgFileMimeHeader ); // pushes 'in' to the stack
		InternalizeL(in);
		in.Close();// make sure we close the stream
		CleanupStack::PopAndDestroy();
		}	
#else
	RMsvReadStream in;
	in.OpenLC( aMessageStore, KUidMsgFileMimeHeader ); // pushes 'in' to the stack
	InternalizeL(in);
	in.Close();// make sure we close the stream
	CleanupStack::PopAndDestroy();
#endif
	}

 
EXPORT_C void CImMimeHeader::StoreL( CMsvStore& aMessageStore ) const
/** Stores settings to a specified message store.

@param aMessageStore Message store to write to */
	{
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	if(aMessageStore.IsDbStore())
		{
		StoreMimeDBL(aMessageStore);
		}
	else
		{
		RMsvWriteStream out;
		out.AssignLC( aMessageStore, KUidMsgFileMimeHeader ); // pushes 'out' to the stack
		ExternalizeL(out);
		out.CommitL();
		CleanupStack::PopAndDestroy();
		}	
#else
	RMsvWriteStream out;
	out.AssignLC( aMessageStore, KUidMsgFileMimeHeader ); // pushes 'out' to the stack
	ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy();
#endif
	}


EXPORT_C void CImMimeHeader::StoreWithoutCommitL( CMsvStore& aMessageStore ) const
/** Stores, but does not commit, settings to a specified message store.

@param aMessageStore Message store to write to */
	{
	RMsvWriteStream out;
	out.AssignLC( aMessageStore, KUidMsgFileMimeHeader ); // pushes 'out' to the stack
	ExternalizeL(out);
	out.CommitL();
	out.Close(); // make sure we close the stream
	CleanupStack::PopAndDestroy();
	}

EXPORT_C TInt CImMimeHeader::Size()
/** Estimates the size of the MIME header, constructed using the values stored.

@return Size (bytes) */
	{
	TInt size=sizeof (TUint16) + sizeof (TImEncodingType);
	if (iContentType)
		size+=ContentType().Length();
	if (iContentSubType)
		size+=ContentSubType().Length();
	if (iContentDisposition)
		size+=ContentDisposition().Length();
	if (iContentDescription)
		size+=ContentDescription().Length();

	if (iContentBase)
		size+=ContentBase().Length();
	if (iContentLocation)
		size+=ContentLocation().Length();
	if (iContentID)
		size+=ContentID().Length();

	TInt i;
	for (i=0 ; i<ContentTypeParams().Count(); ++i)
		size+=ContentTypeParams()[i].Length();

	for (i=0 ; i<ContentDispositionParams().Count(); ++i)
		size+=ContentDispositionParams()[i].Length();

	for (i=0 ; i<XTypeParams().Count(); ++i)
		size+=XTypeParams()[i].Length();

	return size;
	}


EXPORT_C void CImMimeHeader::SetContentTypeL(const TDesC8& aDesc)
/** Sets the Content-Type field value (e.g. "text" in Content-Type:text/plain). 

The value is MIME defined and should always be 8-bit. 

@param aDesc Field value
*/
	{
	HBufC8* buf = aDesc.AllocL();
	delete iContentType;
	iContentType = buf;
	}


EXPORT_C const TPtrC8 CImMimeHeader::ContentType() const
/** Gets the Content-Type field value (e.g. "text" in Content-Type:text/plain). 

@return Field value */
	{
	return iContentType ? TPtrC8(*iContentType) : TPtrC8();
	}


EXPORT_C void CImMimeHeader::SetContentSubTypeL(const TDesC8& aDesc)
/** Sets the Content-Type subtype field value (e.g. "plain" in Content-Type:text/plain). 

The value is MIME defined and should always be 8-bit. 

@param aDesc Field value */
	{
	HBufC8* buf = aDesc.AllocL();
	delete iContentSubType;
	iContentSubType = buf;
	}


EXPORT_C const TPtrC8 CImMimeHeader::ContentSubType() const
/** Gets the Content-Type subtype field value (e.g. "plain" in Content-Type:text/plain). 

The value is MIME defined and should always be 8-bit. 

@return Field value */
	{
	return iContentSubType ? TPtrC8(*iContentSubType) : TPtrC8();
	}


EXPORT_C void CImMimeHeader::SetContentDispositionL(const TDesC8& aDesc)
/** Sets the Content-Disposition field value (either inline or attachment). 

The value is MIME defined and should always be 8-bit. 

@param aDesc Field value */
	{
	HBufC8* buf = aDesc.AllocL();
	delete iContentDisposition;
	iContentDisposition = buf;
	}


EXPORT_C const TPtrC8 CImMimeHeader::ContentDisposition() const
/** Gets the Content-Disposition field value (either inline or attachment). 

The value is MIME defined and should always be 8-bit. 

@return Field value */
	{
	return iContentDisposition ? TPtrC8(*iContentDisposition) : TPtrC8();
	}


EXPORT_C void CImMimeHeader::SetContentDescriptionL(const TDesC8& aDesc)
/** Sets the Content-Description field value.

The value is MIME defined and should always be 8-bit. 

@param aDesc Field value */
	{
	HBufC8* buf = aDesc.AllocL();
	delete iContentDescription;
	iContentDescription = buf;
	}


EXPORT_C const TPtrC8 CImMimeHeader::ContentDescription() const
/** Gets the Content-Description field value.

The value is MIME defined and should always be 8-bit. 

@return Field value */
	{
	return iContentDescription ? TPtrC8(*iContentDescription) : TPtrC8();
	}


EXPORT_C void CImMimeHeader::SetContentBaseL(const TDesC8& aDesc)
/** Sets the Content-Base field value.

@param aDesc Field value */
	{
	HBufC8* buf = aDesc.AllocL();
	delete iContentBase;
	iContentBase = buf;
	}


EXPORT_C const TPtrC8 CImMimeHeader::ContentBase() const
/** Gets the Content-Base field value.

@return Field value */
	{
	return iContentBase ? TPtrC8(*iContentBase) : TPtrC8();
	}


EXPORT_C void CImMimeHeader::SetContentIDL(const TDesC8& aDesc)
/** Sets the Content-ID field value.

@param aDesc Field value */
	{
	HBufC8* buf = aDesc.AllocL();
	delete iContentID;
	iContentID = buf;
	}


EXPORT_C const TPtrC8 CImMimeHeader::ContentID() const
/** Gets the Content-ID field value.

@return Field value */
	{
	return iContentID ? TPtrC8(*iContentID) : TPtrC8();
	}

EXPORT_C void CImMimeHeader::SetContentLocationL(const TDesC16& aDesc)
/** Sets the Content-Location field value.

@param aDesc Field value */
	{
	HBufC16* buf = aDesc.AllocL();
	delete iContentLocation;
	iContentLocation = buf;
	}

EXPORT_C const TPtrC16 CImMimeHeader::ContentLocation() const
/** Gets the Content-Location field value.

@return Field value */
	{
	return iContentLocation ? TPtrC16(*iContentLocation) : TPtrC16();
	}

EXPORT_C void CImMimeHeader::SetContentTransferEncodingL(const TDesC8& aDesc)
/** Sets the Content-Transfer-Encoding field value.

@param aDesc Field value */
	{
	iContentTransferEncoding = EEncodingTypeUnknown;

	if (KErrNotFound!=aDesc.MatchF(KMiutWildcardBase64))
		iContentTransferEncoding = EEncodingTypeBASE64;
	else if (KErrNotFound!=aDesc.MatchF(KMiutWildcardQP))
		iContentTransferEncoding = EEncodingTypeQP;
	else if (KErrNotFound!=aDesc.MatchF(KMiutWildcard7Bit))
		iContentTransferEncoding = EEncodingType7Bit;
	else if (KErrNotFound!=aDesc.MatchF(KMiutWildcard8Bit))
		iContentTransferEncoding = EEncodingType8Bit;
	else if (KErrNotFound!=aDesc.MatchF(KMiutWildcardBinary))
		iContentTransferEncoding = EEncodingTypeBinary;
	else if (KErrNotFound!=aDesc.MatchF(KMiutWildcardUU))	// can't be right - there's no MIME standard text to describe UU is there??
		iContentTransferEncoding = EEncodingTypeUU;
	}


EXPORT_C TImEncodingType CImMimeHeader::ContentTransferEncoding() const
/** Gets the Content-Transfer-Encoding field value.

@return Field value */
	{
	return iContentTransferEncoding;
	}


EXPORT_C TImEncodingType CImMimeHeader::ContentTransferEncoding(TDes8& rType) const
/** Gets the Content-Transfer-Encoding field value as a string.

@param rType On return, the field value as a string
@return Field value */
	{
	switch (iContentTransferEncoding)
		{
	case EEncodingTypeBASE64:
		rType.Insert(0,KMiutBase64String);
		break;
	case EEncodingTypeQP:
		rType.Insert(0,KMiutQPString);
		break;
	case EEncodingType7Bit:
		rType.Insert(0,KMiut7BitString);
		break;
	case EEncodingType8Bit:
		rType.Insert(0,KMiut8BitString);
		break;
	case EEncodingTypeBinary:
		rType.Insert(0,KMiutBinaryString);
		break;
	case EEncodingTypeUU:
		rType.Insert(0,KMiutUUString);
		break;
	case EEncodingTypeNone:
	case EEncodingTypeUnknown:
	default:
		rType.Zero();
		break;
		}
	return iContentTransferEncoding;
	}


EXPORT_C const TPtrC8 CImMimeHeader::GetContentTypeValue(const TDesC8& aContentTypeParameter) const
/** Gets the value for a particular Content-Type parameter. 

For example, for "content-type: text/plain; name=umlaut.txt", GetContentType(_L8("name")) 
returns "umlaut.txt".

@param aContentTypeParameter Parameter type
@return Parameter value */
	{
	__ASSERT_DEBUG (!(iContentTypeParams->Count()&1), gPanic(EMiutArrayHasOddNumberOfElements));

	TInt result;
	if (KErrNone==iContentTypeParams->Find(aContentTypeParameter,result,ECmpFolded8))
		{
		result++;
		if ((result&1) && result < iContentTypeParams->Count())	// check result+1 is odd & entry exists
			{	
			return ContentTypeParams()[result];
			}
		}
	return TPtrC8();	// couldn't find match so return an empty descriptor
	}


// IMAP-specific info describing relative location of entry w.r.t. parent Email message

EXPORT_C void CImMimeHeader::SetRelativePathL(const TDesC8& aRelativePath)
/** Sets IMAP-specific information for the relative location of the entry to the 
parent email message.

@param aRelativePath Relative location */
	{
	HBufC8* newRelativePath = aRelativePath.AllocL();
	delete iRelativePath;
	iRelativePath = newRelativePath;
	}


EXPORT_C const TPtrC8 CImMimeHeader::RelativePath() const
/** Gets IMAP-specific information for the relative location of the entry to the 
parent email message.

@return Relative location */
	{
	return iRelativePath ? TPtrC8(*iRelativePath) : TPtrC8();
	}


EXPORT_C TUint CImMimeHeader::MimeCharset() const
/** Gets the character set used in decoding the email MIME part.

UID values for different MIME charsets are in charconv.h 

@return Character set */
	{
	return iMimeCharset;
	}

EXPORT_C void CImMimeHeader::SetMimeCharset(const TUint aCharset)
/** Sets the character set used in decoding the email MIME part.

UID values for different MIME charsets are in charconv.h 

@param aCharset Character set */
	{
	iMimeCharset = aCharset;
	}

//
//
//

EXPORT_C TInt TImRfc822DateField::ParseDateField(const TDesC8& aRfc822DateField, TTime& aTime)
/** Converts an RFC822 date field into a TTime value representing local time.  
Timezone information is added to represent time in local timezone.

@param aRfc822DateField The RFC822 formatted date field
@param aTime The returned local time
@return KErrNone on success, KErrGeneral otherwise */
	{
	// time syntax = [day ","] date time
	// with date = dd Mmm yy, although most engines produce dd Mmm ccyy
	// and  time = hh:mm[:ss] zone
	// where zone can be 'UT', 'GMT' etc but is usually "+"/"-" 4 digits HHMM,
	// giving the local differential from GMT
	
	// remove leading data from the received header field	
	TLex8 source( aRfc822DateField );
	TLexMark8 monthMark;
	TLexMark8 tzMark;
	TBool validDate = EFalse;
	TBool validTime = EFalse;
	TBool validZone = EFalse;
	TLocale localeTime;
	
	// leading whitespace
	source.SkipSpace();

	// possibly, day of week, comma & whitespace
	if ( source.Peek().IsAlpha() )
		{
		source.SkipCharacters();
		source.SkipSpace();
		}
	
	// now at the date
	TInt16 day=1;
	TInt16 year=0;

	TMonth month=EJanuary;
	TInt16 hh=0;
	TInt16 mm=0;
	TInt16 ss=0;
	TInt tzOffsetMins = 0;

	// here we expect to see a date with the format DD MMM YY
	if ( source.Peek().IsDigit() )
		validDate = (source.Val( day ) == KErrNone);
	
	if ( validDate )
		{
		source.SkipSpace();
		
		// start of the month triple
		source.Mark( monthMark );
		source.SkipCharacters( );
		TPtrC8 monthName( source.MarkedToken( monthMark ) );
		validDate = GetMonth( monthName, month );
		}
	
	if ( validDate )
		{
		source.SkipSpace();
		if ( source.Peek().IsDigit() )
			{
			validDate = (source.Val( year ) == KErrNone);
			}			
		else
			{
			validDate = EFalse;
			}			
		}
	
	if ( validDate )
		{
		// make sure we have a century.
		if ( year < KCenturyThreshold )
			if ( year < KCenturyOffset )
				{
				year += KNextCentury;
				}				
			else
				{
				year += KThisCentury;
				}				

		source.SkipSpace();
		// now at the time, hh:mm[:ss]
		if ( source.Peek().IsDigit() )
			{
			validTime = (source.Val( hh ) == KErrNone);
			}			
		else 
			{
			validTime = EFalse;
			}			
		}
	
	if ( validTime && source.Peek() == ':' )
		{
		source.Inc( 1 ); // :
		if ( source.Peek().IsDigit() )
			{
			validTime = (source.Val( mm ) == KErrNone);
			}			
		else 
			{
			validTime = EFalse;
			}			
		}
	else
		{
		validTime = EFalse;
		}
		

	if ( validTime && source.Peek() == ':' )
		{
		source.Inc( 1 ); // :
		if ( source.Peek().IsDigit() )
			{
			validTime = (source.Val ( ss ) == KErrNone);
			}				
		else 
			{
			validTime = EFalse;
			}			
		}

	
	if ( validTime )
		{
		source.SkipSpaceAndMark( tzMark ); // now pointing at timezone
		source.SkipCharacters();
		TPtrC8 tzInfo( source.MarkedToken( tzMark ));
		validZone = GetTimezone( tzInfo, tzOffsetMins );
		}
	

	TDateTime dateTime(1970, EJanuary, 1, 0, 0, 0, 0);		// year "zero" for Email app.
	TInt setDateError = KErrNone;
	if ( validDate && validTime && validZone)
		{
		if ((dateTime.SetYear( year )!=KErrNone)||(dateTime.SetMonth( month )!=KErrNone)||(dateTime.SetDay( day-1 )!=KErrNone))
			{
			setDateError = KErrGeneral;
			}			

		if ((dateTime.SetHour( hh )!=KErrNone)||(dateTime.SetMinute( mm )!=KErrNone)||(dateTime.SetSecond( ss )!=KErrNone))
			{
			setDateError = KErrGeneral;
			}
			
		aTime = dateTime;

		// Display local time (wherever mail was created).
		// Use zone info to reflect time of email relative to local time.
		aTime -= (TTimeIntervalMinutes)tzOffsetMins;
		
		}
	else
		aTime.UniversalTime();


	// if there are any errors while setting the date i.e an invalid date then the
	//	date/time is set to the current time/date.
	if (setDateError != KErrNone)
		{
		aTime.UniversalTime();
		}		

	// Return KErrNone if all OK, or KErrGeneral if not.
	// If the code to set validZone is re-instated,  then a test that 
	//  validZone is True should be added to the check below
	return (validDate && validTime && (setDateError == KErrNone)) ? KErrNone : KErrGeneral;	
	}

TBool TImRfc822DateField::GetMonth( const TDesC8& name, TMonth& month )
	{
	TBool validMonth = ETrue;
	TInt iLen;
	iLen = name.Length();
	if (iLen > 0)
		{
		switch ( name[0] )
			{
			case 'J': // jan , jun, jul
			case 'j':
				if (iLen > 1 && (name[1] == 'a' || name[1] == 'A')) 
					month = EJanuary; 
                else if (iLen > 2) 
                    month = ( ( name[2] == 'n' || name[2] == 'N' ) ? EJune : EJuly); 
                else 
                    validMonth = EFalse;
				break;
			case 'F': // feb
			case 'f':
				month = EFebruary;
				break;
			case 'M': // mar, may
			case 'm':
				if (iLen > 2) 
					month = ( (name[2] == 'r' || name[2] == 'R') ? EMarch : EMay); 
                else 
                    validMonth = EFalse;
				break;
			case 'A': // apr, aug
			case 'a':
				if (iLen > 1) 
                    month = ( (name[1] == 'p' || name[1] == 'P') ? EApril : EAugust); 
                else 
					validMonth = EFalse;
				break;
			case 'S': // sep
			case 's':
				month = ESeptember;
				break;
			case 'O': // oct
			case 'o':
				month = EOctober;
				break;
			case 'N': // nov
			case 'n':
				month = ENovember;
				break;
			case 'D': // dec
			case 'd':
				month = EDecember;
				break;
			default:
				validMonth = EFalse;
			}
		}
	else
		validMonth = EFalse;

	return validMonth;
	}


TBool TImRfc822DateField::GetTimezone( const TDesC8& name, TInt& minsOffset ) 
/** 
@deprecated since 6.1 */
	{
	TBool ret = ETrue;

	// just make sure there is any TZ info
	if ( name.Length() < 1 )
		{
		minsOffset = 0;
		return ret;
		}
	
	TChar c = name[0];

	if ( (name.Length() == 1) && c.IsAlpha() )	
		// the TZ specifier is the Military format
		{
		if ( c == 'Z' )
			minsOffset = 0;
		else if ( c >= 'A' && c < 'J' )
			minsOffset = 60*(64-c);
		else if ( c >= 'K' && c <= 'M' )
			minsOffset = 60*(65-c);
		else if ( c >= 'N' && c <= 'Y')
			minsOffset = 60*(c-(TUint)77);
		else
			return ( EFalse );
		}
	else // multi-char TZ name or +/- & HHMM
		{
		switch ( c )
			{
			case '+':
			case '-': // offset explicitly given as HHMM
				{
				TBool pos = (c == '+'); // grab +ve or -ve offset
				TLex8 HHMM( name );
				HHMM.Inc();
				TInt hhmm;
				ret = (HHMM.Val( hhmm ) == KErrNone);
				// check that the offset value is valid - I know that TZ values should be no more than 13 hours
				// but I've seen one of 20 hours (sign up on the mailing list at www.tudogs.com & you'll get one too)
				if ( !ret || hhmm >= 2400 ) 
					hhmm = 0;
				minsOffset = (hhmm / 100)*60; // hours
				minsOffset += hhmm - (hhmm / 100)*100; // mins
				minsOffset *= (pos ? 1 : -1);
				break;
				}
			case 'U': // Universal time, same as...
			case 'G': // good ol' GMT
				minsOffset = 0;
				break;
			case 'B': // BST
				minsOffset = 60;
				break;
			case 'E': // EST or EDT
				minsOffset = (name[1]=='S' ? -300 : -240);
				break;
			case 'C': // CST or CDT
				minsOffset = (name[1]=='S' ? -360 : -300);
				break;
			case 'M': // MST or MDT
				minsOffset = (name[1]=='S' ? -420 : -360);
				break;
			case 'P': // PST or PDT
				minsOffset = (name[1]=='S' ? -480 : -420);
				break;
			default: //For all the malformed values return false
				return (EFalse);
			}
		}

	return ret;
	}


//
//
//


EXPORT_C void TImRfc822DateField::SetDate(const TTime& aTimeDate, TDes8& rOutputLine)
/** Sets a descriptor to contain an RFC822 formatted date field from a TTime parameter.  
Timezone information is obtained from the locale settings.

@param aTimeDate The TTime value to convert
@param rOutputLine The returned RFC822 formatted date field*/
	{
	TBuf8<KMiutDateStringLength> timeString;

	// day of week
	TInt dayOfWeek = aTimeDate.DayNoInWeek();
	TPtrC8 dowPtr(TPtrC8(KMiutDayNames).Ptr()+dayOfWeek*3, 3);

	// date in format 1*2DIGIT month 2DIGIT
	TDateTime date = aTimeDate.DateTime();
	TPtrC8 monthPtr(TPtrC8(KMiutMonthNames).Ptr()+date.Month()*3,3);

	// produce a string in format Mon, 1 Sep 97 09:34:12
	// _LIT("%s, %2d %s %02d %02d:%02d:%02d ")
	timeString.Format(KMiutDateFormat, &dowPtr, (date.Day()+1), &monthPtr, date.Year(), date.Hour(), date.Minute(), date.Second());

	TTimeIntervalSeconds offsetTemp=User::UTCOffset();
	TInt32 offset = offsetTemp.Int();
	
	TBool negative = (offset < 0);
	if (negative)
		offset = -offset;
	
	TInt32 offsetHours = offset / 3600;
	TInt32 offsetMins = (offset - offsetHours * 3600) / 60;


	TBuf8<5> offsetString;
	if (negative)
		offsetString.Format(KMiutTimeZoneNeg, offsetHours, offsetMins);
	else
		offsetString.Format(KMiutTimeZonePos, offsetHours, offsetMins);
	
	rOutputLine.Append(timeString);
	rOutputLine.Append(offsetString);
	}



//
//
//



EXPORT_C TMsvEmailEntry::TMsvEmailEntry() : TMsvEntry()
/** Default constructor. */
	{
	// TMsvEmailEntry uses these general purpose data members to store flag info,
	// so make sure they are reset when the object is created.
	iMtmData1=0;
	iMtmData2=0;
	iMtmData3=0;
	iSize=0;
	}
//
// copy from a TMsvEntry to a TMsvEmailEntry
//

EXPORT_C TMsvEmailEntry::TMsvEmailEntry(const TMsvEntry& aGenericEntry) : TMsvEntry()
/** Copy constructor using a TMsvEntry object.

@param aGenericEntry Object to copy */
	{
	Mem::Copy(this,&aGenericEntry,sizeof(*this));
	}

//
// compare a TMsvEntry with this TMsvEmailEntry
//
EXPORT_C TBool TMsvEmailEntry::operator==(const TMsvEntry& aEntry) const
/** Equality operator for comparison to a TMsvEntry.

@param aEntry Object to compare against
@return True if equal */
	{
	TInt size=sizeof(*this)-2*sizeof(TPtrC);
	TBool equal = !(Mem::Compare((TUint8*)&aEntry, size, (TUint8*)this, size));
	equal &= (aEntry.iDescription==this->iDescription);
	equal &= (aEntry.iDetails==this->iDetails);
	return equal;
	}


//
// compare another TMsvEmailEntry with this TMsvEmailEntry
//
EXPORT_C TBool TMsvEmailEntry::operator==(const TMsvEmailEntry& aEntry) const
/** Equality operator for comparison to a TMsvEmailEntry.

@param aEntry Object to compare against
@return True if equal */
	{
	TInt size=sizeof(*this)-2*sizeof(TPtrC);
	TBool equal = !(Mem::Compare((TUint8*)&aEntry, size, (TUint8*)this, size));
	equal &= (aEntry.iDescription==this->iDescription);
	equal &= (aEntry.iDetails==this->iDetails);
	return equal;
	}


EXPORT_C void TMsvEmailEntry::GetIMAP4Flags(TBool& rUnread,TBool& rSeen,TBool& rAnswered,TBool& rFlagged,TBool& rDeleted,TBool& rDraft,TBool& rRecent)
/** Gets IMAP4-specific flags from the email entry: Unread, Seen, Answered, Flagged, 
Deleted, Draft and Recent.

@param rUnread Unread flag
@param rSeen Seen flag
@param rAnswered Answered flag
@param rFlagged Flagged flag
@param rDeleted Deleted flag
@param rDraft Draft flag
@param rRecent Recent flag */
	{
	rUnread=UnreadIMAP4Flag();
	rSeen=SeenIMAP4Flag();
	rAnswered=AnsweredIMAP4Flag();
	rFlagged=FlaggedIMAP4Flag();
	rDeleted=DeletedIMAP4Flag();
	rDraft=DraftIMAP4Flag();
	rRecent=RecentIMAP4Flag();
	}


EXPORT_C void TMsvEmailEntry::SetIMAP4Flags(TBool aUnread,TBool aSeen,TBool aAnswered,TBool aFlagged,TBool aDeleted,TBool aDraft,TBool aRecent)
/** Sets IMAP4-specific flags from the email entry: Unread, Seen, Answered, Flagged, 
Deleted, Draft and Recent.

@param aUnread Unread flag
@param aSeen Seen flag
@param aAnswered Answered flag
@param aFlagged Flagged flag
@param aDeleted Deleted flag
@param aDraft Draft flag
@param aRecent Recent flag */
	{
	SetUnreadIMAP4Flag(aUnread);
	SetSeenIMAP4Flag(aSeen);
	SetAnsweredIMAP4Flag(aAnswered);
	SetFlaggedIMAP4Flag(aFlagged);
	SetDeletedIMAP4Flag(aDeleted);
	SetDraftIMAP4Flag(aDraft);
	SetRecentIMAP4Flag(aRecent);
	}


EXPORT_C void TMsvEmailEntry::SetMessageFolderType(TImEmailFolderType aFolderType)
/** Sets a flag on an IMAP MTM folder entry indicating that it stores a 
multipart body structure.

Multipart body structure parts are represented in the message 
store by a folder entry with this flag set to the appropriate 
multipart type string.

@param aFolderType Folder type */
	{
	// Quick-implementation of this Set function
	// Final version should use more compact strategy to set/reset bytes
	// as opposed to using mutually-exclusive flags

	iMtmData1 = iMtmData1 & ~KMsvMimeFolderMask;	// reset all bits
	TInt32 folderMask;

	switch (aFolderType)
		{
	case EFolderTypeRelated:
		folderMask=KMsvMimeFolderRelatedMask;
		break;
	case EFolderTypeMixed:
		folderMask=KMsvMimeFolderMixedMask;
		break;
	case EFolderTypeAlternative:
		folderMask=KMsvMimeFolderAlternativeMask;
		break;
	case EFolderTypeParallel:
		folderMask=KMsvMimeFolderParallelMask;
		break;
	case EFolderTypeDigest:
		folderMask=KMsvMimeFolderDigestMask;
		break;
	case EFolderTypePartial:
		folderMask=KMsvMimeFolderPartialMask;
		break;
	case EFolderTypeDirectory:
		folderMask=KMsvMimeFolderDirectoryMask;
		break;
	case EFolderTypeExternal:
		folderMask=KMsvMimeFolderExternalMask;
		break;
	case EFolderTypeRFC822:
		folderMask=KMsvMimeFolderRFC822Mask;
		break;
	case EFolderTypeUnknown:
	default:
		folderMask=KMsvMimeFolderUnknownMask;
		break;
		}
	iMtmData1|=folderMask;	// set new folder flags
	}


EXPORT_C TImEmailFolderType TMsvEmailEntry::MessageFolderType() const
/** Gets a flag on an IMAP MTM folder entry indicating that it stores a 
multipart body structure.

Multipart body structure parts are represented in the message 
store by a folder entry with this flag set to the appropriate  
multipart type string.

@return Folder type */
	{
	TInt32 folderType = iMtmData1 & KMsvMimeFolderMask;
	switch (folderType)
		{
	case TMsvEmailEntry::KMsvMimeFolderRelatedMask:
		return EFolderTypeRelated;

	case TMsvEmailEntry::KMsvMimeFolderMixedMask:
		return EFolderTypeMixed;

	case TMsvEmailEntry::KMsvMimeFolderAlternativeMask:
		return EFolderTypeAlternative;

	case TMsvEmailEntry::KMsvMimeFolderParallelMask:
		return EFolderTypeParallel;

	case TMsvEmailEntry::KMsvMimeFolderDigestMask:
		return EFolderTypeDigest;

	case TMsvEmailEntry::KMsvMimeFolderPartialMask:
		return EFolderTypePartial;

	case TMsvEmailEntry::KMsvMimeFolderDirectoryMask:
		return EFolderTypeDirectory;

	case TMsvEmailEntry::KMsvMimeFolderExternalMask:
		return EFolderTypeExternal;

	case TMsvEmailEntry::KMsvMimeFolderRFC822Mask:
		return EFolderTypeRFC822;

	case TMsvEmailEntry::KMsvMimeFolderUnknownMask:	// drop thru' to default state...
	default:
		return EFolderTypeUnknown;
		}
	}

EXPORT_C void TMsvEmailEntry::SetDisconnectedOperation(TImDisconnectedOperationType aDisconnectedOperationType)
/** Sets the disconnected operation type.

@param aDisconnectedOperationType Disconnected operation type */
	{
	iMtmData1 = iMtmData1 & ~KMsvEmailEntryDisconnectedOperation;	// reset all bits
	TInt32 disconnectedOperation;

	switch (aDisconnectedOperationType)
		{
		case ENoDisconnectedOperations:
			disconnectedOperation = KMsvEmailEntryNoDisconnectedOperations;
			break;
		case EDisconnectedCreateOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedCreateOperation;
			break;
		case EDisconnectedDeleteOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedDeleteOperation;
			break;
		case EDisconnectedChangeOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedChangeOperation;
			break;
		case EDisconnectedCopyToOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedCopyToOperation;
			break;
		case EDisconnectedCopyFromOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedCopyFromOperation;
			break;
		case EDisconnectedCopyWithinServiceOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedCopyWithinServiceOperation;
			break;
		case EDisconnectedMoveToOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedMoveToOperation;
			break;
		case EDisconnectedMoveFromOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedMoveFromOperation;
			break;
		case EDisconnectedMoveWithinServiceOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedMoveWithinServiceOperation;
			break;
		case EDisconnectedSpecialOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedSpecialOperation;
			break;
		case EDisconnectedMultipleOperation:
			disconnectedOperation = KMsvEmailEntryDisconnectedMultipleOperation;
			break;
		case EDisconnectedUnknownOperation:
		default:
			disconnectedOperation = KMsvEmailEntryDisconnectedUnknownOperation;
			break;
		}
	iMtmData1|=disconnectedOperation;	// set new disconnected operation flags
	}

EXPORT_C TImDisconnectedOperationType TMsvEmailEntry::DisconnectedOperation() const
/** Gets the disconnected operation type.

@return Disconnected operation type */
	{
	TInt32 disconnectedOperation = iMtmData1 & KMsvEmailEntryDisconnectedOperation;
	switch (disconnectedOperation)
		{
		case TMsvEmailEntry::KMsvEmailEntryNoDisconnectedOperations:
			return ENoDisconnectedOperations;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedCreateOperation:
			return EDisconnectedCreateOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedDeleteOperation:
			return EDisconnectedDeleteOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedChangeOperation:
			return EDisconnectedChangeOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedCopyToOperation:
			return EDisconnectedCopyToOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedCopyFromOperation:
			return EDisconnectedCopyFromOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedCopyWithinServiceOperation:
			return EDisconnectedCopyWithinServiceOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedMoveToOperation:
			return EDisconnectedMoveToOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedMoveFromOperation:
			return EDisconnectedMoveFromOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedMoveWithinServiceOperation:
			return EDisconnectedMoveWithinServiceOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedSpecialOperation:
			return EDisconnectedSpecialOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedMultipleOperation:
			return EDisconnectedMultipleOperation;

		case TMsvEmailEntry::KMsvEmailEntryDisconnectedUnknownOperation: // drop thru' to default state...
		default:
			return EDisconnectedUnknownOperation;
		}
	}

EXPORT_C TBool TMsvEmailEntry::PartialDownloaded() const
/** Tests if body text is partially downloaded. 

@return True if body text is partially downloaded. */
	{
	return iMtmData1 & KMsvEmailEntryPartialDownloadFlag;
	}

EXPORT_C void TMsvEmailEntry::SetPartialDownloaded(TBool aPartialDownloaded)
/** Sets if the body text is partially downloaded.

@param aPartialDownloaded True if body text is partially downloaded. */
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryPartialDownloadFlag) | (aPartialDownloaded?KMsvEmailEntryPartialDownloadFlag:KMsvEmailEntryClearFlag);
	}


#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
/**
Stores message header to a specified message store.

@param aMessageStore Message store to write to
@return None.
*/
void CImHeader::StoreDBL(CMsvStore& aMessageStore) const
	{
	_LIT(KDetails,"");

	CHeaderFields* emailHeaderFields = new (ELeave)CHeaderFields();
	CleanupStack::PushL(emailHeaderFields);
	emailHeaderFields->iUid = KUidMsgFileIMailHeader;
	
	//version 0
	CFieldPair* emailHeaderVersionfield = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailHeaderVersionfield);
	emailHeaderVersionfield->iFieldNumValue = Version();
	emailHeaderFields->iFieldPairList.AppendL(emailHeaderVersionfield);
	CleanupStack::Pop(emailHeaderVersionfield);

	//ReceiptAddress 1
	CFieldPair* emailReceiptAddress = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailReceiptAddress);
	emailReceiptAddress->iFieldTextValue = (LimitStringSize(ReceiptAddress(), KMaxImHeaderStringLength)).AllocL();
	emailHeaderFields->iFieldPairList.AppendL(emailReceiptAddress);
	CleanupStack::Pop(emailReceiptAddress);
	
	CFieldPair* emailImMsgId = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailImMsgId);
	emailImMsgId->iFieldTextValue = Convert8to16L(LimitStringSize(ImMsgId(), KMaxImHeaderStringLength));;
	emailHeaderFields->iFieldPairList.AppendL(emailImMsgId);
	CleanupStack::Pop(emailImMsgId);
	
	//From 3
	CFieldPair* emailFrom = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailFrom);
	emailFrom->iFieldTextValue = (LimitStringSize(From(), KMaxImHeaderStringLength)).AllocL();
	emailHeaderFields->iFieldPairList.AppendL(emailFrom);
	CleanupStack::Pop(emailFrom);

	//ReplyTo 4
	CFieldPair* emailReplyTo = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailReplyTo);
	emailReplyTo->iFieldTextValue = (LimitStringSize(ReplyTo(), KMaxImHeaderStringLength)).AllocL();
	emailHeaderFields->iFieldPairList.AppendL(emailReplyTo);
	CleanupStack::Pop(emailReplyTo);
	
	//ESubject 5
	CFieldPair* emailSubject = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailSubject);
	emailSubject->iFieldTextValue = (LimitStringSize(Subject(), KMaxImHeaderStringLength)).AllocL();
	emailHeaderFields->iFieldPairList.AppendL(emailSubject);
	CleanupStack::Pop(emailSubject);

	//iRemoteSize 6
	CFieldPair* emailRemoteSize = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailRemoteSize);
	emailRemoteSize->iFieldNumValue = iRemoteSize;
	emailHeaderFields->iFieldPairList.AppendL(emailRemoteSize);
	CleanupStack::Pop(emailRemoteSize);
	
	//ToRecipients 7
	CFieldPair* emailToRecipients = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailToRecipients);
	
	RBuf16 torecipients;
	CleanupClosePushL(torecipients);
	CreateBufferL(torecipients, *iTo);
	
	if(iTo->MdcaCount()>0)
		{
		emailToRecipients->iFieldTextValue = torecipients.AllocL();	
		}
	else
		{
		emailToRecipients->iFieldTextValue = KNullDesC().AllocL();		
		}
	
	emailHeaderFields->iFieldPairList.AppendL(emailToRecipients);
	torecipients.Close();
	CleanupStack::Pop(); 		//torecipients  
	CleanupStack::Pop(emailToRecipients);	//emailToRecipients

	
	//CcRecipients 8
	CFieldPair* emailCcRecipients = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailCcRecipients);

	RBuf16 ccrecipients;
	CleanupClosePushL(ccrecipients);
	CreateBufferL(ccrecipients, *iCc);
	if(iCc->MdcaCount() >0)
		{
		emailCcRecipients->iFieldTextValue = ccrecipients.AllocL();	
		}
	else
		{
		emailCcRecipients->iFieldTextValue = KNullDesC().AllocL();	
		}
	
	emailHeaderFields->iFieldPairList.AppendL(emailCcRecipients);
	ccrecipients.Close();
	CleanupStack::Pop();			//ccrecipients
	CleanupStack::Pop(emailCcRecipients);		//emailCcRecipients
	
	//BccRecipients 9
	CFieldPair* emailBCcRecipients = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailBCcRecipients);

	RBuf16 bccrecipients;
	CleanupClosePushL(ccrecipients);
	CreateBufferL(bccrecipients, *iBcc);
	if(iBcc->MdcaCount() >0)
		{
		emailBCcRecipients->iFieldTextValue = bccrecipients.AllocL();	
		}
	else
		{
		emailBCcRecipients->iFieldTextValue = KNullDesC().AllocL();	
		}
	
	emailHeaderFields->iFieldPairList.AppendL(emailBCcRecipients);
	bccrecipients.Close();
	CleanupStack::Pop();					//bccrecipients
	CleanupStack::Pop(emailBCcRecipients);	//emailBCcRecipients 
	
	//EncodingInfo 10
	CFieldPair* emailEncodingInfo = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailEncodingInfo);
	
	RBuf16 encodinginfo;
	CleanupClosePushL(encodinginfo);
	encodinginfo.CreateL(iEncodingInfo->Count() * sizeof(TImHeaderEncodingInfo));
		
	for (TInt i=0; i<iEncodingInfo->Count(); i++)
		{
		encodinginfo.AppendNum(iEncodingInfo->At(i).Field());
		encodinginfo.Append(KComma);	
		encodinginfo.AppendNum(iEncodingInfo->At(i).Offset());
		encodinginfo.Append(KComma);
		encodinginfo.AppendNum(iEncodingInfo->At(i).Length());
		encodinginfo.Append(KComma);
		encodinginfo.AppendNum(iEncodingInfo->At(i).EncodingType());
		encodinginfo.Append(KComma);
		encodinginfo.AppendNum(iEncodingInfo->At(i).ArrayValue());
		encodinginfo.Append(KComma);
		encodinginfo.AppendNum(iEncodingInfo->At(i).CharsetUid());
		encodinginfo.Append(KComma);
		encodinginfo.AppendNum(iEncodingInfo->At(i).AddSpace()); 
		encodinginfo.Append(KComma);
		encodinginfo.AppendNum(iEncodingInfo->At(i).EncodedLength()); 
		encodinginfo.Append(KDelimiter);
		}
	
	emailEncodingInfo->iFieldTextValue = encodinginfo.AllocL();
	emailHeaderFields->iFieldPairList.AppendL(emailEncodingInfo);
	encodinginfo.Close();
	CleanupStack::Pop();					//encodinginfo
	CleanupStack::Pop(emailEncodingInfo);
	
	//ResentMsgId 11
	CFieldPair* emailResentMsgId = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailResentMsgId);
	emailResentMsgId->iFieldTextValue = Convert8to16L(LimitStringSize(ResentMsgId(), KMaxImHeaderStringLength));;
	emailHeaderFields->iFieldPairList.AppendL(emailResentMsgId);
	CleanupStack::Pop(emailResentMsgId);
	
	//ResentFrom 12
	CFieldPair* emailResentForm = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailResentForm);
	emailResentForm->iFieldTextValue = (LimitStringSize(ResentFrom(), KMaxImHeaderStringLength).AllocL());
	emailHeaderFields->iFieldPairList.AppendL(emailResentForm);
	CleanupStack::Pop(emailResentForm);
	
	//ResentToRecipients 13
	CFieldPair* emailResentToRecipients= new (ELeave)CFieldPair();
	CleanupStack::PushL(emailResentToRecipients);

	RBuf16 resentToRecipientsbuf;
	CleanupClosePushL(resentToRecipientsbuf);
	CreateBufferL(resentToRecipientsbuf, *iResentTo);
	if(iResentTo->MdcaCount() >0)
		{
		emailResentToRecipients->iFieldTextValue = resentToRecipientsbuf.AllocL();	
		}
	else
		{
		emailResentToRecipients->iFieldTextValue = KNullDesC().AllocL();	
		}
	
	emailHeaderFields->iFieldPairList.AppendL(emailResentToRecipients);
	resentToRecipientsbuf.Close();
	CleanupStack::Pop();						//resentToRecipientsbuf
	CleanupStack::Pop(emailResentToRecipients);

	//ResentCcRecipients 14
	CFieldPair* emailResentCcRecipients= new (ELeave)CFieldPair();
	CleanupStack::PushL(emailResentCcRecipients);
	
	RBuf16 resentccRecipientsbuf;
	CleanupClosePushL(resentccRecipientsbuf);
	CreateBufferL(resentccRecipientsbuf, *iResentCc);
	
	if(iResentCc->MdcaCount() >0)
		{
		emailResentCcRecipients->iFieldTextValue = resentccRecipientsbuf.AllocL();	
		}
	else
		{
		emailResentCcRecipients->iFieldTextValue = KNullDesC().AllocL();	
		}

	emailHeaderFields->iFieldPairList.AppendL(emailResentCcRecipients);
	resentccRecipientsbuf.Close();
	CleanupStack::Pop();						//resentccRecipientsbuf
	CleanupStack::Pop(emailResentCcRecipients);

	//ResentBccRecipients 15
	CFieldPair* emailResentBCcRecipients= new (ELeave)CFieldPair();
	CleanupStack::PushL(emailResentBCcRecipients);

	RBuf16 resentbccRecipientsbuf;
	CleanupClosePushL(resentbccRecipientsbuf);	
	CreateBufferL(resentbccRecipientsbuf, *iResentBcc);
	
	if(iResentCc->MdcaCount() >0)
		{
		emailResentBCcRecipients->iFieldTextValue = resentbccRecipientsbuf.AllocL();	
		}
	else
		{
		emailResentBCcRecipients->iFieldTextValue = KNullDesC().AllocL();	
		}
	emailHeaderFields->iFieldPairList.AppendL(emailResentBCcRecipients);
	resentbccRecipientsbuf.Close();
	CleanupStack::Pop();						//resentbccRecipientsbuf
	CleanupStack::Pop(emailResentBCcRecipients);
	
	//InReplyTo 16
	CFieldPair* emailInReplyTo = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailInReplyTo);
	emailInReplyTo->iFieldTextValue = Convert8to16L(LimitStringSize(InReplyTo(), KMaxImHeaderStringLength));
	emailHeaderFields->iFieldPairList.AppendL(emailInReplyTo);
	CleanupStack::Pop(emailInReplyTo);

	//BodyEncoding 17
	CFieldPair* emailBodyEncoding = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailBodyEncoding);
	emailBodyEncoding->iFieldNumValue = BodyEncoding();
	emailHeaderFields->iFieldPairList.AppendL(emailBodyEncoding);
	CleanupStack::Pop(emailBodyEncoding);

	//Charset 18
	CFieldPair* emailCharset = new (ELeave)CFieldPair();
	CleanupStack::PushL(emailCharset);
	emailCharset->iFieldNumValue = Charset();
	emailHeaderFields->iFieldPairList.AppendL(emailCharset);
	CleanupStack::Pop(emailCharset);
	
	//Detail This is used for MIME and Encoded Filed, 
	//We are Storing MIME and Encoded data in this Filed.
	CFieldPair* emaildetailField = new (ELeave)CFieldPair();
	CleanupStack::PushL(emaildetailField);
	emaildetailField->iFieldName =  KDetails().AllocL();
	emaildetailField->iFieldType = ETextField;
	emaildetailField->iFieldTextValue = KNullDesC().AllocL();
	emailHeaderFields->iFieldPairList.AppendL(emaildetailField);
	CleanupStack::Pop(emaildetailField);
	
	TMsvWriteStore storeWriter(aMessageStore);
	storeWriter.AssignL(emailHeaderFields);
	
	if (iEncodedHeader)
		{
		iEncodedHeader->StoreEncodedDBL(aMessageStore);
		}
		
	storeWriter.CommitL();
	CleanupStack::Pop(emailHeaderFields);
	}

/**
 * Create the buffer size.
 * @param abuf Buffer to be created.
 * @param ainto Array of iTo,iCc,iBcc.
 * @return None.
 */

void CImHeader::CreateBufferL( RBuf16& aBuf, CDesCArray& aRecipients)const
	{
	TInt size = 0;
	TInt count = 0;
	for(count=0 ; count < aRecipients.MdcaCount() ; count++)
		{
		size += (aRecipients.MdcaPoint(count)).Length();
		}
	size= size + count + 1 ; //To keep space for delimiter.
	aBuf.CreateL(size);
	
	if(aRecipients.MdcaCount()>0)
		{
		for(count=0 ; count < aRecipients.MdcaCount() ; count++)
			{
			aBuf.Append(aRecipients.MdcaPoint(count));
			if(count< aRecipients.MdcaCount()-1)
				aBuf.Append(KComma);
			}
		aBuf.Append(KDelimiter);
		}
	}

/**
 * Convert the 8 bit descripter to 16 bit.
 * @param astr A descripter to be convert into16 bit discripter.
 * @return HBufC16* A 16 bit discripter.
 */

HBufC16* CImHeader::Convert8to16L(const TDesC8& aStr)const
	{
	HBufC16* newFrom1 = HBufC16::NewL(aStr.Length());
	newFrom1->Des().Copy(aStr);
	return newFrom1;
	}

/**
 * Restores  message header from a specified message store.
 * 
 * @param aMessageStore Message store to read from. 
 * @return None.
 */

void CImHeader::ReStoreDBL(CMsvStore& aMessageStore)
	{
	TInt error;
	CHeaderFields* RcvHeaderRow = NULL;
	TMsvReadStore storeReader(aMessageStore, KUidMsgFileIMailHeader);
	storeReader.LoadL(RcvHeaderRow);
	
	Reset();
	
	TInt i = 0;  // Index.

	//header version 0
	iVersion = RcvHeaderRow->iFieldPairList[i++]->iFieldNumValue; // header version.

	//iReceipt 1
	iReceipt =  RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	
	//iImMsgId 2
	iImMsgId = Convert16to8L(*(RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue));
	
	//iFrom 3
	iFrom = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();

	//iReplyTo 4
	iReplyTo = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();

	//iSubject 5
	iSubject = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();

	//iRemoteSize 6
	iRemoteSize = RcvHeaderRow->iFieldPairList[i++]->iFieldNumValue;
	
	//ToRecipients() 7 ---------------------
	HBufC* 	torecipientslist = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	TPtrC strToRecipients = torecipientslist->Des();
	TRAP(error,CreateImHeaderArrayListL(strToRecipients,*iTo));
	delete torecipientslist;
	User::LeaveIfError(error);
	
	//CcRecipients 8	
	HBufC* 	ccrecipientslist = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	TPtrC strCCRecipients = ccrecipientslist->Des();
	TRAP(error,CreateImHeaderArrayListL(strCCRecipients,*iCc));
	delete ccrecipientslist;
	User::LeaveIfError(error);
	
	//BCcRecipients 9	
	HBufC* 	bccrecipientslist = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	TPtrC strbccrecipients = bccrecipientslist->Des();
	TRAP(error,CreateImHeaderArrayListL(strbccrecipients,*iBcc));
	delete bccrecipientslist;
	User::LeaveIfError(error);

	//EncodingInfo 10
	HBufC16* encodinginfoList = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	TPtr16 encodinginfoListPtr = (encodinginfoList->Des());
	TRAP(error,CreateEncodingInfoL(encodinginfoListPtr));
	delete encodinginfoList;
	User::LeaveIfError(error);
	
	iResentMsgId = Convert16to8L(*(RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue));
	
	//ResentFrom  12	
	iResentFrom = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	
	//ResentToRecipients 13
	HBufC* 	torecientrecipientslist = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	TPtrC strtorecientrecipients = torecientrecipientslist->Des();
	if(strtorecientrecipients.Length() >1 )
		TRAP(error,CreateImHeaderArrayListL(strtorecientrecipients,*iResentTo));
	delete torecientrecipientslist;
	User::LeaveIfError(error);
	
	//ResentCcRecipients 14
	HBufC* 	ccrecientrecipientslist = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	TPtrC strccrecientrecipients = ccrecientrecipientslist->Des();
	if(strccrecientrecipients.Length() >1 )
		TRAP(error,CreateImHeaderArrayListL(strccrecientrecipients,*iResentCc));
	delete ccrecientrecipientslist;
	User::LeaveIfError(error);
	
	//ResentCcRecipients 14
	HBufC* 	bccrecientrecipientslist = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue->Des().AllocL();
	TPtrC strbccrecientrecipientslist = bccrecientrecipientslist->Des();
	if(strbccrecientrecipientslist.Length() >1 )
		TRAP(error,CreateImHeaderArrayListL(strbccrecientrecipientslist,*iResentBcc));
	delete bccrecientrecipientslist;
	User::LeaveIfError(error);
	
	iInReplyTo = Convert16to8L(*(RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue));
	
	//BodyEncoding 17
	iBodyEncoding = (TMsgOutboxBodyEncoding)RcvHeaderRow->iFieldPairList[i++]->iFieldNumValue;
	
	//Charset 18
	i822HeaderCharset = RcvHeaderRow->iFieldPairList[i++]->iFieldNumValue;
	
	// Check if a stream for the encoded header exists.
	if (aMessageStore.IsPresentL(KUidMsgFileIMailEncodedHeader)) 
		{
		if (!iEncodedHeader)
			{
			iEncodedHeader = CImEncodedHeader::NewL();
			}
		iEncodedHeader->ReStoreEncodedDBL(aMessageStore);
		}
	}


/**
 * Create Array List(iTo,iCc,iBcc) from a given string.
 * @param aStr A given string.
 * @param aInto Array list to be created.
 * @return None.
 */
void CImHeader::CreateImHeaderArrayListL(TDesC16& aStr, CDesCArray& aRecipients)
	{
	if(aStr.Length() > 0)
		{
		TPtrC16 recvstr = aStr;
		TInt startPos = recvstr.Locate(',') ;
		if(startPos == KErrNotFound)
			{
			TInt startrecvPos = recvstr.Locate(';') ;
			TPtrC16 partstr = recvstr.Left(startrecvPos);
			aRecipients.AppendL(partstr);
			return;
			}

		while(1)
			{
			TPtrC16 partstrx = recvstr.Left(startPos);
			aRecipients.AppendL(partstrx);
			startPos++;
			recvstr.Set(recvstr.Mid(startPos, recvstr.Length()- startPos));
		
			startPos = recvstr.Locate(',');
			if(startPos == KErrNotFound)
				{
				TPtrC16 partstrxy;
				partstrxy.Set(recvstr.Mid(0,recvstr.Length()-1));
				aRecipients.AppendL(partstrxy);
				break;
				}
			}
		}
	}

/**
 * Create encoding info from a given string.
 * @param aStr A given string.
 * @return None.
 */

void CImHeader::CreateEncodingInfoL(TDesC16& aEncodingStr1)
	{
	TPtrC16 aEncodingStr = aEncodingStr1.Left(aEncodingStr1.Length());
	if(aEncodingStr.Length()>0)
		{
		TInt startPos = 0;
		TInt firstSemiColonPos = aEncodingStr.Locate(';');
		TInt lastSemiColonPos = aEncodingStr.LocateReverse(';');

		iEncodingInfo->Reset();
		TImHeaderEncodingInfo headerencodinginfo;
		RArray<TInt> encodingData;

		do	{
			TPtrC16 str1 = aEncodingStr.Left(firstSemiColonPos+1); // First recipient
			startPos = str1.Locate(',') ;
			
			while(startPos != KErrNotFound )
				{
				TPtrC16 str2 = str1.Left(startPos);
				TInt a= ConvertToTInt(str2);
				encodingData.AppendL(a);
				
				startPos++;
				str1.Set(str1.Mid(startPos, str1.Length()- startPos));
			
				startPos = str1.Locate(',');
				if(startPos == KErrNotFound)
					{
					TPtrC16 str3;
					str3.Set(str1.Mid(0,str1.Length()-1));
					encodingData.Append(ConvertToTInt(str3));
					break;
					}
					
				}

			headerencodinginfo.SetField(( TImHeaderEncodingInfo::TFieldList)encodingData[0]);
			headerencodinginfo.SetOffset(encodingData[1]);
			headerencodinginfo.SetLength(encodingData[2]);
			headerencodinginfo.SetEncodingType((TImHeaderEncodingInfo::TEncodingType)encodingData[3]);
			headerencodinginfo.SetArrayValue(encodingData[4]);
			headerencodinginfo.SetCharsetUid(encodingData[5]);
			headerencodinginfo.SetAddSpace(encodingData[6]);
			headerencodinginfo.SetEncodedLength(encodingData[7]); 
			iEncodingInfo->AppendL(headerencodinginfo);
			encodingData.Reset();	

			if(firstSemiColonPos != lastSemiColonPos)
				{
				aEncodingStr.Set(aEncodingStr.Mid(firstSemiColonPos+1,aEncodingStr.Length()-firstSemiColonPos-1));
				firstSemiColonPos = aEncodingStr.Locate(';');
				lastSemiColonPos = aEncodingStr.LocateReverse(';');
				}
			else
				{
				break;
				}
			} while(ETrue);

		encodingData.Close();
		}
	}




/**
 * Stores mime header to a specified message store.
 * @param aMessageStore Message store to write to.
 * @return None.
 */

void CImMimeHeader::StoreMimeDBL(CMsvStore& aMessageStore) const
	{
	CHeaderFields* emailHeaderFields = new (ELeave)CHeaderFields();
	CleanupStack::PushL(emailHeaderFields);
	emailHeaderFields->iUid = KUidMsgFileMimeHeader;

	//----------------------MIME Info From Filed 19th------------------------------	
	CFieldPair* emaildetailField = new (ELeave)CFieldPair();
	CleanupStack::PushL(emaildetailField);

	//Make a string from  mime header info 
	RBuf8 mimeheaderinfo;
	CleanupClosePushL(mimeheaderinfo);
	mimeheaderinfo.CreateL(this->BufSize());

	//Version
	mimeheaderinfo.AppendNum(Version());	
	mimeheaderinfo.Append(KDelimiter);

	//iRelativePath
	mimeheaderinfo.Append(LimitStringSize(RelativePath(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(KDelimiter);
	
	//iContentType;
	mimeheaderinfo.Append(LimitStringSize(ContentType(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(KDelimiter);
	
	//iContentSubType;
	mimeheaderinfo.Append(LimitStringSize(ContentSubType(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(KDelimiter);
	
	//iContentTransferEncoding
	mimeheaderinfo.AppendNum(ContentTransferEncoding());	
	mimeheaderinfo.Append(KDelimiter);

	//iContentDisposition
	mimeheaderinfo.Append(LimitStringSize(ContentDisposition(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(KDelimiter);

	//iContentDescription
	mimeheaderinfo.Append(LimitStringSize(ContentDescription(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(KDelimiter);

	//iContentBase
	mimeheaderinfo.Append(LimitStringSize(ContentBase(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(KDelimiter);
	
	//iContentLocation
	//16bit to 8bit
	HBufC8* newFrom = HBufC8::NewL((LimitStringSize(ContentLocation(), KMaxImMimeFieldLength)).Length());
	newFrom->Des().Copy(LimitStringSize(ContentLocation(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(newFrom->Des());
	mimeheaderinfo.Append(KDelimiter);
	delete newFrom;
	//iContentID
	mimeheaderinfo.Append(LimitStringSize(ContentID(), KMaxImMimeFieldLength));
	mimeheaderinfo.Append(KDelimiter);
	
	//iContentTypeParams
	for(TInt i=0 ; i<iContentTypeParams->MdcaCount() ; i++)
		{
		mimeheaderinfo.Append(iContentTypeParams->MdcaPoint(i));
		if(i< (iContentTypeParams->MdcaCount())-1)
			mimeheaderinfo.Append(KComma);
		}
	mimeheaderinfo.Append(KDelimiter);
	
	//iContentDispositionParams
	for(TInt i=0 ; i<iContentDispositionParams->MdcaCount() ; i++)
		{
		mimeheaderinfo.Append(iContentDispositionParams->MdcaPoint(i));
		if(i<(iContentDispositionParams->MdcaCount())-1)
			mimeheaderinfo.Append(KComma);
		}
	mimeheaderinfo.Append(KDelimiter);
	
	//iXTypeParams
	for(TInt i=0 ; i<iXTypeParams->MdcaCount() ; i++)
		{
		mimeheaderinfo.Append(iXTypeParams->MdcaPoint(i));
		if(i<(iXTypeParams->MdcaCount())-1)
			mimeheaderinfo.Append(KComma);
		}
	mimeheaderinfo.Append(KDelimiter);

	//iMimeCharset
	mimeheaderinfo.AppendNum(MimeCharset());	
	mimeheaderinfo.Append(KDelimiter);
	
	//----------------------MIME Info end 19 ------------------------------	
	// 8bit to 16bit
	HBufC* newFrom1 = HBufC::NewL(mimeheaderinfo.Length());
	newFrom1->Des().Copy(mimeheaderinfo);
	emaildetailField->iFieldTextValue = newFrom1;
	emailHeaderFields->iFieldPairList.AppendL(emaildetailField);
	CleanupStack::Pop();  // Rbuf
	CleanupStack::Pop(emaildetailField);
	
	TMsvWriteStore storeWriter(aMessageStore);
	storeWriter.AssignL(emailHeaderFields);
	storeWriter.CommitL();
	

	mimeheaderinfo.Close();
	CleanupStack::Pop(emailHeaderFields);
	}




/**
 * Create Buffer size for Mime header.
 * @param  None.
 * @return TInt size of the Mime hedaer.
 */

TInt CImMimeHeader::BufSize() const
	{
	TInt size = 4; 	// Version
	
	if(iRelativePath)
		size += Align4(iRelativePath->Size());
	if(iContentType)
		size += Align4(iContentType->Size());
	if(iContentSubType)
		size += Align4(iContentSubType->Size());
	if(iContentDisposition)
		size += Align4(iContentDisposition->Size());
	if(iContentDescription)
		size += Align4(iContentDescription->Size());
	if(iContentBase)
		size += Align4(iContentBase->Size());
	if(iContentLocation)
		size += Align4(iContentLocation->Size());
	if(iContentID)
		size += Align4(iContentID->Size());
	if(iContentDisposition)
		size += Align4(iContentDisposition->Size());
	
	size += sizeof(TImEncodingType);
	size += sizeof(TUint);			// iMimeCharset
	
	TInt i;
	for (i=0 ; i<ContentTypeParams().Count(); ++i)
		{
		size += Align4(ContentTypeParams()[i].Size()) + 1; // 1 for Comma.
		}
	for (i=0 ; i<ContentDispositionParams().Count(); ++i)
		{
		size += Align4(ContentDispositionParams()[i].Size()) + 1;
		}
	for (i=0 ; i<XTypeParams().Count(); ++i)
		{
		size += Align4(XTypeParams()[i].Size()) + 1;
		}
	return (size+14); //For Delimiter (;)
	}



	
/**
 * Restores Mime header to a specified message store.
 * @param aMessageStore Message store to read from 
 * @return None
 */

void CImMimeHeader::ReStoreMimeDBL(CMsvStore& aMessageStore)
	{
	CHeaderFields* RcvHeaderRow = NULL;
	TMsvReadStore storeReader(aMessageStore, KUidMsgFileMimeHeader);
	storeReader.ReadL(RcvHeaderRow);
		
	Reset();

	TInt i = 0; //Mime Header storeed in 19th Filed of the Email Header table.

	HBufC* 	mimeheaderinfo = RcvHeaderRow->iFieldPairList[i++]->iFieldTextValue ;
	TPtr16 mimeheaderinfoPtr = (mimeheaderinfo->Des());	

	//Parsing the encodedheader string 
	TInt firstSemiColonPos = mimeheaderinfo->Locate(';');
	TInt lastSemiColonPos = mimeheaderinfo->LocateReverse(';');
	
	if(firstSemiColonPos != lastSemiColonPos) // Expected MIME string :-0; 
	    {
        RPointerArray<HBufC16>  mimeheaderinfoData;
        CleanupClosePushL(mimeheaderinfoData);
        TInt index = 0 ;
    
        do
            {
            TPtrC16 str1 = mimeheaderinfoPtr.Left(firstSemiColonPos); // First data
            HBufC16* tt3 = str1.AllocL();
            firstSemiColonPos++;
            mimeheaderinfoData.AppendL(tt3); 
            
            if(firstSemiColonPos != lastSemiColonPos)
                {
                mimeheaderinfoPtr = (mimeheaderinfoPtr.Mid(firstSemiColonPos,mimeheaderinfoPtr.Length()-(firstSemiColonPos) ));
                firstSemiColonPos = mimeheaderinfoPtr.Locate(';');
                lastSemiColonPos = mimeheaderinfoPtr.LocateReverse(';');
                if(firstSemiColonPos == lastSemiColonPos)
                    {
                    TPtrC16 str1 = mimeheaderinfoPtr.Left(firstSemiColonPos); // Last data
                    mimeheaderinfoData.AppendL(str1.AllocL()); 
                    }
                }
            
            index++;	
            }while(firstSemiColonPos != lastSemiColonPos);
    
            iVersion = ConvertToTInt(*mimeheaderinfoData[0]);
            iRelativePath = Convert16to8L(*mimeheaderinfoData[1]);
            iContentType  = Convert16to8L(*mimeheaderinfoData[2]);
            iContentSubType = Convert16to8L(*mimeheaderinfoData[3]);
            TInt ContentTransferEncodingint = ConvertToTInt(*mimeheaderinfoData[4]);
            iContentTransferEncoding = (TImEncodingType)(ContentTransferEncodingint);
            iContentDisposition = Convert16to8L(*mimeheaderinfoData[5]);
            iContentDescription = Convert16to8L(*mimeheaderinfoData[6]);
            iContentBase = Convert16to8L(*mimeheaderinfoData[7]);
            iContentLocation = mimeheaderinfoData[8]->Des().AllocL();
            iContentID = Convert16to8L(*mimeheaderinfoData[9]);
            
            //iContentTypeParams
            if ((mimeheaderinfoData[10]->Des().Locate(',')) > -1 )
                {
                CreateMimeArrayListL(mimeheaderinfoData[10]->Des(),0); //To
                }
            else
                {
                if(mimeheaderinfoData[10]->Length()>0)
                    iContentTypeParams->AppendL(Convert16to8L(*mimeheaderinfoData[10])->Des());
                }
        
            //iContentDispositionParams
            if ((mimeheaderinfoData[11]->Des().Locate(',')) > -1 )
                {
                CreateMimeArrayListL(mimeheaderinfoData[11]->Des(),1); //To
                }
            else
                {
                if(mimeheaderinfoData[11]->Length()>0)
                    iContentDispositionParams->AppendL(Convert16to8L(*mimeheaderinfoData[11])->Des());
                }
        
            //iXTypeParams
            if ((mimeheaderinfoData[12]->Des().Locate(',')) > -1 )
                {
                CreateMimeArrayListL(mimeheaderinfoData[12]->Des(),2); //To
                }
            else
                {
                if(mimeheaderinfoData[12]->Length()>0)
                    iXTypeParams->AppendL(Convert16to8L(*mimeheaderinfoData[12])->Des());
                }
    
        iMimeCharset = ConvertToTInt(*mimeheaderinfoData[13]);
        
        mimeheaderinfoData.ResetAndDestroy();
        mimeheaderinfoData.Close();
        CleanupStack::Pop();   //RpointerArray
	    }// End of if(firstSemiColonPos != lastSemiColonPos)
	else
	    {
	    iMimeCharset = 0; // As Mime String is 0; Setting MimeCharacterSet 0
	    }
	}

/**
 * Create Mime Header List from a string.
 * @param  aStr  A descripter String.
 * @return None.
 */
void CImMimeHeader::CreateMimeArrayListL(TPtrC16 aStr ,TInt aI)
	{
	TInt startPos = aStr.Locate(',') ;
	while(startPos > -1 )
		{
		TPtrC16 str2 = aStr.Left(startPos);
		if(aI == 0)
			{
			//iContentTypeParams
			iContentTypeParams->AppendL(Convert16to8L(str2)->Des());	
			}
		if(aI == 1)
			{
			//iContentDispositionParams
			iContentDispositionParams->AppendL(Convert16to8L(str2)->Des());
			}
		if( aI == 2)
			{
			//iXTypeParams
			iXTypeParams->AppendL(Convert16to8L(str2)->Des());
			}
		startPos++;
		aStr.Set(aStr.Mid(startPos, aStr.Length()- startPos));
	
		startPos = aStr.Locate(',');
		if(startPos == -1)
			{
			if(aStr.Length() > 0)
				{
				if(aI == 0)
					{
					//iContentTypeParams
					iContentTypeParams->AppendL(Convert16to8L(aStr)->Des());	
					}
				if(aI == 1)
					{
					//iContentDispositionParams
					iContentDispositionParams->AppendL(Convert16to8L(aStr)->Des());
					}
				if( aI == 2)
					{
					//iXTypeParams
					iXTypeParams->AppendL(Convert16to8L(aStr)->Des());
					}
					
				break;
				} //if(aStr.Length() > 0)
			} //if(startPos == -1)
		} //while(startPos >= -1 )
	}

#endif
