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

#ifndef OPMLPARSER_H_
#define OPMLPARSER_H_

#include <xml/parser.h> // for CParser
#include <xml/contenthandler.h>	// for Xml::MContentHandler
#include <xml/documentparameters.h>
#include "FeedEngine.h"

_LIT(KTagOpml, "opml");
_LIT(KTagBody, "body");
_LIT(KTagOutline, "outline");
_LIT(KTagXmlUrl, "xmlUrl");
_LIT(KTagText, "text");
_LIT(KTagHtmlUrl, "htmlUrl");

enum TOpmlState {
	EStateOpmlRoot,
	EStateOpmlBody,
	EStateOpmlOutline,
	EStateOpmlOutlineOutline
	};

class COpmlParser : public CBase, public Xml::MContentHandler 
{
public:
	COpmlParser(CFeedEngine& aFeedEngine, RFs& aFs);
	virtual ~COpmlParser();
	
public:
	void ParseOpmlL(const TFileName &feedFileName, TBool aSearching);
	
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
private:
	CFeedEngine& iFeedEngine;
	TOpmlState iOpmlState;

	TBuf<KBufferLength> iBuffer;
	TEncoding iEncoding;
	RFs& iFs;
	TBool iSearching;
	TUint iNumFeedsAdded;
};

#endif
