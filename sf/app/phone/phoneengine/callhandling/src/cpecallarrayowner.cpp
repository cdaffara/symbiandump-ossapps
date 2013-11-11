/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPECallArrayOwner class 
*                member functions.
*
*/


// INCLUDE FILES
#include <talogger.h>
#include <pepanic.pan>
#include <ccce.h>

#include "cpecallarrayowner.h"
#include "cpesinglecall.h"
#include "cpevideocallhandling.h"

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPECallArrayOwner::CPECallArrayOwner
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPECallArrayOwner::CPECallArrayOwner()
    {
    }

// Destructor
CPECallArrayOwner::~CPECallArrayOwner()
    {
    TEFLOGSTRING( KTAOBJECT, 
        "CALL CPECallArrayOwner::~CPECallArrayOwner: Start." );
    
    if ( iCallArray )
        {
        iCallArray->ResetAndDestroy();
        iCallArray->Close();
        delete iCallArray;
        iCallArray = NULL;
        }
        
    TEFLOGSTRING( KTAOBJECT, 
        "CALL: CPECallArrayOwner::~CPECallArrayOwner: Complete." );
    }    

CPECallArrayOwner* CPECallArrayOwner::NewL(
        CCCE& iConvergedCallEngine, 
        MPEMessageSender& aOwner )
    {
    TEFLOGSTRING( KTAOBJECT,
         "CALL CPECallArrayOwner::NewL: start." );
    
    CPECallArrayOwner* self = new ( ELeave ) CPECallArrayOwner();
    CleanupStack::PushL( self );
    self->ConstructL( iConvergedCallEngine, aOwner );
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, 
        "CALL CPECallArrayOwner::NewL: complete." );
    
    return self;
    }
    
void CPECallArrayOwner::ConstructL(
        CCCE& iConvergedCallEngine, 
        MPEMessageSender& aOwner )

    {
    TEFLOGSTRING( KTAOBJECT,
         "CALL CPECallArrayOwner::ConstructL: start." );
    
    iCallArray = new ( ELeave ) RPointerArray< CPESingleCall >;
    
    // Open emergency call, this done here so that phone app doesn't allocate memory
    // when emeregency call is really created.
    CPESingleCall* call = CreateEmergencyCallL( aOwner );
    call->SetCall( iConvergedCallEngine.InitializeEmergencyCall( *call ) );
        
    TEFLOGSTRING( KTAOBJECT, 
        "CALL CPECallArrayOwner::ConstructL: complete." );
    }
    
// -----------------------------------------------------------------------------
// CPECallArrayOwner::CreateCallL
// 
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallArrayOwner::CreateCallL( MPEMessageSender& aOwner )
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::CreateCallL");
    CPESingleCall* call( NULL );

    if( iCallArray->Count() < ( KPEMaximumNumberOfVoiceCalls ) )
        {
        // Find free call id
        for( TInt callId = 0; callId < KPEMaximumNumberOfVoiceCalls; callId++ )
            {
            if( !GetCallObject( callId ) )
                {
                call = CPESingleCall::NewL( aOwner );
                call->SetCallId( callId );
                iCallArray->Append( call );
                break;
                }
            }
        }
    else
        {
        TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::CreateCallL Leave( ECCPRequestFailure )");
        User::Leave( ECCPRequestFailure );
        }
    return call;
    }
    
