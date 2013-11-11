// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cimencodeheader.cpp
//

#include "cimencodedheader.h"
#include "MIUT_ERR.H"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "miut_errconsts.h"
#include "timrfc822datefield.h"
#endif

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
#include <msvstd.h>
#endif 

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
_LIT16(KComma, ",");
_LIT16(KDelimiter, ";");
#endif

const TInt KArrayGranularity = 16;

// Identifies which version of CImEncodedHeader has been stored in stream
const TUint16 KImEncodedHeaderVersion = 1; 

extern TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize);
extern TPtrC16 LimitStringSize(const TPtrC16& aString, TInt aMaxSize);
extern void ExternalizeL(const CDesC8Array& anArray,RWriteStream& aStream);
extern void InternalizeL(CDesC8Array& anArray,RReadStream& aStream);
extern void ExternalizeL(const CDesC16Array& anArray,RWriteStream& aStream);
extern void InternalizeL(CDesC16Array& anArray,RReadStream& aStream);

//------------------------------------------------------------------------------
//              Class  CImEncodedHeader  Functions
//------------------------------------------------------------------------------

CImEncodedHeader::CImEncodedHeader() : iDecodeCharset(0)
	{
	}

//------------------------------------------------------------------------------

CImEncodedHeader* CImEncodedHeader::NewLC()
/** Allocates and creates a new CImHeader object, leaving the object on the cleanup 
stack.

@return New CImEncodedHeader object */
	{
	CImEncodedHeader* self = new(ELeave) CImEncodedHeader;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//------------------------------------------------------------------------------

CImEncodedHeader* CImEncodedHeader::NewL()
/** Allocates and creates a new CImHeader object, leaving the object on the cleanup 
stack.

@return New CImEncodedHeader object */
	{
	CImEncodedHeader* self = CImEncodedHeader::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::ConstructL()
	{
	iTo  = new (ELeave)CDesCArrayFlat(KArrayGranularity);
	iCc  = new (ELeave)CDesCArrayFlat(KArrayGranularity);	
	iBcc = new (ELeave)CDesCArrayFlat(KArrayGranularity);
	}

//------------------------------------------------------------------------------

CImEncodedHeader::~CImEncodedHeader()
	{
	Reset();
	delete iTo;
	delete iCc;
	delete iBcc;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::Reset()
	{
	iVersion = KImEncodedHeaderVersion;
	delete iFrom;    iFrom    = NULL;
	delete iSubject; iSubject = NULL;
	delete iReplyTo; iReplyTo = NULL;

	if (iTo)
		iTo->Reset();
	if (iCc)
		iCc->Reset();
	if (iBcc)
		iBcc->Reset();

	iDecodeCharset = 0;
	}

//------------------------------------------------------------------------------

const TPtrC8 CImEncodedHeader::Subject() const
/** Gets the "Subject" header field.

@return Field value */
	{
	return iSubject ? TPtrC8(*iSubject) : TPtrC8();
	}

//------------------------------------------------------------------------------

const TPtrC8 CImEncodedHeader::From() const
/** Gets the "From" header field. 

This consists of an address and (possibly) an alias.

@return Field value */
	{
	return iFrom ? TPtrC8(*iFrom) : TPtrC8();
	}

//------------------------------------------------------------------------------

const TPtrC8 CImEncodedHeader::ReplyTo() const
/** Gets the "ReplyTo" header field. 

@return Field value */
	{
	return iReplyTo ? TPtrC8(*iReplyTo) : TPtrC8();
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::SetSubjectL( const TDesC8& aSubject )
/** Sets the "Subject" header field. 

@param aSubject Field value */
	{
	HBufC8* newSubject = aSubject.AllocL();
	delete iSubject;
	iSubject = newSubject;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::SetFromL( const TDesC8& aFrom )
/** Sets the "From" header field. 

@param aFrom Field value */
	{
	HBufC8* newFrom = aFrom.AllocL();
	delete iFrom;
	iFrom = newFrom;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::SetReplyToL( const TDesC8& aReplyTo )
/** Sets the "ReplyTo" header field. 

@param aReplyTo Field value */
	{
	HBufC8* newReplyTo = aReplyTo.AllocL();
	delete iReplyTo;
	iReplyTo = newReplyTo;
	}

//------------------------------------------------------------------------------

CDesCArray& CImEncodedHeader::ToRecipients ()
	{
	return *iTo;
	}

//------------------------------------------------------------------------------

CDesCArray& CImEncodedHeader::CcRecipients ()
/** Gets a list of "Cc" recipients.

@return Recipient list */
	{
	return *iCc;
	}

//------------------------------------------------------------------------------
	
CDesCArray& CImEncodedHeader::BccRecipients()
/** Gets a list of "Bcc" recipients.

@return Recipient list */
	{
	return *iBcc;
	}

//------------------------------------------------------------------------------

const CDesCArray& CImEncodedHeader::ToRecipients () const 
/** Gets a const list of "To" recipients.

@return Recipient list */
	{
	return *iTo;
	}

//------------------------------------------------------------------------------

const CDesCArray& CImEncodedHeader::CcRecipients () const
/** Gets a const list of "Cc" recipients.

@return Recipient list */
	{
	return *iCc;
	}

//------------------------------------------------------------------------------
	
const CDesCArray& CImEncodedHeader::BccRecipients() const
/** Gets a const list of "Bcc" recipients.

@return Recipient list */
	{
	return *iBcc;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::ExternalizeL( RMsvWriteStream& aWriteStream ) const
/** Externalises the settings to a specified stream.

@param aWriteStream Stream to write to */
	{
	aWriteStream.WriteUint16L(Version());	// This MUST be the 1st item written into the stream

	aWriteStream << LimitStringSize(From(), KMaxImHeaderStringLength);
	aWriteStream << LimitStringSize(ReplyTo(), KMaxImHeaderStringLength);
	aWriteStream << LimitStringSize(Subject(), KMaxImHeaderStringLength);

	aWriteStream << ToRecipients();
	aWriteStream << CcRecipients();
	aWriteStream << BccRecipients();

	aWriteStream.WriteUint32L(iDecodeCharset);
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::InternalizeL( RMsvReadStream& aReadStream )
/** Internalises the settings from a specified stream.

@param aReadStream Stream to read from */
	{
	Reset();

	iVersion = aReadStream.ReadUint16L();
	iFrom    = HBufC8::NewL(aReadStream, KMaxImHeaderStringLength);
	iReplyTo = HBufC8::NewL(aReadStream, KMaxImHeaderStringLength);
	iSubject = HBufC8::NewL(aReadStream, KMaxImHeaderStringLength);

	aReadStream >> ToRecipients();
	aReadStream >> CcRecipients();
	aReadStream >> BccRecipients();

	iDecodeCharset = aReadStream.ReadUint32L();
	}

void CImEncodedHeader::StoreL(CMsvStore& aMessageStore) const
	/** Stores settings to a specified message store.

	@param aMessageStore Message store to write to */
	{
	//------------------------------------------------------------------------------
	RMsvWriteStream out;
	out.AssignLC(aMessageStore, KUidMsgFileIMailEncodedHeader);
	ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy(&out);
	}
 

void CImEncodedHeader::RestoreL(CMsvStore& aMessageStore)
	/** Restores settings from a specified message store.

	@param aMessageStore Message store to read from */
	{
	RMsvReadStream in;
	in.OpenLC( aMessageStore, KUidMsgFileIMailEncodedHeader );
	InternalizeL(in);
	in.Close();
	CleanupStack::PopAndDestroy(&in);
	}
//------------------------------------------------------------------------------

TUint16 CImEncodedHeader::Version() const
	{
	return iVersion;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::SetVersion( TUint16 aVersion )
	{
	iVersion = aVersion;
	}

//------------------------------------------------------------------------------

TUint CImEncodedHeader::DecodeCharset() const
	{
	return iDecodeCharset;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::SetDecodeCharset(TUint aCharset)
	{
	iDecodeCharset = aCharset;
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::CopyFromHeaderL(
	const CImHeader& aFromHeader, TBool aDeleteExistingData)
	{
	// From field
	const TPtrC& fromDes = aFromHeader.From();
	if (fromDes.Length() && (!iFrom || aDeleteExistingData))
		{
		HBufC8* buffer = HBufC8::NewL(fromDes.Length());
		buffer->Des().Copy(fromDes);
		delete iFrom;
		iFrom = buffer;
		}

	// Subject field
	const TPtrC& subjectDes = aFromHeader.Subject();
	if (subjectDes.Length() && (!iSubject || aDeleteExistingData))
		{
		HBufC8* buffer = HBufC8::NewL(subjectDes.Length());
		buffer->Des().Copy(subjectDes);
		delete iSubject;
		iSubject = buffer;
		}
	
	// ReplyTo field
	const TPtrC& replyToDes = aFromHeader.ReplyTo();
	if (replyToDes.Length() && (!iReplyTo || aDeleteExistingData))
		{
		HBufC8* buffer = HBufC8::NewL(replyToDes.Length());
		buffer->Des().Copy(replyToDes);
		delete iReplyTo;
		iReplyTo = buffer;
		}
	// Copy iTo, iCc, iBcc
	const CDesCArray& srcToArray = aFromHeader.ToRecipients();
	if (srcToArray.Count() && ((iTo->Count() == 0) || aDeleteExistingData))
		::CopyArrayL(srcToArray, *iTo);
	
	const CDesCArray& srcCcArray = aFromHeader.CcRecipients();
	if (srcCcArray.Count() && ((iCc->Count() == 0) || aDeleteExistingData))
		::CopyArrayL(srcCcArray, *iCc);

	const CDesCArray& srcBccArray = aFromHeader.BccRecipients();
	if (srcBccArray.Count() && ((iBcc->Count() == 0) || aDeleteExistingData))
		::CopyArrayL(srcBccArray, *iBcc);
	}

//------------------------------------------------------------------------------

void CImEncodedHeader::CopyToHeaderL(CImHeader& aToHeader) const
	{
	//
	aToHeader.SetFromL(From());
	aToHeader.SetSubjectL(Subject());
	aToHeader.SetReplyToL(ReplyTo());

	// Copy iTo, iCc, iBcc
	::CopyArrayL(*iTo, aToHeader.ToRecipients());
	::CopyArrayL(*iCc, aToHeader.CcRecipients());
	::CopyArrayL(*iBcc, aToHeader.BccRecipients());
	}

//------------------------------------------------------------------------------

TInt CImEncodedHeader::DataSize() const
/** Gets the combined length of all the field values stored. 

@return Combined length */
	{
	TInt headerSize = 2; // CRLF at end of header
	const CImEncodedHeader* self = this;

	headerSize += self->From().Length();
	headerSize += self->ReplyTo().Length();
	headerSize += self->Subject().Length();
	headerSize += sizeof(TUint);				// size of Charset

	TInt count;
	CDesCArray& toArray = *self->iTo;
	for (count = 0; count < toArray.Count(); count++)
		{
		headerSize += toArray[count].Length();
		}

	CDesCArray& ccArray = *self->iCc;
	for (count = 0; count < ccArray.Count(); count++)
		{
		headerSize += ccArray[count].Length(); 
		}

	CDesCArray& bccArray = *self->iBcc;
	for (count = 0; count < bccArray.Count(); count++)
		{
		headerSize += bccArray[count].Length();
		}

	return headerSize;
	}


#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
/**Stores encoded header to a specified message store.
 * @param aMessageStore Message store to write to
 * @return None.
 */
void CImEncodedHeader::StoreEncodedDBL(CMsvStore& aMessageStore) const
	{
	CHeaderFields* emailHeaderFields = new (ELeave)CHeaderFields();
	CleanupStack::PushL(emailHeaderFields);
	emailHeaderFields->iUid = KUidMsgFileIMailEncodedHeader;

	//----------------------Encoded Info  19th Field ------------------------------	// 19
	CFieldPair* emaildetailField = new (ELeave)CFieldPair();
	CleanupStack::PushL(emaildetailField);
	
	//Make a string from  encoded header info.
	RBuf8 encodedheader;
	CleanupClosePushL(encodedheader);
	encodedheader.CreateL(EncodedBufSize()); //
	
	encodedheader.AppendNum(Version());	
	encodedheader.Append(KDelimiter);

	encodedheader.Append(LimitStringSize(From(), KMaxImHeaderStringLength));
	encodedheader.Append(KDelimiter);
	encodedheader.Append(LimitStringSize(ReplyTo(), KMaxImHeaderStringLength));
	encodedheader.Append(KDelimiter);
	encodedheader.Append(LimitStringSize(Subject(), KMaxImHeaderStringLength));
	encodedheader.Append(KDelimiter);
	
	//ToRecipients 
	for(TInt i=0 ; i<iTo->MdcaCount() ; i++)
		{
		encodedheader.Append(iTo->MdcaPoint(i));
		if(i< iTo->MdcaCount()-1)
			encodedheader.Append(KComma);
		}
	encodedheader.Append(KDelimiter);
	
	//CcRecipients
	for(TInt i=0 ; i<iCc->MdcaCount() ; i++)
		{
		encodedheader.Append(iCc->MdcaPoint(i));
		if(i< iCc->MdcaCount()-1)
			encodedheader.Append(KComma);
		}
	encodedheader.Append(KDelimiter);

	//BCcRecipients
	for(TInt i=0 ; i<iBcc->MdcaCount() ; i++)
		{
		encodedheader.Append(iBcc->MdcaPoint(i));
		if(i< iBcc->MdcaCount()-1)
			encodedheader.Append(KComma);
		}
	encodedheader.Append(KDelimiter);
	
	encodedheader.AppendNum(iDecodeCharset);	
	encodedheader.Append(KDelimiter);

	//8 bit to 16 bit
	HBufC* newFrom = HBufC::NewL(encodedheader.Length());
	newFrom->Des().Copy(encodedheader);
	emaildetailField->iFieldTextValue = newFrom;
	emailHeaderFields->iFieldPairList.AppendL(emaildetailField);
	CleanupStack::Pop();  //RBuf8 
	CleanupStack::Pop(emaildetailField);
	
	//---------------------------Encoded info end-------------------------------

	TMsvWriteStore storeWriter(aMessageStore);
	storeWriter.AssignL(emailHeaderFields);
	encodedheader.Close();
	CleanupStack::Pop(emailHeaderFields);
	}

/**
 * Restores encoded header to a specified message store.
 * @param aMessageStore Message store to read from.
 * @return NOne.
 */

void CImEncodedHeader::ReStoreEncodedDBL(CMsvStore& aMessageStore)
	{
	Reset();
	
	CHeaderFields* aEncodedHeader = NULL;
	TMsvReadStore storeReader(aMessageStore, KUidMsgFileIMailEncodedHeader);
	storeReader.ReadL(aEncodedHeader);

	TInt i = 0; // EncodedHeader field is on 19th coloum  of header table.
	HBufC* 	encodedheaderinfo = aEncodedHeader->iFieldPairList[i++]->iFieldTextValue ;

	TPtr16 encodedheaderPtr = (encodedheaderinfo->Des());	
	
	//Parsing the encodedheader string 
	TInt firstSemiColonPos = encodedheaderPtr.Locate(';');
	TInt lastSemiColonPos = encodedheaderPtr.LocateReverse(';');
	
	RPointerArray<HBufC16> encodedheaderinfoData;
	CleanupClosePushL(encodedheaderinfoData);
	TInt ii = 0 ;

	do
		{
		TPtrC16 str1 = encodedheaderPtr.Left(firstSemiColonPos); // First data
		HBufC16* tt = str1.AllocL();
		firstSemiColonPos++;
		encodedheaderinfoData.AppendL(tt); 

		if(firstSemiColonPos != lastSemiColonPos)
			{
			encodedheaderPtr = (encodedheaderPtr.Mid(firstSemiColonPos,encodedheaderPtr.Length()-(firstSemiColonPos) ));
			firstSemiColonPos = encodedheaderPtr.Locate(';');
			lastSemiColonPos = encodedheaderPtr.LocateReverse(';');
			if(firstSemiColonPos == lastSemiColonPos)
				{
				TPtrC16 str1 = encodedheaderPtr.Left(firstSemiColonPos); // Last data
				HBufC16* tt = str1.AllocL();
				encodedheaderinfoData.AppendL(tt); 
				}
			}
		
		ii++;	
		}while(firstSemiColonPos != lastSemiColonPos);
		
		iVersion= ConvertToTInt(*encodedheaderinfoData[0]);
		
		HBufC8* form = HBufC8::NewL((encodedheaderinfoData[1])->Length());
		form->Des().Copy(encodedheaderinfoData[1]->Des());
		iFrom = form;
		
		HBufC8* replyTo = HBufC8::NewL((encodedheaderinfoData[2])->Length());
		replyTo->Des().Copy(encodedheaderinfoData[2]->Des());
		iReplyTo = replyTo ;
	
		HBufC8* subject = HBufC8::NewL((encodedheaderinfoData[3])->Length());
		subject->Des().Copy(encodedheaderinfoData[3]->Des());
		iSubject = subject ;
			
		//iTo
		HBufC16* tmpBuf = encodedheaderinfoData[4]->Des().AllocL();		
		if ((encodedheaderinfoData[4]->Des().Locate(',')) > -1 )
			{
			CreateEncodedArrayListL(tmpBuf->Des(),0); //To
			}
		else
			{
			if(encodedheaderinfoData[4]->Length()>0)
				iTo->AppendL(tmpBuf->Des());
			}
		delete tmpBuf;
		//iCc
		tmpBuf = encodedheaderinfoData[5]->Des().AllocL();
		if ((encodedheaderinfoData[5]->Des().Locate(',')) > -1 )
			{
			CreateEncodedArrayListL(tmpBuf->Des(),1); //CC
			}
		else
			{
			if(encodedheaderinfoData[5]->Length()>0)
				iCc->AppendL(tmpBuf->Des());
			}
		delete tmpBuf;
		//iBcc
		tmpBuf = encodedheaderinfoData[6]->Des().AllocL();
		if ((encodedheaderinfoData[6]->Des().Locate(',')) > -1 )
			{
			CreateEncodedArrayListL(tmpBuf->Des(),2); //Bcc
			}
		else
			{
			if(encodedheaderinfoData[6]->Length()>0)
				iBcc->AppendL(tmpBuf->Des());
				}
		
		delete tmpBuf;
		iDecodeCharset= ConvertToTInt(*encodedheaderinfoData[7]);
		encodedheaderinfoData.ResetAndDestroy();
		encodedheaderinfoData.Close();
		CleanupStack::Pop();   //RpointerArray	

		
	}


/**
 * Create Encoded Array List.
 * @param aEncodedStr : A string.
 * @param aI :A Integer value.
 * @return None.
 */	
void CImEncodedHeader::CreateEncodedArrayListL(TPtrC16 aEncodedStr ,TInt aI)
	{
	TInt startPos = aEncodedStr.Locate(',') ;
	while(startPos >= -1 )
		{
		TPtrC16 str2 = aEncodedStr.Left(startPos);
		//ainto.AppendL(str2);
		if(aI == 0)
			{
			//iTo
			iTo->AppendL(str2);	
			}
		if(aI == 1)
			{
			//iCc
			iCc->AppendL(str2);
			}
		if( aI == 2)
			{
			//iBcc
			iBcc->AppendL(str2);
			}
		startPos++;
		aEncodedStr.Set(aEncodedStr.Mid(startPos, aEncodedStr.Length()- startPos));
	
		startPos = aEncodedStr.Locate(',');
		if(startPos == -1)
			{
			TInt ll = aEncodedStr.Length();
			TPtrC16 str3;
			str3.Set(aEncodedStr.Mid(0,aEncodedStr.Length()));
			if(aI == 0)
				{
				//iTo
				iTo->AppendL(str3);	
				}
			if(aI == 1)
				{
				//iCc
				iCc->AppendL(str3);
				}
			if( aI == 2)
				{
				//iBcc
				iBcc->AppendL(str3);
				}
			break;
			}
		}
	}


TInt CImEncodedHeader::EncodedBufSize() const
/** Gets the combined length of all the field values stored. 

@return Combined length */
	{
	TInt headerSize = 4; // Version
	const CImEncodedHeader* self = this;

	headerSize += Align4(self->From().Size());
	headerSize += Align4(self->ReplyTo().Size());
	headerSize += Align4(self->Subject().Size());
	headerSize += sizeof(TUint);				// size of Charset

	TInt count;
	for (count=0; count<iTo->MdcaCount(); count++)
		{
		headerSize += Align4(iTo->MdcaPoint(count).Size()) + 1; //1 for Comma
		}
	for (count=0; count<iCc->MdcaCount(); count++)
		{
		headerSize += Align4(iCc->MdcaPoint(count).Size()) + 1; //1 for Comma
		}
	for (count=0; count<iBcc->MdcaCount(); count++)
		{
		headerSize += Align4(iBcc->MdcaPoint(count).Size()) + 1;//1 for Comma
		}

	return headerSize + 8; //8 delimiters ";".
	}

#endif 
