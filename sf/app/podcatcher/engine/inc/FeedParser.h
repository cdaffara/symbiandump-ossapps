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

#ifndef FEEDPARSER_H_
#define FEEDPARSER_H_

#include "FeedParserObserver.h"
#include "FeedInfo.h"
#include <xml/parser.h> // for CParser
#include <xml/contenthandler.h>	// for Xml::MContentHandler
#include <xml/documentparameters.h>

_LIT(KTagItem, "item");
_LIT(KTagTitle, "title");
_LIT(KTagImage, "image");
_LIT(KTagLink, "link");
_LIT(KTagDescription, "description");
_LIT(KTagUrl, "url");
_LIT(KTagLength, "length");
_LIT(KTagChannel, "channel");
_LIT(KTagEnclosure, "enclosure");
_LIT(KTagPubDate, "pubDate");
_LIT(KTagLastBuildDate, "lastBuildDate");
_LIT(KTagHref, "href");

enum TFeedState {
	EStateRoot,
	EStateChannel,
	EStateChannelTitle,
	EStateChannelLink,
	EStateChannelLastBuildDate,
	EStateChannelDescription,
	EStateChannelImage,
	EStateChannelImageUrl,
	EStateItem,
	EStateItemTitle,
	EStateItemLink,
	EStateItemEnclosure,
	EStateItemDescription,
	EStateItemPubDate
};

enum TEncoding {
	EUtf8,
	ELatin1
};

const int KBufferLength = 1024;

class CFeedParser : public CBase, public Xml::MContentHandler 
{
public:
	CFeedParser(MFeedParserObserver& aCallbacks, RFs& aFs);
	virtual ~CFeedParser();
	
public:
	void ParseFeedL(const TFileName &feedFileName, CFeedInfo *item, TUint aMaxItems);
	
public: // from MContentHandler
	void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	void OnError(TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);
	CFeedInfo& ActiveFeed();
private:
	MFeedParserObserver& iCallbacks;
	TFeedState iFeedState;

	CShowInfo* iActiveShow;
	CFeedInfo *iActiveFeed;

	TBuf<KBufferLength> iBuffer;
	
	TUint iMaxItems;
	TUint iItemsParsed;
	TBool iStoppedParsing;
	TEncoding iEncoding;
	RFs& iRfs;
};

#endif
