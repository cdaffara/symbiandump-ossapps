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
// TDumper test module
// 
//

// System includes
#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <s32stor.h>
#include <charconv.h>

// User includes
#include "HLPMODEL.H"

// Globals
static RFs								TheFs;
static RTest							TheTest(_L("TDUMPER - Dumps a help file to a plain text file"));
static CTrapCleanup*					TheTrapCleanup;
static CActiveScheduler*				TheScheduler;
static CCnvCharacterSetConverter*		TheConverter;

// Literal constants
_LIT(KDumpOutputFile,					"C:\\%S-Dump.txt");
_LIT(KSQLTopicIdColumn,					"TopicId");
_LIT(KSQLIndexIdColumn,					"IndexId");
_LIT(KSQLTopicTitleColumn,				"TopicTitle");
_LIT(KSQLTopicTextColumn,				"TopicText");
_LIT(KSQLCategoryColumn,				"Category");
_LIT(KSQLSynonymColumn,					"Synonym");
_LIT(KSQLIndexColumn,					"Index");
_LIT(KSQLContextColumn,					"Context");
_LIT(KSQLCategoryUidColumn,				"CategoryUID");

// Constants
const TInt KTestCleanupStack	= 0x20;

// Typedefs
typedef TBuf<80> TConsoleLine;


static TBool GetLine(TConsoleLine& aLine)
	{
	TPoint posStart = TheTest.Console()->CursorPos();
	TheTest.Console()->Write(aLine);

	TKeyCode key;
	TConsoleLine line(aLine);
	FOREVER
		{
		key = TheTest.Console()->Getch();
		
		switch(key)
			{
			case EKeyEscape:
				return EFalse;
			case EKeyEnter:
				aLine = line;
				return ETrue;
			case EKeyBackspace:
			case EKeyDelete:
				{
				if	(line.Length())
					line = line.Left(line.Length()-1);
				break;
				}
			default:
				line.Append(TChar(key));
				break;
			}

		TheTest.Console()->SetCursorPosAbs(posStart);
		TheTest.Console()->ClearToEndOfLine();
		TheTest.Console()->Write(line);
		}
	}

