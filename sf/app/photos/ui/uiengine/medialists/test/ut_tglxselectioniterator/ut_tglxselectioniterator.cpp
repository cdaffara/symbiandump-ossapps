/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   TGlxFromFocusOutwardIterator unit tests
*
*/



#include <EUnitMacros.h>

#include "ut_tglxselectioniterator.h"

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
T_CGlxSelectionIterator* T_CGlxSelectionIterator::NewL()
    {
    T_CGlxSelectionIterator* self = T_CGlxSelectionIterator::NewLC();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//
T_CGlxSelectionIterator* T_CGlxSelectionIterator::NewLC()
    {
    T_CGlxSelectionIterator* self = new( ELeave ) T_CGlxSelectionIterator();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
T_CGlxSelectionIterator::~T_CGlxSelectionIterator()
    {
    // Don't do anything
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
T_CGlxSelectionIterator::T_CGlxSelectionIterator()
: iList(NULL)
	{
	// Don't do anything
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
void T_CGlxSelectionIterator::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// T_CGlxSelectionIterator::SetupL()
// ----------------------------------------------------------------------------
//
void T_CGlxSelectionIterator::SetupL()
    {
    iVirtualIterator = &iConcreateIterator; 
    }

// ----------------------------------------------------------------------------
// T_CGlxSelectionIterator::Teardown()
// ----------------------------------------------------------------------------
//
void T_CGlxSelectionIterator::Teardown()
    {
    // Don't do anything
    } 

// ----------------------------------------------------------------------------
// T_CGlxSelectionIterator::UT_TestOperatorPlusPlusL()
// ----------------------------------------------------------------------------
//
void T_CGlxSelectionIterator::UT_TestOperatorPlusPlusL()
    {
    
    RArray<TInt> selection;
    CleanupClosePushL(selection);
    
    selection.AppendL(0);
    selection.AppendL(1);
    selection.AppendL(2);
    
    Test(3, EFalse, 3, 0, selection, "012e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    Test(3, ETrue, 3, 0, selection, "012e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    
    Test(2, EFalse, 3, 0, selection, "01e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    Test(2, ETrue, 3, 0, selection, "e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    
    selection.Reset();
    
    selection.AppendL(0);
    selection.AppendL(3);
    selection.AppendL(4);
    selection.AppendL(7);
    selection.AppendL(9);
    
    Test(13, EFalse, 14, 0, selection, "03479e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    Test(13, ETrue, 14, 0, selection, "03479e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
       
    selection.Reset();
    
    selection.AppendL(5);
    selection.AppendL(9);
    
    Test(13, EFalse, 11, 0, selection, "59e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    Test(1, EFalse, 11, 0, selection, "5e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    Test(1, ETrue, 11, 0, selection, "e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    
    selection.Reset();
    
    // Focus no selection
    Test(13, EFalse, 10, 4, selection, "4e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result

    Test(13, ETrue, 10, 4, selection, "4e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    Test(1, ETrue, 10, 4, selection, "4e"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
       
    CleanupStack::PopAndDestroy(&selection);
    }

// ----------------------------------------------------------------------------
// T_CGlxSelectionIterator::UT_TestInRangeL()
// ----------------------------------------------------------------------------
//
void T_CGlxSelectionIterator::UT_TestInRangeL()
	{
    RArray<TInt> selection;
    CleanupClosePushL(selection);
    
    selection.AppendL(0);
    selection.AppendL(1);
    selection.AppendL(2);
    
    TestSingleInRange(3, EFalse, 3, 0, selection, "xxx"); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    TestSingleInRange(4, EFalse, 4, 0, selection, "xxx ");
    TestSingleInRange(4, EFalse, 3, 0, selection, "xxx");
    TestSingleInRange(4, ETrue,  3, 0, selection, "xxx");
    TestSingleInRange(2, EFalse, 4, 0, selection, "xx  ");
    TestSingleInRange(2, ETrue,  4, 0, selection, "    ");
  
    selection.Reset();
 
    selection.AppendL(2);
    selection.AppendL(4);
 
    TestSingleInRange(3, EFalse, 6, 0, selection, "  x x "); // range, disable_if_more_than_range_selected, list count, focus index, selection, result
    TestSingleInRange(4, EFalse, 5, 0, selection, "  x x");
    TestSingleInRange(4, ETrue,  6, 0, selection, "  x x ");
    TestSingleInRange(2, EFalse, 6, 0, selection, "  x x ");
    TestSingleInRange(2, ETrue,  6, 0, selection, "  x x ");
    TestSingleInRange(1, EFalse, 6, 0, selection, "  x   ");
    TestSingleInRange(1, ETrue,  6, 0, selection, "      ");
    
    CleanupStack::PopAndDestroy(&selection);
	}

// ----------------------------------------------------------------------------
// T_CGlxSelectionIterator::Test
// ----------------------------------------------------------------------------
//
void T_CGlxSelectionIterator::Test(TInt aRange, TBool aDisabledIfMoreThanRangeSelected, 
		  TInt aListCount, TInt aFocusIndex, RArray<TInt>& aSelection,  const char* aResults)
	{

    __ASSERT_DEBUG(aFocusIndex >= 0 && aFocusIndex < aListCount && aListCount >= 0, Panic(EGlxPanicIllegalArgument)); // Make sure is testing all cases, also failed case

    iConcreateIterator.SetRange(aRange);
    iConcreateIterator.SetDisabledIfMoreThanRangeSelected(aDisabledIfMoreThanRangeSelected);
    iVirtualIterator->SetToFirst(&iList); 
    iList.iCount = aListCount; 
    iList.iFocus = aFocusIndex;
    iList.iSelection = aSelection;
    TInt resultCount = User::StringLength(reinterpret_cast<const TUint8*>(aResults));
    	  
    for (TInt i = 0; i < resultCount; i++)
    	{
    	TInt result = (*iVirtualIterator)++;
    	char c = aResults[i];
    	TInt expectedResult = c - '0';
    	if (c == 'e') 
    		{
    		expectedResult = KErrNotFound;
    		}
    	EUNIT_ASSERT(result == expectedResult);
    	}
	}
	
// ----------------------------------------------------------------------------
// T_CGlxSelectionIterator::TestSingleInRange
// ----------------------------------------------------------------------------
//
void T_CGlxSelectionIterator::TestSingleInRange(
		TInt aRange, TBool aDisabledIfMoreThanRangeSelected, TInt aListCount, 
		       TInt aFocusIndex, RArray<TInt>& aSelection, const char* aResult)
	{
	__ASSERT_DEBUG(aFocusIndex >= 0 && aFocusIndex < aListCount && aListCount >= 0, Panic(EGlxPanicIllegalArgument)); // Make sure is testing all cases, also failed case
    iConcreateIterator.SetRange(aRange);
    iConcreateIterator.SetDisabledIfMoreThanRangeSelected(aDisabledIfMoreThanRangeSelected);
    iVirtualIterator->SetToFirst(&iList); 
    iList.iCount = aListCount; 
    iList.iFocus = aFocusIndex;
    iList.iSelection = aSelection;

    
    for (TInt i = 0; i < aListCount; i++)
    	{
    	char c = aResult[i];
     	__ASSERT_DEBUG(c != 0, Panic(EGlxPanicIllegalArgument)); // Not enough results
   	
    	TBool inRange = iVirtualIterator->InRange(i);
    	EUNIT_ASSERT((inRange && c == 'x') || (!inRange && c == ' '));
    	}

	}
		

	
// ----------------------------------------------------------------------------
// Test table
// ----------------------------------------------------------------------------
//

EUNIT_BEGIN_TEST_TABLE(
    T_CGlxSelectionIterator,
    "Unit tests for T_CGlxSelectionIterator",
    "UNIT" )
 
EUNIT_TEST(
    "Test operator++",
    "TGlxSelectionIterator",
    "++",
    "FUNCTIONALITY",
    SetupL, UT_TestOperatorPlusPlusL, Teardown)
    
EUNIT_TEST(
    "Test InRange",
    "TGlxSelectionIterator",
    "++",
    "FUNCTIONALITY",
    SetupL, UT_TestInRangeL, Teardown)
    
EUNIT_END_TEST_TABLE
//  END OF FILE
