/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of class CSPEtelCallWaitingRequester
*
*/



#include "cspetelcallwaitingrequester.h"

#include <mmretrieve.h>
#include <etelmm.h>

#include "csplogger.h"
#include "cspconsts.h"


// ---------------------------------------------------------------------------
// Constructs the requester via two phased constructing.
// ---------------------------------------------------------------------------
//
CSPEtelCallWaitingRequester* CSPEtelCallWaitingRequester::NewL(
            RMobilePhone& aPhone ) 
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallWaitingRequester::NewL()" );
    CSPEtelCallWaitingRequester* self = 
        new ( ELeave ) CSPEtelCallWaitingRequester( aPhone );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPEtelCallWaitingRequester::~CSPEtelCallWaitingRequester( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallWaitingRequester::~CSPEtelCallWaitingRequester()" );
    Cancel();
    delete iList;
    }

// ---------------------------------------------------------------------------
// CSPEtelCallWaitingRequester::GetCallWaitingL
// ---------------------------------------------------------------------------
//
void CSPEtelCallWaitingRequester::GetCallWaitingL( const CCCECallParameters& 
                                                      aParams,
                                                   TBool& aCallWaitingStatus )
    {
    CSPLOGSTRING(CSPINT, "CSPEtelCallWaitingRequester::GetCallWaitingL()" );
    
    if ( iMode == ECwRequesterInactive )
        {
        iMode = ECwRequesterGetStarted;
        iList->Start( iStatus );
        SetActive();
        iWait.Start();
        
        if ( iStatus == -5252 )
            {
            aCallWaitingStatus = ETrue; 
            }
        else
            {
            // Get the list of call waiting entries
            CMobilePhoneCWList* cwList = iList->RetrieveListL();
            CleanupStack::PushL(cwList);
            
            TInt count = cwList->Enumerate();
            RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfo;        
            
            aCallWaitingStatus = EFalse; // Default false
            CSPLOGSTRING(CSPINT, "CSPEtelCallWaitingRequester::GetCallWaitingL() iterate" );
            for( TInt i=0; i < count ; i++ )
               {
               cwInfo = cwList->GetEntryL( i );
     
                if ( cwInfo.iStatus == RMobilePhone::ECallWaitingStatusActive )
                    {
                    if ( cwInfo.iServiceGroup == RMobilePhone::EAuxVoiceService )
                        {
                        iAuxLineCwStatus = ETrue;
                        }
                    else
                        {
                        iPrimaryLineCwStatus = ETrue;
                        }
                    }
               }       
            
            GetCallWaitingStatusByLineType( aParams, aCallWaitingStatus );
            
            CSPLOGSTRING2(CSPINT, 
                "CSPEtelCallWaitingRequester::GetCallWaitingL() res:%d", 
                        aCallWaitingStatus );
            CleanupStack::PopAndDestroy( cwList );
            
            // Initial value has been read from network, 
            // now we must keep the setting value up to date.
            StartMonitoring();
            }           
        }

    else if ( iMode == ECwRequesterMonitoring )
        {
        CSPLOGSTRING(CSPINT, "CSPEtelCallWaitingRequester::GetCallWaitingL() use cached value" );
        GetCallWaitingStatusByLineType( aParams, aCallWaitingStatus );
        }       
    else if ( iMode == ECwRequesterGetStarted )
        {
        CSPLOGSTRING(CSPINT, "CSPEtelCallWaitingRequester::GetCallWaitingL() KErrInUse" );
        User::Leave( KErrInUse );
        }       
    }
    
