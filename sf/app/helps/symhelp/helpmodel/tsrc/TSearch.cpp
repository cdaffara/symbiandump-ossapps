// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test Help Model module to check the PlatSec search order.
// This test uses and additional drive typically F: It should be mapped in the epoc.ini file.
// Mapping is achieved for example thus: _EPOC_DRIVE_F c:\temp
// 
//

// System includes
#include <e32std.h>
#include <e32def.h>


#include <e32test.h>
#include <e32math.h>
#include <f32file.h>
#include <coehelp.h>

// User includes
#include <bautils.h>
#include "HLPMODEL.H"
#include <coreappstest/testserver.h>
#include "SearchOrderTest.h"

// Globals
static TInt64				TheSeed;
static RFs					TheFs;
static RTest				TheTest(_L("TSearch - Test Help Model Searching"));
static CTrapCleanup*		TheTrapCleanup;
static CActiveScheduler*	TheScheduler;

// Constants
const TInt KTestCleanupStack=0x20;


// Panic enum
enum THlpPanic
	{
	EHlpPanicNoBossFile,
	EHlpPanicLast
	};


//
// ---------> Global functions
//

static void Panic(THlpPanic aPanic)
	{
	_LIT(KHlpPanicCategory, "TSearch");
	User::Panic(KHlpPanicCategory, aPanic);
	}


static TInt Rand(const TInt aLow, const TInt aHigh)
//
//	Generate a random number based upon a seed and a range
//
	{
	TReal initialRand = (Math::FRand(TheSeed) * (aHigh - aLow));
	TInt32 rand;

	// Round to 0 decimal places, ie. the nearest whole numer
	Math::Round(initialRand, initialRand, 0);
	Math::Int(rand, initialRand);

	//aSeed = seed;
	return (aLow + rand);
	}



//
// ---------> CHlpSearchTest (header)
//
class CHlpSearchTest: public CBase, public MHlpModelObserver
	{
public: // CONSTRUCT / DESTRUCT
	static void TestHelpModelLD(TBool aOomTesting = EFalse);
	~CHlpSearchTest();

private:
	CHlpSearchTest();	

public: // NEW
	void TestL(TBool aOomTesting = EFalse);

public: // FROM MHlpModelObserver
	void HandleModelEventL(TInt aEvent);

private: // TESTING METHODS
	void BasicCreateAndCloseL();
	void IndexListL(TBool aOomTesting = EFalse);
	void CategoryListL(TBool aOomTesting = EFalse);
	void TopicListForCategoryL();
	void ContextSearchL(TBool aOomTesting = EFalse);
	void IndexSearchL();
	void TextSearchL(TBool aOomTesting = EFalse);
	void CategoryUidSearchL(TBool aOomTesting = EFalse);

private:
	void Reset();
	TBool FileExists(const TDesC& aFileName);
	CDesCArray* CategoryListLC();
	
private: // MEMBER DATA
	TInt iResponseCount;
	TInt iLastHelpModelResponse;
	TInt iSearchProgressCount;
	TInt iSearchCancelValue;
	TBool iIsAsynchronous;
	TBool iSchedulerStarted;
	CHlpModel* iModel;
	};






//
// ---------> CHlpSearchTest (source)
//

CHlpSearchTest::CHlpSearchTest()
	{
	}


CHlpSearchTest::~CHlpSearchTest()
	{
	delete iModel;
	}


void CHlpSearchTest::TestHelpModelLD(TBool aOomTesting)
	{
	CHlpSearchTest* self = new(ELeave) CHlpSearchTest();
	CleanupStack::PushL(self);
	self->TestL(aOomTesting);
	CleanupStack::PopAndDestroy();
	}


//
//
//


void CHlpSearchTest::TestL(TBool aOomTesting)
	{
	iModel = CHlpModel::NewL(TheFs, this);
	iModel->OpenL();


	BasicCreateAndCloseL();
	IndexListL(aOomTesting);
	CategoryListL(aOomTesting);
	TopicListForCategoryL();
	ContextSearchL(aOomTesting);
	TextSearchL(aOomTesting);
	CategoryUidSearchL(aOomTesting);
	}


//
//
//

