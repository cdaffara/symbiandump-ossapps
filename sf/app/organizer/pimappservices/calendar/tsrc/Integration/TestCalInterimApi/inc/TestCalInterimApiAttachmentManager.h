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



#ifndef TEST_CALINTERIMAPI_ATTACHMENT_MANAGER_H
#define TEST_CALINTERIMAPI_ATTACHMENT_MANAGER_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calentry.h>
#include <calattachment.h>
#include <calattachmentmanager.h>

class CCalEntryView;

/**
This test step sorts the file attachment based on the sort type given i.e sorts by size or last modified date.
It also gets the local Uid of the entries associated with the specified attachment
*/
class CTestCalInterimApiAttachmentManager : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiAttachmentManager();
	~CTestCalInterimApiAttachmentManager();

	virtual TVerdict	doTestStepL();

private:
	void	GetFileAttachmentsBySortTypeL();
	void	CompareAttachSizeAndLastModifiedDateL(CCalAttachment* aAttachment, const TPtrC& aSortType);
	void	GetEntriesForFileAttachmentL();
	void	VerifyEntriesWithFileAttachmentL();
	void	CheckAttachmentCountL();

	CCalAttachmentManager::TSortOrder	ConvertToSortType(const TPtrC& aSortType);

private:
	CCalAttachmentManager*	iCalAttachmentManager;
	CCalAttachmentIterator*	iAttachmentIterator;
	TInt	iSize;
	TTime	iLastModifiedDateOfFirstAttachment;
	RArray<TCalLocalUid>	iLocalIds;
	RArray<TCalLocalUid>	iExpectedLocalIdArray;
	
	};

_LIT(KTestCalInterimApiAttachmentManager,"TestCalInterimApiAttachmentManager");

#endif