static void WriteTopicTableL(RFile& aOutputFile, RDbStoreDatabase& aDatabase)
	{
	TInt count = 0;
	_LIT(KCategoryTable, "TOPIC");

	// Open the topic table so that we can use it to build the category list
	RDbTable table;
	User::LeaveIfError(table.Open(aDatabase, KCategoryTable, RDbRowSet::EReadOnly));
	CleanupClosePushL(table);

	// Now build the list
	if	(!table.FirstL())
		{
		CleanupStack::PopAndDestroy(); // table
		return; // assume no rows
		}

	// Get the column of the category
	CDbColSet* colset		= table.ColSetL();
	TDbColNo titleCol		= colset->ColNo(KSQLTopicTitleColumn);
	TDbColNo catCol			= colset->ColNo(KSQLCategoryColumn);
	TDbColNo topicIdCol		= colset->ColNo(KSQLTopicIdColumn);
	TDbColNo synonymCol		= colset->ColNo(KSQLSynonymColumn);
	TDbColNo catIdCol		= colset->ColNo(KSQLCategoryUidColumn);
	TDbColNo topicTextCol	= colset->ColNo(KSQLTopicTextColumn);
	delete colset;

	//
	// WRITE THE TOPIC TABLE
	//
	TBuf8<1000> buffer;
	aOutputFile.Write(_L8("==============================\r\n"));
	aOutputFile.Write(_L8("==       TOPIC TABLE        ==\r\n"));
	aOutputFile.Write(_L8("==============================\r\n"));
	do
		{
		count++;
		table.GetL();

		// Write category & category Id
		TBuf8<150> category;
		User::LeaveIfError(TheConverter->ConvertFromUnicode(category, table.ColDes(catCol)));
		buffer.Format(_L8("Category (Id): %S (%d)\r\n"), &category, table.ColUint32(catIdCol));
		aOutputFile.Write(buffer);

		// Write topic Id
		buffer.Format(_L8("Topic Id: %d\r\n"), table.ColUint32(topicIdCol));
		aOutputFile.Write(buffer);

		// Write title
		TBuf8<150> title;
		User::LeaveIfError(TheConverter->ConvertFromUnicode(title, table.ColDes(titleCol)));
		buffer.Format(_L8("Title: %S\r\n"), &title);
		aOutputFile.Write(buffer);

		// Write synonym
		TBuf8<200> synonym;
		User::LeaveIfError(TheConverter->ConvertFromUnicode(synonym, table.ColDes(synonymCol)));
		buffer.Format(_L8("Synonym: %S\r\n"), &synonym);
		aOutputFile.Write(buffer);

		// Write topic text
		TInt len = table.ColLength(topicTextCol);
		HBufC* text = HBufC::NewLC(len);
		TPtr pText(text->Des());
	
		// Retrieve actual text from column
		RDbColReadStream stream;
		stream.OpenLC(table, topicTextCol);
		stream.ReadL(pText, len);
		CleanupStack::PopAndDestroy(); // stream

		// Write the text
		HBufC8* narrowText = HBufC8::NewLC(len);
		TPtr8 pNarrowText(narrowText->Des());
		User::LeaveIfError(TheConverter->ConvertFromUnicode(pNarrowText, pText));
		CleanupStack::Pop(); // narrowText
		CleanupStack::PopAndDestroy(); // text

		aOutputFile.Write(_L8("Body: "));
		aOutputFile.Write(*narrowText);
		aOutputFile.Write(_L8("\r\n"));
		delete narrowText;

		aOutputFile.Write(_L8("\r\n"));
		}
	while (table.NextL());
	aOutputFile.Write(_L8("\r\n"));

	CleanupStack::PopAndDestroy(); // table
	}

static void WriteContextTableL(RFile& aOutputFile, RDbStoreDatabase& aDatabase)
	{
	_LIT(KContextTable, "CONTEXT");

	// Open the topic table so that we can use it to build the category list
	RDbTable table;
	User::LeaveIfError(table.Open(aDatabase, KContextTable, RDbRowSet::EReadOnly));
	CleanupClosePushL(table);

	// Now build the list
	if	(!table.FirstL())
		{
		CleanupStack::PopAndDestroy(); // table
		return; // assume no rows
		}

	// Get the column of the category
	CDbColSet* colset		= table.ColSetL();
	TDbColNo contextCol		= colset->ColNo(KSQLContextColumn);
	TDbColNo topicIdCol		= colset->ColNo(KSQLTopicIdColumn);
	delete colset;

	//
	// WRITE THE TOPIC TABLE
	//
	TBuf8<1000> buffer;
	aOutputFile.Write(_L8("==============================\r\n"));
	aOutputFile.Write(_L8("==      CONTEXT TABLE       ==\r\n"));
	aOutputFile.Write(_L8("==============================\r\n"));
	do
		{
		table.GetL();

		{
		// Write context
		TBuf8<150> context;
		User::LeaveIfError(TheConverter->ConvertFromUnicode(context, table.ColDes(contextCol)));
		buffer.Format(_L8("Context: %S\r\n"), &context);
		aOutputFile.Write(buffer);
		}
		{
		// Write topic Id
		buffer.Format(_L8("Topic Id: %d\r\n"), table.ColUint32(topicIdCol));
		aOutputFile.Write(buffer);
		}
		aOutputFile.Write(_L8("\r\n"));
		}
	while (table.NextL());
	aOutputFile.Write(_L8("\r\n"));

	CleanupStack::PopAndDestroy(); // table
	}


