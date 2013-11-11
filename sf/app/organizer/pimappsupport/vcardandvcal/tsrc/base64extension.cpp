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

/*
This test provides an example of how to configure Versit parser to internalize vFormat data that
contains multi-line base64 data which does not conform to the standard line termination.  That
is, there is neither blank line at the end nor leading spaces at each line, e.g.

BEGIN:VCARD
LOGO;VALUE=uri;ENCODING=BASE64:
aHR0cDovL2ltYWdlLmV4Y2l0ZS5jby5qcC9qcC93b3JsZC93b3JsZF9oZWFkZXIu
Z2lm
LOGO;VALUE=uri;ENCODING=BASE64:aHR0cDovL2ltYWdlLmV4Y2l0ZS5jby5qcC9qcC93b3JsZC93b3JsZF9oZWFkZXIu
Z2lm
LOGO;VALUE=uri;ENCODING=BASE64:aHR0cDovL2ltYWdlLmV4Y2l0ZS5jby5qcC9qcC93b3JsZC93b3JsZF9oZWFkZXIu
  Z2lm
END:VCARD

In order for versit to detect the end of the property LOGO above, the client needs

1. Derive your extension interface from MVersitPlugIn and MVersitPlugInExtensionBase64Ending
2. Implement the virtual functions of basic MVersitPlugIn and the extension interface MVersitPlugInExtensionBase64Ending.
	particularly, BlankLineAndLeadingSpaceNotRequired should return ETrue.
3. Set your plugin via API CVersitParser::SetPlugIn(MVersitPlugIn* aPlugIn)
4. Call API CVersitParser::InternalizeL to internalize the data as usual.

*/

#include "base64extension.h"

//Returns the interface
void CTestVersitExtension::GetInterface(TUid aInterfaceUid, TAny*& aInterface)
/** Returns interface extension. */
	{
	if (aInterfaceUid == KUidVersitPluginExtensionBase64Termination)
		{
		aInterface = static_cast<MVersitPlugInExtensionBase64Ending*>(this);
		}
	}

//Implementation of the extension interface interface MVersitPlugInExtensionBase64Ending

CTestVersitExtension* CTestVersitExtension::NewL()
	{
	CTestVersitExtension* self = new(ELeave) CTestVersitExtension;
	return self;
	}

TBool CTestVersitExtension::BlankLineAndLeadingSpaceNotRequired()
	{
	return ETrue;
	}

//Implementation of basic plugin interface MVersitPlugIn
TBool CTestVersitExtension::AddSpace()
	{//Default behaviour
	return ETrue;
	}
	
TBool CTestVersitExtension::DeleteAllSpaces()
	{//Default behaviour
	return ETrue;
	}
	
TBool CTestVersitExtension::NeedsBlankLine()
	{//This doesn't take effect if BlankLineAndLeadingSpaceNotRequired returns ETrue
	return EFalse;
	}
	
void CTestVersitExtension::RemoveEscaping(TPtr16& /*aText*/)
	{//Default behaviour
	}
	
void CTestVersitExtension::AddEscaping(HBufC16*& /*aText*/)
	{//Default behaviour
	}
	
TBool CTestVersitExtension::WrapLine(RWriteStream& /*aStream*/, TInt& /*aCurrentLineLength*/, const TPtr8& /*aText*/)
	{//Default behaviour
	return EFalse;
	}
		
TBool CTestVersitExtension::EncodingType(Versit::TVersitEncoding& /*aEncoding*/, TBool /*aRequiresEncoding*/,
					Versit::TVersitEncoding /*aDefaultEncoding*/, TUid /*aPropertyUid*/, TUint /*aPropertyCharsetId*/)
	{//Default behaviour
	return EFalse;
	}
	
const TDesC8& CTestVersitExtension::EncodingName(Versit::TVersitEncoding /*aEncoding*/)
	{//Default behaviour
	return KNullDesC8;
	}


