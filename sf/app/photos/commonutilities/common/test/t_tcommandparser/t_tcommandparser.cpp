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
#include "t_tcommandparser.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxcollectionpath.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmediageneraldefs.h>
#include <glxcommandgeneraldefs.h>
//  INTERNAL INCLUDES
#include "glxcommandparser.h"

_LIT(KTestUri, "https://www.test.uri.nokia.com/thisisapath");
_LIT(KTestAlbumTitle, "New Album");
_LIT(KTitle, "Title");
_LIT(KDriveLetter, "D:");
_LIT(KComment, "This is a comment");
// CONSTRUCTION
t_tcommandparser* t_tcommandparser::NewL()
    {
    t_tcommandparser* self = t_tcommandparser::NewLC();
    CleanupStack::Pop();

    return self;
    }

t_tcommandparser* t_tcommandparser::NewLC()
    {
    t_tcommandparser* self = new( ELeave ) t_tcommandparser();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
t_tcommandparser::~t_tcommandparser()
    {
    }

// Default constructor
t_tcommandparser::t_tcommandparser()
    {
    }

// Second phase construct
void t_tcommandparser::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void t_tcommandparser::SetupL()
    {
    }
    
void t_tcommandparser::Teardown()
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
    
void t_tcommandparser::TestRemove1L()
    {
    iCurrentTest = ETestRemove1;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestRemove2L()
    {
    iCurrentTest = ETestRemove2;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestAdd1L()
    {
    iCurrentTest = ETestAdd1;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestAdd2L()
    {
    iCurrentTest = ETestAdd2;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestAdd3L()
    {
    iCurrentTest = ETestAdd3;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestAdd4L()
    {
    iCurrentTest = ETestAdd4;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestSet1L()
    {
    iCurrentTest = ETestSet1;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestSet2L()
    {
    iCurrentTest = ETestSet2;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::TestSet3L()
    {
    iCurrentTest = ETestSet3;
    T_TCommandParser_ParseLL();
    }

void t_tcommandparser::T_TCommandParser_ParseLL()
    {

	CMPXCommand* command = CommandFactoryLC();

    TUid uid = {0};
    TUint32 sessionId = 0;

    TRAPD(err, TGlxCommandParser::IdsL(uid, sessionId, *command));

    EUNIT_ASSERT_EQUALS_DESC(uid, iUid1, "Uid is correct");
    EUNIT_ASSERT_EQUALS_DESC(sessionId, iUint1, "Session id is correct");

    if(err == KErrNone)
    	{
    	TRAP(err, TGlxCommandParser::ParseL(*this, *command));
    	}


    switch (iCurrentTest)
    {
    case ETestAdd1: // fall through
    case ETestAdd2:
    case ETestAdd3:
    case ETestAdd4:
    case ETestSet1:
    case ETestSet2:
    case ETestSet3:
    case ETestSet5:
    case ETestRemove1:
    case ETestRemove2:
    EUNIT_ASSERT_EQUALS_DESC(err, KErrNone, "ParseL did not leave with an error");
    EUNIT_ASSERT_DESC(iCallbackCalled, "A callback was called");
    break;
    default:
    EUNIT_ASSERT_DESC( EFalse, "Test handled by switch statement in T_TCommandParser_ParseL()");
    break;
    }
    CleanupStack::PopAndDestroy(command);
    }


void t_tcommandparser::RemoveFromContainerL(const RArray<TGlxMediaId>& aItemIds, const TGlxMediaId& aContainerId)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestRemove1, "Current test is ETestRemove");
	
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aItemIds), "Item ID array is consistent with array set on CMPXCommand object");
	EUNIT_ASSERT_EQUALS_DESC(iId1, aContainerId, "Container ID is consistent with ID set on CMPXCommand object");	
	}

void t_tcommandparser::DeleteL(const RArray<TGlxMediaId>& aItemIds)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestRemove2, "Current test is ETestRemove");
	
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aItemIds), "Item ID array is consistent with array set on CMPXCommand object");	
	}

void t_tcommandparser::AddToContainerL(const RArray<TGlxMediaId>& aSourceItemIds, const RArray<TGlxMediaId>& aTargetIds)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd1, "Current test is ETestAdd1");
	
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aSourceItemIds), "aSourceItemIds is consistent with the source array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC( CompareArray(iArray2, aTargetIds), "aTargetIds is consistent with the target array set on the CMPXCommand object");
	}

void t_tcommandparser::AddContainerL(const TDesC& aContainerName)
    {
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd3, "Current test is ETestAdd3");
	EUNIT_ASSERT_DESC(aContainerName.Compare(*iDesc1) == 0, "aContainerName is consistent with the container name set on the CMPXCommand object");
    }

void t_tcommandparser::AddToContainerL(const TDesC& aSourceUri, const RArray<TGlxMediaId>& aTargetIds)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd2, "Current test is ETestAdd2");
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aTargetIds), "aTargetIds is consistent with the target array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC(aSourceUri.Compare(*iDesc1) == 0, "aSourceUri is consistent with the source uri set on the CMPXCommand object");
	}