// ---------------------------------------------------------------------------
// From CActive
// CSPEtelCallWaitingRequester::RunL
// ---------------------------------------------------------------------------
//
void CSPEtelCallWaitingRequester::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, "CSPEtelCallWaitingRequester::RunL: status: %d", 
                              iStatus.Int() );
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    else if ( KErrNone == iStatus.Int() )
        {
        RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfo = iDesCwStatus();
        switch (cwInfo.iStatus)
            {
            case RMobilePhone::ECallWaitingStatusActive:
                if ( cwInfo.iServiceGroup == RMobilePhone::EAuxVoiceService )
                    {
                    iAuxLineCwStatus = ETrue;
                    }
                else
                    {
                    iPrimaryLineCwStatus = ETrue;
                    }
                break;
            case RMobilePhone::ECallWaitingStatusNotActive:
                if ( cwInfo.iServiceGroup == RMobilePhone::EAuxVoiceService )
                    {
                    iAuxLineCwStatus = EFalse;
                    }
                else
                    {
                    iPrimaryLineCwStatus = EFalse;
                    }   
                break;
            default:
                break;
            }            
        
        CSPLOGSTRING3(CSPINT, 
                        "CSPEtelCallWaitingRequester::RunL: service:%d new cw status:%d", 
                        cwInfo.iServiceGroup, cwInfo.iStatus );
        
        StartMonitoring();
        }
    else
        {
        iMode = ECwRequesterInactive;
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// CSPEtelCallWaitingRequester::DoCancel
// ---------------------------------------------------------------------------
//
void CSPEtelCallWaitingRequester::DoCancel()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallWaitingRequester::DoCancel()" );
    if ( iMode == ECwRequesterGetStarted )
        {
        iList->Cancel();
        iMode = ECwRequesterInactive;
        }
    else if ( iMode == ECwRequesterMonitoring )
        {
        iPhone.CancelAsyncRequest( EMobilePhoneNotifyCallWaitingStatusChange );
        iMode = ECwRequesterInactive;
        }
    }

// ---------------------------------------------------------------------------
// Constructs the requester.
// ---------------------------------------------------------------------------
//
CSPEtelCallWaitingRequester::CSPEtelCallWaitingRequester(RMobilePhone& aPhone): 
    CActive( EPriorityStandard ),
    iPhone( aPhone )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallWaitingRequester::CSPEtelCallWaitingRequester()" );
    CActiveScheduler::Add( this );
    iPrimaryLineCwStatus = EFalse;
    iAuxLineCwStatus = EFalse;
    iMode = ECwRequesterInactive;
    }

// ---------------------------------------------------------------------------
// CSPEtelCallWaitingRequester::ConstructL
// ---------------------------------------------------------------------------
//    
void CSPEtelCallWaitingRequester::ConstructL()
    {
    CSPLOGSTRING(CSPINT, "CSPEtelCallWaitingRequester::ConstructL()" );
    iList = CRetrieveMobilePhoneCWList::NewL(iPhone);
    }

// ---------------------------------------------------------------------------
// Start listening call waiting status change notifications. 
// ---------------------------------------------------------------------------
//
void CSPEtelCallWaitingRequester::StartMonitoring()
    {
    iPhone.NotifyCallWaitingStatusChange( iStatus, iDesCwStatus );
    SetActive();
    iMode = ECwRequesterMonitoring;
    }

// ---------------------------------------------------------------------------
// Returns the call waiting status from call waiting status list by linetype.
// ---------------------------------------------------------------------------
//
void CSPEtelCallWaitingRequester::GetCallWaitingStatusByLineType( const CCCECallParameters& 
                                                                    aParams,
                                                                  TBool& aCallWaitingStatus )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallWaitingRequester:GetCallWaitingStatusByLineType" );
    
    if ( aParams.LineType() == CCCECallParameters::ECCELineTypePrimary )
        {
        aCallWaitingStatus = iPrimaryLineCwStatus;
        }
    else
        {
        aCallWaitingStatus = iAuxLineCwStatus;
        }
    CSPLOGSTRING3(CSPINT, 
                "CSPEtelCallWaitingRequester::GetCallWaitingStatusByLineType: line:%d res:%d", 
                        aParams.LineType(), aCallWaitingStatus );
    }

// End of file
