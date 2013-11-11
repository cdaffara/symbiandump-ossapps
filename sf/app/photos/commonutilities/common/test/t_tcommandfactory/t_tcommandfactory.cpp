/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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



//  CLASS HEADER
#include "t_tcommandfactory.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxcollectionpath.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmediageneraldefs.h>
#include <glxcommandgeneraldefs.h>
#include "glxcommandfactory.h"
#include <e32def.h>
//  INTERNAL INCLUDES
#include "glxcommandparser.h"

_LIT(KTitle, "Title");
_LIT(KDriveLetter, "D:");

/**
 * 
 * Expo + 15300 -EUserParasite_eka2.dll! 1000ea94()
 * RArrayBase::Grow(void) <F:\src\cedar\generic\base\e32\common\array.cpp #667> + 24
 * RArrayBase::Append(void const *) <F:\src\cedar\generic\base\e32\common\array.cpp #680> + 8
 * RArray<class TMPXItemId>::Append(class TMPXItemId const &) <X:\epoc32\include\e32cmn.inl #4957> + 28
 * RArray<class TMPXItemId>::AppendL(class TMPXItemId const &) <X:\epoc32\include\e32std.inl #6044> + 28
 * CMPXCollectionPath::SelectionL(class RArray<class TMPXItemId> &) <X:\mpx_wk22\commonframework\common\src\mpxcollectionpath.cpp #583>
 * __InitializeThreadDataIndex <D:\Products\Layout\Symbian_MSLRuntime_b0022\Symbian_Support\MSL\MSL_C\MSL_Win32\Src\ThreadLocalData.c #49> + 2
 * __CxxThrowException@8 <D:\Products\Layout\Symbian_MSLRuntime_b0022\Symbian_Support\Runtime\Runtime_x86\Runtime_Win32\(Source)\excrtl.cpp #1200> + 1
 * ContentAccess::CF32AgentManager::GetStringAttributeSet(class ContentAccess::RStringAttributeSet &,class ContentAccess::TVirtualPathPtr const &) <M:\src\common\generic\syslibs\CAF2\source\f32agent\f32agentmanager.cpp #168> + 2
 * RSubSessionBase::RSubSessionBase(void) <M:\epoc32\include\e32std.inl #4069> + 3
 * CEUnitTestSuiteClass::TTestCase::RunTestL(void) <C:\nightly_build\3_1\s60_30\EUnit\EUnitImplementation\CORE\src\CEUnitTestSuiteClass.cpp #205>
 * TEUnitTestCaseExtension::`vector deleting destructor'(unsigned int) <C:\nightly_build\3_1\s60_30\EUnit\EUnitImplementation\CORE\src\MEUnitTestCase.cpp #101>
 * MEUnitTestCase::Run(class MEUnitTestResult &) <C:\nightly_build\3_1\s60_30\EUnit\EUnitImplementation\CORE\src\MEUnitTestCase.cpp #293> + 67
 * ___msl_close <D:\Products\Layout\Symbian_MSLRuntime_b0018\Symbian_Support\MSL\MSL_C\MSL_Win32\Src\file_io.win32.c #293> + 2
 * ___msl_write <D:\Products\Layout\Symbian_MSLRuntime_b0018\Symbian_Support\MSL\MSL_C\MSL_Win32\Src\file_io.win32.c #220> + 6
 * ___sys_alloc <D:\Products\Layout\Symbian_MSLRuntime_b0018\Symbian_Support\MSL\MSL_C\MSL_Win32\Src\pool_alloc.win32.c #44> + 5
 * MEUnitTestCase::Accept(class MEUnitTestVisitor &) <C:\nightly_build\3_1\s60_30\EUnit\EUnitImplementation\CORE\src\MEUnitTestCase.cpp #274>
 * __ftell <D:\Products\Layout\Symbian_MSLRuntime_b0018\Symbian_Support\MSL\MSL_C\MSL_Common\Src\file_pos.c #36> + 18
 * CEUnitTestDll::Accept(class MEUnitTestVisitor &) <C:\nightly_build\3_1\s60_30\EUnit\EUnitImplementation\CORE\src\CEUnitTestDll.cpp #63>
 * ___flush_all <D:\Products\Layout\Symbian_MSLRuntime_b0018\Symbian_Support\MSL\MSL_C\MSL_Common\Src\ansi_files.c #358> + 1 
 */

