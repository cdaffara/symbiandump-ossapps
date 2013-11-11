/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "OpmlParser.h"
#include "debug.h"
#include <f32file.h>
#include <bautils.h>
#include <s32file.h>
#include <charconv.h>
#include <xml/stringdictionarycollection.h>
#include <utf.h>

using namespace Xml;
const TInt KMaxParseBuffer = 2048;
const TInt KMaxStringBuffer = 1024;
COpmlParser::COpmlParser(CFeedEngine& aFeedEngine, RFs& aFs) : iFeedEngine(aFeedEngine),iFs(aFs)
{	
}

COpmlParser::~COpmlParser()
{	
}

void COpmlParser::ParseOpmlL(const TFileName &feedFileName, TBool aSearching)
	{
	DP1("ParseOpmlL BEGIN: %S", &feedFileName);
	
	iSearching = aSearching;
	_LIT8(KXmlMimeType, "text/xml");
	// Contruct the parser object
	CParser* parser = CParser::NewLC(KXmlMimeType, *this);
	iOpmlState = EStateOpmlRoot;
	iEncoding = EUtf8;
	iNumFeedsAdded = 0;
	ParseL(*parser, iFs, feedFileName);

	CleanupStack::PopAndDestroy(parser);	
	//DP("ParseFeedL END");
	}

// from MContentHandler
void COpmlParser::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt /*aErrorCode*/)
	{
	DP("OnStartDocumentL()");
	HBufC* charset = HBufC::NewLC(KMaxParseBuffer);
	charset->Des().Copy(aDocParam.CharacterSetName().DesC());
	iEncoding = EUtf8;
	if (charset->CompareF(_L("utf-8")) == 0) {
		DP("setting UTF8");
		iEncoding = EUtf8;
	} else if (charset->CompareF(_L("ISO-8859-1")) == 0) {
		iEncoding = EUtf8; //Latin1;
	} else {
		DP1("unknown charset: %S", &charset);
	}
	CleanupStack::PopAndDestroy(charset);
	}

void COpmlParser::OnEndDocumentL(TInt aErrorCode)
	{
	iFeedEngine.OpmlParsingCompleteL(aErrorCode, iNumFeedsAdded);
	//DP("OnEndDocumentL()");
	}

