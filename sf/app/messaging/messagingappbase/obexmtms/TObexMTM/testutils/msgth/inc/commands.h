// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COMMANDS_H
#define __COMMANDS_H
#include "harness.h"

const TInt KMaxTestBodyTxt = 200;

//
//
// CCleanMessageFolder
//

class CCleanMessageFolder : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CCleanMessageFolder(CMsvTestUtils& aTestUtils);

protected:
	CMsvTestUtils& iTestUtils;
	};


//
//
// CAddComment
//

class CAddComment : public CBase, public MBaseTestState
	{
public:
	IMPORT_C static CAddComment* NewL(const CDesCArrayFlat& aComment, CMsvTestUtils& aTestUtils);
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C ~CAddComment();

private:
	CAddComment(CMsvTestUtils& aTestUtils);
	void ConstructL(const CDesCArrayFlat& aComment);

protected:
	CMsvTestUtils& iTestUtils;
	HBufC* iComment;
	};


//
//
// CGoClientSide
//

class CGoClientSide : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CGoClientSide(CMsvTestUtils& aTestUtils);

protected:
	CMsvTestUtils& iTestUtils;
	};



//
//
// CGoServerSide
//

class CGoServerSide : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CGoServerSide(CMsvTestUtils& aTestUtils);

protected:
	CMsvTestUtils& iTestUtils;
	};



//
//
// CResetMessageServer
//

class CResetMessageServer : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CResetMessageServer(CMsvClientTest& aParentTestHarness);

protected:
	CMsvClientTest& iParentTestHarness;
	};




//
//
// CCheckMessageFolder
//

class CCheckMessageFolder : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CCheckMessageFolder(const TDesC& aFileName, CMsvTestUtils& aTestUtils);

protected:
	CMsvTestUtils& iTestUtils;
	TFileName iFileName;
	};



//
//
// CSelectEntry
//

class CSelectEntry : public CBase, public MBaseTestState
	{
public:
	IMPORT_C static CSelectEntry* NewL(const TDesC& aName, const TMsvId& aCurrentFolderId, TMsvId* aEntryId, CMsvTestUtils& aTestUtils);
	IMPORT_C static CSelectEntry* NewL(const TDesC& aName, const TMsvId& aCurrentFolderId, CMsvEntrySelection* aSelection, CMsvTestUtils& aTestUtils);
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C ~CSelectEntry();

private:
	CSelectEntry(const TMsvId& aCurrentFolderId, TMsvId* aEntryId, CMsvTestUtils& aTestUtils);
	CSelectEntry(const TMsvId& aCurrentFolderId, CMsvEntrySelection* aSelection, CMsvTestUtils& aTestUtils);
	void ConstructL(const TDesC& aName);

private:
	CMsvTestUtils& iTestUtils;
	const TMsvId& iCurrentFolderId;
	TMsvId* iEntryId;
	CMsvEntrySelection* iEntrySelection;
	HBufC* iEntryName;
	};

/*
class CSelectEntry : public CBase, public MBaseTestState
	{
public:
	static CSelectEntry* NewL(const TDesC& aName, const TMsvId& aCurrentFolderId, TMsvId& aEntryId, CMsvTestUtils& aTestUtils);
	void StartL(TRequestStatus& aStatus);
	~CSelectEntry();

private:
	CSelectEntry(const TMsvId& aCurrentFolderId, TMsvId& aEntryId, CMsvTestUtils& aTestUtils);
	void ConstructL(const TDesC& aName);

private:
	CMsvTestUtils& iTestUtils;
	const TMsvId& iCurrentFolderId;
	TMsvId& iEntryId;
	HBufC* iEntryName;
	};
*/

//`//
//
// CCheckSelectionCount
//

class CCheckSelectionCount : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CCheckSelectionCount(TInt aCount, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils); 

private:
	void LogCommentFormatL(TRefByValue<const TDesC> format,...);

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	TInt iTestCount;
	};

//
//
// CDumpMessageStore
//

class CDumpMessageStore : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CDumpMessageStore(CMsvTestUtils& aTestUtils); 

private:
	CMsvTestUtils& iTestUtils;
	};




//`//
//
// CCheckNewFlag
//

class CCheckNewFlag : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CCheckNewFlag(TInt aSelectedIndex, TBool aTestNewStatus, 
				  CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils); 

private:
	void LogCommentFormatL(TRefByValue<const TDesC> format,...);

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	TInt iSelectedIndex;
	TBool iTestNewStatus;
	};



//`//
//
// CCheckPriority
//

class CCheckPriority : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CCheckPriority(TInt aSelectedIndex, TInt aTestPriority, 
				   CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils); 

private:
	void LogCommentFormatL(TRefByValue<const TDesC> format,...);

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	TInt iSelectedIndex;
	TBool iTestPriority;
	};


//`//
//
// CCheckAttachment
//

class CCheckAttachment : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CCheckAttachment(TInt aSelectedIndex, TBool aTestAttachment, 
					 CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils); 