// CONSTRUCTION
t_tcommandfactory* t_tcommandfactory::NewL()
    {
    t_tcommandfactory* self = t_tcommandfactory::NewLC();
    CleanupStack::Pop();

    return self;
    }

t_tcommandfactory* t_tcommandfactory::NewLC()
    {
    t_tcommandfactory* self = new( ELeave ) t_tcommandfactory();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
t_tcommandfactory::~t_tcommandfactory()
    {
    }

// Default constructor
t_tcommandfactory::t_tcommandfactory()
    {
    }

// Second phase construct
void t_tcommandfactory::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void t_tcommandfactory::SetupL()
    {
    }
    
void t_tcommandfactory::Teardown()
    {
    iArray1.Reset();
    iArray2.Reset();
    iId1 = TGlxMediaId(0);
    iUid1 = TUid::Uid(0);
    iUint1 = 0;
    delete iDesc1;
    iDesc1 = NULL;
    iCallbackCalled  = EFalse;
    iCurrentTest = ENoTest;
    }
    
void t_tcommandfactory::TestRemoveL()
    {
    iCurrentTest = ETestRemove;
    T_TCommandParser_ParseLL();
    }

void t_tcommandfactory::TestSet4L()
    {
    iCurrentTest = ETestSet4;
    T_TCommandParser_ParseLL();
    }

void t_tcommandfactory::TestSet5L()
    {
    iCurrentTest = ETestSet5;
    T_TCommandParser_ParseLL();
    }

void t_tcommandfactory::TestAdd1L()
    {
    iCurrentTest = ETestAdd1;
    T_TCommandParser_ParseLL();
    }

void t_tcommandfactory::TestAdd2L()
    {
    iCurrentTest = ETestAdd2;
    T_TCommandParser_ParseLL();
    }

void t_tcommandfactory::TestAdd3L()
    {
    iCurrentTest = ETestAdd3;
    T_TCommandParser_ParseLL();
    }

void t_tcommandfactory::TestAdd4L()
    {
    iCurrentTest = ETestAdd4;
    T_TCommandParser_ParseLL();
    }

void t_tcommandfactory::TestThumbnailCleanupL()
	{
	iCurrentTest = ETestThumbnailCleanup;
	CMPXCommand* command = TGlxCommandFactory::ThumbnailCleanupCommandLC();
	
	
    TRAPD(err, TGlxCommandParser::ParseL(*this, *command));
    EUNIT_ASSERT_EQUALS_DESC(err, KErrNone, "ParseL did not leave with an error");
    EUNIT_ASSERT_DESC(iCallbackCalled, "A callback was called");
    
    CleanupStack::PopAndDestroy(command);
	}

void t_tcommandfactory::T_TCommandParser_ParseLL()
    {

	CMPXCommand* command = NULL;
	switch(iCurrentTest)
	{
	case ETestAdd1:
		{
		iUid1 = TUid::Uid(0xAAA);
		
		iId1 = TGlxMediaId(0xBBB);
		
		PopulateArrayL(iArray1);
		PopulateArrayAltL(iArray2);
		
		CMPXCollectionPath* sourcePath = CMPXCollectionPath::NewL();
		CleanupStack::PushL(sourcePath);
		
		sourcePath->AppendL(iUid1.iUid); // The first level of the path is the collection plugin's uid.
		sourcePath->AppendL(iId1.Value()); // The second level of the path is the container that we are using.
		
		RArray<TMPXItemId> array;
		ConvertArrayLC(array, iArray1);
		sourcePath->AppendL(array.Array()); // The array of items contained by the container.
		CleanupStack::PopAndDestroy(&array);
		TInt pathCount = sourcePath->Count();
		for (TInt i = 0; i < pathCount; i++)
			{
			sourcePath->SelectL(i); // select 
			}
		
		CMPXCollectionPath* targetPath = CMPXCollectionPath::NewL();
		CleanupStack::PushL(targetPath);
		ConvertArrayLC(array, iArray2);
		targetPath->AppendL(array.Array());
		CleanupStack::PopAndDestroy(&array);
		pathCount = targetPath->Count();
			{
			for (TInt i = 0; i < pathCount; i++)
				{
				targetPath->SelectL(i); // select 
				}
			}
		
		command = TGlxCommandFactory::AddToContainerCommandLC(*sourcePath,*targetPath);
		CleanupStack::Pop(command);
		CleanupStack::PopAndDestroy(targetPath);
		CleanupStack::PopAndDestroy(sourcePath);	
		}
	break;
	case ETestAdd2:
		{
		iUid1 = TUid::Uid(0xABBA);
		
		iId1 = TGlxMediaId(0xBAAB);
		
		PopulateArrayAltL(iArray2);
				
		CMPXCollectionPath* targetPath = CMPXCollectionPath::NewL();
		CleanupStack::PushL(targetPath);
		
		targetPath->AppendL(iUid1.iUid); // The first level of the path is the collection plugin's uid.	
		
		RArray<TMPXItemId> array;
		ConvertArrayLC(array, iArray2);
		targetPath->AppendL(array.Array()); // The array of containers to add to
		CleanupStack::PopAndDestroy(&array);
		TInt pathCount = targetPath->Count();
			{
			for (TInt i = 0; i < pathCount; i++)
				{
				targetPath->SelectL(i); // select 
				}
			}
		
	    iDesc1 = KTitle().AllocL();
	    
		command = TGlxCommandFactory::AddToContainerCommandLC(*iDesc1,*targetPath);
		CleanupStack::Pop(command);
		CleanupStack::PopAndDestroy(targetPath);   
		}
	break;
	case ETestAdd3:
		{
		iUid1 = TUid::Uid(0xABBA);
		iDesc1 = KTitle().AllocL();
		
         command = TGlxCommandFactory::AddContainerCommandLC(*iDesc1, iUid1.iUid);
         CleanupStack::Pop(command);
		 }
	break;
	case ETestAdd4:
		{
		iUid1 = TUid::Uid(0xAAA);
		
		iId1 = TGlxMediaId(0xBBB);
		
		PopulateArrayL(iArray1);
		
		CMPXCollectionPath* sourcePath = CMPXCollectionPath::NewL();
		CleanupStack::PushL(sourcePath);
		
		sourcePath->AppendL(iUid1.iUid); // The first level of the path is the collection plugin's uid.
		sourcePath->AppendL(iId1.Value()); // The second level of the path is the container that we are using.
		
		RArray<TMPXItemId> array;
		ConvertArrayLC(array, iArray1);
		sourcePath->AppendL(array.Array()); // The array of items contained by the container.
		CleanupStack::PopAndDestroy(&array);
		TInt pathCount = sourcePath->Count();
		for (TInt i = 0; i < pathCount; i++)
			{
			sourcePath->SelectL(i); // select 
			}
		
		iDesc1 = KDriveLetter().AllocL();
		command = TGlxCommandFactory::CopyCommandLC(*iDesc1, *sourcePath);
		CleanupStack::Pop(command);
		CleanupStack::PopAndDestroy(sourcePath);
		}
	break;
	case ETestRemove: 
		{
		PopulateArrayL(iArray1);
		
		iUid1 = TUid::Uid(0x9ABCDEF);
		
		iId1 = TGlxMediaId(0xABCDEF);
		
		CMPXCollectionPath* path = CMPXCollectionPath::NewL();
		CleanupStack::PushL(path);
		
		path->AppendL(iUid1.iUid); // The first level of the path is the collection plugin's uid.
		path->AppendL(iId1.Value()); // The second level of the path is the container that we are using.
		RArray<TMPXItemId> array;
		ConvertArrayLC(array, iArray1);
		path->AppendL(array.Array()); // The array of items contained by the container.
		CleanupStack::PopAndDestroy(&array);
		TInt pathCount = path->Count();
		for (TInt i = 0; i < pathCount; i++)
			{
			path->SelectL(i); // select 
			}
		command = TGlxCommandFactory::RemoveFromContainerCommandLC(*path);
		CleanupStack::Pop(command);
		CleanupStack::PopAndDestroy(path);
		}
    break;
    case ETestSet5:
        {
        PopulateArrayL(iArray2);
        CMPXCollectionPath* path = PathFactoryLC(iArray2);      
        command = TGlxCommandFactory::DeleteLocationCommandLC(*path);
        CleanupStack::Pop(command);
        CleanupStack::PopAndDestroy(path);
        }
    break;
    default:
    break;
	}
	
    CleanupStack::PushL(command);
    
    TUid uid = {0};
    TUint32 sessionId = 0;

    TRAPD(err, TGlxCommandParser::IdsL(uid, sessionId, *command));

    EUNIT_ASSERT_EQUALS_DESC(uid, iUid1, "Uid is correct");
    
    if(err == KErrNone)
    	{
    	TRAP(err, TGlxCommandParser::ParseL(*this, *command));
    	}

    switch (iCurrentTest)
	    {
	    case ETestAdd1:
	    case ETestAdd2:
	    case ETestAdd3:
	    case ETestAdd4:
	    case ETestRemove:
        case ETestSet4:
        case ETestSet5:
		    EUNIT_ASSERT_EQUALS_DESC(err, KErrNone, "ParseL did not leave with an error");
		    EUNIT_ASSERT_DESC(iCallbackCalled, "A callback was called");
	    break;
	    default:
	    	EUNIT_ASSERT_DESC( EFalse, "Test handled by switch statement in T_TCommandParser_ParseL()");
	    break;
	    }
    CleanupStack::PopAndDestroy(command);
    }


void t_tcommandfactory::RemoveFromContainerL(const RArray<TGlxMediaId>& aItemIds, const TGlxMediaId& aContainerId)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestRemove, "Current test is ETestRemove");
	
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aItemIds), "Item ID array is consistent with array set on CMPXCommand object");
	EUNIT_ASSERT_EQUALS_DESC(iId1, aContainerId, "Container ID is consistent with ID set on CMPXCommand object");
	}

