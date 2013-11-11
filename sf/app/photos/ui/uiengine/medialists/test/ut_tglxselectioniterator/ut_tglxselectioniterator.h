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
* Description:   TGlxSelectionIterator unit tests
*
*/



#ifndef __T_GLXSELECTIONITERATOR_H__
#define __T_GLXSELECTIONITERATOR_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES
#include "mglxmedialist.h"
#include "glxmedialistiterator.h"
#include "tmglxmedialist_stub.h"
//  FORWARD DECLARATIONS

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( T_CGlxSelectionIterator )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors
        static T_CGlxSelectionIterator* NewL();
        static T_CGlxSelectionIterator* NewLC();
        ~T_CGlxSelectionIterator();

    private:    // Constructors and destructors
        T_CGlxSelectionIterator();
        void ConstructL();
 
    private:    // New methods
        void SetupL();
        void Teardown();
        
        void UT_TestOperatorPlusPlusL();
        void UT_TestInRangeL();
       
        /**
         * Run a single test
         * @param aCount Number of items in list
         * @param aFocusIndex index of focus
         * @param aRange
         * @param aResult expected results in format "iiiii*e",
         *				  where i is an index, and e means KErrNotFound.
         *				  E.g., "14523ee" means iOrder++ should return 1 first,
         *				  4 then, etc. and finally KErrNotFound two times
         */
		void Test(TInt aRange, TBool aDisabledIfMoreThanRangeSelected, 
				  TInt aListCount, TInt aFocusIndex, RArray<TInt>& aSelection, const char* aResult);
       
        /**
         * Run a single "InRange()" test
         * @param aCount Number of items in list
         * @param aFocusIndex index of focus
         * @param aFrontOffset
         * @param aRearOffset
         * @param aResult expected results in format "xxx  xx",
         *				  where x means the n-th item is in range, and space means it is not
         */
		void TestSingleInRange(TInt aRange, TBool aDisabledIfMoreThanRangeSelected,
				  TInt aListCount, TInt aFocusIndex, RArray<TInt>& aSelection, const char* aResult);
		
    private:    // Data
		MGlxMediaListIterator* iVirtualIterator; // To access virtual functions only
		TMGlxMediaList_Stub iList;
		TGlxSelectionIterator iConcreateIterator;
		
        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif      //  __T_GLXSELECTIONITERATOR_H__