void t_tcommandparser::CopyL(const RArray<TGlxMediaId>& aSourceItemIds, const TDesC& aDrive)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestAdd4, "Current test is ETestAdd4");
	EUNIT_ASSERT_DESC( CompareArray(iArray1, aSourceItemIds), "aSourceItemIds is consistent with the source array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC(aDrive.Compare(*iDesc1) == 0, "aDrive is consistent with the container name set on the CMPXCommand object");
	}

void t_tcommandparser::RenameL(const TGlxMediaId& aSourceItemId, const TDesC& aTitle)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestSet1, "Current test is ETestSet1");
	TUint id = aSourceItemId.Value();
	EUNIT_ASSERT_EQUALS_DESC( iArray2[0], id, "aSourceItemId is consistent with the first element of the source array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC(aTitle.Compare(*iDesc1) == 0, "aTitle is consistent with the container name set on the CMPXCommand object");
	}

void t_tcommandparser::MoveL(const RArray<TGlxMediaId>& aSourceItemIds, const TDesC& aDrive)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestSet2, "Current test is ETestSet2");
	EUNIT_ASSERT_DESC( CompareArray(iArray2, aSourceItemIds), "aSourceItemIds is consistent with the source array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC(aDrive.Compare(*iDesc1) == 0, "aDrive is consistent with the container name set on the CMPXCommand object");
	}

void t_tcommandparser::SetDescriptionL(const RArray<TGlxMediaId>& aSourceItemIds, const TDesC& aDescription)
	{
	iCallbackCalled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestSet3, "Current test is ETestSet3");
	EUNIT_ASSERT_DESC( CompareArray(iArray2, aSourceItemIds), "aSourceItemIds is consistent with the source array set on the CMPXCommand object");
	EUNIT_ASSERT_DESC(aDescription.Compare(*iDesc1) == 0, "aDescription is consistent with the container name set on the CMPXCommand object");
	}

void t_tcommandparser::SetCaptureLocationL(const RArray<TGlxMediaId>& aItemIds, const TCoordinate& aCoordinate)
    {
    iCallbackCalled = ETrue;
    EUNIT_ASSERT_EQUALS_DESC(iCurrentTest, ETestSet5, "Current test is ETestSet5");
    EUNIT_ASSERT_DESC( CompareArray(iArray2, aItemIds), "aItemIds is consistent with the source array set on the CMPXCommand object");
    EUNIT_ASSERT_EQUALS_DESC(iCoordinate.Latitude(), aCoordinate.Latitude(), "latitude is consistent with the latitude set on the coordinate set on the CMPXCommand object");
    EUNIT_ASSERT_EQUALS_DESC(iCoordinate.Longitude(), aCoordinate.Longitude(), "longitude is consistent with the longitude set on the coordinate set on the CMPXCommand object");
    }

