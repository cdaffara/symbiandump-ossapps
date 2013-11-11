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
* Description:  Internal operation class implementation
*
*/


// INCLUDE FILES
#include "cvtenginternaloperation.h"
#include "mvtengoperationobserver.h"
#include "CVtEngStateManager.h"
#include "TVtEngOperationUtils.h"
#include "VtEngUtils.h"
#include "cvtlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngOperation::NewL
// Symbian two-phase constructor
//
// -----------------------------------------------------------------------------
//
CVtEngInternalOperation* CVtEngInternalOperation::NewL(
    TVtEngCommandId aCommandId,
    CVtEngHandlerContainer& aHandlers,
    MVtEngOperationObserver& aObserver,
    TBool aDelete )
    {
    __VTPRINTENTER( "CVtEngInternalOperation.NewL<" )
    CVtEngInternalOperation* self = new ( ELeave )
        CVtEngInternalOperation( aCommandId, aHandlers, aObserver, aDelete );
    __VTPRINTEXIT( "CVtEngInternalOperation.NewL>" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngInternalOperation::CVtEngInternalOperation
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngInternalOperation::CVtEngInternalOperation(
    TVtEngCommandId aCommandId,
    CVtEngHandlerContainer& aHandlers,
    MVtEngOperationObserver& aObserver,
    TBool aDelete ) : CVtEngOperation( aCommandId, aHandlers, aObserver,
        aDelete )
    {
    __VTPRINTENTER( "CVtEngInternalOperation.CVtEngInternalOperation" )
    __VTPRINTEXIT( "CVtEngInternalOperation.CVtEngInternalOperation" )
    }

// -----------------------------------------------------------------------------
// CVtEngInternalOperation::~CVtEngInternalOperation
// Destructor
//
// -----------------------------------------------------------------------------
//
CVtEngInternalOperation::~CVtEngInternalOperation()
    {
    __VTPRINTENTER( "CVtEngInternalOperation.~" )
    __VTPRINTEXIT( "CVtEngInternalOperation.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngInternalOperation::HandleOpCompleteL
// Destructor
//
// -----------------------------------------------------------------------------
//
void CVtEngInternalOperation::HandleOpComplete( const TInt aResult )
    {
    __VTPRINTENTER( "CVtEngInternalOperation.HandleOpComplete" )
    #ifdef VTDEBUG
    TRAPD( err, iObserver.CommandCompleteL( iCommand, aResult ) );
    __VTPRINT2( DEBUG_GEN, "EngOp.Complete err=", err )
#else
    TRAP_IGNORE( iObserver.CommandCompleteL( iCommand, aResult ) );
#endif // VTDEBUG
    if ( iDelete )
        {
        delete this;
        }
    __VTPRINTEXITR( "CVtEngInternalOperation.HandleOpComplete %d>", aResult )
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// CVtEngInternalOperation::ExecuteL
// Destructor
//
// -----------------------------------------------------------------------------
//
TBool CVtEngInternalOperation::ExecuteL()
    {
    __VTPRINTENTER( "CVtEngInternalOperation.ExecuteL" )
    const CVtEngStateManager* states = CVtEngUtility::StateManager();
    TInt err( KErrNotReady );
    if ( states->IsCommandPossible( iCommand ) )
        {
        err = KErrNone;
        switch ( iCommand )
            {
            case KVtEngTerminateSession:
                TVtEngEngineInitUtil::HandleL( *this, iHandlers );
                break;
            case KVtEngMuteOutgoingAudio:
            case KVtEngUnmuteOutgoingAudio:
                TVtEngAudioPlaybackUtil::HandleL( *this, iHandlers );
                break;            
            default:
                __VTPRINT( DEBUG_GEN, "EngOp.ExecuteL not handled" )
                err = KErrNotSupported;
                break;
            }        
        }
    User::LeaveIfError( err );
    __VTPRINTEXIT( "CVtEngInternalOperation.ExecuteL" )
    return EFalse;
    }

//  End of File