void t_tcommandfactory::DeleteL(const RArray<TGlxMediaId>& aItemIds)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestRemove, "Current test is ETestRemove");
	
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aItemIds), "Item ID array is consistent with array set on CMPXCommand object");	
	}

void t_tcommandfactory::AddToContainerL(const RArray<TGlxMediaId>& aSourceItemIds, const RArray<TGlxMediaId>& aTargetIds)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd1, "Current test is ETestAdd1");
	
	
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aSourceItemIds), "aSourceItemIds is consistent with the source array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC( CompareArray(iArray2, aTargetIds), "aTargetIds is consistent with the target array set on the CMPXCommand object");
	}

void t_tcommandfactory::AddContainerL(const TDesC& aContainerName)
    {
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd3, "Current test is ETestAdd3");
	EUNIT_ASSERT_DESC(aContainerName.Compare(*iDesc1) == 0, "aContainerName is consistent with the container name set on the CMPXCommand object");
    }

void t_tcommandfactory::AddToContainerL(const TDesC& aSourceUri, const RArray<TGlxMediaId>& aTargetIds)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd2, "Current test is ETestAdd2");
	EUNIT_ASSERT_DESC(CompareArray(iArray2, aTargetIds), "aTargetIds is consistent with the target array set on the CMPXCommand object");
	
	EUNIT_ASSERT_DESC(aSourceUri.Compare(*iDesc1) == 0, "aSourceUri is consistent with the source uri set on the CMPXCommand object");
	}

