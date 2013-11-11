/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_tglxexclusioniterator.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES

// CONSTRUCTION
t_tglxexclusioniterator* t_tglxexclusioniterator::NewL()
    {
    t_tglxexclusioniterator* self = t_tglxexclusioniterator::NewLC();
    CleanupStack::Pop();

    return self;
    }

t_tglxexclusioniterator* t_tglxexclusioniterator::NewLC()
    {
    t_tglxexclusioniterator* self = new( ELeave ) t_tglxexclusioniterator();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
t_tglxexclusioniterator::~t_tglxexclusioniterator()
    {
    }

// Default constructor
t_tglxexclusioniterator::t_tglxexclusioniterator(): iList(0),iExclusionOrder(iIncludeIterator,iExcludeIterator)
    {
    }

// Second phase construct
void t_tglxexclusioniterator::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void t_tglxexclusioniterator::SetupL(  )
    {
    iExclusionOrder.SetToFirst(&iList);     
    }
void t_tglxexclusioniterator::Teardown(  )
    {
     // Do nothing
     EUNIT_PRINT(_L("test done"));
    }
    
void t_tglxexclusioniterator::T_InRangeL()
    {
    
    EUNIT_PRINT(_L(" test for T_InRangeL"));	
    
     // test for less  than a screenfull of items
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(2,0,1);

    
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(2,1,1);

      

    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(4,2,2);

   
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(15,8,7);


  
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(16,9,9);
    
    //test for more than a screenfull of items
    
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_True(100,20,9);

      
    
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_True(100,17,9);

   
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_True(100,18,9);

   
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_True(100,19,9);

   
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(100,99,0);

    
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(100,98,0);

    
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(100,97,0);

   
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);
    TestRange_False(100,96,0);
    }
void t_tglxexclusioniterator::TestRange_True(TInt aCount,TInt aIndex,TInt aFocusIndex) 
    {
    iList.iCount = aCount; 
    iList.iFocus = aFocusIndex;
    EUNIT_ASSERT(iExclusionOrder.InRange(aIndex)==ETrue);

    }
void t_tglxexclusioniterator::TestRange_False(TInt aCount,TInt aIndex,TInt aFocusIndex)
    {
    iList.iCount = aCount; 
    iList.iFocus = aFocusIndex;
    EUNIT_ASSERT(iExclusionOrder.InRange(aIndex)==EFalse);
    }
void t_tglxexclusioniterator::T_PlusPlusL()
    {  
    EUNIT_PRINT(_L(" Test T_PlusPlusL L-R "));

    Test(100,17,8);

    Test(100,18,9);

    Test(100,19,10);

    Test(100,20,11);

    EUNIT_PRINT(_L(" Test T_PlusPlusL--R-L "));

    Test(100,91,92);

    Test(100,90,89);

    Test(100,89,88);

    Test(100,86,87);
    }
void t_tglxexclusioniterator::Test( TInt aCount,TInt aIndex,TInt aFocusIndex)
    {
    iList.iCount = aCount; 
    iList.iFocus = aFocusIndex;
    iIncludeIterator.SetRange(2*7);
    iExcludeIterator.SetRangeOffsets(7,7);

    TInt newIndex = iExclusionOrder++;
    EUNIT_ASSERT(iExclusionOrder.InRange(newIndex )); 
    }
//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_tglxexclusioniterator,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Test InRange",
    "t_tglxexclusioniterator",
    "T_InRangeL",
    "FUNCTIONALITY",
    SetupL,T_InRangeL,Teardown)  
EUNIT_TEST(
    "Test PlusPlus ",
    "t_tglxexclusioniterator",
    "T_PlusPlusL",
    "FUNCTIONALITY",
    SetupL,T_PlusPlusL,Teardown)        
    

EUNIT_END_TEST_TABLE

//  END OF FILE
