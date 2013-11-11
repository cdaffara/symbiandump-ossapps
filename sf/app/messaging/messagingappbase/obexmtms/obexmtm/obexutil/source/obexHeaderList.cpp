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
//

#include <s32mem.h>
#include <cmsvattachment.h>

#include <msvstore.h>
#include <obexheaderlist.h>
#include <obex.h> //for CObexBaseObject & CObexHeader
#include <obexconstantsinternal.h>

const TInt KDefaultObexHeaderListGranularity = 4;
const TInt KObexHeaderListIndexOutOfRange = KErrArgument;
const TInt KObexHeaderListSegmentSize = 512;

CObexHeaderList::CObexHeaderList()
	/**
     *  Constructor function used in 2 phase construction in NewLC()
     */
	{
	}


void CObexHeaderList::ConstructL()
	/**
	 * Leaving constructor function used in 2 phase construction in NewLC()
	 */
	{
	iObexHeaders = 
	 new (ELeave) RPointerArray<CObexHeader>(KDefaultObexHeaderListGranularity);
	}

EXPORT_C CObexHeaderList* CObexHeaderList::NewLC()
	/**
	 * Factory function to return a CObexHeaderList* and leave it on the 
	 * CleanupStack
	 */
	{
	CObexHeaderList* self = new (ELeave) CObexHeaderList();
    CleanupStack::PushL(self);
	self->ConstructL();
    return self;
	}

EXPORT_C CObexHeaderList* CObexHeaderList::NewL()
	/**
	 * Factory function to return a CObexHeaderList*
	 */
	{
	CObexHeaderList* self = NewLC();
    CleanupStack::Pop(self);
    return self;
	}

EXPORT_C CObexHeaderList::~CObexHeaderList()
	/**
	 * Destructor which frees all allocated memory with the list
	 */
	{
	if (iObexHeaders != NULL)
        {
		if(iObexHeaders->Count() > 0)
			iObexHeaders->ResetAndDestroy();
		else
			iObexHeaders->Reset();	// it is possible to have an array with no elements
									// which still holds some memory. We will reset here.

	    delete iObexHeaders;
		iObexHeaders = NULL;
        } 
	}

EXPORT_C TInt CObexHeaderList::AddHeader(const CObexHeader* aHeader)
	/**
	 * Add a CObexHeader object to the list. The CObexHeader object added 
	 * should have its attribute, header ID and header value set before
	 * it is added to the list. CObexHeaderlist will not change these
	 * values of its contained CObexHeader objects
	 * @param  aHeader is a pointer to a constant CObexHeader object
	 *  
	 * @return KErrNone, if the operation is successful, otherwise
	 *         one of the system wide error codes  
	 */
	{
	return iObexHeaders->Append(aHeader);
	}

EXPORT_C  TInt CObexHeaderList::Count() const
	/**
	 * Return the number of  CObexHeader object pointers contained in the list
	 *
	 * 
	 * @return return the number of CObexHeader object pointers in the list
	 */
	{
	return iObexHeaders->Count();
	}

EXPORT_C CObexHeader*& CObexHeaderList::ObexHeaderL(TInt aPos) 
	/**
	 * Get a non constant reference to the CObexHeader object pointer at 
	 * position aPos into the list.
	 *
	 *
	 * @param aPos is a list index specifying the CObexHeader object pointer to
	 *        be returned
	 *  
	 * @leave leave if aPos is out of range(KErrArgument) or the list is empty
	 *        (KErrNotFound)
	 * @return return a non constant reference to the CObexHeader object 
	 *         pointer
	 */
	{
	TInt headerCount = iObexHeaders->Count();
	if(headerCount == 0)
		User::Leave(KErrNotFound);
	if(aPos < 0 || aPos >= headerCount)
		User::Leave(KObexHeaderListIndexOutOfRange);
	return (*iObexHeaders)[aPos];
	}