CMPXCommand* t_tcommandparser::CommandFactoryLC()
{
	CMPXCommand* command = CMPXCommand::NewL();
	CleanupStack::PushL(command);

	// All commands have a Collection UID
	iUid1 = TUid::Uid(0x9ABCDEF);
	command->SetTObjectValueL<TUid>(KMPXCommandGeneralCollectionId, iUid1);
	
	// All commands have a session ID
	iUint1 = 123;
	command->SetTObjectValueL<TMPXItemId>(KMPXCommandGeneralSessionId, iUint1);
	
	switch (iCurrentTest)
	{
	case ETestRemove1:
		{
		command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdRemove);
		PopulateArrayL(iArray1);
		
		CMPXCollectionPath * path = PathFactoryLC(iArray1);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path);
		CleanupStack::PopAndDestroy(path);
		
		iId1 = TGlxMediaId(456);
		TMPXItemId id = TMPXItemId(iId1.Value());
		command->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralContainerId, id);	
		}
	break;
	case ETestRemove2:
		{
		command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdRemove);
		PopulateArrayL(iArray1);
		
		CMPXCollectionPath * path = PathFactoryLC(iArray1);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path);
		CleanupStack::PopAndDestroy(path);
		
		command->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralContainerId, KGlxContainerDevice);	
		}
	break;
	case ETestAdd1:
		{
		command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdAdd);
		
		PopulateArrayL(iArray1);
		CMPXCollectionPath * path1 = PathFactoryLC(iArray1);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path1);
		CleanupStack::PopAndDestroy(path1);

		PopulateArrayAltL(iArray2);
		CMPXCollectionPath * path2 = PathFactoryLC(iArray2);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXMediaGeneralContainerId,path2);
		CleanupStack::PopAndDestroy(path2);		
		}
	break;
	case ETestAdd2:
        {
    	command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdAdd);
    	
		PopulateArrayAltL(iArray1);
		CMPXCollectionPath * path = PathFactoryLC(iArray1);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXMediaGeneralContainerId,path);
		CleanupStack::PopAndDestroy(path);	
		
		iDesc1 = KTestUri().AllocL();
		command->SetTextValueL(KMPXCommandGeneralSourceUri,*iDesc1); 
        }
    break;
	case ETestAdd3:
	    {
	    command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdAdd);
        iDesc1 = KTestAlbumTitle().AllocL();
    	command->SetTextValueL(KMPXMediaGeneralTitle,*iDesc1);     
	    }
	break;
	case ETestAdd4:
        {
        command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdAdd);
        iDesc1 = KDriveLetter().AllocL();
	    command->SetTextValueL(KMPXMediaGeneralDrive,*iDesc1);     
        
		PopulateArrayAltL(iArray1);
		CMPXCollectionPath * path1 = PathFactoryLC(iArray1);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path1);
		CleanupStack::PopAndDestroy(path1);
        
        }
    break;
	case ETestSet1:
	    {
	    command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdSet);
		
	    PopulateArrayL(iArray2);
		CMPXCollectionPath * path1 = PathFactoryLC(iArray2);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path1);
		CleanupStack::PopAndDestroy(path1);
		
		
        iDesc1 = KTitle().AllocL();
	    command->SetTextValueL(KMPXMediaGeneralTitle,*iDesc1);  
	    }
	break;
	case ETestSet2:
	    {
		command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdSet);
			
		PopulateArrayL(iArray2);
		CMPXCollectionPath * path1 = PathFactoryLC(iArray2);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path1);
		CleanupStack::PopAndDestroy(path1);
		
        iDesc1 = KDriveLetter().AllocL();
	    command->SetTextValueL(KMPXMediaGeneralDrive,*iDesc1);     
	    }
	break;
	case ETestSet3:
	    {
	    command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdSet);
			
		PopulateArrayL(iArray2);
		CMPXCollectionPath * path1 = PathFactoryLC(iArray2);
		command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path1);
		CleanupStack::PopAndDestroy(path1);
	    	
		iDesc1 = KComment().AllocL();
		command->SetTextValueL(KMPXMediaGeneralComment,*iDesc1);
	    }
	break;

    case ETestSet5:
        {
        command->SetTObjectValueL<TUint32>(KMPXCommandGeneralId, KGlxCommandIdSet);
            
        PopulateArrayL(iArray2);
        CMPXCollectionPath * path1 = PathFactoryLC(iArray2);
        command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds,path1);
        CleanupStack::PopAndDestroy(path1);
        
        iCoordinate.SetCoordinate(52.794614, -1.639882);
        command->SetTObjectValueL<const TCoordinate>(KGlxMediaGeneralLocation, iCoordinate);    
        }
    break;
	default:
		break;
	};
	return command;
}

CMPXCollectionPath* t_tcommandparser::PathFactoryLC(const RArray<TGlxMediaId>& aArray)
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

void t_tcommandparser::PopulateArrayL(RArray<TGlxMediaId>& aArray)
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

void t_tcommandparser::PopulateArrayAltL(RArray<TGlxMediaId>& aArray)
{
	aArray.Reset();
	for (TInt i = 0; i < 10001; i++)
	{
		aArray.AppendL(TGlxMediaId(i*i));
	}
}

TBool t_tcommandparser::CompareArray(const RArray<TGlxMediaId>& aArray1, const RArray<TGlxMediaId>& aArray2)
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

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_tcommandparser,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
     "Test Add1",
     "TGlxCommandParser",
     "Tests Add",
	 "FUNCTIONALITY",
	    SetupL, TestAdd1L, Teardown)

EUNIT_TEST(
	 "Test Add2",
	 "TGlxCommandParser",
	 "Tests Add",
	 "FUNCTIONALITY",
	  SetupL, TestAdd2L, Teardown)

EUNIT_TEST(
     "Test Add3",
     "TGlxCommandParser",
     "Tests Add",
     "FUNCTIONALITY",
		  SetupL, TestAdd3L, Teardown)
		  
EUNIT_TEST(
     "Test Add4",
     "TGlxCommandParser",
     "Tests Add",
     "FUNCTIONALITY",
	 SetupL, TestAdd4L, Teardown)
	    
EUNIT_TEST(
	"Test Set1",
    "TGlxCommandParser",
    "Tests Set",
    "FUNCTIONALITY",
    SetupL, TestSet1L, Teardown)

EUNIT_TEST(
     "Test Set2",
	 "TGlxCommandParser",
	 "Tests Set",
	 "FUNCTIONALITY",
	 SetupL, TestSet2L, Teardown)
	 
EUNIT_TEST(
	 "Test Set3",
	 "TGlxCommandParser",
	 "Tests Set",
	 "FUNCTIONALITY",
	 SetupL, TestSet3L, Teardown)
EUNIT_TEST(
	"Test Remove",
    "TGlxCommandParser",
    "Tests Remove",
    "FUNCTIONALITY",
    SetupL, TestRemove1L, Teardown)

EUNIT_TEST(
	"Test Remove",
	"TGlxCommandParser",
	"Tests Remove",
	"FUNCTIONALITY",
	 SetupL, TestRemove2L, Teardown)
	    
    
EUNIT_END_TEST_TABLE

//  END OF FILE