void t_tcommandfactory::CopyL(const RArray<TGlxMediaId>& aSourceItemIds, const TDesC& aDrive)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd4, "Current test is ETestAdd4");
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aSourceItemIds), "aSourceItemIds is consistent with the source array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC(aDrive.Compare(*iDesc1) == 0, "aDrive is consistent with the container name set on the CMPXCommand object");
	}

void t_tcommandfactory::RenameL(const TGlxMediaId& /*aSourceItemId*/, const TDesC& /*aTitle*/)
	{
	EUNIT_ASSERT_DESC(0, "Wrong call back");
	}

void t_tcommandfactory::MoveL(const RArray<TGlxMediaId>& /*aSourceItemIds*/, const TDesC& /*aDrive*/)
	{
	EUNIT_ASSERT_DESC(0, "Wrong call back");
	}

void t_tcommandfactory::SetDescriptionL(const RArray<TGlxMediaId>& /*aSourceItemIds*/, const TDesC& /*aDescription*/)
	{
	EUNIT_ASSERT_DESC(0, "Wrong call back");
	}


void t_tcommandfactory::SetCaptureLocationL(const RArray<TGlxMediaId>& aItemIds, const TCoordinate& /*aCoordinate*/)
    {
    iCallbackCalled = ETrue;
    EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestSet5, "Current test is ETestSet5");
    EUNIT_ASSERT_DESC( CompareArray(iArray2, aItemIds), "aItemIds is consistent with the source array set on the CMPXCommand object");
    }