/**
Exports the Obex header list to the attachment. The header list is then stored with
the attachment.
@param aAttachment The attachment to store the header list for.
@leave System-wide error codes.
*/
EXPORT_C void CObexHeaderList::ExportToAttachmentL(CMsvAttachment& aAttachment) const
	{
	// create a buffer stream to hold the header data
	CBufFlat* obexHdrBuf = CBufFlat::NewL(KObexHeaderListSegmentSize);
	CleanupStack::PushL(obexHdrBuf);
	
	// externalise the header list to the buffer
	RBufWriteStream writeStream(*obexHdrBuf);
	writeStream.PushL();
	writeStream << *this;
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	
	// set the headers in the attachment
	aAttachment.SetDesC8AttributeL(KUidObexHeaders, obexHdrBuf->Ptr(0));

	CleanupStack::PopAndDestroy(obexHdrBuf);
	}

/**
Imports the Obex header list from the attachment. The header list is retrieved and
stored in this object. Any existing header data is over-written with the headers from
the attachment. If the headers have not been set, this obex list will still be
over written and result in zero headers.
@param aAttachment The attachment to retieve the header list from.
@leave System-wide error codes.
*/	
EXPORT_C void CObexHeaderList::ImportFromAttachmentL(CMsvAttachment& aAttachment)
	{
	TPtrC8 headerData;
	if( aAttachment.GetDesC8Attribute(KUidObexHeaders, headerData) == KErrNone )
		{
		// header data exists, attempt to import it
		RDesReadStream readStream(headerData);
		readStream.PushL();
		readStream >> *this;	
		CleanupStack::PopAndDestroy(&readStream);
		}
	else
		{
		// headers have not been set so no headers, clear out list
		iObexHeaders->ResetAndDestroy();
		}
	}

EXPORT_C void CObexHeaderList::ExternalizeL(RWriteStream& aWriteStream) const
	/**
	 * Externalise the contents of the CObexHeaderList class to a 
	 * stream in CMsvStore
	 *
	 * @param  aWriteStream is a reference to a RMsvWriteStream
	 *  
	 * @leave it may leave with one of the system wide error code 
	 */
	{
	TInt32 headerCount = iObexHeaders->Count();
	aWriteStream.WriteInt32L(headerCount);
	TInt32 loopCount;
	for(loopCount = 0; loopCount<headerCount; loopCount++)
	//Data stored in the class CObexHeader is externalized in this order:
	//header attribute, header ID, header type and 
	//header value, which is encoded according to  the header type 
		{
		aWriteStream.WriteUint16L((*iObexHeaders)[loopCount]->Attributes()); 
		aWriteStream.WriteUint8L((*iObexHeaders)[loopCount]->HI()); 
		CObexHeader::THeaderType headerType = (*iObexHeaders)[loopCount]->Type();
		aWriteStream.WriteUint8L(headerType);
		TInt32 bufLength = 0;
		switch(headerType)
			{
			case CObexHeader::EUnicode:
				bufLength = static_cast<TInt32> 
					         ((*iObexHeaders)[loopCount]->AsUnicode().Length());
				aWriteStream.WriteInt32L(bufLength); 
				if(bufLength > 0)
					aWriteStream.WriteL((*iObexHeaders)[loopCount]->AsUnicode());
				break;
			case CObexHeader::EByteSeq:
				bufLength = static_cast<TInt32>
					         ((*iObexHeaders)[loopCount]->AsByteSeq().Length());
				aWriteStream.WriteInt32L(bufLength); 
				if(bufLength > 0)
					aWriteStream.WriteL((*iObexHeaders)[loopCount]->AsByteSeq());
				break;
			case CObexHeader::EByte:
				aWriteStream.WriteUint8L((*iObexHeaders)[loopCount]->AsByte());
				break;
			case CObexHeader::EFourByte:
			default:
				aWriteStream.WriteUint32L((*iObexHeaders)[loopCount]->AsFourByte());		
			}
		}
	}

