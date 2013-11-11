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

#include <utf.h>
#include <s32strm.h>

#include "pbapxml.h"

#include "btaccesshostlog.h"


//DTD strings (in UTF-8 encoding)
_LIT8(KXMLVersion, 			"<?xml version=\"1.0\"?>\n");
_LIT8(KXMLDocType, 			"<!DOCTYPE vcard-listing SYSTEM \"vcard-listing.dtd\">\n");
_LIT8(KXMLBegin, 			"<vCard-listing version=\"1.0\">\n");
_LIT8(KXMLEnd, 				"</vCard-listing>\n");
_LIT8(KXMLEntryPreamble,	"<card handle = \"");
_LIT8(KXMLEntryMidamble, 	"\" name = \"");
_LIT8(KXMLEntryPostamble, 	"\"/>\n");
_LIT8(KXMLHandleExtension,	".vcf");
_LIT8(KXMLHandleFormat,		"%x");

_LIT8(KXMLEscapedQuote, 		"&quot;");
_LIT8(KXMLEscapedAmpersand,		"&amp;");
_LIT8(KXMLEscapedLessThan,		"&lt;");
_LIT8(KXMLEscapedGreaterThan,	"&gt;");


const TInt KXMLHandleLength = 8; // handle represented as 32-bit hex string


/*static*/ void PbapDTD::WriteBeginL(RWriteStream& aWriteStream)
	{
	LOG_STATIC_FUNC
	aWriteStream.WriteL(KXMLVersion);
	aWriteStream.WriteL(KXMLDocType);
	aWriteStream.WriteL(KXMLBegin);		
	}
		
/*static*/ void PbapDTD::WriteListingEntryL(RWriteStream& aWriteStream, TInt aHandle, const TDesC& aName)
	{
	LOG_STATIC_FUNC
	// convert handle to hexadecimal string
	TBuf8<KXMLHandleLength> handleBuf;	
	handleBuf.Format(KXMLHandleFormat, aHandle);
	
	// convert formatted name to UTF-8
	RBuf8 name(CnvUtfConverter::ConvertFromUnicodeToUtf8L(aName));
	CleanupClosePushL(name);

	// escape &,<,>, and " characters in name
	EscapeAttributeValueL(name);
		
	// write to the stream		
	aWriteStream.WriteL(KXMLEntryPreamble);
	aWriteStream.WriteL(handleBuf);
	aWriteStream.WriteL(KXMLHandleExtension);
	aWriteStream.WriteL(KXMLEntryMidamble);
	aWriteStream.WriteL(name);
	aWriteStream.WriteL(KXMLEntryPostamble);

	CleanupStack::PopAndDestroy(); //name
	}


/*static*/ void PbapDTD::WriteEndL(RWriteStream& aWriteStream)
	{
	LOG_STATIC_FUNC
	aWriteStream.WriteL(KXMLEnd);
	}

/**
Add XML escaping to attribute value. Replaces " with &quot;, & with &amp;,
< with &lt; and > with &gt; (attribute values always surrounded by quotes
so no need to escape apostrophes)  
*/	
/*static*/ void PbapDTD::EscapeAttributeValueL(RBuf8& aValue)
	{
	LOG_STATIC_FUNC
	// iterate through descriptor adding escaping if necessary. The descriptor has
	// to be resized each time an escape sequence is added, however these characters
	// are unlikely to appear in the attribute value
	for (TInt ii = 0; ii < aValue.Length(); ++ii)
		{
		switch (aValue[ii])
			{
			case '\"':
				{
				aValue.ReAllocL(aValue.Length() + KXMLEscapedQuote().Length());
				aValue.Replace(ii, 1, KXMLEscapedQuote);
				}
				break;
			case '&':
				{	
				aValue.ReAllocL(aValue.Length() + KXMLEscapedAmpersand().Length());
				aValue.Replace(ii, 1, KXMLEscapedAmpersand);
				}
				break;
			case '<':
				{	
				aValue.ReAllocL(aValue.Length() + KXMLEscapedLessThan().Length());
				aValue.Replace(ii, 1, KXMLEscapedLessThan);
				}
				break;
			case '>':
				{	
				aValue.ReAllocL(aValue.Length() + KXMLEscapedGreaterThan().Length());
				aValue.Replace(ii, 1, KXMLEscapedGreaterThan);
				}
				break;
			default:
				break;
			}
		}
	}

