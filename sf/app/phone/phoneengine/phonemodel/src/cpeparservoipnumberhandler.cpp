/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CPEParserVoipNumberHandler class methods implementation
*
*/



// INCLUDE FILES
#include "cpeclientcallrequestmonitor.h"
#include "cpemessagehandler.h"
#include "cpeparservoipnumberhandler.h"
#include "mpeservicehandling.h"
#include "pevirtualengine.h"
#include <featmgr.h>
#include <mpecallhandling.h>
#include <mpedatastore.h>
#include <talogger.h>


// CONSTANTS
const TUint KDtmfPlus = '+';
const TUint KDtmfWait = 'w';
// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPEParserVoipNumberHandler::CPEParserVoipNumberHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEParserVoipNumberHandler::CPEParserVoipNumberHandler( 
        CPEMessageHandler& aOwner, 
        MPECallHandling& aCallHandling,
        MPEServiceHandling& aServiceHandling,
        MPEDataStore& aDataStore
        ) : iOwner( aOwner ), 
            iCallHandling( aCallHandling ),
            iDataStore( aDataStore ),
            iServiceHandling( aServiceHandling )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEParserVoipNumberHandler::CPEParserVoipNumberHandler" )
    }
    
CPEParserVoipNumberHandler* CPEParserVoipNumberHandler::NewL( CPEMessageHandler& aOwner, 
                                     MPECallHandling& aCallHandling,
                                     MPEServiceHandling& aServiceHandling,
                                     MPEDataStore& aDataStore )
    {
    CPEParserVoipNumberHandler* self = CPEParserVoipNumberHandler::NewLC( aOwner, aCallHandling, aServiceHandling, aDataStore );
    CleanupStack::Pop( self );
    return self;
    }

CPEParserVoipNumberHandler* CPEParserVoipNumberHandler::NewLC( CPEMessageHandler& aOwner, 
                                      MPECallHandling& aCallHandling,
                                      MPEServiceHandling& aServiceHandling,
                                      MPEDataStore& aDataStore )
    {
    CPEParserVoipNumberHandler* self = new( ELeave ) CPEParserVoipNumberHandler( aOwner, aCallHandling, aServiceHandling, aDataStore );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CPEParserVoipNumberHandler::ConstructL()
    {
    }

CPEParserVoipNumberHandler::~CPEParserVoipNumberHandler()
    {
    }

// -----------------------------------------------------------------------------
// CPEParserVoipNumberHandler::ProcessDialToVoipNumberL
// Process dial to number.
// -----------------------------------------------------------------------------
//
void CPEParserVoipNumberHandler::ProcessDialToVoipNumberL( 
        const TDesC& aNumber,
        const TDesC& aDtmfPostfix
        )
    {
    TEFLOGSTRING3( KTAINT, 
          "PE CPEParserVoipNumberHandler::ProcessDialToVoipNumberL, aNumber: %S, aDtmfPostfix: %S", 
          &aNumber, 
          &aDtmfPostfix );

    TPEPhoneNumber phoneNumber;
    
    // Check if phone is locked
    iOwner.CheckIfPhoneIsLockedL();
  
    if ( aNumber.Length() == 0 )
        {
        User::Leave( ECCPErrorInvalidPhoneNumber );
        }
    
    phoneNumber.Append( aNumber );
    
    TPECallType processType = iDataStore.CallTypeCommand();
 
    // Remove possible + or w chartes.      
    TPEPhoneNumber postfix = FilterPostfix( aDtmfPostfix );
    if( postfix.Length() )
        {
        phoneNumber.Append( postfix );
        }
    iDtmfString = aDtmfPostfix;
     
    TEFLOGSTRING3( KTAINT, 
          "PE CPEParserVoipNumberHandler::ProcessDialToVoipNumberL, phoneNumber: %S, iDtmfString: %S"
            , &phoneNumber
            , &iDtmfString );
  
    // Temporary hack for enabling client calls with service id 3
    // Proper solution to be done to CallUi and AIW dial data
        

    TUint32 serviceId = iDataStore.ServiceIdCommand();
    iServiceHandling.EnableServiceL( serviceId );

    }

// -----------------------------------------------------------------------------
// CPEParserVoipNumberHandler::FilterPostfix
// -----------------------------------------------------------------------------
// 
TPtrC CPEParserVoipNumberHandler::FilterPostfix( TPtrC aPostfix )
    {
    TLex input( aPostfix );
    TInt stripStart = KErrNotFound;
    TInt postfixLength = aPostfix.Length();
    for ( TInt i = 0; i != postfixLength; i ++ )
        {
        TChar ch( input.Peek() );
        if ( ch == KDtmfWait ||
             ch == KDtmfPlus )
            {
            if ( i < stripStart || stripStart == KErrNotFound )
                {
                stripStart = i;
                }
            }
        }
    if ( stripStart != KErrNotFound )
        {
        return aPostfix.Left( stripStart );
        }
    else
        {
        return aPostfix;
        }
    } 

// -----------------------------------------------------------------------------
// CPEParserVoipNumberHandler::::ContinueVoipDial
// -----------------------------------------------------------------------------
//
TInt CPEParserVoipNumberHandler::ContinueVoipDial() const
    {
    TEFLOGSTRING( KTAINT, "PE CPEParserVoipNumberHandler::ContinueVoipDial" );
    TInt callId = 0;
    
    TPEPhoneNumber phoneNumber = iDataStore.PhoneNumber();
    TEFLOGSTRING2( KTAINT, 
          "PE CPEParserVoipNumberHandler::ContinueVoipDial, phoneNumber: %S", 
          &phoneNumber );
    TBool clientCall = ( iDataStore.CallOriginCommand() != EPECallOriginPhone );
    // DialCall method will set call origin as unknow
    TInt errorCode = iCallHandling.DialCall( phoneNumber, callId );
    
    // Set dtmf string to dataStore
    iDataStore.SetDtmfPostFix( iDtmfString, callId );
    
    //PhoneClient originated call
    if( clientCall && ( ECCPErrorNone == errorCode ) )
        {
        iOwner.SetClientInformation( callId, phoneNumber );
        }
    
    TEFLOGSTRING2( KTAINT, 
            "PE CPEParserVoipNumberHandler::ContinueVoipDial, error code: %d"
            , errorCode );

    return errorCode;
    }

//  End of File