// -----------------------------------------------------------------------------
// CPECallArrayOwner::CallByCallId
// 
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallArrayOwner::GetCallObject( 
        const TInt aCallId ) const
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::GetCallObject");
    CPESingleCall* call( NULL );
    TInt callIndex;

    __ASSERT_DEBUG( 
        aCallId <= ( KPEMaximumNumberOfVoiceCalls + KPEMaximumNumberOfDataCalls ),
        Panic( EPEPanicIndexOutOfRange) );
        
    for( callIndex = 0; callIndex < iCallArray->Count(); callIndex++ )
        {
        if( aCallId == (*iCallArray)[callIndex]->GetCallId() )
            {
            call = (*iCallArray)[callIndex];
            TEFLOGSTRING2( KTAMESINT, "CALL CPECallArrayOwner::GetCallObject %d", callIndex);
            break;
            }
        }
    return call;
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::GetCallObjectByIndex
// 
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallArrayOwner::GetCallObjectByIndex( 
        const TInt aIndex ) const
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::GetCallObjectByIndex");

    __ASSERT_DEBUG( 
        aIndex < ( KPEMaximumNumberOfVoiceCalls ),
        Panic( EPEPanicIndexOutOfRange) );
        
    return aIndex < Count() ? (*iCallArray)[aIndex] : NULL;
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::DeleteCallObject
// 
// -----------------------------------------------------------------------------
//
TInt CPECallArrayOwner::DeleteCallObject( 
        const TInt aCallId )
    {
    TEFLOGSTRING2( KTAMESINT, "CALL CPECallArrayOwner::DeleteCallObject, call id: %d", aCallId );
    CPESingleCall* call( NULL );
    TInt callIndex;
    TInt errorCode( KErrNotFound );

    __ASSERT_DEBUG( 
        aCallId <= ( KPEMaximumNumberOfVoiceCalls + KPEMaximumNumberOfDataCalls ),
        Panic( EPEPanicIndexOutOfRange) );
    
    if( aCallId != KPEEmergencyCallId )
        {
        // Normal Voice Calls
        for( callIndex = 0; callIndex < iCallArray->Count(); callIndex++ )
            {
            if( aCallId == (*iCallArray)[callIndex]->GetCallId() )
                {
                call = (*iCallArray)[callIndex];
                delete call;
                iCallArray->Remove(callIndex);
                iCallArray->Compress();
                errorCode = KErrNone;
                break;
                }
            }
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::Count()
// 
// -----------------------------------------------------------------------------
//
TInt CPECallArrayOwner::Count() const
    {
    TEFLOGSTRING2( KTAMESINT, "CALL CPECallArrayOwner::Count %d", iCallArray->Count() );
    return iCallArray->Count();
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::ActiveCallCount()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallArrayOwner::ActiveCallCount() const
    {
    TInt numberOfCalls = 0;

    // Count ongoing voice calls
    numberOfCalls = iCallArray->Count();
    
    for( TInt callIndex = 0; 
         callIndex < iCallArray->Count();
         callIndex++ )
        {
        if( EPEStateIdle == (*iCallArray)[callIndex]->GetCallState() )
            {
            numberOfCalls--;
            }
        }
    TEFLOGSTRING2( KTAMESINT, "CALL CPECallArrayOwner::ActiveCallCount %d", numberOfCalls );

    return numberOfCalls;
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::CreateEmercencyCallL
// Method 
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallArrayOwner::CreateEmergencyCallL( MPEMessageSender& aOwner )
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::CreateEmercencyCallL");
    CPESingleCall* call = GetCallObject( KPEEmergencyCallId );
    
    if( !call )
        {
        call = CPESingleCall::NewL( aOwner );
        call->SetCallId( KPEEmergencyCallId );
        iCallArray->Append( call );
        }
    else
        {
        TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::CreateEmercencyCallL: Already exist");
        User::Leave( KErrAlreadyExists );
        }

    return call;
    }
    
    
// -----------------------------------------------------------------------------
// CPECallArrayOwner::CreateDataCallL
// 
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallArrayOwner::CreateDataCallL( MPEMessageSender& aOwner )
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::CreateDataCallL");
    CPESingleCall* call( NULL );

    if( iCallArray->Count() < ( KPEMaximumNumberOfVoiceCalls ) )
        {
        // find free call id
        for( TInt callId = KPEDataCallIdBase; 
            callId < KPEDataCallIdBase + KPEMaximumNumberOfDataCalls; callId++ )
            {
            if( !GetCallObject( callId ) )
                {
                call = CPESingleCall::NewL( aOwner );
                call->SetCallId( callId );
                iCallArray->Append( call );
                break;
                }
            }
        if( !call )
            {
            // Leave if all datacallids are reserved
            TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::CreateCallL all datacallids are reserved Leave( ECCPRequestFailure )");
            User::Leave( ECCPRequestFailure );
            }
        }
    else
        {
        TEFLOGSTRING( KTAMESINT, "CALL CPECallArrayOwner::CreateCallL Leave( ECCPRequestFailure )");
        User::Leave( ECCPRequestFailure );
        }
    return call;
    }
    
// -----------------------------------------------------------------------------
// CPECallArrayOwner::CallPointerByState
// returns CPESingleCall voice call object by State, NULL if not found
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallArrayOwner::CallPointerByState( 
        TPEState aState ) const
    {
    CPESingleCall* returnValue = NULL;
    
    TEFLOGSTRING2( 
        KTAREQEND, 
        "CALL CPECallHandling::CallPointerByState: asked aState: %d", 
        aState );
    
    for( TInt callIndex = 0; 
         callIndex < iCallArray->Count();
         callIndex++ )
        {
        if( aState == (*iCallArray)[callIndex]->GetCallState() )
            {
            TEFLOGSTRING2( 
                KTAREQEND, 
                "CALL CPECallHandling::CallPointerByState: callIndex: %d", 
                callIndex );
            returnValue = (*iCallArray)[callIndex];
            break;
            }
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::IsCallByState
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPECallArrayOwner::IsCallByState( 
        const TPEState aState ) const
    {
    TEFLOGSTRING2( 
        KTAREQEND, 
        "CALL CPECallHandling::CallByState: asked aState: %d", 
        aState );
  
    return CallByState( aState ) ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::IsCallByCallId
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPECallArrayOwner::IsCallByCallId( 
        const TInt aCallId ) const
    {
    TEFLOGSTRING2( 
        KTAREQEND, 
        "CALL CPECallHandling::CallByCallId: asked aCallId: %d", 
        aCallId );

    return GetCallObject( aCallId ) ? ETrue : EFalse;
    }
    
// -----------------------------------------------------------------------------
// CPECallArrayOwner::CallByState
// -----------------------------------------------------------------------------
//
EXPORT_C MPECall* CPECallArrayOwner::CallByState( 
        const TPEState aState ) const
    {
    TEFLOGSTRING2( 
        KTAREQEND, 
        "CALL CPECallHandling::CallByState: asked aState: %d", 
        aState );
  
    return CallPointerByState( aState );
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::CallByCallId
// -----------------------------------------------------------------------------
//
EXPORT_C MPECall* CPECallArrayOwner::CallByCallId( 
        const TInt aCallId ) const
    {
    TEFLOGSTRING2( 
        KTAREQEND, 
        "CALL CPECallHandling::CallByCallId: asked aCallId: %d", 
        aCallId );

    return GetCallObject( aCallId );
    }

// -----------------------------------------------------------------------------
// CPECallArrayOwner::CallByName
// -----------------------------------------------------------------------------
//
EXPORT_C MPECall* CPECallArrayOwner::CallByName( 
        const TName& aName ) const
    {
    TEFLOGSTRING2( 
        KTAREQEND, 
        "CALL CPECallHandling::CallByName: aName: %S", 
        &aName );
    return CallPointerByName( aName );      
    }
    
// -----------------------------------------------------------------------------
// CPECallArrayOwner::CallPointerByName
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallArrayOwner::CallPointerByName( 
        const TName& aName ) const
    {
    CPESingleCall* call( NULL );
    for ( TInt index=0; index < Count(); index++ )
        {
        CPESingleCall* callData = GetCallObjectByIndex( index );
        if( callData )
            {
            RMobileCall::TMobileCallInfoV3 callInfo;
            callData->GetCallInfo( callInfo );
            if( callInfo.iCallName == aName )
                {
                call = callData;
                break;
                }
            }
        }
    TEFLOGSTRING2( 
        KTAREQEND, 
        "CALL CPECallHandling::CallPointerByName: call: %d", 
        call );
    return call;      
    }
    
// ================= OTHER EXPORTED FUNCTIONS ==============================

// End of File 
