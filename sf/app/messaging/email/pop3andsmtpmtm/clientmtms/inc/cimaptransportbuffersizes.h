// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIMAPTRANSPORTBUFFERSIZES_H__
#define __CIMAPTRANSPORTBUFFERSIZES_H__

#include <e32base.h>

/**
Maximum number of transport buffer size entries

@publishedPartner
@released
*/
const TInt KMaxImapTransportBufferSizes = 32;

/**
Stores the transport buffer sizes to be used for IMAP operations.

Each rule consists of a bearer types field, and some buffer size information.
The bearer types are defined by the TCommDbBearer enumeration and it is
possible to specify multiple bearer types for one rule by performing a
bitwise OR operation on the values from the enumeration and passing the
resulting value to the various APIs.
The buffer size options supported are:
1) Fetch request size - Maximum amount of data that can be requested from the
server by a FETCH command. Fetching large email components such as atatchments
may require multiple fetch requests to be made dependent on the size specified.
2) Fetch commands outstanding - The maximum number of fetch responses that can
be outstanding at any one time. Fetching large email components such as
attachments may require multiple fetch requests to be made. It is possible to
send multiple fetch requests to the server at the same time, and this option
determines how many responses can be outstanding at any one time.

@publishedPartner
@released
*/
class CImapTransportBufferSizes : public CBase
	{
public:
	IMPORT_C static CImapTransportBufferSizes* NewL();
	IMPORT_C static CImapTransportBufferSizes* NewLC();
	~CImapTransportBufferSizes();

	IMPORT_C CImapTransportBufferSizes* CopyL();
	IMPORT_C CImapTransportBufferSizes* CopyLC();	
	IMPORT_C TInt AddSizesL(TInt aPos, TUint32 aBearerTypes, TInt aFetchRequestSize, TInt aMaxOutstandingFetchResponses);
	IMPORT_C TBool DeleteSizesL(TInt aPos);
	IMPORT_C TInt DeleteBearerTypesL(TUint32 aBearerTypes, TBool& aSizesRemoved);
	IMPORT_C TBool GetSizesL(TInt aPos, TUint32& aBearerTypes, TInt& aFetchRequestSize, TInt& aMaxOutstandingFetchResponses) const;
	IMPORT_C TInt GetSizesL(TUint32 aBearerTypes, TInt& aFetchRequestSize, TInt& aMaxOutstandingFetchResponses) const;
	IMPORT_C void Reset();

private:
	CImapTransportBufferSizes();
	void ConstructL();

private:
	struct TImapTransportBufferSizes
		{
		TUint32 iBearerTypes;
		TInt iFetchRequestSize;
		TInt iMaxOutstandingFetchResponses;
		};

private:
	RArray<TImapTransportBufferSizes> iSizesList;
	};

#endif //__CIMAPTRANSPORTBUFFERSIZES_H__
