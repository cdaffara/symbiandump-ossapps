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
* Description:  Contains the implementation of class 
*                CSPEtelConferenceCallRequester.
*
*/


#include <etelmm.h>
#include <mccpconferencecallobserver.h>

#include "cspetelconferencecallrequester.h"
#include "csplogger.h"
#include "cspconferencecall.h"


// ---------------------------------------------------------------------------
// Constructs the requester via two phased constructing.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceCallRequester* CSPEtelConferenceCallRequester::NewL(
            CSPConferenceCall& aOwner,
            RMobileConferenceCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelConferenceCallRequester::NewL()" );
    CSPEtelConferenceCallRequester* self = new ( ELeave ) CSPEtelConferenceCallRequester( 
                                        aOwner, aCall );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceCallRequester::~CSPEtelConferenceCallRequester( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelConferenceCallRequester::~CSPEtelConferenceCallRequester()" );
    CSPLOGSTRING2(CSPINT, "CSPEtelConferenceCallRequester:: type: %d", iRequestType );
    Cancel();
    }

// ---------------------------------------------------------------------------
// CSPEtelConferenceCallRequester::MakeAddCallRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelConferenceCallRequester::MakeAddCallRequest( const TName& aCallName )
    {
    if ( !IsActive() )
        {
        iRequestType = EConferenceRequestTypeAddCall;
        iCall.AddCall( iStatus, aCallName );
        SetActive();
        }
    else
        {
        return KErrInUse;
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSPEtelConferenceCallRequester::MakeRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelConferenceCallRequester::MakeRequest( 
                                        TConferenceRequestType aRequest )
    {
    TInt result = KErrUnknown;

    if ( !IsActive() )
        {
        if ( aRequest != EConferenceRequestTypeAddCall )
            {
            iRequestType = aRequest;
            }
            
        CSPLOGSTRING2( CSPREQOUT, 
            "CSP: CSPEtelConferenceCallRequester::MakeRequest %d", iRequestType );
        switch ( aRequest )
            {
            case EConferenceRequestTypeAddCall:
                {
                // Requested through EConferenceRequestTypeAddCall as a special case because
                // of the parameters.
                result = KErrNotSupported;
                break;
                }
            case EConferenceRequestTypeHangup:
                {
                iCall.HangUp( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case EConferenceRequestTypeHold:
                {
                iCall.Swap( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case EConferenceRequestTypeResume:
                {
                iCall.Swap( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case EConferenceRequestTypeCreateConference:
                {
                iCall.CreateConference( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case EConferenceRequestTypeSwap:
                {
                iCall.Swap( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            default:
                {
                CSPLOGSTRING2( CSPERROR, 
                    "CSP: CSPEtelConferenceCallRequester::MakeRequest: Unspecified \
                                        request type: %d", iRequestType );
                result = KErrArgument;
                }
            }
        
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelConferenceCallRequester::MakeRequest: Already active" );
        
        if ( aRequest == EConferenceRequestTypeHangup )
            {
            // First cancel ongoing request.
            Cancel();

            iCall.HangUp( iStatus );
            SetActive();
            result = KErrNone;            
            }
        else
            {
            result = KErrInUse;
            }
        }
        
    return result;
    }

// ---------------------------------------------------------------------------
// CSPEtelConferenceCallRequester::RunL
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceCallRequester::RunL()
    {
    // from CActive
    CSPLOGSTRING2( CSPREQEND, 
        "CSP: CSPEtelConferenceCallRequester::RunL: status: %d", iStatus.Int() );
    
    if ( iStatus == KErrNone )
        {
        switch ( iRequestType )
            {
            case EConferenceRequestTypeAddCall:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Add call request OK" );
                break;
                }
            case EConferenceRequestTypeHangup:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Hangup request OK" );
                break;
                }
            case EConferenceRequestTypeHold:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Hold request OK" );
                break;
                }
            case EConferenceRequestTypeResume:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Resume request OK" );
                break;
                }
            case EConferenceRequestTypeCreateConference:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Conference create request OK" );
                break;
                }
            case EConferenceRequestTypeSwap:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Conference swap request OK" );
                break;
                }
            default:
                {
                CSPLOGSTRING2( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Unspecified request \
                                                type: %d", iRequestType );
                break;
                }
            }
        }
    else
        {
        // Error situations 
        
        CSPLOGSTRING2( CSPERROR, "CSP CSPEtelConferenceCallRequester::RunL: request \
                                completed with error: %d", iStatus.Int() );
        switch ( iRequestType )
            {
            case EConferenceRequestTypeAddCall:
                {
                CSPLOGSTRING( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Add call request" );
                iOwner.NotifyConferenceError( 
                           ECCPConferenceErrorAddCall );
                break;
                }
            case EConferenceRequestTypeHangup:
                {
                CSPLOGSTRING( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Hangup request" );
                iOwner.NotifyConferenceError( 
                           ECCPConferenceErrorHangUp );
                break;
                }
            case EConferenceRequestTypeHold:
                {
                CSPLOGSTRING( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Hold request" );
                iOwner.NotifyConferenceError( 
                           ECCPConferenceErrorHold );
                break;
                }
            case EConferenceRequestTypeResume:
                {
                CSPLOGSTRING( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Resume request err" );
                iOwner.NotifyConferenceError( 
                           ECCPConferenceErrorResume );
                break;
                }
            case EConferenceRequestTypeCreateConference:
                {
                CSPLOGSTRING( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Conference create request" );
                iOwner.NotifyConferenceError( 
                           ECCPConferenceErrorAddCall );
                break;
                }
            case EConferenceRequestTypeSwap:
                {
                CSPLOGSTRING( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Conference swap request" );
                iOwner.NotifyConferenceError( 
                           ECCPConferenceErrorSwap );

                break;
                }
            default:
                {
                CSPLOGSTRING2( CSPERROR, 
                    "CSP CSPEtelConferenceCallRequester::RunL: Unspecified request \
                                                type: %d", iRequestType );
                break;
                }
            }
        
        }    
    }

// ---------------------------------------------------------------------------
// CSPEtelConferenceCallRequester::DoCancel
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceCallRequester::DoCancel()
    {
    if ( IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, "CSP: CSPEtelConferenceCallRequester::DoCancel" );
        switch ( iRequestType )
            {
            case EConferenceRequestTypeAddCall:
                {
                iCall.CancelAsyncRequest( EMobileConferenceCallAddCall );
                break;
                }
            case EConferenceRequestTypeHangup:
                {
                iCall.CancelAsyncRequest( EMobileConferenceCallHangUp );
                break;
                }
            case EConferenceRequestTypeHold:
            case EConferenceRequestTypeResume:
                {
                iCall.CancelAsyncRequest( EMobileConferenceCallSwap );
                break;
                }
            case EConferenceRequestTypeCreateConference:
                {
                iCall.CancelAsyncRequest( EMobileConferenceCallCreateConference );
                break;
                }
            case EConferenceRequestTypeSwap:
                {
                iCall.CancelAsyncRequest( EMobileConferenceCallSwap );
                break;
                }
            default:
                {
                CSPLOGSTRING2( CSPERROR, "CSP CSPEtelConferenceCallRequester::DoCancel: \
                                Unspecified request type: %d", iRequestType );
                }
            }
        
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelConferenceCallRequester::DoCancel: Already active" );
        }
    }

// ---------------------------------------------------------------------------
// Constructs the requester.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceCallRequester::CSPEtelConferenceCallRequester(
            CSPConferenceCall& aOwner,
            RMobileConferenceCall& aCall ) : 
                CActive( EPriorityStandard ), 
                iOwner( aOwner ), 
                iCall ( aCall )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelConferenceCallRequester::CSPEtelConferenceCallRequester()" );
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// Constructing in the second phase.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceCallRequester::ConstructL()
    {
    }
    
// End of File

