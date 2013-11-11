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
class CPELogExternalData;
class CPELogHandling;
class CPEEngineInfo;
class CSmcMockContext;
class CRepository;
class MPEDataStore;

class CPELogEvent;
class CPELogHandlingCommand;
class CLogClient;
class CPELogInfo;





// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CPELogEvent )
    : public CEUnitTestSuiteClass, public MPEPhoneModelInternal
    {
    public:  // Constructors and destructor

        static UT_CPELogEvent* NewL();
        static UT_CPELogEvent* NewLC();
        ~UT_CPELogEvent();
 
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

        UT_CPELogEvent();
        void ConstructL();

    private: // New methods
        
         void SetupL();
        
         void Teardown();
	   
         void T_CPELogEvent_CallIdL();
        
         void T_CPELogEvent__CPELogEventL();
        
         void T_CPELogEvent_SaveLL();
         
         void T_CPELogEvent_UpdateLogInfoL();
        
         void T_CPELogEvent_ResetLogInfoL();
                        
         void T_CPELogEvent_IsCompletedL();
        
         void T_CPELogEvent_SetPhoneNumberIdL();
        
         void T_CPELogEvent_SaveLeaveL();
         
         void T_CPELogEvent_SaveL2L();
         
         void T_CPELogEvent_SaveL3L();
         
         void T_CPELogEvent_SaveL4L();
         
         void T_CPELogEvent_SaveL5L();

    private: // Data

        CPELogEvent* iCPELogEvent;
        CPELogInfo* iLogInfoMock;
        CPELogHandlingCommand* iLogHCMock;
        CLogClient* iLogClient;
        RFs iFsSession;
        MPEDataStore* iDataStore;   
        CSmcMockContext* iMockContext;
        CPELogHandling* iLogHandlingMock;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPELOGEVENT_H__
