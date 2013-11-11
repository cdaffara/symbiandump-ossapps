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

#include "tstore.h"

// System includes
#include "DBWRITER.H"
#include <txtrich.h>
#include <txtfmlyr.h>
#include <txtmrtsr.h>
#include <gdi.h>

// Constants
const TUint32 KBaseCategoryUID = 1;
const TUint32 KHelpFileUID = 0;
const TUint32 KNumberHelpTopics = 20;

// Literal constants
_LIT(KProgressText, ".");

// Classes
CTestWriter::CTestWriter(RFs& aFs)
:	iFs(aFs), iCategoryUID(KBaseCategoryUID)
	{
	}

CTestWriter::~CTestWriter()
	{
	delete iWriter;
	delete iRichText;
	delete iCharFormatLayer;
	delete iParaFormatLayer;
	}

CTestWriter* CTestWriter::NewL(RFs& aFs)
	{
	CTestWriter* self = new(ELeave) CTestWriter(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestWriter::ConstructL()
	{
	iWriter = CHlpDbWriter::NewL(iFs);
	iCharFormatLayer = CCharFormatLayer::NewL();
	iParaFormatLayer = CParaFormatLayer::NewL();
	iRichText = CRichText::NewL(iParaFormatLayer, iCharFormatLayer);	
	}

void CTestWriter::BuildFilesL()
	{
	//SinceDBWriter can not add/remove files from the protected rsc directory we put the file in the public area
	_LIT(KHelpFileNameFormatString, "C:\\Public\\Help\\file%d.dat");

	TheFsSession.MkDirAll(KHelpFileNameFormatString);
	DeleteFilesL();
	TInt i = 0;
	TFileName fileName;
	TInt numberOfFiles=25;

	// build 25 1UID database files 
	
	
	TheTest.Next(_L("Creating 25 databases with 1 category"));
	for (i=0; i<numberOfFiles; i++)
		{
		iFileCount=i;
		fileName.Format(KHelpFileNameFormatString, i);
		BuildFileL(fileName, TUid::Uid(KHelpFileUID + i), 1);
		}

	// build 25 3UID database files 
		
	TheTest.Next(_L("\nCreating 25 databases with 3 categories"));
	for (i=0; i<numberOfFiles; i++)
		{
		iFileCount=i;
		fileName.Format(KHelpFileNameFormatString, (i+numberOfFiles));
		BuildFileL(fileName, TUid::Uid(KHelpFileUID + i), 3);
		}
	
	// build 25 15UID database files 
	
	TheTest.Next(_L("\nCreating 25 databases with 15 categories"));
	for (i=0; i<numberOfFiles; i++)
		{
		iFileCount=i;
		fileName.Format(KHelpFileNameFormatString, (i+2*numberOfFiles));
		BuildFileL(fileName, TUid::Uid(KHelpFileUID + i), 15);
		}
	}

void CTestWriter::BuildFileL(const TDesC& aFileName, TUid aUid, TUint aNumberCategoryUID)
	{
	// Pre writing
	iWriter->CreateFileL(aFileName);
	iWriter->SetDatabaseUidL(aUid);
	iWriter->CreateDatabaseL();

	// Writing
	WriteUidTableL(aNumberCategoryUID);
	WriteTopicTableL(aNumberCategoryUID);
	WriteContextTableL();
	WriteTopicIndexTableL();
	WriteIndexTableL();

	// Post writing
	iWriter->CompressDatabaseL();
	iWriter->CloseFileL();
	}

// write out the uid table 
void CTestWriter::WriteUidTableL(TUint aNumberCategoryUID)
	{
	for (TUint i=0; i < aNumberCategoryUID; i++)
		{
		TUid uid;
		uid.iUid=iCategoryUID+i;
		iWriter->SetUidL(uid);
		}
	}

void CTestWriter::WriteTopicTableL( TUint aNumberCategoryUID)
	{
	iWriter->BeginTransaction();

	// loop for the number of categories in the database
	for( TUint i = 0; i < aNumberCategoryUID;i++)
		{
		// write out a set of topics
		WriteTopicsL(i);
		iCategoryUID++;
		}

	// commit the transaction
	iWriter->CommitTransaction();

	TheTest.Printf(KProgressText);
	}

// write out a set of topics
void CTestWriter::WriteTopicsL(TUint aTopicIDMultiplier)
	{
	TBuf<32> buf;
	TBuf<1042> buf2;

	CHlpTable* table = iWriter->TopicTable();

	for(TUint i=0; i<KNumberHelpTopics; i++)
		{
		// create and write out a title
		table->InsertL();
		buf.Zero();
		buf.Format(KTopicTitle, (KNumberHelpTopics*aTopicIDMultiplier)+i+(1000*iFileCount), iFileCount);
		table->SetColL(KTopicTblTopicTitle,buf);

		// add the title to the body text
		buf2.Append(buf);
		buf2.Append(TChar(2029));

		// create and write the category string
		buf.Zero();
		buf.Format(KCategory, iCategoryUID);
		table->SetColL(KTopicTblCategoryTitle, buf);

		// add the category to the body text
		buf2.Append(buf);
		buf2.Append(TChar(2029));
		buf2.Append(KBodyText);
		iRichText->Reset();
		iRichText->InsertL(0, buf2);
		
		// write out the body text
		table->SetColL(KTopicTblTopicText, KTopicTblTopicMarkup, *iRichText);

		// write out some synonyms
		buf2.Zero();
		table->SetColL(KTopicTblSynonym, CONST_CAST(TDesC&, KSynonymText()));

		// calculate and write out the topicID
		TInt id;
		id=(KNumberHelpTopics*aTopicIDMultiplier)+i+(1000*iFileCount);
		table->SetColL(KTopicTblTopicId, id);

		// set the category UID
		table->SetColL(KTopicTblCategoryUID, iCategoryUID);
		
		table->PutL();
		}

	TheTest.Printf(KProgressText);
	}


void CTestWriter::WriteContextTableL()
	{
	CHlpTable* table = iWriter->ContextTable();
	iWriter->BeginTransaction();

	TBuf<32> buf;
	for(TInt i=1; i <=20; i++)
		{
		buf = KContextText;
		buf.AppendNum(iFileCount);
		buf.AppendNum(i);
		table->InsertL();
		table->SetColL(KContextTblTopicId, i+(1000*iFileCount));
		table->SetColL(KContextTblContext, buf);
		table->PutL();
		}

	iWriter->CommitTransaction();

	TheTest.Printf(KProgressText);
	}

void CTestWriter::WriteIndexTableL()
	{
	CHlpTable* table = iWriter->IndexTable();
	iWriter->BeginTransaction();

	TBuf<120> buf;
	for(TInt i=1; i<=20; i++)
		{
		table->InsertL();
		buf.Format(KIndexPhrase, i, iFileCount);
		table->SetColL(KIndexTblIndex, buf);
		table->SetColL(KIndexTblIndexId, i);
		table->PutL();
		}

	iWriter->CommitTransaction();

	TheTest.Printf(KProgressText);
	}

void CTestWriter::WriteTopicIndexTableL()
	{
	CHlpTable* table = iWriter->TopicIndexTable();
	iWriter->BeginTransaction();

	for(TInt i=1; i<=20; i++)
		{
		table->InsertL();
		const TInt id = i+(1000*iFileCount);
		table->SetColL(KTopicIndexTblTopicId, id);
		table->SetColL(KTopicIndexTblIndexId, i);
		table->PutL();
		}

	iWriter->CommitTransaction();

	TheTest.Printf(KProgressText);
	}

void CTestWriter::DeleteFilesL()
	{
	_LIT(KHelpFileNameFormatString, "C:\\Public\\Help\\*.*");
	CFileMan* fileman = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileman);
	TInt error = (fileman->Delete(KHelpFileNameFormatString)); 
	if	(error != KErrNone && error != KErrNotFound)
		User::Leave(error);

	CleanupStack::PopAndDestroy(fileman);
	}

static void StartTestL()
    {
	TheWriter = CTestWriter::NewL(TheFsSession);

	TRAPD(error,
		TheWriter->BuildFilesL();
	); 
	if	(error)
		TheTest.Printf(_L("leave code=%d\n"), error);
	else
		TheTest.Printf(_L("\nok"));
	TheTest(error == KErrNone || error == KErrNoMemory);

	TheTest.Printf(_L("Last category written %d\n"), TheWriter->CategoryUID());
	delete TheWriter;
    }

static void TidyFilesL(TAny*)
	{
	TheWriter = CTestWriter::NewL(TheFsSession);
	CleanupStack::PushL(TheWriter);
	TheWriter->DeleteFilesL();
	CleanupStack::PopAndDestroy(TheWriter);
	}

static void DoOOMTestL()
	{
	TInt error = KErrNoMemory;
	TInt failAt = 0;
	
	while(error!=KErrNone)
		{
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(error, StartTestL());
		if	(error != KErrNone)
			{
			__UHEAP_MARKEND;
			}

		__UHEAP_RESET;
		TheTest(error == KErrNoMemory || error == KErrNone);
		}
	}

static void doMainL()
	{
	TInt error = KErrNone;

	CleanupStack::PushL(TCleanupItem(TidyFilesL, 0));

	// Normal test
	TRAP(error, StartTestL());
	TheTest(error == KErrNone);

	// OOM test
	TRAP(error, DoOOMTestL());
	TheTest(error == KErrNone);

	CleanupStack::PopAndDestroy();
	}

/**
@SYMTestCaseID PIM-TSTORE-0001
*/	
GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTestCaseID PIM-TSTORE-0001 Creating cleanup objects"));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TheTest(TheFsSession.Connect() == KErrNone);

	TRAPD(error, doMainL());
	TheTest(error == KErrNone);

	// Cleanup & close
	delete cleanup;
	TheFsSession.Close();

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
