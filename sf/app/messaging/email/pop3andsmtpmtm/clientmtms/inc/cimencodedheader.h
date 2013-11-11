// CImEncodedHeader.h
//

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

/**
 @file
 @internalTechnology 
*/

#ifndef __CIMENCODEDHEADER_H__
#define __CIMENCODEDHEADER_H__

#include <msvstore.h>	// CMsvStore
#include "MIUTHDR.H"

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	class CCHeaderFields;
#endif 

// IMail-specific stream type within an IMail file store for CImEncodedHeader
const TUid KUidMsgFileIMailEncodedHeader = {0x101FD0E3};		// 270520547 decimal

//------------------------------------------------------------------------------

class CImEncodedHeader : public CBase
/** Encapsulates an undecoded Internet Mail header.
This class encapsulates the 8 bit data of supported fields for the CImHeader
prior to decoding. It allows this information to populate an associated
CImHeader class for redecoding.

@internalTechnology
@released
*/
	{
public:

	static CImEncodedHeader* NewLC();
	static CImEncodedHeader* NewL();
	~CImEncodedHeader();
	void Reset();

	void InternalizeL(RMsvReadStream& aReadStream);
	void ExternalizeL(RMsvWriteStream& aWriteStream) const;
	void RestoreL(CMsvStore& aMessageStore);
	void StoreL  (CMsvStore& aMessageStore) const;
	// From field
	const TPtrC8 From() const;
	void SetFromL(const TDesC8& aFrom);

	// Subject field
	const TPtrC8 Subject() const;
	void SetSubjectL(const TDesC8& aSubject);

	// ReplyTo field
	const TPtrC8 ReplyTo() const;
	void SetReplyToL(const TDesC8& aReplyTo);
	void CopyFromHeaderL(
		const CImHeader& aFromHeader, TBool aDeleteExistingData = false);
	void CopyToHeaderL(CImHeader& aToHeader) const;

	void  SetDecodeCharset(TUint aCharset);
	TUint DecodeCharset() const;

	TUint16 Version() const;
	void SetVersion(TUint16 aVersion);
	TInt DataSize() const;

	CDesCArray& ToRecipients ();
	CDesCArray& CcRecipients ();
	CDesCArray& BccRecipients();

	const CDesCArray& ToRecipients () const;
	const CDesCArray& CcRecipients () const;
	const CDesCArray& BccRecipients() const;

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	void StoreEncodedDBL  (CMsvStore& aMessageStore) const;
	void ReStoreEncodedDBL(CMsvStore& aMessageStore);
	void CreateEncodedArrayListL(TPtrC16 aEncodedStr ,TInt aI);
	TInt EncodedBufSize()const;
#endif 

private:
	CImEncodedHeader();
	void ConstructL();
	friend void CopyArrayL(const CDesCArray& aSource, CDesCArray& aDestination);

private:
	TUint16     iVersion;
	HBufC8*     iFrom;
	HBufC8*     iSubject;
	HBufC8*     iReplyTo;
	CDesCArray* iTo;
	CDesCArray* iCc;
	CDesCArray* iBcc;
	TUint       iDecodeCharset;
	};

#endif
