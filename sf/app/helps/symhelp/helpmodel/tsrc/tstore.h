// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSTORE_H__
#define __TSTORE_H__

// System includes
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>

// Literal constants
_LIT(KContextText, "Context");
_LIT(KTopicTitle, "Topic Title %d File %d");
_LIT(KCategory, "Category %d");
_LIT(KIndexPhrase, "Index Phrase %d File %d");
_LIT(KSynonymText, "SynonymA SynonymB SynonymC");
_LIT(KBodyText,	"When a program requests a service, this service can be performed either synchronously or asynchronously. A synchronous service is the “usual” pattern for function calls: when the function returns, the service requested has been performed or an error code returned. An asynchronous service is requested by a function call, but completion occurs later, and is indicated by a signal. Between the issue of the request and the signal, the request is said to be pending. The requesting program may do other processing while the request is pending, else it issues a wait. The operating system will wake up the program when completion of any of its pending requests is signalled.");

// Classes referenced
class CRichText;
class CTestWriter;
class CHlpDbWriter;
class CCharFormatLayer;
class CParaFormatLayer;

// Globals
static RTest TheTest(_L("TStore"));
static RFs TheFsSession;
static CTestWriter* TheWriter;

// Classes
class CTestWriter : public CBase
	{
public:
	static CTestWriter* NewL(RFs& aFs);
	~CTestWriter();

private:
	CTestWriter(RFs& aFs);
	void ConstructL();

public:
	void BuildFilesL();
	void DeleteFilesL();
	TUint CategoryUID(){return iCategoryUID;}

private:
	void BuildFileL(const TDesC& aFileName, TUid aUid, TUint aNumberCategoryUID);
	void WriteTopicTableL(TUint aNumberCategoryUID);
	void WriteIndexTableL();
	void WriteTopicIndexTableL();
	void WriteContextTableL();
	void WriteUidTableL(TUint aNumberCategoryUID);
	void WriteTopicsL(TUint aTopicIDMultiplier);

private:
	RFs& iFs;
	TInt iFileCount;
	TUint32 iCategoryUID; 
	//
	CRichText* iRichText;
	CHlpDbWriter* iWriter;
	CCharFormatLayer* iCharFormatLayer;
	CParaFormatLayer* iParaFormatLayer;
	};

/*
LOCAL_C void WriteToTablesL();
LOCAL_C void WriteContextTableL();
LOCAL_C void WriteIndexTableL();
LOCAL_C void WriteTopicTableL();
LOCAL_C void WriteTopicIndexTableL();
LOCAL_C void WriteUidTableL();
LOCAL_C void CreateFilesL();
LOCAL_C void BuildFileL(const TDesC& aFileName);

_LIT(KTestDbName, "c:\\testdb.dat");
*/

#endif
