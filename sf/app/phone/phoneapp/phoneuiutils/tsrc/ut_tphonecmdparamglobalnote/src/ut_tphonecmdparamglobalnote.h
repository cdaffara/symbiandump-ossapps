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
#ifndef __UT_TPhoneCmdParamGlobalNote_H__
#define __UT_TPhoneCmdParamGlobalNote_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

// FORWARD DECLARATIONS

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_TPhoneCmdParamGlobalNote ) : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static UT_TPhoneCmdParamGlobalNote* NewL();
        static UT_TPhoneCmdParamGlobalNote* NewLC();
        ~UT_TPhoneCmdParamGlobalNote();

    private: // Constructors

        UT_TPhoneCmdParamGlobalNote();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void ut_TimeoutL();
         void ut_SetTimeoutL();

    private: // Data

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_TPhoneCmdParamGlobalNote_H__
