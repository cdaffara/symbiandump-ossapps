/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
/*
-----------------------------------------------------------------------------
This file has been generated with EUnit Pro
http://www.sysopendigia.com/qualitykit
-----------------------------------------------------------------------------
*/
#ifndef __ut_tphonecmdparamcallheaderdata__
#define __ut_tphonecmdparamcallheaderdata__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "tphonecmdparamcallheaderdata.h"

// FORWARD DECLARATIONS

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_TPhoneCmdParamCallHeaderData )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static UT_TPhoneCmdParamCallHeaderData* NewL();
		
        static UT_TPhoneCmdParamCallHeaderData* NewLC();
        
		// Default destructor.
		~UT_TPhoneCmdParamCallHeaderData();

    private: // Constructors

		// C++ constructor
        UT_TPhoneCmdParamCallHeaderData();
		
		// Constructor which can leave
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();

	private:
	
		void UT_TPhoneCmdParamCallHeaderData_LeaveCleanup ();
		
	private:
		void LeaveFunctionL ();
		
    private: // Data
        TPhoneCmdParamCallHeaderData iTPhoneCmdParamCallHeaderData;
		
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __ut_tphonecmdparamcallheaderdata__
