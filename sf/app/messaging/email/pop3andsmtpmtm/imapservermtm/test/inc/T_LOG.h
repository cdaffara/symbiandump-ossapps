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
//

class COutputter : public CBase
	{
public:
	static COutputter* NewL(RTest* aTest);
	~COutputter();
	
	void SetRFs( RFs* aFs);
	void SetFilePrefix( TUint aNum );
	void SetFileL( const TDesC& leafname, TBool aUnicode=EFalse );
	void UnSetFile();
	void PrintL( const TDesC8& aText );
	void PrintL( const TDesC16& aText );
	void Printf( TRefByValue<const TDesC> aFmt,... );
	void PrintIfNotNullL( const TDesC& aLabel, const TDesC8& aValue );
	void PrintIfNotNullL( const TDesC& aLabel, const TDesC16& aValue );
	void PrintListL( const TDesC& aLabel, CDesC8Array& aList );
	void PrintListL( const TDesC& aLabel, CDesC16Array& aList );
	void PrintDataL( const TDesC8& aData );

private:
	void ConstructL();
	
private:
	TInt iFilesOpen;
	RFile iFile[4];
	RTest* iTest;
	RFs* iFs;
	TDes16Overflow* iOverflow;
	TBuf<9> iPrefix;
	TBool iUnicode;

public:
	TBool iUseFiles;
	};

//----------------------------------------------------------------------------------------

typedef TBool(*MsgMatchFn)( CMsvServerEntry* aEntry);

//----------------------------------------------------------------------------------------

class CMsgOutputter : public CBase
	{
public:
	static CMsgOutputter* NewL( COutputter* aPutter, CMsvServerEntry* aEntry );
	~CMsgOutputter();
	void DumpL( TMsvId aId,
				TInt aSummary,
				TInt aParts,
				TBool aRecurse,
				TBool aShowIds,
				MsgMatchFn aMatch =0);

private:
	CMsgOutputter( COutputter* aPutter, CMsvServerEntry* aEntry );
//	void ConstructL(CMsvServer* aServer);
	
	void BodyDataL(CMsvStore& aFileStore);
	void MimeHeaderL(CMsvStore& aFileStore);
	void HeaderL(CMsvStore& aFileStore);
	void MultipartDataL(CMsvStore& aFileStore);
	void SettingsL(const TMsvEntry& aEntry);
	void Imap4SettingsL(const TMsvEntry& aEntry);
	void QueuedOpsL( CMsvStore& aFileStore );

	TPtrC PriorityString( const TMsvEmailEntry& entry );
	TPtrC DisconnectedOpString( const TMsvEmailEntry& entry );
	TPtrC TypeString( const TMsvEmailEntry& entry );
	void FlagsString( TBuf<8>&other, const TMsvEmailEntry& entry );
	void IMAP4FlagsString( TBuf<7>&other, const TMsvEmailEntry& entry );
	TPtrC SubscribeTypeString( TFolderSubscribeType aType );
	TPtrC SyncTypeString( TFolderSyncType aType );
	TPtrC OffLineOpTypeString(CImOffLineOperation::TOffLineOpType op);
	TPtrC GetMailOptionsString( TImap4GetMailOptions aOpts );

	void OutputPartsL(TBuf<5>& aStreams);
	void RemoveRichtextFormatting(TDes& aSourceLine);

	void DumpRecurseL();

private:
	// passed in on init
	COutputter* iPutter;

	// created in constructor
	CMsvServerEntry* iEntry;
	TMsvId iMsgId;

	// behaviour flags
	TInt iSummary;
	TInt iParts;
	TBool iRecurse;
	TBool iShowIds;
	MsgMatchFn iMatch;
	
	// context info
	TInt iDepth;				// depth into the recursion

	// statistics info
	TInt iEntryCount;
	TInt iFileCount;

public:
	enum
		{
		ENone = 0,
		EToScreen,
		EToOneFile,
		EToMultipleFiles
		};

	};

//----------------------------------------------------------------------------------------
