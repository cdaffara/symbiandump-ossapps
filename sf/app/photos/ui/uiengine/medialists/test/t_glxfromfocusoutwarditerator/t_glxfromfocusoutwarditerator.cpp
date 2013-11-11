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

#include "t_glxfromfocusoutwarditerator.h"

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
T_CGlxFromFocusOutwardIterator* T_CGlxFromFocusOutwardIterator::NewL()
    {
    T_CGlxFromFocusOutwardIterator* self = T_CGlxFromFocusOutwardIterator::NewLC();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//
T_CGlxFromFocusOutwardIterator* T_CGlxFromFocusOutwardIterator::NewLC()
    {
    T_CGlxFromFocusOutwardIterator* self = new( ELeave ) T_CGlxFromFocusOutwardIterator();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
T_CGlxFromFocusOutwardIterator::~T_CGlxFromFocusOutwardIterator()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
T_CGlxFromFocusOutwardIterator::T_CGlxFromFocusOutwardIterator()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::SetupL(  )
    {
    iOrder = &iFocusOrder;
    iList.iCount = 0;
    iList.iFocusIndex = -1;
    iOrder->SetToFirst(&iList);
    iFocusOrder.SetRangeOffsets(0, 0);
    }

// ----------------------------------------------------------------------------
// Teardown
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::Teardown(  )
    {
    } 

// ----------------------------------------------------------------------------
// UT_TestEmptyL
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::UT_TestEmptyL(  )
    {
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && iFocusOrder.iFrontOffset == 0 && 
    	iFocusOrder.iRearOffset == 0 && iFocusOrder.iList == &iList);
	TInt index;

	// Test with empty list
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);
    EUNIT_ASSERT(iFocusOrder.iFrontOffset == 0 && iFocusOrder.iRearOffset == 0 && iFocusOrder.iList == &iList);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);
    
	// Test with a range
    iOrder->SetToFirst(&iList); // (Does nothing)
    iFocusOrder.SetRangeOffsets(1, 1);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);
    EUNIT_ASSERT(iFocusOrder.iFrontOffset == 1 && iFocusOrder.iRearOffset == 1 && iFocusOrder.iList == &iList);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);    
    
	// Test with a range
    iOrder->SetToFirst(&iList); // (Does nothing)
    iFocusOrder.SetRangeOffsets(4, 0);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);
    EUNIT_ASSERT(iFocusOrder.iFrontOffset == 0 && iFocusOrder.iRearOffset == 4 && iFocusOrder.iList == &iList);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);    
    
	// Test with a range
    iOrder->SetToFirst(&iList); // (Does nothing)
    iFocusOrder.SetRangeOffsets(0, 5);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);
    EUNIT_ASSERT(iFocusOrder.iFrontOffset == 5 && iFocusOrder.iRearOffset == 0 && iFocusOrder.iList == &iList);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && index == KErrNotFound);    
    }