void CHlpSearchTest::BasicCreateAndCloseL()
	{
	TheTest.Console()->ClearScreen();
	TheTest.Printf(_L("Testing basic open, close and destruction\n"));

	__UHEAP_MARK;
	CHlpModel::NewLC(TheFs, NULL);
	CleanupStack::PopAndDestroy(); // model
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	CHlpModel* model = CHlpModel::NewLC(TheFs, NULL);
	model->OpenL();
	CleanupStack::PopAndDestroy(); // model
	__UHEAP_MARKEND;
	}

void CHlpSearchTest::IndexListL(TBool aOomTesting)
	{
	TheTest.Console()->ClearScreen();
	TheTest.Printf(_L("Testing index listing\n"));
	Reset();
	
	iModel->SearchL(EIndexList);

	if(!aOomTesting)
		{
		TheTest(iLastHelpModelResponse == EIndexListAvailable);
		}
	TheTest(iResponseCount == 1);

	// Load the complete list of entries for all help files
	CHlpList* list = CHlpList::NewLC();
	iModel->LoadListL(list);
	TheTest.Printf(_L("\tComplete index listing for all help files..."));
	TInt i=0;
	for(i=0; i<list->MdcaCount(); i++)
		{
		CHlpItem* item = list->Item(i);
		TPtrC pItem(item->Title());
		TheTest.Printf(_L("\nIndex list [0x%08x][indexId=%d]: %S "), item->HelpFileUid(), item->Id(), &pItem);
		}
	TheTest.Printf(_L("\n"));

	// Next select an item from the complete list (at random)

	if(list->MdcaCount()!=0)
		{
		CHlpItem* item = list->Item(Rand(0, list->MdcaCount()-1));
		
		// Save these for later...
		TUint helpFileUid = item->HelpFileUid().iUid;
		TUint indexId = item->Id();

		// Next perform a topic search based on this item
		iModel->IndexSearchL(*item);
		TheTest(iLastHelpModelResponse == EIndexSearchListAvailable);
		CleanupStack::PopAndDestroy(); // list

		// Next load the list of topics which match the index id
		list = CHlpList::NewLC();
		iModel->LoadListL(list);
		TheTest.Printf(_L("\tTopic listing in help file 0x%08x, whose index id = %d"), helpFileUid, indexId);
		for(i=0; i<list->MdcaCount(); i++)
			{
			CHlpItem* item = list->Item(i);
			TPtrC pItem(item->Title());
			TheTest.Printf(_L("\nTopic [Uid: 0x%08x]: %S "), item->HelpFileUid(), &pItem);
			}
	}
	TheTest.Printf(_L("\n"));

	CleanupStack::PopAndDestroy(); // list
	}


void CHlpSearchTest::CategoryListL(TBool aOomTesting)
	{
	TheTest.Console()->ClearScreen();
	TheTest.Printf(_L("Testing category listing\n"));

	Reset();
	iModel->SearchL(ECategoryList);
	if(!aOomTesting)
		{
		TheTest(iLastHelpModelResponse == ECategoryListAvailable);
		}

	// Display the index listing...
	CHlpList* list = CHlpList::NewLC();
	iModel->LoadListL(list);

	for(TInt i=0; i<list->MdcaCount(); i++)
		{
		TPtrC pItem(list->MdcaPoint(i));
		TheTest.Printf(_L("\nCategory item %d: %S"), i, &pItem);
		}
	TheTest.Printf(_L("\n"));
	CleanupStack::PopAndDestroy(); // list
	}


void CHlpSearchTest::TopicListForCategoryL()
	{
	TheTest.Printf(_L("Testing topic listing\n"));
	Reset();

	// First get a category listing, then get the list of topics
	// for each specified category.
	CDesCArrayFlat* array = STATIC_CAST(CDesCArrayFlat*, CategoryListLC());
	const TInt count = array->Count();
	for(TInt i=0; i<count; i++)
		{
		TPtrC pItem = array->MdcaPoint(i);
		TheTest.Printf(_L("\nTopics for Category %S\n"), &pItem);

		// Calling this will result in the topics for this category
		// being printed out in the mixin handler
		iModel->SearchL(ETopicListForCategory, pItem);
		}
	CleanupStack::PopAndDestroy(); // array
	}


