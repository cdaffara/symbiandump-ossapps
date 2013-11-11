/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains phone engine central repository monitor base class
*
*/


// INCLUDE FILES
#include "cpepubsubmonitor.h"
#include <centralrepository.h>
#include <pepanic.pan>
#include <talogger.h>

// -----------------------------------------------------------------------------
// CPEPubSubMonitor::CPEPubSubMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEPubSubMonitor::CPEPubSubMonitor(
        MPEPhoneModelInternal& aModel
        ) : CActive( EPriorityStandard ),
            iModel(aModel)
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CPEPubSubMonitor::~CPEPubSubMonitor(
        // None
        )
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CPEPubSubMonitor::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEPubSubMonitor::BaseConstructL(
        TUid  aCategory,  // category of the property
        TUint aKey,       // key of the property
        TUint /*aType*/ ) // type of the property
    {
    #ifdef TF_LOGGING_ENABLED    
    TUidName name = aCategory.Name();
    
    TEFLOGSTRING3( KTAOBJECT, "CPEPubSubMonitor::BaseConstructL: Category %S, key %d", 
                   &name, aKey );                   
    
    iCategory = aCategory;
    iKey = aKey;
    #endif
           
    TInt error = iProperty.Attach( aCategory, aKey  );   
    if ( ( error!=KErrNone ) )
        {
        TEFLOGSTRING3( KTAOBJECT, 
            "PE CPEPubSubMonitor::BaseConstructL: Error while attaching. Category %S, key %d", 
                   &name, aKey );
        User::Leave(error);
        }
    

    SubmitNotifyRequestL();
    }

// -----------------------------------------------------------------------------
// CPEPubSubMonitor::Get
// Getting an integer value
// -----------------------------------------------------------------------------
//
TInt CPEPubSubMonitor::Get(
        TInt& aValue
        )
    {
    return ( iProperty.Get( aValue ) );
    }


// -----------------------------------------------------------------------------
// CPEPubSubMonitor::Get
// Getting a binary value
// -----------------------------------------------------------------------------
//
TInt CPEPubSubMonitor::Get(
        TDes8& aValue
        )
    {
    return ( iProperty.Get( aValue ) );
    }

// -----------------------------------------------------------------------------
// CPEPubSubMonitor::Get
// Getting a text value
// -----------------------------------------------------------------------------
//
TInt CPEPubSubMonitor::Get(
        TDes16& aValue
        )
    {
    return ( iProperty.Get( aValue ) );
    }


// -----------------------------------------------------------------------------
// CPEPubSubMonitor::SubmitNotifyRequestL
// -----------------------------------------------------------------------------
//
void CPEPubSubMonitor::SubmitNotifyRequestL(
        // None
        )
    {
    #ifdef TEF_LOGGING_ENABLED    
    TUidName name = iCategory.Name();
    #endif    
    TEFLOGSTRING3( KTAINT, "CPEPubSubMonitor::SubmitNotifyRequestL: Category %S, key %d", 
                   &name, iKey );
    
    __ASSERT_DEBUG( !IsActive(), Panic( EPEPanicPropertyAlreadyActive ) );
    iProperty.Subscribe( iStatus );
    SetActive( );
    }

// -----------------------------------------------------------------------------
// CPEPubSubMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CPEPubSubMonitor::DoCancel(
        // None
        )
    {
    #ifdef TF_LOGGING_ENABLED    
    TUidName name = iCategory.Name();
    #endif    
    TEFLOGSTRING3( KTAINT, "CPEPubSubMonitor::DoCancel: Category %S, key %d", 
                   &name, iKey );
    
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CPEPubSubMonitor::RunError
// -----------------------------------------------------------------------------
//
TInt CPEPubSubMonitor::RunError(
        #ifdef TEF_LOGGING_ENABLED
        TInt aError // Log the leavecode from RunL
        #else
        TInt /*aError*/
        #endif
        )
    {
    #ifdef TEF_LOGGING_ENABLED
    TUidName name = iCategory.Name(); 
    TEFLOGSTRING4( KTAERROR, "CPEPubSubMonitor::RunError %d, Category: %S, key %d",
                   aError, &name, iKey );
                   
    #endif
    return ( KErrNone );
    }

// -----------------------------------------------------------------------------
// CPEPubSubMonitor::RunL
// -----------------------------------------------------------------------------
//
void CPEPubSubMonitor::RunL(
        // None
        )
    {
    TEFLOGSTRING2( KTAREQEND, "CPEPubSubMonitor::RunL, status: %d", iStatus.Int() );

    // Don't resubmit the request on error
    User::LeaveIfError(iStatus.Int());
    SubmitNotifyRequestL();
    UpdateL();
    }

// End of file
