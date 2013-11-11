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

#ifndef __CFETCHBODYINFO_H__
#define __CFETCHBODYINFO_H__

#include <e32std.h>
#include <msvstd.h>
#include <miuthdr.h>
#include <cimcaf.h>

/**
Represents a message body or attachent part  
@internalTechnology
@prototype
*/

class CFetchBodyInfo : public CBase
	{
public:
	IMPORT_C static CFetchBodyInfo* NewLC(TMsvId aPartId);
	IMPORT_C static CFetchBodyInfo* NewL(TMsvId aPartId);
	~CFetchBodyInfo();
	
	IMPORT_C TInt32 SizeToFetch();
	IMPORT_C TMsvId PartId();
	IMPORT_C HBufC8* RelativePath() const;
	IMPORT_C TInt32 BodyPartRemainingSize();
	IMPORT_C TBool IsText();
	
	IMPORT_C void SetSizeToFetch(TInt32 aSize);
	IMPORT_C void SetIsText(TBool aIsText);
	IMPORT_C void SetPartId(const TMsvId aId);
	IMPORT_C void SetRelativePathL(const TDesC8& aPath); 
	IMPORT_C void SetBodyPartRemainingSize(const TInt32 aSize);
	IMPORT_C void SetContentTransferEncoding(const TImEncodingType);
	IMPORT_C void SetCharsetId(const TUint aCharsetId);
	IMPORT_C TImEncodingType ContentTransferEncoding() const;
	IMPORT_C TUint CharsetId() const;
	IMPORT_C void SetCaf(CImCaf* aCaf);
	IMPORT_C void ResetCaf();
	IMPORT_C CImCaf* Caf() const;
	IMPORT_C TInt32 BytesFetched();
	IMPORT_C void IncrementBytesFetched(TInt32 aByteCount);
private:
	CFetchBodyInfo(TMsvId aPartId);
	void ConstructL();

private:
	//the size of the data we want to retrieve
	TInt32 iSizeToFetch;
	//if the data is text then its a body part, otherwise its an attachment
	TBool iIsText;
	// the tmsvid where the data will be stored in the message store
	TMsvId  iPartId;
	//  the body sections we will be retrieving, eg 1, or 1.2 or 1.text
	HBufC8* iRelativePath;
	//for a partial fetch this is the amount of data not being fetched
	TInt32  iBodyPartRemainingSize;
	// the encoding method thats been used to encode the data
	TImEncodingType iContentTransferEncoding;
	// the chrachter set id
	TUint iCharsetId;
	//Content Access Framework Wrapper class, not owned by this class.
	CImCaf* iCaf;
	//A count of the bytes that have been fetched.
	TInt32 iBytesFetched;
	};


#endif // __CFETCHBODYINFO_H__