static void WriteTopicIndexTableL(RFile& aOutputFile, RDbStoreDatabase& aDatabase)
	{
	_LIT(KTopicIndexTable, "TOPICINDEX");

	// Open the topic table so that we can use it to build the category list
	RDbTable table;
	User::LeaveIfError(table.Open(aDatabase, KTopicIndexTable, RDbRowSet::EReadOnly));
	CleanupClosePushL(table);

	// Now build the list
	if	(!table.FirstL())
		{
		CleanupStack::PopAndDestroy(); // table
		return; // assume no rows
		}

	// Get the column of the category
	CDbColSet* colset			= table.ColSetL();
	TDbColNo topicIdCol			= colset->ColNo(KSQLTopicIdColumn);
	TDbColNo indexIdCol			= colset->ColNo(KSQLIndexIdColumn);
	TDbColNo categoryUidCol		= colset->ColNo(KSQLCategoryUidColumn);
	TDbColNo titleCol			= colset->ColNo(KSQLTopicTitleColumn);
	delete colset;

	//
	// WRITE THE TOPIC TABLE
	//
	TBuf8<1000> buffer;
	aOutputFile.Write(_L8("==============================\r\n"));
	aOutputFile.Write(_L8("==    TOPIC/INDEX TABLE     ==\r\n"));
	aOutputFile.Write(_L8("==============================\r\n"));
	do
		{
		table.GetL();

		// Write topic Id
		buffer.Format(_L8("Topic Id: %d\r\n"), table.ColUint32(topicIdCol));
		aOutputFile.Write(buffer);

		// Write index Id
		buffer.Format(_L8("Index Id: %d\r\n"), table.ColUint32(indexIdCol));
		aOutputFile.Write(buffer);

		// Write out the category uid
		buffer.Format(_L8("Category Uid: %d\r\n"), table.ColUint32(categoryUidCol));
		aOutputFile.Write(buffer);

		{
		// Write out the topic title
		TBuf8<150> topicTitle;
		User::LeaveIfError(TheConverter->ConvertFromUnicode(topicTitle, table.ColDes(titleCol)));
		buffer.Format(_L8("TopicTitle: %S\r\n"), &topicTitle);
		aOutputFile.Write(buffer);
		}

		aOutputFile.Write(_L8("\r\n"));
		}
	while (table.NextL());
	aOutputFile.Write(_L8("\r\n"));

	CleanupStack::PopAndDestroy(); // table
	}

static void WriteIndexTableL(RFile& aOutputFile, RDbStoreDatabase& aDatabase)
	{
	_LIT(KIndexTable, "INDEX");

	// Open the topic table so that we can use it to build the category list
	RDbTable table;
	User::LeaveIfError(table.Open(aDatabase, KIndexTable, RDbRowSet::EReadOnly));
	CleanupClosePushL(table);

	// Now build the list
	if	(!table.FirstL())
		{
		CleanupStack::PopAndDestroy(); // table
		return; // assume no rows
		}

	// Get the column of the category
	CDbColSet* colset		= table.ColSetL();
	TDbColNo indexCol		= colset->ColNo(KSQLIndexColumn);
	TDbColNo indexIdCol		= colset->ColNo(KSQLIndexIdColumn);
	delete colset;

	//
	// WRITE THE TOPIC TABLE
	//
	TBuf8<1000> buffer;
	aOutputFile.Write(_L8("==============================\r\n"));
	aOutputFile.Write(_L8("==       INDEX TABLE        ==\r\n"));
	aOutputFile.Write(_L8("==============================\r\n"));
	do
		{
		table.GetL();

		// Write topic Id
		TBuf8<140> index;
		User::LeaveIfError(TheConverter->ConvertFromUnicode(index, table.ColDes(indexCol)));
		buffer.Format(_L8("Index (Id): %S (%d)\r\n"), &index, table.ColUint32(indexIdCol));
		aOutputFile.Write(buffer);

		aOutputFile.Write(_L8("\r\n"));
		}
	while (table.NextL());
	aOutputFile.Write(_L8("\r\n"));

	CleanupStack::PopAndDestroy(); // table
	}

