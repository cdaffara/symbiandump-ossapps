/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the class CSPCallAddedHandler
*
*/


#include "cspcalladdedhandler.h"

#include <etelmm.h>
#include <mccpcsobserver.h>

#include "cspcallarray.h"
#include "cspetellinestatusmonitor.h"
#include "mcsplinestatusobserver.h"
#include "rcsplinecontainer.h"
#include "csplogger.h"
#include "cspclientvoicecall.h"
#include "cspclientvideocall.h"
#include "rcsplinecontainer.h"
#include "mcspcommoninfo.h"
#include "cspconsts.h"


// ---------------------------------------------------------------------------
// CSPCallAddedHandler::NewL.
// ---------------------------------------------------------------------------
//
CSPCallAddedHandler* CSPCallAddedHandler::NewL( 
                                   const MCCPCSObserver& aObserver,
                                   RCSPLineContainer& aLineContainer,
                                   CSPCallArray& aCallArray,
                                   MCSPCommonInfo& aCommonInfo,
                                   CSPAudioHandlerBase& aAudioHandler )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCallAddedHandler::NewL()" );
    CSPCallAddedHandler* self = new ( ELeave ) CSPCallAddedHandler( 
                                        aObserver, aLineContainer,
                                        aCallArray, aCommonInfo, 
                                        aAudioHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPCallAddedHandler::~CSPCallAddedHandler( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCallAddedHandler::~CSPCallAddedHandler" );
    delete iAuxLineMonitor;
    delete iVoiceLineMonitor;
    delete iDataLineMonitor;
    }

// ---------------------------------------------------------------------------
// CSPCallAddedHandler::CallAdded
// ---------------------------------------------------------------------------
//
void CSPCallAddedHandler::CallAdded( RMobileLine& aLine, 
                        const TName& aCallName,
                        RCSPLineContainer::TCSPLineId aLineId )
    {
    CSPLOGSTRING2(CSPINT, 
            "CSPCallAddedHandler::CallAdded line: %d", aLineId);
    CSPLOGSTRING2(CSPINT, 
            "CSPCallAddedHandler::CallAdded call name: %S", &aCallName);
    
    // Find call by name
    if ( !iCallArray.FindCall( aCallName ) )
        {        
        TInt err( KErrNone );
        
        // Resolve call type
        TUint32 serviceId( 0 );
        CCPCall::TCallType callType;
        CCCECallParameters::TCCELineType lineType;

        iLineContainer.ResolveCallInfo( aLineId, serviceId, callType, lineType );
        
        CSPCall* call = NULL;
        CCCECallParameters* callParameters = NULL;
        
        TRAP_IGNORE( callParameters = CCCECallParameters::NewL() );
        if ( callParameters )
            {
            callParameters->SetServiceId(serviceId);
            callParameters->SetCallType(callType);
            callParameters->SetLineType(lineType);

            CSPLOGSTRING2(CSPINT, 
                    "CSPCallAddedHandler::CallAdded call type: %d", callType );
            
            if ( callType == CCPCall::ECallTypeCSVoice )
                {
                TRAP( err, call = CSPClientVoiceCall::NewL( aCallName, 
                                                    aLine, *callParameters, 
                                                    iCommonInfo ));
                }                
            else if ( callType == CCPCall::ECallTypeVideo )
                {
                CSPLOGSTRING(CSPINT, 
                        "CSPCallAddedHandler::CallAdded creating video call");
                TRAP( err, call = CSPClientVideoCall::NewL( aCallName, 
                                                    aLine, *callParameters, 
                                                    iCommonInfo ));
                }

            delete callParameters;
            callParameters = NULL; 
            }
        
        CSPLOGSTRING2(CSPINT, 
           "CSPCallAddedHandler::CallAdded Client call creation res %d", err );
        
        if ( call )
            {
            // Set audio handler
            call->SetAudioHandler( &iAudioHandler );
            
            MCCPCallObserver::TCCPCallState callState = call->State(); 
            CSPLOGSTRING2(CSPINT, 
                    "CSPCallAddedHandler::CallAdded call state: %d", callState );
            
            if ( callState == MCCPCallObserver::ECCPStateDialling )
               {                   
               CSPLOGSTRING(CSPINT, 
                       "CSPCallAddedHandler::CallAdded add Dialling call" );
               
               iCallArray.Add( call );
               
               CSPLOGSTRING(CSPINT, 
                       "CSPCallAddedHandler::CallAdded notifying dialling state" );
               
               call->NotifyCallStateChangedETel( RMobileCall::EStatusDialling ); 
               }
            else if ( callState == MCCPCallObserver::ECCPStateConnecting || 
                      callState == MCCPCallObserver::ECCPStateConnected )
               {
               CSPLOGSTRING(CSPINT, 
                       "CSPCallAddedHandler::CallAdded add Connecting/Connected call" );
               iCallArray.Add( call );                              
               iCommonInfo.IndicateClientCall( call );
               }
            else 
                {
                CSPLOGSTRING(CSPERROR, 
                        "CSPCallAddedHandler::CallAdded ignore call" );
                delete call;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPCallAddedHandler::LineStatusChanged
// ---------------------------------------------------------------------------
//
void CSPCallAddedHandler::LineStatusChanged( RMobileLine& aLine, 
                                             RCSPLineContainer::TCSPLineId aLineId,
                                             RMobileCall::TMobileCallStatus& aLineStatus )
    {    
    CSPLOGSTRING2(CSPINT, 
            "CSPCallAddedHandler::LineStatusChanged line: %d", aLineId );
    CSPLOGSTRING2(CSPINT, 
            "CSPCallAddedHandler::LineStatusChanged line status: %d", 
            aLineStatus );
    
    if ( aLineStatus == RMobileCall::EStatusDialling )
        {
        TInt err(KErrNone);
        RLine::TLineInfo lineInfo;
        err = aLine.GetInfo( lineInfo );
        TName callName; 
        
        if ( err == KErrNone )
            {
            callName = lineInfo.iNameOfLastCallAdded;
            CallAdded( aLine, callName, aLineId); 
            }
        else 
            {
            CSPLOGSTRING2(CSPERROR, 
                    "CSPCallAddedHandler::LineStatusChanged get info err:%d", 
                    err);
            }
        }
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPCallAddedHandler::CSPCallAddedHandler(
            const MCCPCSObserver& aObserver,
            RCSPLineContainer& aLineContainer,
            CSPCallArray& aCallArray,
            MCSPCommonInfo& aCommonInfo,
            CSPAudioHandlerBase& aAudioHandler ) :
            iLineContainer( aLineContainer ),
            iObserver( aObserver ),            
            iCallArray( aCallArray ),
            iCommonInfo( aCommonInfo ),
            iAudioHandler( aAudioHandler )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCallAddedHandler::CSPCallAddedHandler");
    }
    
// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSPCallAddedHandler::ConstructL()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCallAddedHandler::ConstructL <");
    
    if ( iLineContainer.LineIsOpen( RCSPLineContainer::ECSPLineSpeech ) )
        {
        RMobileLine& line = iLineContainer.LineByType( 
                                RCSPLineContainer::ECSPLineSpeech );
        iVoiceLineMonitor = CSPEtelLineStatusMonitor::NewL( *this, line, 
                                            RCSPLineContainer::ECSPLineSpeech );
        iVoiceLineMonitor->StartMonitoring();
        }
    // This is for videocalls
    if ( iLineContainer.LineIsOpen( RCSPLineContainer::ECSPLineData ) )
        {
        RMobileLine& line = iLineContainer.LineByType( 
                                RCSPLineContainer::ECSPLineData );

        iDataLineMonitor = CSPEtelLineStatusMonitor::NewL( *this, line, 
                                                    RCSPLineContainer::ECSPLineData );
        iDataLineMonitor->StartMonitoring();
        }

    if ( iLineContainer.LineIsOpen( RCSPLineContainer::ECSPLineAuxSpeech ) )
        {
        RMobileLine& line = iLineContainer.LineByType( 
                                RCSPLineContainer::ECSPLineAuxSpeech );
        iAuxLineMonitor = CSPEtelLineStatusMonitor::NewL( *this, line, 
                                            RCSPLineContainer::ECSPLineAuxSpeech );
        iAuxLineMonitor->StartMonitoring();
        }
    
    CSPLOGSTRING(CSPOBJECT, "CSPCallAddedHandler::ConstructL >");
    }
    
// End of File
