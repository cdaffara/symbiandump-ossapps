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
#ifndef __UT_TPhoneCmdParamQuery_H__
#define __UT_TPhoneCmdParamQuery_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "TPhoneCmdParamQuery.h"

// FORWARD DECLARATIONS

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_TPhoneCmdParamQuery )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static UT_TPhoneCmdParamQuery* NewL();
        static UT_TPhoneCmdParamQuery* NewLC();
        ~UT_TPhoneCmdParamQuery();

    private: // Constructors

        UT_TPhoneCmdParamQuery();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_TPhoneCmdParamQuery_SetQueryResourceIdL();
        
         void UT_TPhoneCmdParamQuery_QueryResourceIdL();
        
         void UT_TPhoneCmdParamQuery_SetQueryPromptL();
        
         void UT_TPhoneCmdParamQuery_QueryPromptL();
        
         void UT_TPhoneCmdParamQuery_SetDefaultCbaL();
        
         void UT_TPhoneCmdParamQuery_DefaultCbaL();
        
         void UT_TPhoneCmdParamQuery_SetContentCbaL();
        
         void UT_TPhoneCmdParamQuery_ContentCbaL();
        
         void UT_TPhoneCmdParamQuery_SetDataTextL();
        
         void UT_TPhoneCmdParamQuery_DataTextL();
        
         void UT_TPhoneCmdParamQuery_SetQueryTypeL();
        
         void UT_TPhoneCmdParamQuery_QueryTypeL();
        
         void UT_TPhoneCmdParamQuery_SetSendKeyEnabledL();
        
         void UT_TPhoneCmdParamQuery_SendKeyEnabledL();
        
         void UT_TPhoneCmdParamQuery_SetCommandParamIdL();
        
         void UT_TPhoneCmdParamQuery_SetTimeOutL();
        
         void UT_TPhoneCmdParamQuery_TimeOutL();
        
         void T_SetToneL();
        
         void T_ToneL();
         
         void T_SetCbaCommandMappingL();
         
         void T_CbaCommandMappingL();
         
         void T_GetCustomCommandForTimeOutL();
         
         void T_SetCustomCommandForTimeOutL();
         
         void T_AssignmentOperatorL();
         
         void T_CopyConstructorL();
        

    private: // Data

        TPhoneCmdParamQuery iTPhoneCmdParamQuery;
        RBuf iDataText;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_TPhoneCmdParamQuery_H__