EXPORT_C void CObexHeaderList::InternalizeL(RReadStream& aReadStream)
	/**
	 * Internalise the contents of the CObexHeaderList class from a 
	 * stream in CMsvStore
	 *
	 * @param  aReadStream is a reference to a RMsvReadStream
	 *  
	 * @leave it may leave with one of the system wide error code 
	 */
	{
	if(iObexHeaders->Count() > 0) //remove old data if present
		iObexHeaders->ResetAndDestroy();

	// Read new data from stream
	
	//get the number of headers
	TInt32 headerCount = aReadStream.ReadInt32L();
	
	TInt32 loopCount;
	for(loopCount = 0; loopCount < headerCount; loopCount++)
	//Data stored in the class CObexHeader is internalized in this order:
	//header attribute, header ID, header type and 
	//header value, which is encoded according to  the header type
	//Note the header ID and value are set together by calling one of the 
	//Set???() member functions of CObexHeader class 
		{
		CObexHeader* obexHeader = CObexHeader::NewL();
		CleanupStack::PushL(obexHeader); 

		obexHeader->SetAttributes(aReadStream.ReadUint16L());

		TUint8 headerId = aReadStream.ReadUint8L();

		CObexHeader::THeaderType headerType = 
			static_cast<CObexHeader::THeaderType>(aReadStream.ReadUint8L());

		TInt32 bufLength = 0;
		switch(headerType)
			{
			case CObexHeader::EUnicode:
				bufLength = aReadStream.ReadInt32L();
				if(bufLength > 0)
					{
					HBufC16* buf = HBufC16::NewLC(bufLength);
					TPtr16 bufPtr = buf->Des();

					aReadStream.ReadL(bufPtr, bufLength);

					obexHeader->SetUnicodeL(headerId,*buf);
					CleanupStack::PopAndDestroy(buf);
					}
				break;
			case CObexHeader::EByteSeq:
				bufLength = aReadStream.ReadInt32L();
				if(bufLength > 0)
					{
					HBufC8* buf = HBufC8::NewLC(bufLength);
					TPtr8 bufPtr = buf->Des();

					aReadStream.ReadL(bufPtr, bufLength);

					obexHeader->SetByteSeqL(headerId,*buf);
					CleanupStack::PopAndDestroy(buf);
					}
				break;
			case CObexHeader::EByte:
				obexHeader->SetByte(headerId,aReadStream.ReadUint8L());
				break;
			case CObexHeader::EFourByte:
			default:
				obexHeader->SetFourByte(headerId,aReadStream.ReadUint32L());
			}
		
		//CObexHeaderList object takes ownership of the internalised obexheader
		iObexHeaders->Append(obexHeader);
		CleanupStack::Pop(obexHeader);
		}
	}

EXPORT_C void CObexHeaderList::AddHeadersToBaseObjectL
                               (CObexBaseObject& aObexBaseObj)
	/**
	 * Add all the CObexHeader objects contained in a list to a 
	 * CObexBaseObject
	 *
	 *
	 * @param aObexBaseObj is a reference to a CObexBaseObject
	 *  
	 * @leave it may leave with one of the system wide error code 
	 * 
	 */                               
	{
	TInt headerCount = iObexHeaders->Count();
	TInt loopCount;
	for(loopCount = 0; loopCount < headerCount; loopCount++)
		{
		aObexBaseObj.AddHeaderL(*(*iObexHeaders)[0]);
		
		//transfer ownership of the CObexHeader object just added to 
		//aObexBaseObj to aObexBaseObj
		iObexHeaders->Remove(0);

		}
	iObexHeaders->Compress();
	TObexHeaderMask currentMask = aObexBaseObj.HeaderMask(); 
	aObexBaseObj.SetHeaderMask(static_cast<TObexHeaderMask>(currentMask | KObexHdrUserDefined));
	}
