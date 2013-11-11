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

#ifndef __UT_CPELOGEVENT_H__
#define __UT_CPELOGEVENT_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include "MPEPhoneModelInternal.h"

// FORWARD DECLARATIONS
class CPELogInfo;
class MPEDataStore;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CPELogInfo )
    : public CEUnitTestSuiteClass, public MPEPhoneModelInternal
    {
    public:  // Constructors and destructor

        static UT_CPELogInfo* NewL();
        static UT_CPELogInfo* NewLC();
        ~UT_CPELogInfo();
   
        TInt SaveCallEntry( const TInt aCallId );
        
        MPEDataStore* DataStore();
        void HandleInternalMessage( const TInt aMessage );
        void SendMessage( 
               const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage );
        void SendMessage( 
               const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
               const TInt aCallId );
        MPEExternalDataHandler* DataStoreExt();
        CPERemotePartyInfoMediator* MediatorCommunicationHandler();
        

    private: // Constructors

        UT_CPELogInfo();
        void ConstructL();

    private: // New methods
        
         void SetupL();
        
         void Teardown();
	   
         void T_CPELogInfo_VoIPAddressL();
        
         void T_CPELogInfo_EventTypeL();
         
         void T_CPELogInfo_CallDirectionL();
         
         void T_CPELogInfo_CallIdL();
         
         void T_CPELogInfo_PhoneNumberL();
         
         void T_CPELogInfo_NameL();
         
         void T_CPELogInfo_CallstateL();
         
         void T_CPELogInfo_CallDurationL();
         
         void T_CPELogInfo_CurrentLineL();
         
         void T_CPELogInfo_PhoneNumberIdL();
         
         void T_CPELogInfo_LoggingEnabledL();
         
         void T_CPELogInfo_CallStartTimeL();
         
         void T_CPELogInfo_MissedCallL();
         
         void T_CPELogInfo_ForcedCompletionL();
         
         void T_CPELogInfo_ServiceIdL();
         
         void T_CPELogInfo_MyAddressL();
         
         void T_CPELogInfo_ContactLinkL();
         
         void T_CPELogInfo_CopyLogInfoL();
         
         void T_CPELogInfo_SetEventDataL();

    private: // Data

        CPELogInfo* iLogInfo;
        MPEDataStore* iDataStore;   

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPELOGEVENT_H__
