// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cimprunemessage.h
//

/**
 * @file 
 * @publishedPartner
 * @released
 */

#if !defined (__CIMPRUNEMESSAGE_H__)
#define __CIMPRUNEMESSAGE_H__

class CImEntryFinder;

class CImPruneMessage : public CMsgActive
/** Deletes the body text and attachment data from an individually specified message. 

Example uses of this class are:

1. to cleanup after a populating operation has failed

2. to remove body text and attachment data from remote messages, while preserving 
the message structure 
@publishedPartner 
@released
*/
	{
public:
	IMPORT_C static CImPruneMessage* NewL(CMsvEntry& aEntry, RFs& aFs);
	IMPORT_C static CImPruneMessage* NewLC(CMsvEntry& aEntry, RFs& aFs);

	IMPORT_C void StartL(TMsvId aMessageEntry, TRequestStatus &aStatus);
	~CImPruneMessage();
	
private:
	CImPruneMessage(CMsvEntry& aEntry, RFs& aFs);
	void ConstructL();
	void DoRunL();
	void DoComplete(TInt& );
	void DoCancel();
	void PruneEntryL();
	void ResetStoreL();

private:
	enum TImPruningState
		{
		EImPruneUpdatingEntry,
		EImPruneUpdatingFirstEntry,
		EImPruneFindingEntry,
		EImPruneFindFirstEntry,
		EImPruneFindNextEntry
		};

	TImPruningState iState;

	CMsvEntry& iCurrentEntry;
	RFs& iFs;
	CImEntryFinder* iEntryFinder;
	CFileMan* iFileManager;
	TMsvId iRootEntryId;
	CMsvOperation* iChangeOperation;
	CMsvStore* iStore;
	};

#endif //__CIMPRUNEMESSAGE_H__