void COpmlParser::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt /*aErrorCode*/)
	{
	TBuf<KMaxStringBuffer> str;
	str.Copy(aElement.LocalName().DesC());
	DP2("OnStartElementL START state=%d, element=%S", iOpmlState, &str);
	iBuffer.Zero();
	switch (iOpmlState) {
	case EStateOpmlRoot:
		// <body>
		if (str.CompareF(KTagBody) == 0) {
			iOpmlState = EStateOpmlBody;
		}
		break;
	case EStateOpmlBody:
		// <body> <outline>
		
		if(str.CompareF(KTagOutline) == 0) {
			iOpmlState = EStateOpmlOutline;
		}
		
		// there are two variations on OPML, where the <outline> tags sit either
		// directly below <body>, or inside a collective <body> <outline>
		// by checking if the <body> <outline> has arguments, we can support both
		// by falling through to the nextstate
		if (aAttributes.Count() == 0)
			{
			break;
			}
	case EStateOpmlOutline:
		// <body> <outline> <outline...
		if(str.CompareF(KTagOutline) == 0) {
			iOpmlState=EStateOpmlOutlineOutline;
			CFeedInfo* newFeed = CFeedInfo::NewLC();
			
			TBool hasTitle = EFalse;
			TBool hasUrl = EFalse;
			
			for (int i=0;i<aAttributes.Count();i++) {
				RAttribute attr = aAttributes[i];
				TBuf<KMaxStringBuffer> attr16;
				attr16.Copy(attr.Attribute().LocalName().DesC().Left(KMaxStringBuffer));
				HBufC* val16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
						attr.Value().DesC().Left(KMaxParseBuffer));
				CleanupStack::PushL(val16);
				
				// xmlUrl=...
				if (attr16.Compare(KTagXmlUrl) == 0 || attr16.Compare(KTagUrl) == 0) {
					newFeed->SetUrlL(*val16);
					hasUrl = ETrue;			
				// htmlUrl
				} else if (attr16.Compare(KTagHtmlUrl) == 0) {
					newFeed->SetLinkL(*val16);
					hasUrl = ETrue;
				// title=...
				} else if (attr16.Compare(KTagTitle) == 0) {
					newFeed->SetTitleL(*val16);
					hasTitle = ETrue;
				// description=
				} else if (attr16.Compare(KTagDescription) == 0) {
					newFeed->SetDescriptionL(*val16);
				// text=
				} else if (attr16.Compare(KTagText) == 0) {
					if (!hasTitle) {
						newFeed->SetTitleL(*val16);
						hasTitle = ETrue;
					}
				} 
				CleanupStack::PopAndDestroy(val16);
			}

			if (!hasUrl) {
				CleanupStack::PopAndDestroy(newFeed);
				break;
			}
			
			if (!hasTitle) {
				newFeed->SetTitleL(newFeed->Url());
			}
			
			// if the title is the same as the URL, it is hardly a custom
			// title, so let's replace it on update
			if (newFeed->Title().Length() &&
					newFeed->Url().Length() &&
					newFeed->Title().Compare(newFeed->Url()) != 0) {
				newFeed->SetCustomTitle();
			}
			
			if (iSearching) {
				iFeedEngine.AddSearchResultL(newFeed);
				CleanupStack::Pop(newFeed);
			} else {
				TRAPD(err, iFeedEngine.AddFeedL(*newFeed))
				if (err == KErrNone)
					{
					iNumFeedsAdded++;
					}
				CleanupStack::PopAndDestroy(newFeed);
			}
		}
		break;
	default:
		DP2("Ignoring tag %S when in state %d", &str, iOpmlState);
		break;
	}
	DP1("OnStartElementL END state=%d", iOpmlState);
	}

void COpmlParser::OnEndElementL(const RTagInfo& aElement, TInt /*aErrorCode*/)
	{
	
	TDesC8 lName = aElement.LocalName().DesC();
	TBuf<KMaxStringBuffer> str;
	str.Copy(aElement.LocalName().DesC());

	DP2("OnEndElementL START state=%d, element=%S", iOpmlState, &str);

	switch (iOpmlState) {
		case EStateOpmlOutlineOutline:
			iOpmlState=EStateOpmlOutline;
			break;
		case EStateOpmlOutline:
			iOpmlState=EStateOpmlBody;
			break;
		case EStateOpmlBody:
			iOpmlState = EStateOpmlRoot;
			break;
		default:
			// fall back to body level when in doubt
			iOpmlState = EStateOpmlBody;
			//DP("Don't know how to handle end tag %S when in state %d"), &str, iFeedState);
			break;
	}

	DP1("OnEndElementL END state=%d", iOpmlState);	
	}

void COpmlParser::OnContentL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{
	//DP("OnContentL()");
	}

void COpmlParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/)
	{
	//DP("OnStartPrefixMappingL()");
	}

void COpmlParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
	{
	//DP("OnEndPrefixMappingL()");
	}

void COpmlParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{
	//DP("OnIgnorableWhiteSpaceL()");
	}

void COpmlParser::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
	{
	//DP("OnSkippedEntityL()");
	}

void COpmlParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/)
	{
	//DP("OnProcessingInstructionL()");
	}

void COpmlParser::OnError(TInt aErrorCode)
	{
	DP1("COpmlParser::OnError %d", aErrorCode);
	TRAP_IGNORE(iFeedEngine.OpmlParsingCompleteL(aErrorCode, iNumFeedsAdded));
	}

TAny* COpmlParser::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	//DP("GetExtendedInterface()");
	return NULL;
	}
