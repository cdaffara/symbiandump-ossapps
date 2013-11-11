// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __VPROPBINARYFILE_H__
#define __VPROPBINARYFILE_H__

#include <s32strm.h>

NONSHARABLE_CLASS(CParserPropertyValueBinaryFile) : public CParserPropertyValueBinary
/** A file property value parser.

It is used to store a file handle so that the binary data can be read through the handle when exporting.
It is not used when importing.
The UID for a binary property value is KVersitPropertyBinaryUid. 
@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CParserPropertyValueBinaryFile* NewL(const RFile& aFileHandle);
	IMPORT_C ~CParserPropertyValueBinaryFile();
public: // from CParserPropertyValue
	void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput);

private:
	void ConstructL(const RFile& aFileHandle);
private:
	RFileReadStream iFileStream;
	};
	
#endif //__VPROPBINARYFILE_H__
