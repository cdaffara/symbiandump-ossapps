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

#ifndef __BASE64EXTENSION_H__
#define __BASE64EXTENSION_H__

#ifndef __E32STD_H__
#include <e32std.h>
#endif

#ifndef __VOBSERV_H__
#include <vobserv.h>
#endif


class CTestVersitExtension : public CBase, public MVersitPlugIn, public MVersitPlugInExtensionBase64Ending
	{
public: //from MVersitPlugIn
	virtual TBool AddSpace(); 
	virtual TBool DeleteAllSpaces();
	virtual TBool NeedsBlankLine();
	virtual void RemoveEscaping(TPtr16& aText);
	virtual void AddEscaping(HBufC16*& aText);
	virtual TBool WrapLine(RWriteStream& aStream, TInt& aCurrentLineLength, const TPtr8& aText);	
	virtual TBool EncodingType(Versit::TVersitEncoding& aEncoding, TBool aRequiresEncoding,
								Versit::TVersitEncoding aDefaultEncoding, TUid aPropertyUid, TUint aPropertyCharsetId);
	virtual const TDesC8& EncodingName(Versit::TVersitEncoding aEncoding);
	virtual void GetInterface(TUid aInterfaceUid, TAny*& aInterface);
	
	static CTestVersitExtension* NewL();
public:	//from MVersitPlugInExtensionBase64Ending
	virtual TBool BlankLineAndLeadingSpaceNotRequired();
	};


#endif
