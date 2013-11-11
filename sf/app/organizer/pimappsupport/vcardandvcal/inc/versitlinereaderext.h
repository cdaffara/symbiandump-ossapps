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
#ifndef __VERSITLINEREADEREXT_H__
#define __VERSITLINEREADEREXT_H__

#include <e32base.h>

/** Extension mechanism for CLineReader
This is an internal class and is only for use by CLineReader
@internalComponent
@released
*/
NONSHARABLE_CLASS(CLineReaderExtension) : public CBase
	{
	friend class CLineReader;
private:
	CLineReaderExtension();
	~CLineReaderExtension();

	static CLineReaderExtension* NewL();
	
	void DeleteBase64ValueBuffer();
	HBufC8* CreateBase64ValueBufferL();
	HBufC8* Base64ValueBuffer();
private:
	TUint8 iBuf[1024];
	TInt iOffset;
	TInt iSize;

// Hold the line buffer temporarily. 
// It is only used when the client wants
// the base64 extended behaviour defined
// in MVersitPlugInExtensionBase64Ending
	HBufC8* iLineBase64Value;
	};
	
#endif //__VERSITLINEREADEREXT_H__