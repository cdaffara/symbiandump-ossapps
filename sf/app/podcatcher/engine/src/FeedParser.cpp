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

#include "FeedParser.h"
#include <f32file.h>
#include <bautils.h>
#include <s32file.h>
#include <charconv.h>
#include <xml/stringdictionarycollection.h>
#include <utf.h>
#include <tinternetdate.h>
#include "debug.h"
#include "podcastutils.h"

using namespace Xml;
const TInt KMaxParseBuffer = 1024;
const TInt KMaxStringBuffer = 100;

CFeedParser::CFeedParser(MFeedParserObserver& aCallbacks, RFs& aFs) : 	iCallbacks(aCallbacks), iRfs(aFs)
{
}

CFeedParser::~CFeedParser()
{	
}

void CFeedParser::ParseFeedL(const TFileName &feedFileName, CFeedInfo *info, TUint aMaxItems)
	{
	//DP1("ParseFeedL BEGIN: %S", &feedFileName);		
	
	_LIT8(KXmlMimeType, "text/xml");
	// Contruct the parser object
	CParser* parser = CParser::NewLC(KXmlMimeType, *this);
	iActiveFeed = info;
	iFeedState = EStateRoot;
	iActiveShow = NULL;
	iItemsParsed = 0;
	iMaxItems = aMaxItems;
	iStoppedParsing = EFalse;
	iEncoding = ELatin1;

	ParseL(*parser, iRfs, feedFileName);

	CleanupStack::PopAndDestroy(parser);	
	
	//DP("ParseFeedL END");
	}

// from MContentHandler
void CFeedParser::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt /*aErrorCode*/)
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
	CleanupStack::PopAndDestroy(charset);//buffer
	}

void CFeedParser::OnEndDocumentL(TInt /*aErrorCode*/)
	{
	//DP("OnEndDocumentL()");
	iCallbacks.ParsingCompleteL(iActiveFeed);
	}

void CFeedParser::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt /*aErrorCode*/)
	{
	if (iStoppedParsing) {
		iActiveShow = NULL;
		return;
	}
	
	TBuf<KMaxStringBuffer> str;
	str.Copy(aElement.LocalName().DesC());
	//DP2("OnStartElementL START state=%d, element=%S", iFeedState, &str);
	iBuffer.Zero();
	switch (iFeedState) {
	case EStateRoot:
		// <channel>
		if (str.CompareF(KTagChannel) == 0) {
			iFeedState = EStateChannel;
		}
		break;
	case EStateChannel:
		// <channel> <item>
		if(str.CompareF(KTagItem) == 0) {
			//DP("New item");
			iFeedState=EStateItem;

			iActiveShow = NULL;
			iActiveShow = CShowInfo::NewL();
			if (iActiveShow == NULL) {
				DP("Out of memory!");
				iStoppedParsing = ETrue;
				return;
			}
			iActiveShow->SetFeedUid(iActiveFeed->Uid());

		// <channel> <lastBuildDate>
		} else if (str.CompareF(KTagLastBuildDate) == 0) {
			DP("LastBuildDate BEGIN");
			iFeedState=EStateChannelLastBuildDate;
		// <channel> <link>
		}else if (str.CompareF(KTagTitle) == 0) {
			iFeedState=EStateChannelTitle;
		// <channel> <link>
		} else if (str.CompareF(KTagLink) == 0) {
			iFeedState = EStateChannelLink;
		// <channel> <description>
		} else if (str.CompareF(KTagDescription) == 0) {
			iFeedState=EStateChannelDescription;
		// <channel> <image>
		} else if (str.CompareF(KTagImage) == 0) {
			for (int i=0;i<aAttributes.Count();i++) {
				RAttribute attr = aAttributes[i];
				TBuf<KMaxStringBuffer> attr16;
				attr16.Copy(attr.Attribute().LocalName().DesC().Left(KMaxStringBuffer));
				HBufC* val16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(attr.Value().DesC().Left(KMaxParseBuffer));
				CleanupStack::PushL(val16);
						
				// href=...
				if (attr16.Compare(KTagHref) == 0) {
					iActiveFeed->SetImageUrlL(*val16);
				}
				CleanupStack::PopAndDestroy(val16);
			}
					
			iFeedState=EStateChannelImage;
		}
		break;
	case EStateChannelImage:
		// <channel> <image> <url>
		if (str.CompareF(KTagUrl) == 0) {
			iFeedState=EStateChannelImageUrl;
		} else {
			iFeedState=EStateChannelImage;
		}
		break;
	case EStateItem:
		// <channel> <item> <title>
		if (str.CompareF(KTagTitle) == 0) {
			iFeedState=EStateItemTitle;
		// <channel> <item> <link>
		} else if (str.CompareF(KTagLink) == 0) {
			iFeedState=EStateItemLink;
		// <channel> <item> <enclosure ...>
		} else if (str.CompareF(KTagEnclosure) == 0) {
			//DP("Enclosure START");
			for (int i=0;i<aAttributes.Count();i++) {
				RAttribute attr = aAttributes[i];
				TBuf<KMaxStringBuffer> attr16;
				attr16.Copy(attr.Attribute().LocalName().DesC());
				// url=...
				if (attr16.Compare(KTagUrl) == 0) {
					HBufC* val16 = HBufC::NewLC(KMaxParseBuffer);
					val16->Des().Copy(attr.Value().DesC());
					iActiveShow->SetUrlL(*val16);
					
					if (PodcastUtils::IsVideoShow(*val16)) {
						iActiveShow->SetShowType(EVideoPodcast);
					}
					CleanupStack::PopAndDestroy(val16);
				// length=...
				} else if (attr16.Compare(KTagLength) == 0) {
					TLex8 lex(attr.Value().DesC());
					TUint size = 0;
					lex.Val(size, EDecimal);
					iActiveShow->SetShowSize(size);
				}
			}
		// <channel> <item> <description>
		} else if (str.CompareF(KTagDescription) == 0) {
			iFeedState=EStateItemDescription;
		// <channel> <item> <pubdate>
		} else if (str.CompareF(KTagPubDate) == 0) {
			//DP("LastBuildDate BEGIN");
			iFeedState = EStateItemPubDate;
		}
		break;
	default:
		//DP2("Ignoring tag %S when in state %d", &str, iFeedState);
		break;
	}
//	DP1("OnStartElementL END state=%d", iFeedState);
	}

