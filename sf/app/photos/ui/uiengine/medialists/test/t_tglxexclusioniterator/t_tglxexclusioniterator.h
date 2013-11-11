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



#ifndef __T_TGLXEXCLUSIONITERATOR_H__
#define __T_TGLXEXCLUSIONITERATOR_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  FORWARD DECLARATIONS
#include <mglxmedialist.h>
#include <glxmedialistiterator.h>
#include "tmglxmedialist_stub.h"

#include <e32def.h>

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_tglxexclusioniterator )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
        * Two phase construction
        */
        static t_tglxexclusioniterator* NewL();
        static t_tglxexclusioniterator* NewLC();
        /**
        * Destructor
        */
        ~t_tglxexclusioniterator();

    private:    // Constructors and destructors

        t_tglxexclusioniterator();
        void ConstructL();

    private:    // New methods

        void SetupL();

        void Teardown();

        void T_InRangeL();

         // unit test for the InRange function.
        void TestRange_True(TInt aCount,TInt aIndex,TInt aFocusIndex);
         // unit test for the InRange function.
        void TestRange_False(TInt aCount,TInt aIndex,TInt aFocusIndex);

        // unit test for the plus plus operator	     
        void T_PlusPlusL();

        void Test( TInt aCount,TInt aIndex,TInt aFocusIndex);

 	     
    private:    // Data

        MGlxMediaListIterator* iVirtualIterator; // To access virtual functions only
        TMGlxMediaList_Stub iList;
        TGlxSequentialIterator iIncludeIterator;
        TGlxFromFocusOutwardIterator iExcludeIterator;
        TGlxExclusionIterator iExclusionOrder;
 
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_TGLXEXCLUSIONITERATOR_H__

// End of file
