/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef __UT_CPHONERESOURCERESOLVERBASE_H__
#define __UT_CPHONERESOURCERESOLVERBASE_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include "TPhoneTouchButtonConfig.h"


// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneResourceResolverBase;
class CPhoneMainResourceResolverBaseTester;


// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphoneresourceresolverbase )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneresourceresolverbase* NewL();
        static ut_cphoneresourceresolverbase* NewLC();
        ~ut_cphoneresourceresolverbase();

    private: // Constructors

        ut_cphoneresourceresolverbase();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_NumberAcqMenubar();
         
         void T_IncomingCallButtonsL();
         
         //void T_IncomingCallSilenceButtonsL();
          

    private: // Data
   
      
        CSmcMockContext* iMockContext;
                
        CPhoneMainResourceResolverBaseTester * iResourceResolverBase;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONERESOURCERESOLVERBASE_H__
