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

#ifndef __UT_CPELOGEXTERNALDATA_H__
#define __UT_CPELOGEXTERNALDATA_H__

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

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CPELogExternalData )
	: public CEUnitTestSuiteClass, public MPEPhoneModelInternal
    {
    public:  // Constructors and destructor

        static UT_CPELogExternalData* NewL();
        static UT_CPELogExternalData* NewLC();
        ~UT_CPELogExternalData();
        
        // from MPEPhoneModelInternal
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

        UT_CPELogExternalData();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_UpdateCallTimersL();
        
         void T_UpdateLastCallTimerByLineL();
        
         void T_UpdateLastCallTimerL();
        
         void T_IncreaseMissedCallValueL();
        
         void T_SetFirstCallStartTimeL();

    private: // Data

        CPELogExternalData* iCPELogExternalData;
        RFs iFsSession;
        CPEEngineInfo* iEngineInfo;

        CSmcMockContext* iMockContext;
        CPELogHandling* iLogHandlingMock;
        CRepository* iRepositoryMock;
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPELOGEXTERNALDATA_H__
