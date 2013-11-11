// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client MTM utilities for POP3, SMTP and IMAP4 protocols
// 
//
/**
 * @file 
 * @publishedPartner
 * @released
 */

#if !defined (__IMCMUTIL_H__)
#define __IMCMUTIL_H__

#if !defined (__MSVFTEXT_H__)
#include <msvftext.h>
#endif

#if !defined (__MIUTHDR_H__)
#include <miuthdr.h>
#endif

#if !defined (__MIUTDEF_H__)
#include <miutdef.h>
#endif

class CImClientMTMUtils : public CBase
/**
Utilities for use by email client MTMs for searching a message.
@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CImClientMTMUtils* NewL();
	IMPORT_C ~CImClientMTMUtils();

	IMPORT_C void FindL(const TDesC& aTextToFind, CRichText& aRichText, CImHeader& aHeader, TMsvPartList aPartList, TMsvPartList& rReturnList);
	IMPORT_C TMsvPartList FindInHeaderL(const TDesC& aTextToFind, CImHeader& aHeader, TMsvPartList aPartList);
	IMPORT_C TMsvPartList FindInBodyL(const TDesC& aTextToFind, CRichText& aRichText, TMsvPartList aPartList);
private:
	void ConstructL();
	TBool FindInMessagePartRecipientL(const TDesC& aTextToFind,TMsvPartList aPartList,CDesCArray& aRecipients);

private:
	CMsvFindText* iFindText;
	};

#endif 
