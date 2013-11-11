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

#ifndef __CSMSEMAILFIELDS_H__
#define __CSMSEMAILFIELDS_H__

#include <e32base.h>
#include <s32file.h>
#include <badesca.h>

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
#include <smut.h>
#include <msvstd.h>
#endif

class CMsvStore;

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
class CHeaderFields;
#endif


/**
Encapsulates the address and subject fields for Email sent over SMS.

@publishedAll
@released
*/
NONSHARABLE_CLASS (CSmsEmailFields) : public CBase
	{
public:
	IMPORT_C static CSmsEmailFields* NewL();
	IMPORT_C static CSmsEmailFields* NewL(const CSmsEmailFields& aEmailFields);
	IMPORT_C virtual ~CSmsEmailFields();

	IMPORT_C void Reset();
	IMPORT_C TInt Length() const;
	IMPORT_C TBool HasAddress() const;
	IMPORT_C HBufC* ComposeLC() const;
	TInt ParseL(const TDesC& aBuffer);

	IMPORT_C void AddAddressL(const TDesC& aAddress);
	IMPORT_C void RemoveAddress(TInt aIndex);
	IMPORT_C const MDesCArray& Addresses() const;
	
	IMPORT_C void SetSubjectL(const TDesC& aSubject);
	IMPORT_C const TDesC& Subject() const;
	
	void RestoreL(CMsvStore& aStore);
	void StoreL(CMsvStore& aStore) const;

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	void StoreDBL(CMsvStore& aStore);
	void ReStoreDBL(CMsvStore& aStore);
	TInt ConvertToTInt(TDesC16& aStr);
	void GetRecipientListL(TDesC16& aStr);
	TInt GetBufSize();
#endif

private:
	CSmsEmailFields();
	void ConstructL();
	void ConstructL(const CSmsEmailFields& aEmailFields);

	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
private:	
	CDesCArray*		iAddresses;
	HBufC*			iSubject;
	};

#endif	// __CSMSEMAILFIELDS_H__