static void WriteFileUidL(RFile& aOutputFile, const TDesC& aFileName)
	{
	//
	// WRITE THE TOPIC TABLE
	//
	TBuf8<1000> buffer;
	aOutputFile.Write(_L8("==============================\r\n"));
	aOutputFile.Write(_L8("==       FILE UID           ==\r\n"));
	aOutputFile.Write(_L8("==============================\r\n"));

	TEntry entry;
	TheFs.Entry(aFileName, entry);
	buffer.Format(_L8("File Uids: [0x%x][0x%x][0x%x]\r\n"), entry[0], entry[1], entry[2]);
	aOutputFile.Write(buffer);
	aOutputFile.Write(_L8("\r\n"));
	}

static void Test1L()
	{

	TConsoleLine inputFile(_L("C:\\Resource\\Help\\"));
	TheTest.Printf(_L("Enter filename: "));
	if	(!GetLine(inputFile))
		{
		// Escape'd
		return;
		}

	TFileName outputFileName;
	TParsePtrC parser(inputFile);
	TPtrC pName(parser.Name());
	outputFileName.Format(KDumpOutputFile, &pName);

	RFile outputFile;
	User::LeaveIfError(outputFile.Replace(TheFs, outputFileName, EFileShareExclusive | EFileWrite));
	CleanupClosePushL(outputFile);

	CPermanentFileStore* store = CPermanentFileStore::OpenLC(TheFs, inputFile, EFileRead);
	CStreamDictionary* dictionary = CStreamDictionary::NewLC();
	RStoreReadStream in;
	in.OpenLC(*store, store->Root());
	dictionary->InternalizeL(in);
	CleanupStack::PopAndDestroy(); // in

	// restore the database
	RDbStoreDatabase database;
	database.OpenL(store, dictionary->At(KUidHlpDbStream));
	CleanupClosePushL(database);

	// Write the topic table
	WriteTopicTableL(outputFile, database);

	// Write the context table
	WriteContextTableL(outputFile, database);

	// Write topic/index table
	WriteTopicIndexTableL(outputFile, database);

	// Write index table
	WriteIndexTableL(outputFile, database);

	// Write the uid of the file itself
	WriteFileUidL(outputFile, inputFile);

	CleanupStack::PopAndDestroy(4); // database, dictionary, store, outputFile
	}





static void setupUnicodeConverterL()
//
// Initialise charconv
//
	{
	TheConverter = CCnvCharacterSetConverter::NewL();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable = TheConverter->CreateArrayOfCharacterSetsAvailableL(TheFs);
	CleanupStack::PushL(arrayOfCharacterSetsAvailable);
	TheConverter->PrepareToConvertToOrFromL(0x100012b6, *arrayOfCharacterSetsAvailable, TheFs); // 0x100012b6 is KCharacterSetIdentifierCodePage1252
	TheConverter->SetReplacementForUnconvertibleUnicodeCharactersL(_L8("?"));
	CleanupStack::PopAndDestroy(); // arrayOfCharacterSetsAvailable
	}


static void setupFileServerAndSchedulerL()
//
// Initialise the cleanup stack.
//
	{
	TheTest(TheFs.Connect() == KErrNone);
	TheScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(TheScheduler);
	}


static void setupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup = CTrapCleanup::New();
	TheTest(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TheTest(r==KErrNone);
	}

GLDEF_C TInt E32Main()
//
// Test Help Model API
/**
@SYMTestCaseID PIM-TDUMPER-0001
*/
//
    {
	__UHEAP_MARK;

	TheTest.Start(_L("@SYMTestCaseID PIM-TDUMPER-0001 "));
	TheTest.Title();
	setupCleanup();

	TRAPD(r,
		setupFileServerAndSchedulerL();
		setupUnicodeConverterL();
		Test1L();
		);
	TheTest(r==KErrNone);

	delete TheConverter;
	delete TheScheduler;
	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
