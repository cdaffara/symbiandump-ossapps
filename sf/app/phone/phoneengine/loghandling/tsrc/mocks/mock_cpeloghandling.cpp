/** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <smcmockclassincludes.h>
#include <spsettings.h>
#include <spproperty.h>
#include <telloggingextension.h>
#include <ECom.h> 
#include "CPELogHandling.h"
#include "CPELogEvent.h"
#include "CPELogHandlingCommand.h"
#include "CPELogExternalData.h"
#include <TALogger.h>
#include <logcli.h>
#include <pepanic.pan>
#include <MPEPhoneModelInternal.h>
#include <MPEDataStore.h>
#include "cpeloghandling.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPELogHandling::CPELogHandling
// -----------------------------------------------------------------------------
//
CPELogHandling::CPELogHandling( 
        MPEPhoneModelInternal & aModel,
        RFs & aFsSession ) : iModel( aModel ),
            iDataStore( *( aModel.DataStore( ) ) ),
            iFsSession( aFsSession )
    {
    
    }


// -----------------------------------------------------------------------------
// CPELogHandling::ConstructL
// -----------------------------------------------------------------------------
//
void CPELogHandling::ConstructL(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPELogHandling::NewL
// -----------------------------------------------------------------------------
//
MPELogHandling * CPELogHandling::NewL( 
        MPEPhoneModelInternal & aModel,
        RFs & aFsSession )
    {
    SMC_MOCK_NEWL2( MPELogHandling *, MPEPhoneModelInternal &, aModel, 
        RFs &, aFsSession )

    CPELogHandling* self = new( ELeave ) CPELogHandling( aModel, aFsSession );
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPELogHandling::~CPELogHandling
// -----------------------------------------------------------------------------
//
CPELogHandling::~CPELogHandling(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPELogHandling::SaveCallEntry
// -----------------------------------------------------------------------------
//
TInt CPELogHandling::SaveCallEntry( 
        const TInt aCallId )
    {
    SMC_MOCK_METHOD1( TInt, const TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::SendMessage
// -----------------------------------------------------------------------------
//
void CPELogHandling::SendMessage( 
        MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        TInt aErrorCode )
    {
    SMC_MOCK_METHOD2( void, MEngineMonitor::TPEMessagesFromPhoneEngine, aMessage, 
        TInt, aErrorCode )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LogStringIn
// -----------------------------------------------------------------------------
//
TLogString & CPELogHandling::LogStringIn(  )
    {
    SMC_MOCK_METHOD0( TLogString & )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LogStringOut
// -----------------------------------------------------------------------------
//
TLogString & CPELogHandling::LogStringOut(  )
    {
    SMC_MOCK_METHOD0( TLogString & )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LogStringMissed
// -----------------------------------------------------------------------------
//
TLogString & CPELogHandling::LogStringMissed(  )
    {
    SMC_MOCK_METHOD0( TLogString & )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LogStringDelivery
// -----------------------------------------------------------------------------
//
TLogString & CPELogHandling::LogStringDelivery(  )
    {
    SMC_MOCK_METHOD0( TLogString & )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LogStringInAlt
// -----------------------------------------------------------------------------
//
TLogString & CPELogHandling::LogStringInAlt(  )
    {
    SMC_MOCK_METHOD0( TLogString & )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LogStringOutAlt
// -----------------------------------------------------------------------------
//
TLogString & CPELogHandling::LogStringOutAlt(  )
    {
    SMC_MOCK_METHOD0( TLogString & )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LogStringUnknown
// -----------------------------------------------------------------------------
//
TLogString & CPELogHandling::LogStringUnknown(  )
    {
    SMC_MOCK_METHOD0( TLogString & )
    }


// -----------------------------------------------------------------------------
// CPELogHandling::ResetMissedCalls
// -----------------------------------------------------------------------------
//
void CPELogHandling::ResetMissedCalls( 
        const TInt aCallId )
    {
    SMC_MOCK_METHOD1( void, const TInt, aCallId )
    }