void t_tcommandfactory::ThumbnailCleanupL()
	{
	iCallbackCalled = ETrue;
    EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestThumbnailCleanup, "Current test is ETestThumbnailCleanup");
	}

CMPXCollectionPath* t_tcommandfactory::PathFactoryLC(const RArray<TGlxMediaId>& aArray)
	{
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL(path);
	
	RArray<TMPXItemId> ids;
	CleanupClosePushL(ids);
	
	for (TInt i = 0; i < aArray.Count(); i++)
		{
		ids.AppendL(TMPXItemId(aArray[i].Value()));
		}
	path->AppendL(ids.Array());
	
	CleanupStack::PopAndDestroy(&ids);
	
	TInt pathCount = path->Count();
	for (TInt i = 0; i < pathCount; i++)
		{
		path->SelectL(i); // select 
		}
	return path;
	}

void t_tcommandfactory::PopulateArrayL(RArray<TGlxMediaId>& aArray)
	{
	aArray.Reset();
	TUint a = 0;
	TUint b = 1;
	aArray.AppendL(TGlxMediaId(a));
	aArray.AppendL(TGlxMediaId(b));
	for (TInt i = 0; i < 9998; i++)
		{
		TUint tmp = a + b;
		a = b;
		b = tmp;
		aArray.AppendL(TGlxMediaId(tmp));
		}
	}

void t_tcommandfactory::PopulateArrayAltL(RArray<TGlxMediaId>& aArray)
	{
	aArray.Reset();
	for (TInt i = 0; i < 10001; i++)
		{
		aArray.AppendL(TGlxMediaId(i*i));
		}
	}

TBool t_tcommandfactory::CompareArray(const RArray<TGlxMediaId>& aArray1, const RArray<TGlxMediaId>& aArray2)
	{
	TInt count1 = aArray1.Count();
	TInt count2 = aArray2.Count();
	
	if (count1 != count2)
		{
		return EFalse;
		}
	
	for (TInt i = 0; i < count1; i++)
		{
		if (aArray1[i] != aArray2[i])
			{
			return EFalse;
			}
		}
	return ETrue;
	}

void t_tcommandfactory::ConvertArrayLC(RArray<TMPXItemId>& aDest, const RArray<TGlxMediaId>& aSource)
	{
	CleanupClosePushL(aDest);
	
	TInt count = aSource.Count();
	
	for (TInt i = 0; i < count; i++)
		{
		aDest.AppendL(aSource[i].Value());
		}
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_tcommandfactory,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
	"Test Add1",
	"TGlxCommandFactory",
	"Tests Add",
	"FUNCTIONALITY",
	SetupL, TestAdd1L, Teardown)

EUNIT_TEST(
	"Test Add2",
	"TGlxCommandFactory",
	"Tests Add",
	"FUNCTIONALITY",
	SetupL, TestAdd2L, Teardown)

EUNIT_TEST(
    "Test Add3",
    "TGlxCommandFactory",
    "Tests Add",
    "FUNCTIONALITY",
    SetupL, TestAdd3L, Teardown)
			  
EUNIT_TEST(
    "Test Add4",
    "TGlxCommandFactory",
    "Tests Add",
    "FUNCTIONALITY",
    SetupL, TestAdd4L, Teardown)
		 
EUNIT_TEST(
	"Test Remove",
    "TGlxCommandFactory",
    "Tests Remove",
    "FUNCTIONALITY",
    SetupL, TestRemoveL, Teardown)

EUNIT_TEST(
    "Test Set4",
    "TGlxCommandFactory",
    "Tests Set",
    "FUNCTIONALITY",
    SetupL, TestSet4L, Teardown)
        
EUNIT_TEST(
    "Test Set5",
    "TGlxCommandFactory",
    "Tests Set",
    "FUNCTIONALITY",
     SetupL, TestSet5L, Teardown)
     
EUNIT_TEST(
	"Test Thumbnail Cleanup",
	"TGlxCommandFactory",
	"Tests Thumbnail Cleanup",
	"FUNCTIONALITY",
    SetupL, TestThumbnailCleanupL, Teardown)

     
    
EUNIT_END_TEST_TABLE

//  END OF FILE
