// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "caltestlib.h"

#include <e32test.h>
#include <calsession.h>

_LIT(KTestName, "TCal_FileAccess");
RTest test(KTestName);
_LIT(KFileName, "C:TestFile_FileAccess");
_LIT(KSecondFileName, "C:TestFile_FileAccess_2");
_LIT(KThirdFileName, "C:TestFile_FileAccess_3");


class CFileAccessTestManager : public CBase
	{
public:
    static CFileAccessTestManager* NewLC();
	void RunAllTestsL();
	~CFileAccessTestManager();	
	
private:
	void ConstructL();
	void TestDefaultFileNameL();
	void TestCreateAndOpenFileL();
	void TestListAgendaFilesL();
	void TestDeleteFileL();
	void TestCreateBadFileNameL();
	void TestCreateLongFileNameL();
	
private:	
	CCalSession* iModel;
	CCalTestLibrary* iTestLib;
	};

CFileAccessTestManager* CFileAccessTestManager::NewLC()
	{
	CFileAccessTestManager* self = new (ELeave) CFileAccessTestManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();

	return (self);
	}

CFileAccessTestManager::~CFileAccessTestManager()
	{
	delete iTestLib;
	}

void CFileAccessTestManager::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();	
	iModel = &(iTestLib->GetSession());
	}

void CFileAccessTestManager::TestDefaultFileNameL()
	{
	_LIT(KDefaultFileName, "C:Calendar");
	
	const TPtrC16 KName = iModel->DefaultFileNameL();
	test(KName == KDefaultFileName);
	
	test.Printf(iModel->DefaultFileNameL());
	test.Printf(_L("\n"));
	}


void CFileAccessTestManager::RunAllTestsL()
	{
	
	test.Next(_L("GetDefaultName"));
	TestDefaultFileNameL();

	test.Next(_L("CreateAndOpenFile"));
	TestCreateAndOpenFileL();

	test.Next(_L("ListsAgendaFiles"));
	TestListAgendaFilesL();
	
	test.Next(_L("CloseAndDeleteFile"));
	TestDeleteFileL();

	test.Next(_L("CreateBadFileName"));
	TestCreateBadFileNameL(); 
	
	test.Next(_L("CreateLongFileName"));
	TestCreateLongFileNameL();
	}
	
// Test the Opening of a non-existent File
// Test Creating a File
// Test The Opening of the newly created file	
void CFileAccessTestManager::TestCreateAndOpenFileL()
	{
	_LIT(KBadName, "C:NonExistentFile");
	TRAPD(expectedErr, iModel->OpenL(KBadName));
	test(expectedErr == KErrNotFound);
	
	_LIT(KFullPathName, "C:\\private\\10003a5b\\TestFile_FileAccess");
	TRAP(expectedErr, iModel->OpenL(KFullPathName));
	test(expectedErr == KErrNotSupported);
	
	TRAPD(err, iModel->CreateCalFileL(KFileName));
	test(err == KErrAlreadyExists || err == KErrNone);
		
	iModel->OpenL(KFileName);
	}
	
// Test that three created files are Listing in ListCalFilesL
// (Other files will exist in the Cal private directory due to other tests) 	
void CFileAccessTestManager::TestListAgendaFilesL()
	{
	TBool foundFirstFile((EFalse));
	TBool foundSecondFile((EFalse));
	TBool foundThirdFile((EFalse));
	
	// create Three files and check they can be listed
	TRAPD(err, iModel->CreateCalFileL(KFileName));
	test(err == KErrAlreadyExists);  // created in first test step
	
	TRAPD(errSecondCreate, iModel->CreateCalFileL(KSecondFileName));
	test(errSecondCreate == KErrNone);	
	
	TRAPD(errThirdCreate, iModel->CreateCalFileL(KThirdFileName));
	test(errThirdCreate == KErrNone);	
	
	CDesCArray* agendaFiles(iModel->ListCalFilesL()); 
	CleanupStack::PushL(agendaFiles);
	
	for(TInt i = 0; i < agendaFiles->MdcaCount(); ++i)  // there may be other files from other tests
		{
		test.Printf(agendaFiles->MdcaPoint(i));
		test.Printf(_L("\n"));
		
		foundFirstFile = foundFirstFile || ((*agendaFiles)[i]) == KFileName;
		foundSecondFile = foundSecondFile || ((*agendaFiles)[i]) == KSecondFileName;
		foundThirdFile = foundThirdFile || ((*agendaFiles)[i]) == KThirdFileName;		
		}
		
	test(foundFirstFile);
	test(foundSecondFile);
	test(foundThirdFile);		
		
	CleanupStack::PopAndDestroy(agendaFiles);
	}
	