void CHlpSearchTest::ContextSearchL(TBool aOomTesting)
	{
	TheTest.Printf(_L("Testing context searching\n"));
	Reset();

	// Context search for an item we know exists
	_LIT(KContext1,	"This_is_a_multi_word_context2");

	TUid uid = { 16777216 };
	TCoeHelpContext coeSearchContext(uid, KContext1);
	iModel->ContextSearchL(coeSearchContext);
	if(!aOomTesting)
		{
		TheTest(iLastHelpModelResponse == ETopicAvailable);
		}

	
	// Context search for an item we know doesn't exist
	_LIT(KContext2,	"ABCDEFG");

	TUid uid2 = { 00000000 };
	TCoeHelpContext coeSearchContext2(uid2, KContext2);
	iModel->ContextSearchL(coeSearchContext2);
	TheTest(iLastHelpModelResponse == ETopicNotFound);
	}


void CHlpSearchTest::TextSearchL(TBool aOomTesting)
	{
	if(aOomTesting)
		return;

	_LIT(KFullTextShouldNotBeFound,	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	TheTest.Printf(_L("Testing text searching (when item shouldn't be found)\n"));

	Reset();
	iSchedulerStarted = EFalse;
	iIsAsynchronous = ETrue;
	iModel->SearchL(EFullTextSearch, KFullTextShouldNotBeFound);
	iSchedulerStarted = ETrue;
	CActiveScheduler::Start();
	TheTest(iLastHelpModelResponse == ESearchListNoneFound);

	_LIT(KFullTextShouldBeFound,	"boss");
	TheTest.Printf(_L("Testing text searching (when item should be found)\n"));

	Reset();
	iSchedulerStarted = EFalse;
	iIsAsynchronous = ETrue;
	iModel->SearchL(EFullTextSearch, KFullTextShouldBeFound);
	iSchedulerStarted = ETrue;
	CActiveScheduler::Start();
	TheTest(iLastHelpModelResponse == ESearchListAvailable);

	TheTest.Printf(_L("\tTesting asynch cancel function\n"));
	TInt searchCancelValue = Max(1, iSearchProgressCount / 2); // stop half way through

	Reset();
	iSearchCancelValue = searchCancelValue;
	iSchedulerStarted = EFalse;
	iIsAsynchronous = ETrue;
	iModel->SearchL(EFullTextSearch, KFullTextShouldBeFound);
	iSchedulerStarted = ETrue;
	CActiveScheduler::Start();
	}


void CHlpSearchTest::CategoryUidSearchL(TBool aOomTesting)
	{
	TheTest.Console()->ClearScreen();
	TheTest.Printf(_L("Testing category uid searching - *requires boss help file*\n"));
	

    if	((!FileExists(_L("C:\\Resource\\Help\\Boss-1.hlp"))) && (!FileExists(_L("Z:\\Resource\\Help\\Boss-1.hlp"))))
		Panic(EHlpPanicNoBossFile);
	

	Reset();
	iModel->CategoryUIDSearchL(TUid::Uid(16777216)); // hard coded category uid from Boss1.hlp
	if(!aOomTesting)
		{
		TheTest(iLastHelpModelResponse == ETopicListAvailable);
		}
	TheTest(iResponseCount == 1);

	// Display the index listing...
	CHlpList* list = CHlpList::NewLC();
	iModel->LoadListL(list);

	for(TInt i=0; i<list->MdcaCount(); i++)
		{
		TPtrC pItem(list->MdcaPoint(i));
		TheTest.Printf(_L("\nCategory item %d: %S"), i, &pItem);
		}
	CleanupStack::PopAndDestroy(); // list
	TheTest.Printf(_L("\n"));

	// Test for a category that we know doesn't exist
	Reset();
	iModel->CategoryUIDSearchL(KNullUid);
	TheTest(iLastHelpModelResponse == ETopicListNoneFound);
	TheTest(iResponseCount == 1);
	}


//
//
//


void CHlpSearchTest::Reset()
	{
	iResponseCount = 0;
	iLastHelpModelResponse = KErrGeneral;
	iIsAsynchronous = EFalse;
	iSearchProgressCount = 0;
	iSearchCancelValue = KErrNone-1; // Anything less that KErrNone
	}

TBool CHlpSearchTest::FileExists(const TDesC& aFileName)
	{
	TEntry entry;
	TInt error = TheFs.Entry(aFileName, entry);
	if	(error == KErrInUse)
		return ETrue;
	if	(error < KErrNone)
		return EFalse;
	return (!entry.IsDir());
	}

CDesCArray* CHlpSearchTest::CategoryListLC()
	{
	// Get and print the category listing
	CDesCArrayFlat* array = new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(array);
	iModel->CategoryListL(array);
	return array;
	}


//
//
//


void CHlpSearchTest::HandleModelEventL(TInt aEvent)
	{
	iResponseCount++;
	iLastHelpModelResponse = aEvent;

	// If this test included an asynch search, and there was a cancel value set
	// then cancel the asynch searcher
	if	(++iSearchProgressCount == iSearchCancelValue && iIsAsynchronous)
		{
		// Reset this otherwise we'll get stuck in a loop
		iSearchCancelValue = KErrNone-1;

		// Only works for full test searching as this is the only search type 
		// which uses an active object, and even then it doesn't use them properly.
		TheTest(iModel->CancelSearch() == KErrNone);
		}

	switch (aEvent)
		{
	case ESearchListAvailable:		// text searches
		TheTest.Printf(_L("\n\tESearchListAvailable\n"));
		break;
	
	case ECategoryListAvailable:
		{
		TheTest.Printf(_L("\n\tECategoryListAvailable\n"));

		// Get and print the category listing
		CDesCArrayFlat* array = STATIC_CAST(CDesCArrayFlat*, CategoryListLC());
		const TInt count = array->Count();
		for(TInt i=0; i<count; i++)
			{
			TPtrC pItem = array->MdcaPoint(i);
			TheTest.Printf(_L("Category %d: %S\n"), i, &pItem);
			}
		CleanupStack::PopAndDestroy(); // array
		break;
		}
	case ECategoryListNoneFound:
		TheTest.Printf(_L("\n\tECategoryListNoneFound\n"));
		break;


	case ETopicListAvailable:		// category expansion
		{
		TheTest.Printf(_L("\n\tETopicListAvailable\n"));
		CHlpList* list = CHlpList::NewLC();
		iModel->LoadListL(list);
		TInt count = list->MdcaCount();
		for(TInt i=0; i<count; i++)
			{
			CHlpItem* item = list->Item(i);
			TPtrC pTitle(*item->iTitle);
			TheTest.Printf(_L("Topic %d: %S (id = %d)\n"), i, &pTitle, item->iId);
			}
		CleanupStack::PopAndDestroy(); // list
		break;
		}
	case ETopicListNoneFound:
		TheTest.Printf(_L("\n\tETopicListNoneFound\n"));
		break;

	
	case ETopicAvailable:			// topic text
		TheTest.Printf(_L("\n\tETopicAvailable\n"));
		break;
	case ETopicNotFound:
		TheTest.Printf(_L("\n\tETopicNotFound\n"));
		break;


	case EIndexListAvailable:		// index list
		TheTest.Printf(_L("\n\tEIndexListAvailable\n"));
		break;
	case EIndexListNoneFound:
		TheTest.Printf(_L("\n\tEIndexListNoneFound\n"));
		break;
	case EIndexSearchListAvailable:	// topic list from index phrase
		TheTest.Printf(_L("\n\tEIndexSearchListAvailable\n"));
		break;
	case EIndexSearchListNoneFound:
		TheTest.Printf(_L("\n\tEIndexSearchListNoneFound\n"));
		break;


	case EHlpSearchCancelled:
		TheTest.Printf(_L("<SEARCH CANCELLED>"));
		break;
	case EModelSearchInProgress:
		TheTest.Printf(_L("..."));
		return;
	case ESearchListNoneFound:
		TheTest.Printf(_L("\n\tESearchListNoneFound\n"));
		break;
		}

	if	(iIsAsynchronous && iSchedulerStarted)
		CActiveScheduler::Stop();
	}
	
/**
	doSearchOrderTest
	
	All help files used in this part of the test use the Category UID of 0x01007000
	Within each source RTF file the Category is defined as the drive name.
	Hence the File copied to c:\resource\help has the Category "Drive C:"
	There are 3 help files used in this test:
		PlatSecSearchTestC.hlp - initially resides in z:\System\Programs\THlpmodel\
		PlatSecSearchTestF.hlp - initially resides in z:\System\Programs\THlpmodel\
		PlatSecSearchTest.hlp - (with Category "Drive Z:") resides in z:\Resoruce\Help
	The exports are specified in the Hlpmodel BLD.INF file.
	
	As part of the building process the help files are exported to the
	corresponding directories.

	Thus initially only PlatSecSearchTest.hlp with be visible presenting 
	the Category "Drive Z:". As the test progresses so the 2 remaining files are
	copied (with a change in name) from z:\System\Programs\THlpmodel\ to their corresponding drive.
	Thus PlatSecSearchTestC.hlp becomes PlatSecSearchTest.hlp in c:\Resource\Help
	and PlatSecSearchTestF.hlp becomes PlatSecSearchTest.hlp in f:\Resource\Help
	
	Given the search order, the higher drive letter takes presidence (however z: is last),
	so when there are is a file called PlatSecSearchTest.hlp on z:, c: and f: only f: 
	will be seen.
	
	Note: That the test looks for a drive of type EMediaHardDisk: on Lubbock this will be F:
	on the H2 it is likely to be E: this is managed within CSearchOrderTest.
*/



static void doSearchOrderTestL()
	{
	__UHEAP_MARK;
	TheTest.Printf(_L("-------------------------------------------\n"));				
	TheTest.Printf(_L("   Testing PlatSec drive search Order.\n"));			
	TheTest.Printf(_L("-------------------------------------------\n"));				

	// Initialise the array of mapped drives.
	CSearchOrderTest* sort = CSearchOrderTest::NewL(TheTest);
	CleanupStack::PushL( sort );
	
	TheTest.Next(_L("Test category listing"));
	
	CHlpModel* model = CHlpModel::NewLC(TheFs, NULL);

	CDesCArray* catList = new(ELeave) CDesCArrayFlat(2);
	CleanupStack::PushL(catList);
		
	// Iterate around Drive Loop
	TBool found = EFalse;
	for( TInt operation =CSearchOrderTest::EInitial_DeleteAll;
		 operation<=CSearchOrderTest::EFinal_DeleteAll; 
		 operation++)
		{
		sort->MoveHelpFileL( operation );
		model->OpenL();
		// Get the new gategory list, resulting from the repositioned file.
		model->CategoryListL(catList);
		// Close help model to release file handles.
		model->CloseL();
		// Check listed categories given the current index.
		found = sort->CheckCategoryListL( operation, catList );
		if( found )
			{
			TheTest.Printf(_L("The Correct Category was found."));			
			}
		TheTest(found);
		// Remove the old contents, otherwise new 
		// categories are simply added.
		catList->Reset();
		}
	CleanupStack::PopAndDestroy( catList );
	CleanupStack::PopAndDestroy( model );
	CleanupStack::PopAndDestroy( sort );
	__UHEAP_MARKEND;
	}

static void PerformOOMTestL()
	{
	TInt ret=KErrNoMemory;
	TInt failAt=0;	  
	while (ret!=KErrNone)
		{
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, CHlpSearchTest::TestHelpModelLD(ETrue));
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			}
		__UHEAP_RESET;
		TheTest(ret==KErrNoMemory||ret==KErrNone);
		}
	}

static void setupFileServerAndSchedulerL()
//
// Initialise the cleanup stack.
//
	{
	TTime time;
	time.HomeTime();
	TheSeed = time.Int64();

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
	TRAPD(r,
		{
		for (TInt i=KTestCleanupStack;i>0;i--)
			CleanupStack::PushL((TAny*) 0);
		CleanupStack::Pop(KTestCleanupStack);
		});
	TheTest(r==KErrNone);
	}

/**
@SYMTestCaseID PIM-TSEARCH-0001
*/	
GLDEF_C TInt E32Main()
//
// Test Help Model API
//
    {
	__UHEAP_MARK;

	TheTest.Title();
	TheTest.Start(_L("@SYMTestCaseID PIM-TSEARCH-0001"));
	setupCleanup();
	
	__UHEAP_MARK;
	TRAPD(r, 
			setupFileServerAndSchedulerL();
			CHlpSearchTest::TestHelpModelLD();
			);
	
	TheTest(r==KErrNone);
	__UHEAP_MARKEND;
	
	TRAP(r, PerformOOMTestL());
	TheTest(r==KErrNone);
	
	// PlatSec search order test.
	TRAP(r, doSearchOrderTestL());
	TheTest(r==KErrNone);


	delete TheScheduler;
	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