private:
	void LogCommentFormatL(TRefByValue<const TDesC> format,...);

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	TInt iSelectedIndex;
	TBool iTestAttachment;
	};


//`//
//
// CCheckBodyText
//

class CCheckBodyText : public CBase, public MBaseTestState
	{
public:
	IMPORT_C static CCheckBodyText* NewL(TInt aSelectedIndex, TDesC& aTestBodyText, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils);
	
	IMPORT_C ~CCheckBodyText();

	IMPORT_C void StartL(TRequestStatus& aStatus);
	
private:
	void ConstructL(TDesC& aTestBodyText);
	CCheckBodyText(TInt aSelectedIndex, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils); 

	void LogCommentFormatL(TRefByValue<const TDesC> format,...);

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	TInt iSelectedIndex;
	HBufC* iTestBodyText;
	};

//`//
//
// CCheckRemoteFolderSize
//

class CCheckRemoteFolderSize : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CCheckRemoteFolderSize(TInt aCount, CMsvTestUtils& aTestUtils); 

private:
	void LogCommentFormatL(TRefByValue<const TDesC> format,...);

private:
	CMsvTestUtils& iTestUtils;
	TInt iTestCount;
	};

//`//
//
// CSelectPopulatedInSelection
//

class CSelectPopulatedInSelection : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CSelectPopulatedInSelection(CMsvEntrySelection& aSelection,CMsvTestUtils& aTestUtils); 

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	};

//`//
//
// CSelectLastInSelection
//

class CSelectLastInSelection : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CSelectLastInSelection(TInt aCount, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils); 

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	TInt iTestCount;
	};

//`//
//
// CSelectFirstInSelection
//

class CSelectFirstInSelection : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CSelectFirstInSelection(TInt aCount, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils); 

private:
	CMsvTestUtils& iTestUtils;
	CMsvEntrySelection& iSelection;
	TInt iTestCount;
	};

//
//
// CSelectEntryById
//

class CSelectEntryById : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CSelectEntryById(TMsvId aId, TMsvId& aEntryId, CMsvTestUtils& aTestUtils);

private:
	CMsvTestUtils& iTestUtils;
	TMsvId iIdToSelect;
	TMsvId& iEntryId;
	};

//
//
// CMsvTestEntry
//

class CMsvTestEntry : public CBase, public MBaseTestState
	{
public:
	IMPORT_C CMsvTestEntry(CMsvClientTest& aParentTestHarness);
	IMPORT_C void StartL(TRequestStatus& aStatus);
	
	IMPORT_C void TestVisible(TBool aVisibleValue);
	IMPORT_C void TestComplete(TBool aCompleteValue);

protected:
// What to test
	TBool iTestVisible;
	TBool iTestComplete;

// Expected values
	TBool iVisibleValue;
	TBool iCompleteValue;

protected:
	CMsvClientTest& iParentTestHarness;
	};

//
//
// CSelectAllMessages
//

class CSelectAllMessages : public CBase, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CSelectAllMessages(TMsvId& aParentFolder, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils);

private:
	CMsvTestUtils& iTestUtils;
	TMsvId& iParentFolder;
	CMsvEntrySelection& iSelection;
	};



//
//
// CLongWait
//

class CLongWait : public CActive, public MBaseTestState
	{
public:
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C CLongWait();
	IMPORT_C CLongWait(TInt aWaitTime);
	IMPORT_C ~CLongWait();
	IMPORT_C void RunL();
	IMPORT_C void DoCancel();

private:
	CTestTimer* iTimer;
	TRequestStatus* iReportStatus;
	TInt iWaitTime;
	};


//
//
// CCopySelection
//

class CCopySelection : public CActive, public MBaseTestState
	{
public:
	IMPORT_C CCopySelection(CMsvClientTest& aParentTestHarness);
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C void RunL();
	IMPORT_C void DoCancel();
	IMPORT_C ~CCopySelection();

private:
	CMsvClientTest& iParentTestHarness;
	TRequestStatus* iReportStatus;
	};


//
//
// CMoveSelection
//

class CMoveSelection : public CActive, public MBaseTestState
	{
public:
	IMPORT_C CMoveSelection(CMsvClientTest& aParentTestHarness);
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C void RunL();
	IMPORT_C void DoCancel();
	IMPORT_C ~CMoveSelection();

private:
	CMsvClientTest& iParentTestHarness;
	TRequestStatus* iReportStatus;
	};



//
//
// CDeleteSelection
//

class CDeleteSelection : public CActive, public MBaseTestState
	{
public:
	IMPORT_C CDeleteSelection(CMsvClientTest& aParentTestHarness);
	IMPORT_C void StartL(TRequestStatus& aStatus);
	IMPORT_C void RunL();
	IMPORT_C void DoCancel();
	IMPORT_C ~CDeleteSelection();

private:
	CMsvClientTest& iParentTestHarness;
	TRequestStatus* iReportStatus;
	};


#endif
