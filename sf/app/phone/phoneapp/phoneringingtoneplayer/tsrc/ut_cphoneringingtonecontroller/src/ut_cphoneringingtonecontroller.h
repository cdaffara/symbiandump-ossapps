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

#ifndef __UT_CPHONERINGINGTONEPLAYER_H__
#define __UT_CPHONERINGINGTONEPLAYER_H__

// INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitdecorators.h>


// FORWARD DECLARATIONS

class CPhoneRingingToneController;
class CSmcMockContext;
class CPhoneRingingTone;
class CPhoneCenRepProxy;


// custom comparison method for matching expected and actual SData items
TBool operator==( const CPhoneRingingTone& aData1, const CPhoneRingingTone& aData2 );
// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphoneringingtonecontroller )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneringingtonecontroller* NewL();
        static ut_cphoneringingtonecontroller* NewLC();
        ~ut_cphoneringingtonecontroller();

    private: // Constructors

        ut_cphoneringingtonecontroller();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_PlayRingingtoneRinging();
         
         void T_PlayRingingtoneSilence();
         
         void T_PlayRingingtoneBeepOnce();
         
         void T_PlayRingingtoneAscending();
         
         void T_PlayRingingtoneRingingOnce();
         
         void T_PlayRingingtoneTTS();
         
         void T_StopRingingtone();
        

    private: // Data

        CPhoneRingingToneController* iRingingtoneplayer;
        CPhoneCenRepProxy* iPhoneCenRepProxy; 
        CSmcMockContext* iMockContext; 
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONERINGINGTONEPLAYER_H__
