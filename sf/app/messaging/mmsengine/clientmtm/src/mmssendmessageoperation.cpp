/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Operation class for SendAs API send support
*
*/




// INCLUDE FILES
#include    "mmssendmessageoperation.h"
#include    "mmscmds.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsSendMessageOperation::CMmsSendMessageOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsSendMessageOperation::CMmsSendMessageOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus ):
    CMsvSendOperation( aMsvSession, aObserverRequestStatus )
    {
    }

// -----------------------------------------------------------------------------
// CMmsSendMessageOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsSendMessageOperation::ConstructL(
    const CMsvEntrySelection& aSelection,
    const TDesC8& aParameter )
    {
    iOperation = iMsvSession.TransferCommandL(
        aSelection,
        EMmsScheduledSend,
        (TDesC8&)aParameter,
        iStatus);
    // assigns iMtm, iService and sets active
    Start(iOperation);
    }

// -----------------------------------------------------------------------------
// CMmsSendMessageOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsSendMessageOperation* CMmsSendMessageOperation::NewL(
    CMsvSession& aMsvSession, 
    const CMsvEntrySelection& aSelection, 
    const TDesC8& aParameter, 
    TRequestStatus& aObserverRequestStatus )
    {
    CMmsSendMessageOperation* self = new( ELeave ) CMmsSendMessageOperation(
        aMsvSession, aObserverRequestStatus );
    
    CleanupStack::PushL( self );
    self->ConstructL( aSelection, aParameter );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CMmsSendMessageOperation::~CMmsSendMessageOperation()
    {
    
    }


// -----------------------------------------------------------------------------
// CMmsSendMessageOperation::TranslateProgress
// -----------------------------------------------------------------------------
//
const TDesC8& CMmsSendMessageOperation::TranslateProgress(const TDesC8& /* aProgress */)
    {
    // so far MMS server mtm does not provide any progress information
/*
    TMsvLocalOperationProgress& progress = (TMsvLocalOperationProgress&) aProgress;
*/    
    
    // When progress is not supported, only zeroes are returned.
    iProgress().iProgressMax = 0;
    iProgress().iProgress = 0;

    // get error
    iProgress().iError = KErrNone;

    return iProgress;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