// ----------------------------------------------------------------------------
// UT_TestSymmetricRangeL
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::UT_TestSymmetricRangeL(  )
	{
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 0 && iFocusOrder.iFrontOffset == 0 && 
    	iFocusOrder.iRearOffset == 0 && iFocusOrder.iList == &iList);
	TInt index;

	// Test with a list with 1 item, no range
    iList.iCount = 1; 
    iList.iFocusIndex = 0;
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == 0);
    EUNIT_ASSERT(iFocusOrder.iFrontOffset == 0 && iFocusOrder.iRearOffset == 0 && iFocusOrder.iList == &iList);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == KErrNotFound);
    EUNIT_ASSERT(iFocusOrder.iFrontOffset == 0 && iFocusOrder.iRearOffset == 0 && iFocusOrder.iList == &iList);
    
	// Test with a list with 2 items, no range
    iOrder->SetToFirst(&iList); 
    iList.iCount = 2;
    iList.iFocusIndex = 0; 
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == 0);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == KErrNotFound);
 	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == KErrNotFound);

	// Focus to 1   
    iOrder->SetToFirst(&iList); 
    iList.iFocusIndex = 1;
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == 1);
	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == KErrNotFound);
 	index = (*iOrder)++;
    EUNIT_ASSERT(iFocusOrder.iCurrentItem == 1 && index == KErrNotFound);

	//////////////////////////////////////////////////////////////
	// Test with a list with 2 items, 1-1 range
    // indexes: 01
    // focus:   F
    // order:   01
	TestSingleRange(2, 0, 1, 1, "01ee"); // count, focus, rear, front, result
    // indexes: 01
    // focus:    F
    // order:   10
	TestSingleRange(2, 1, 1, 1, "10ee"); // count, focus, rear, front, result
    
	//////////////////////////////////////////////////////////////
    // Test with a list with 3 items, 1-1 range
    // indexes: 012
    // focus:   F
    // order:   012
	TestSingleRange(3, 0, 1, 1, "012ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:    F
    // order:   201
	TestSingleRange(3, 1, 1, 1, "120ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:     F
    // order:   201
	TestSingleRange(3, 2, 1, 1, "201ee"); // count, focus, rear, front, result
    
	//////////////////////////////////////////////////////////////
    // Test with a list with 4 items, 1-1 range
    // indexes: 0123
    // focus:   F
    // order:   01 2
	TestSingleRange(4, 0, 1, 1, "013ee"); // count, focus, rear, front, result
    // indexes: 0123
    // focus:    F
    // order:   201 
	TestSingleRange(4, 1, 1, 1, "120ee"); // count, focus, rear, front, result
    // indexes: 0123
    // focus:     F
    // order:    201 
	TestSingleRange(4, 2, 1, 1, "231ee"); // count, focus, rear, front, result
    // indexes: 0123
    // focus:      F
    // order:   1 20
	TestSingleRange(4, 3, 1, 1, "302ee"); // count, focus, rear, front, result
  	}
  	
// ----------------------------------------------------------------------------
// UT_TestAsymmetricRangeL
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::UT_TestAsymmetricRangeL(  )
	{
 	// Test with a list with 5 items, 1-2 range
    // indexes: 01234
    // focus:   F
    // order:   013 2
	TestSingleRange(5, 0, 1, 2, "0142ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:    F
    // order:   2013 
	TestSingleRange(5, 1, 1, 2, "1203ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:     F
    // order:    2013 
	TestSingleRange(5, 2, 1, 2, "2314ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:      F
    // order:   3 201
	TestSingleRange(5, 3, 1, 2, "3420ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:       F
    // order:   13 20
	TestSingleRange(5, 4, 1, 2, "4031ee"); // count, focus, rear, front, result

	//////////////////////////////////////////////////////////////
	// Test with a list with 5 items, 2-1 range
    // indexes: 01234
    // focus:   F
    // order:   01 32 
	TestSingleRange(5, 0, 2, 1, "0143ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:    F
    // order:   201 3 
	TestSingleRange(5, 1, 2, 1, "1204ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:     F
    // order:   3201 
	TestSingleRange(5, 2, 2, 1, "2310ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:      F
    // order:    3201
	TestSingleRange(5, 3, 2, 1, "3421ee"); // count, focus, rear, front, result
    // indexes: 01234
    // focus:       F
    // order:   1 320
	TestSingleRange(5, 4, 2, 1, "4032ee"); // count, focus, rear, front, result

	//////////////////////////////////////////////////////////////
	// Test with a list with 6 items, 4-2 range
    // indexes: 012345
    // focus:   F
    // order:   013542
	TestSingleRange(6, 0, 4, 2, "015243ee"); // count, focus, rear, front, result
    // indexes: 012345
    // focus:    F
    // order:   201354  
	TestSingleRange(6, 1, 4, 2, "120354ee"); // count, focus, rear, front, result
    // indexes: 012345
    // focus:     F
    // order:   420135 
	TestSingleRange(6, 2, 4, 2, "231405ee"); // count, focus, rear, front, result
    // indexes: 012345
    // focus:      F
    // order:   542013
	TestSingleRange(6, 3, 4, 2, "342510ee"); // count, focus, rear, front, result
    // indexes: 012345
    // focus:       F
    // order:   354201
	TestSingleRange(6, 4, 4, 2, "453021ee"); // count, focus, rear, front, result
    // indexes: 012345
    // focus:        F
    // order:   135420
	TestSingleRange(6, 5, 4, 2, "504132ee"); // count, focus, rear, front, result

	//////////////////////////////////////////////////////////////
	// Test with a list with 7 items, 4-2 range
    // indexes: 0123456
    // focus:   F
    // order:   0136542
	TestSingleRange(7, 0, 4, 2, "0162543ee"); // count, focus, rear, front, result
    // indexes: 0123456
    // focus:    F
    // order:   2013654 
	TestSingleRange(7, 1, 4, 2, "1203654ee"); // count, focus, rear, front, result
    // indexes: 0123456 
    // focus:     F
    // order:   4201365
	TestSingleRange(7, 2, 4, 2, "2314065ee"); // count, focus, rear, front, result
    // indexes: 0123456
    // focus:      F
    // order:   5420136
	TestSingleRange(7, 3, 4, 2, "3425106ee"); // count, focus, rear, front, result
    // indexes: 0123456
    // focus:       F
    // order:   6542013
	TestSingleRange(7, 4, 4, 2, "4536210ee"); // count, focus, rear, front, result
    // indexes: 0123456
    // focus:        F
    // order:   3654201
	TestSingleRange(7, 5, 4, 2, "5640321ee"); // count, focus, rear, front, result
    // indexes: 0123456
    // focus:         F
    // order:   1365420  
	TestSingleRange(7, 6, 4, 2, "6051432ee"); // count, focus, rear, front, result

	//////////////////////////////////////////////////////////////
	// Test with a list with 8 items, 4-2 range
    // indexes: 01234567
    // focus:   F
    // order:   013 6542
	TestSingleRange(8, 0, 4, 2, "0172654ee"); // count, focus, rear, front, result
    // indexes: 01234567
    // focus:    F
    // order:   2013 654 
	TestSingleRange(8, 1, 4, 2, "1203765ee"); // count, focus, rear, front, result
    // indexes: 01234567 
    // focus:     F
    // order:   42013 65
	TestSingleRange(8, 2, 4, 2, "2314076ee"); // count, focus, rear, front, result
    // indexes: 01234567
    // focus:      F
    // order:   542013 6
	TestSingleRange(8, 3, 4, 2, "3425107ee"); // count, focus, rear, front, result
    // indexes: 01234567
    // focus:       F
    // order:   6542013
	TestSingleRange(8, 4, 4, 2, "4536210ee"); // count, focus, rear, front, result
    // indexes: 01234567
    // focus:        F
    // order:    6542013
	TestSingleRange(8, 5, 4, 2, "5647321ee"); // count, focus, rear, front, result
    // indexes: 01234567
    // focus:         F
    // order:   3 654201  
	TestSingleRange(8, 6, 4, 2, "6750432ee"); // count, focus, rear, front, result
    // indexes: 01234567
    // focus:          F
    // order:   13 65420  
	TestSingleRange(8, 7, 4, 2, "7061543ee"); // count, focus, rear, front, result
  	}
  	
// ----------------------------------------------------------------------------
// UT_TestOneSideRangeL
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::UT_TestOneSideRangeL(  )
	{
	// 1 item, 1-0 range
	TestSingleRange(1, 0, 1, 0, "0ee"); // count, focus, rear, front, result
	// 1 item, 0-1 range
	TestSingleRange(1, 0, 0, 1, "0ee"); // count, focus, rear, front, result

	//////////////////////////////////////////////////////////////
	// Test with a list with 2 items, 1-0 range
    // indexes: 01
    // focus:   F
    // order:   01
	TestSingleRange(2, 0, 1, 0, "01ee"); // count, focus, rear, front, result
    // indexes: 01
    // focus:    F
    // order:   01
	TestSingleRange(2, 1, 1, 0, "10ee"); // count, focus, rear, front, result
	//////////////////////////////////////////////////////////////
	// Test with a list with 2 items, 0-1 range
    // indexes: 01
    // focus:   F
    // order:   01
	TestSingleRange(2, 0, 0, 1, "01ee"); // count, focus, rear, front, result
    // indexes: 01
    // focus:    F
    // order:   01
	TestSingleRange(2, 1, 0, 1, "10ee"); // count, focus, rear, front, result
    
	//////////////////////////////////////////////////////////////
    // Test with a list with 3 items, 0-1 range
    // indexes: 012
    // focus:   F
    // order:   01
	TestSingleRange(3, 0, 0, 1, "01ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:    F
    // order:    01
	TestSingleRange(3, 1, 0, 1, "12ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:     F
    // order:   1 0
	TestSingleRange(3, 2, 0, 1, "20ee"); // count, focus, rear, front, result
    
	//////////////////////////////////////////////////////////////
    // Test with a list with 3 items, 1-0 range
    // indexes: 012
    // focus:   F
    // order:   0 1
	TestSingleRange(3, 0, 1, 0, "02ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:    F
    // order:   10
	TestSingleRange(3, 1, 1, 0, "10ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:     F
    // order:    10
	TestSingleRange(3, 2, 1, 0, "21ee"); // count, focus, rear, front, result

	//////////////////////////////////////////////////////////////
    // Test with a list with 3 items, 0-2 range
    // indexes: 012
    // focus:   F
    // order:   012
	TestSingleRange(3, 0, 0, 2, "012ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:    F
    // order:   201
	TestSingleRange(3, 1, 0, 2, "120ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:     F
    // order:   120
	TestSingleRange(3, 2, 0, 2, "201ee"); // count, focus, rear, front, result
    
	//////////////////////////////////////////////////////////////
    // Test with a list with 3 items, 2-0 range
    // indexes: 012
    // focus:   F
    // order:   021
	TestSingleRange(3, 0, 2, 0, "021ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:    F
    // order:   102
	TestSingleRange(3, 1, 2, 0, "102ee"); // count, focus, rear, front, result
    // indexes: 012
    // focus:     F
    // order:   210
	TestSingleRange(3, 2, 2, 0, "210ee"); // count, focus, rear, front, result

	//////////////////////////////////////////////////////////////
    // Test with a list with 2 items, 0-2 range
    // indexes: 01
    // focus:   F
    // order:   01
	TestSingleRange(2, 0, 0, 2, "01ee"); // count, focus, rear, front, result
    // indexes: 01
    // focus:    F
    // order:   10
	TestSingleRange(2, 1, 0, 2, "10ee"); // count, focus, rear, front, result
    
	//////////////////////////////////////////////////////////////
    // Test with a list with 3 items, 2-0 range
    // indexes: 01
    // focus:   F
    // order:   01
	TestSingleRange(2, 0, 2, 0, "01ee"); // count, focus, rear, front, result
    // indexes: 01
    // focus:    F
    // order:   10
	TestSingleRange(2, 1, 2, 0, "10ee"); // count, focus, rear, front, result
  	}

// ----------------------------------------------------------------------------
// Run a single test
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::TestSingleRange(TInt aCount, 
		TInt aFocusIndex, TInt aRearOffset, TInt aFrontOffset, char* aResults)
	{
    __ASSERT_DEBUG(aFocusIndex >= 0 && aFocusIndex < aCount && aCount >= 0, Panic(EGlxPanicIllegalArgument)); // Make sure is testing all cases, also failed case

	// Test with a list with 5 items, 1-2 range
    iFocusOrder.SetRangeOffsets(aRearOffset, aFrontOffset);
    iOrder->SetToFirst(&iList); 
    iList.iCount = aCount; 
    iList.iFocusIndex = aFocusIndex;
    
    // poor man's strlen...
    TInt resultCount = 0;
    char* tmp = aResults;
    while (*tmp) 
    	{
    	resultCount++;
    	tmp++;
    	}
    	
    __ASSERT_DEBUG(resultCount > aFrontOffset + aRearOffset + 1, Panic(EGlxPanicIllegalArgument)); // Make sure is testing all cases, also failed case
    
    for (TInt i = 0; i < resultCount; i++)
    	{
    	TInt result = (*iOrder)++;
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
// Run a single in range test
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::TestSingleInRange(TInt aCount, 
		TInt aFocusIndex, TInt aRearOffset, TInt aFrontOffset, char* aResults)
	{
    __ASSERT_DEBUG(aFocusIndex >= 0 && aFocusIndex < aCount && aCount >= 0, Panic(EGlxPanicIllegalArgument)); // Make sure is testing all cases, also failed case

	// Test with a list with 5 items, 1-2 range
    iFocusOrder.SetRangeOffsets(aRearOffset, aFrontOffset);
    iOrder->SetToFirst(&iList); 
    iList.iCount = aCount; 
    iList.iFocusIndex = aFocusIndex;
    
    for (TInt i = 0; i < aCount; i++)
    	{
    	char c = aResults[i];
     	__ASSERT_DEBUG(c != 0, Panic(EGlxPanicIllegalArgument)); // Not enough results
   	
    	TBool inRange = iOrder->InRange(i);
    	EUNIT_ASSERT((inRange && c == 'x') || (!inRange && c == ' '));
    	}
	}
		
// ----------------------------------------------------------------------------
// Run a single test
// ----------------------------------------------------------------------------
//
void T_CGlxFromFocusOutwardIterator::UT_TestInRangeL()
	{
	// List of 1
	// indexes: 0
	// focus:   F
	// range:   x
	TestSingleInRange(1, 0, 0, 0, "x"); // count, focus, rear range, front range, result per index
	TestSingleInRange(1, 0, 2, 0, "x"); // count, focus, rear range, front range, result per index
	TestSingleInRange(1, 0, 0, 2, "x"); // count, focus, rear range, front range, result per index
	TestSingleInRange(1, 0, 2, 2, "x"); // count, focus, rear range, front range, result per index

	// List of 3
	// indexes: 012
	// focus:   F
	// range:   x
	TestSingleInRange(3, 0, 0, 0, "x  "); // count, focus, rear range, front range, result per index
	TestSingleInRange(3, 1, 0, 0, " x "); // count, focus, rear range, front range, result per index
	TestSingleInRange(3, 2, 0, 0, "  x"); // count, focus, rear range, front range, result per index

	// List of 4, range of 1
	// indexes: 0123
	// focus:   F
	TestSingleInRange(4, 0, 1, 1, "xx x"); // count, focus, rear range, front range, result per index
	TestSingleInRange(4, 1, 1, 1, "xxx "); // count, focus, rear range, front range, result per index
	TestSingleInRange(4, 2, 1, 1, " xxx"); // count, focus, rear range, front range, result per index
	TestSingleInRange(4, 3, 1, 1, "x xx"); // count, focus, rear range, front range, result per index

	// List of 4, range of 2-3
	// indexes: 0123
	// focus:   F
	// range:   xxxx
	TestSingleInRange(4, 0, 2, 3, "xxxx"); // count, focus, rear range, front range, result per index

	// List of 7, range of 2-4
	// indexes: 0123456
	// focus:   F
	// range:   xxxxxxx
	TestSingleInRange(7, 0, 2, 4, "xxxxxxx"); // count, focus, rear range, front range, result per index
	TestSingleInRange(7, 1, 2, 4, "xxxxxxx"); // count, focus, rear range, front range, result per index
	TestSingleInRange(7, 3, 2, 4, "xxxxxxx"); // count, focus, rear range, front range, result per index
	TestSingleInRange(7, 6, 2, 4, "xxxxxxx"); // count, focus, rear range, front range, result per index

	// List of 8, range of 2-4
	// indexes: 01234567
	// focus:   F
	// range:   xxx xxxx
	TestSingleInRange(8, 0, 4, 2, "xxx xxxx"); // count, focus, rear range, front range, result per index
	// indexes: 01234567
	// focus:    F
	// range:   xxxx xxx
	TestSingleInRange(8, 1, 4, 2, "xxxx xxx"); // count, focus, rear range, front range, result per index
	// indexes: 01234567
	// focus:     F
	// range:   xxxxx xx
	TestSingleInRange(8, 2, 4, 2, "xxxxx xx"); // count, focus, rear range, front range, result per index
	// indexes: 01234567
	// focus:      F
	// range:   xxxxxx x
	TestSingleInRange(8, 3, 4, 2, "xxxxxx x"); // count, focus, rear range, front range, result per index
	// indexes: 01234567
	// focus:       F
	// range:   xxxxxxx 
	TestSingleInRange(8, 4, 4, 2, "xxxxxxx "); // count, focus, rear range, front range, result per index
	// indexes: 01234567
	// focus:        F
	// range:    xxxxxxx 
	TestSingleInRange(8, 5, 4, 2, " xxxxxxx"); // count, focus, rear range, front range, result per index
	// indexes: 01234567
	// focus:         F
	// range:   x xxxxxx 
	TestSingleInRange(8, 6, 4, 2, "x xxxxxx"); // count, focus, rear range, front range, result per index
	// indexes: 01234567
	// focus:          F
	// range:   xx xxxxx 
	TestSingleInRange(8, 7, 4, 2, "xx xxxxx"); // count, focus, rear range, front range, result per index
	
	}
	
// ----------------------------------------------------------------------------
// Test table
// ----------------------------------------------------------------------------
//

EUNIT_BEGIN_TEST_TABLE(
    T_CGlxFromFocusOutwardIterator,
    "Unit tests for TGlxFromFocusOutwardItemOrder",
    "UNIT" )
 
EUNIT_TEST(
    "Test empty list",
    "TGlxFromFocusOutwardItemOrder",
    "++",
    "FUNCTIONALITY",
    SetupL, UT_TestEmptyL, Teardown)

EUNIT_TEST(
    "Test symmetric ranges",
    "TGlxFromFocusOutwardItemOrder",
    "++",
    "FUNCTIONALITY",
    SetupL, UT_TestSymmetricRangeL, Teardown)

EUNIT_TEST(
    "Test asymmetric ranges",
    "TGlxFromFocusOutwardItemOrder",
    "++",
    "FUNCTIONALITY",
    SetupL, UT_TestAsymmetricRangeL, Teardown)

EUNIT_TEST(
    "Test one-sided ranges",
    "TGlxFromFocusOutwardItemOrder",
    "++",
    "FUNCTIONALITY",
    SetupL, UT_TestOneSideRangeL, Teardown)

EUNIT_TEST(
    "Test in range",
    "TGlxFromFocusOutwardItemOrder",
    "InRange",
    "FUNCTIONALITY",
    SetupL, UT_TestInRangeL, Teardown)

EUNIT_END_TEST_TABLE
//  END OF FILE
