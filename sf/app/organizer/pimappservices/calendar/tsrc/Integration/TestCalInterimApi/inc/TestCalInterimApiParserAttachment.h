/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef TEST_CALINTERIMAPI_PARSER_ATTACHMENT_H
#define TEST_CALINTERIMAPI_PARSER_ATTACHMENT_H

#include "TestCalInterimApiParserBase.h"

#include <calattachment.h>

/**
 * Parser that parses attachment related commands from the data file. Sets the properties
 * of the attachment and add a single or multiple attachment to the entry.
 */
class CTestCalInterimApiParserAttachment : public CTestCalInterimApiParserBase
	{
public:

	static CTestCalInterimApiParserAttachment*	NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, CCalAttachment::TType& aAttachType,
										 			RFs& aFs, const TBool& aBinaryData);
	static CTestCalInterimApiParserAttachment*	NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, CCalAttachment::TType& aAttachType,
												RFs& aFs, const TBool& aBinaryData);
	~CTestCalInterimApiParserAttachment();
	
	//	CTestCalInterimParserBase implementation
	virtual TInt 		ParseL(const TDesC& aCommand, const TDesC& aData);

private:
	CTestCalInterimApiParserAttachment(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, CCalAttachment::TType& aAttachType, const TBool& aBinaryData);
	void 				ConstructL(RFs& aFs, const TDesC& aData);

	void				ProcessMIMETypeL(const TDesC& aData);
	void				ProcessLabelL(const TDesC& aData);
	void				ProcessDriveL(const TDesC& aData);
	void				ProcessSetLastModifiedDate(const TDesC& aData);
	void				ProcessSetAttributes(const TDesC& aData);
	void				ProcessClearAttributes(const TDesC& aData);

	void				GetAttachmentInfo();
		
private:
	CCalEntry&						iEntry;
	CCalAttachment*					iAttachment;
	CCalAttachment::TType			iAttachType;
	TInt							iAttributes;
	HBufC8*							iMimeType;
	HBufC*							iLabel;
	TTime							iTime;
	TTime							iLastModifiedDate;
	TInt							iSize;
	TDriveName						iDriveName;
	TBool							iBinaryData;
	};

#endif