void CFeedParser::OnEndElementL(const RTagInfo& aElement, TInt /*aErrorCode*/)
	{
	
	if (iStoppedParsing) {
		return;
	}
	
	iBuffer.Trim();
	
	TDesC8 lName = aElement.LocalName().DesC();
	TBuf<KMaxStringBuffer> str;
	str.Copy(aElement.LocalName().DesC());

	//DP2("OnEndElementL START state=%d, element=%S", iFeedState, &str);

	switch (iFeedState) {
		case EStateChannelTitle:
			if(str.CompareF(KTagTitle) == 0) {
				if (iActiveFeed->CustomTitle() == EFalse) {
					iActiveFeed->SetTitleL(iBuffer);
				}
				iFeedState = EStateChannel;
			}
			break;
		case EStateChannelLink:
			iActiveFeed->SetLinkL(iBuffer);
			iFeedState = EStateChannel;
			break;
		case EStateChannelDescription:
			iActiveFeed->SetDescriptionL(iBuffer);
			iFeedState = EStateChannel;
			break;
		case EStateChannelLastBuildDate:
			{
			//DP("LastBuildDate END");
			TInternetDate internetDate;
			TBuf8<128> temp;
			temp.Copy(iBuffer);
					
			TRAPD(parseError, internetDate.SetDateL(temp));
			if(parseError == KErrNone) {				
				if (TTime(internetDate.DateTime()) > iActiveFeed->BuildDate()) {
					DP("Successfully parsed build date");
					iActiveFeed->SetBuildDate(TTime(internetDate.DateTime()));
				} else {
					DP("*** Nothing new, aborting parsing");
					iStoppedParsing = ETrue;
				}
			} else {
				DP("Failed to parse last build date");
			}
			iFeedState = EStateChannel;
			}
			break;
		case EStateChannelImageUrl:
			//DP1("Image url: %S", &iBuffer);
			iActiveFeed->SetImageUrlL(iBuffer);
			iFeedState = EStateChannelImage;
			break;
		case EStateChannelImage:
			if(str.CompareF(KTagImage) == 0) {
				iFeedState = EStateChannel;
			}
			break;
		case EStateItem:
			if (str.CompareF(KTagItem) == 0) 
				{				
				iCallbacks.NewShowL(*iActiveShow);
				
				delete iActiveShow;				
				
				// We should now be finished with the show.
				iActiveShow = NULL;
				
				iItemsParsed++;
				DP2("iItemsParsed: %d, iMaxItems: %d", iItemsParsed, iMaxItems);
				if (iItemsParsed >= iMaxItems) 
					{
					iStoppedParsing = ETrue;
					DP("*** Too many items, aborting parsing");
					}
				
				iFeedState=EStateChannel;
				}
			break;
		case EStateItemPubDate:
			DP1("PubDate END: iBuffer='%S'", &iBuffer);
			if (str.CompareF(KTagPubDate) == 0) {
				// hack for feeds that don't always write day as two digits
				TChar five(iBuffer[5]);
				TChar six(iBuffer[6]);
				
				if (five.IsDigit() && !six.IsDigit()) {
					TBuf<KMaxStringBuffer> fix;
					fix.Copy(iBuffer.Left(4));
					fix.Append(_L(" 0"));
					fix.Append(iBuffer.Mid(5));
					iBuffer.Copy(fix);
				}
				// end hack
				
				// hack for feeds that write out months in full
				
				if (iBuffer[11] != ' ') {
					TPtrC midPtr = iBuffer.Mid(8);
					
					int spacePos = midPtr.Find(_L(" "));
					
					if (spacePos != KErrNotFound) {
						//DP1("Month: %S", &midPtr.Left(spacePos));
						
						TBuf16<KBufferLength> newBuffer;
						newBuffer.Copy(iBuffer.Left(11));
						newBuffer.Append(_L(" "));
						newBuffer.Append(iBuffer.Mid(11+spacePos));
						//DP1("newBuffer: %S", &newBuffer);
						iBuffer.Copy(newBuffer);
					}
				}
				
				// hack for feeds that write days and months as UPPERCASE
				TChar one(iBuffer[1]);
				TChar two(iBuffer[2]);
				TChar nine(iBuffer[9]);
				TChar ten(iBuffer[10]);

				one.LowerCase();
				two.LowerCase();
				nine.LowerCase();
				ten.LowerCase();
				
				iBuffer[1] = one;
				iBuffer[2] = two;
				iBuffer[9] = nine;
				iBuffer[10] = ten;
				
				TBuf8<128> temp;
				temp.Copy(iBuffer);

				TInternetDate internetDate;
				TRAPD(parseError, internetDate.SetDateL(temp));
				if(parseError == KErrNone) {				
					//DP1("PubDate parse success: '%S'", &iBuffer);
					iActiveShow->SetPubDate(TTime(internetDate.DateTime()));
			
					
					DP6("Successfully parsed pubdate %d/%d/%d %d:%d:%d",
							iActiveShow->PubDate().DateTime().Year(),
							iActiveShow->PubDate().DateTime().Month(),
							iActiveShow->PubDate().DateTime().Day(),
							iActiveShow->PubDate().DateTime().Hour(),
							iActiveShow->PubDate().DateTime().Minute(),
							iActiveShow->PubDate().DateTime().Second());
							
				} else {
					DP2("Pubdate parse error: '%S', error=%d", &iBuffer, parseError);
				}
			}
			iFeedState=EStateItem;
			break;
		case EStateItemTitle:
			//DP1("title: %S", &iBuffer);
			iActiveShow->SetTitleL(iBuffer);
			iFeedState = EStateItem;
			break;
		case EStateItemLink:
			if (iActiveShow->Url().Length() == 0) {
				iActiveShow->SetUrlL(iBuffer);
				
				if (PodcastUtils::IsVideoShow(iBuffer)) {
					iActiveShow->SetShowType(EVideoPodcast);
				}
			}
			iFeedState = EStateItem;
			break;
		case EStateItemDescription:
			iActiveShow->SetDescriptionL(iBuffer);
			iFeedState = EStateItem;
			break;
		default:
			// fall back to channel level when in doubt
			iFeedState = EStateChannel;
			//DP2("Don't know how to handle end tag %S when in state %d", &str, iFeedState);
			break;
	}

	//DP1("OnEndElementL END state=%d", iFeedState);	
	}

void CFeedParser::OnContentL(const TDesC8& aBytes, TInt /*aErrorCode*/)
	{
	TBuf<KBufferLength> temp;
	if (iEncoding == EUtf8) {
		CnvUtfConverter::ConvertToUnicodeFromUtf8(temp, aBytes);
	} else {
		temp.Copy(aBytes);
	}
	
	if(temp.Length() + iBuffer.Length() < KBufferLength) {
		iBuffer.Append(temp);
	}
	}

void CFeedParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/)
	{
	DP("OnStartPrefixMappingL()");
	}

void CFeedParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
	{
	DP("OnEndPrefixMappingL()");
	}

void CFeedParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{
	DP("OnIgnorableWhiteSpaceL()");
	}

void CFeedParser::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
	{
	DP("OnSkippedEntityL()");
	}

void CFeedParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/)
	{
	DP("OnProcessingInstructionL()");
	}

void CFeedParser::OnError(TInt aErrorCode)
	{
	DP1("CFeedParser::OnError %d", aErrorCode);
	}

TAny* CFeedParser::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	DP("GetExtendedInterface()");
	return NULL;
	}

CFeedInfo& CFeedParser::ActiveFeed()
	{
		return *iActiveFeed;
	}