// Delete 2 of the 3 Files and check they are not listed in ListCalFilesL and the other file is	
// Delete a non-existent file
void CFileAccessTestManager::TestDeleteFileL()
	{	
	iModel->DeleteCalFileL(KFileName);
	iModel->DeleteCalFileL(KSecondFileName);
	
	TBool foundFirstFile((EFalse));
	TBool foundSecondFile((EFalse));
	TBool foundThirdFile((EFalse));	
	
	CDesCArray* agendaFiles(iModel->ListCalFilesL()); 
	CleanupStack::PushL(agendaFiles);
	
	for(TInt i = 0; i < agendaFiles->MdcaCount(); ++i)  // there may be other files from other tests
		{		
		foundFirstFile = foundFirstFile || ((*agendaFiles)[i]) == KFileName;
		foundSecondFile = foundSecondFile || ((*agendaFiles)[i]) == KSecondFileName;
		foundThirdFile = foundThirdFile || ((*agendaFiles)[i]) == KThirdFileName;		
		}
		
	test(!foundFirstFile);
	test(!foundSecondFile);
	test(foundThirdFile);		
		
	CleanupStack::PopAndDestroy(agendaFiles);
	iModel->DeleteCalFileL(KThirdFileName);	
	
	TRAPD(deleteErr, iModel->DeleteCalFileL(KThirdFileName));
	test(deleteErr == KErrNotFound);	
	}

void CFileAccessTestManager::TestCreateBadFileNameL()
	{
	// Test for INC122832
 	TBuf<239> nameExceeds238Chars;
 	nameExceeds238Chars.Append(_L("C:"));
 	nameExceeds238Chars.AppendFill('a', 237);
 	TRAPD(err2, iModel->CreateCalFileL(nameExceeds238Chars));
 	test(err2 == KErrBadName);
 	
 	TBuf<237> nameExceeds236CharsWithNoDrive;
 	nameExceeds236CharsWithNoDrive.AppendFill('a', 237);
 	TRAPD(err3, iModel->CreateCalFileL(nameExceeds236CharsWithNoDrive));
 	test(err3 == KErrBadName);
 	}


void CFileAccessTestManager::TestCreateLongFileNameL()
	{
	// Test for DEF113074
 	TBuf<200> maxLengthFilename;
 	maxLengthFilename.Append(_L("C:"));
 	maxLengthFilename.AppendFill('a', 198);
 	TRAPD(err, iModel->CreateCalFileL(maxLengthFilename));
 	test(err == KErrNone);   
 	//Test to create same existing file
 	TRAPD(err1, iModel->CreateCalFileL(maxLengthFilename));
 	test(err1 == KErrAlreadyExists || err1 == KErrNone); 	
 	iModel->OpenL(maxLengthFilename);  	
 	iModel->DeleteCalFileL(maxLengthFilename);
 	
 	
 	//Test to create longfile name with invalid chars
 	TBuf<201> maxLengthInvalidFilename;
 	maxLengthInvalidFilename.Append(_L("C:"));
 	maxLengthInvalidFilename.AppendFill('*', 199);
 	TRAPD(err2, iModel->CreateCalFileL(maxLengthInvalidFilename));
 	test(err2 == KErrBadName);
 	
 	
 	//Test to create no drive name
 	TBuf<200> maxLengthNoDriveFilename;
 	maxLengthNoDriveFilename.AppendFill('a', 200);
 	TRAPD(err3, iModel->CreateCalFileL(maxLengthNoDriveFilename));
 	test(err3 == KErrNone);
 	iModel->DeleteCalFileL(maxLengthNoDriveFilename);
 	
 	//Test to create long filename with different chars
 	TBuf<200> maxLengthFilenameDiffChars;
 	maxLengthFilenameDiffChars.Append(_L("C:"));
 	maxLengthFilenameDiffChars.AppendFill('i', 50);
 	maxLengthFilenameDiffChars.AppendFill('2', 50);
 	maxLengthFilenameDiffChars.AppendFill('@', 48);
 	maxLengthFilenameDiffChars.AppendFill('x', 40);
 	maxLengthFilenameDiffChars.AppendFill('w', 10);
 	TRAPD(err4, iModel->CreateCalFileL(maxLengthFilenameDiffChars));
 	test(err4 == KErrNone);
 	iModel->OpenL(maxLengthFilenameDiffChars); 
 	iModel->DeleteCalFileL(maxLengthFilenameDiffChars);
 	
 	//Test to create file name exceed max length
 	TBuf<201> maxLengthExceedFilename;
 	maxLengthExceedFilename.Append(_L("C:"));
 	maxLengthExceedFilename.AppendFill('g', 199);
 	TRAPD(err5, iModel->CreateCalFileL(maxLengthExceedFilename));
 	test(err5 == KErrBadName);
 	
 	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CFileAccessTestManager* testManager = CFileAccessTestManager::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	testManager->RunAllTestsL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();


	CleanupStack::PopAndDestroy(testManager);
	}


/**
@SYMTestCaseID     PIM-TCAL-FILEACCESS-0001
*/
TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-FILEACCESS-0001 Calendar Interim API File Access test suite"));

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }

