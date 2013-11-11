/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Preset interface implementation
*
*/


#include <ecom.h>
#include <s32mem.h>

#include "pscommon.h"
#include "pspresetinterface.h"
#include "pspresetnotifier.h"
#include "pstransaction.h"
#include "psdebug.h"
#include "psservsession.h"

const TInt KPSExternalizeBufGranularity = 32; // Granularity of the preset data externalisation buffer.

/**
 * Preset interface initialization parameters.
 */
typedef struct
    {
    /** Session with the preset server. */
    RPSServ* iServ;
    /** Data handler of the preset. */
    TUid iDataHandler;
    /** Id of the preset. */
    TInt iId;
    } TPSPresetInterfaceParams;

// ---------------------------------------------------------------------------
// Panics the process.
// ---------------------------------------------------------------------------
//
void Panic( TInt aReason )
    {
    _LIT( category, "CPSPresetInterface" );
    User::Panic( category, aReason );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSPresetInterface* CPSPresetInterface::CreateL( RPSServ& aServ, TInt aId, TUid aDataHandler )
    {
    TPSPresetInterfaceParams params = { &aServ, aDataHandler, aId };
    CPSPresetInterface* self = static_cast<CPSPresetInterface*>( REComSession::CreateImplementationL( aDataHandler, _FOFF( CPSPresetInterface, iDtor ), &params ) );
    CleanupStack::PushL( self );
    self->UpdatePresetL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSPresetInterface::~CPSPresetInterface()
    {
    delete iName;
    delete iNotifier;
    
    if ( iSession )
        {
        iSession->Close();
        }
    delete iSession;
    
    REComSession::DestroyedImplementation( iDtor );
    }

// ---------------------------------------------------------------------------
// Creates a transaction.
// Transfers ownership.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSTransaction* CPSPresetInterface::CreateTransactionL()
    {
    CPSTransaction* transaction = CreateTransactionLC();
    CleanupStack::Pop( transaction );
    return transaction;
    }

// ---------------------------------------------------------------------------
// Creates a transaction and leaves it on the cleanup stack.
// Transfers ownership.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSTransaction* CPSPresetInterface::CreateTransactionLC()
    {
    CPSTransaction* transaction = CPSTransaction::NewL( *this );
    CleanupStack::PushL( transaction );
    return transaction;
    }

// ---------------------------------------------------------------------------
// Sets the name of the preset.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSPresetInterface::SetNameL( const TDesC& aName )
    {
    ValidateDataChange();

    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }

// ---------------------------------------------------------------------------
// Returns the name of the preset.
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPSPresetInterface::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// Sets the index of the preset.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSPresetInterface::SetIndex( TInt aIndex )
    {    
    ValidateDataChange();

    iIndex = aIndex;
    }

// ---------------------------------------------------------------------------
// Returns the index of the preset.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CPSPresetInterface::Index() const
    {
    return iIndex; 
    }

// ---------------------------------------------------------------------------
// Returns the id of the preset.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CPSPresetInterface::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// Returns the data handler of the preset.
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CPSPresetInterface::DataHandler() const
    {
    return iDataHandler;
    }

// ---------------------------------------------------------------------------
// Performs base construction on the preset.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSPresetInterface::BaseConstructL( TAny* aParams )
    {
    const TPSPresetInterfaceParams& params = *static_cast<TPSPresetInterfaceParams*>( aParams );
    
    iDataHandler = params.iDataHandler;
    iId = params.iId;

    iSession = new ( ELeave ) RPSServSession( *params.iServ, iId );
    iNotifier = CPSPresetNotifier::NewL( *params.iServ, *this, iId );
    }

// ---------------------------------------------------------------------------
// Validates the preset for data change.
// ---------------------------------------------------------------------------
//  
EXPORT_C void CPSPresetInterface::ValidateDataChange()
    {
    __ASSERT_ALWAYS( iTransaction, Panic( KErrNotReady ) );
    }

// ---------------------------------------------------------------------------
// Handles changes in the preset.
// ---------------------------------------------------------------------------
//  
EXPORT_C void CPSPresetInterface::HandlePresetChangedL( TInt PSDEBUGVAR( aId ), TUid /*aDataHandler*/, MPSPresetObserver::TPSReason aReason )
    {
    __ASSERT_DEBUG( iId == aId, Panic( KErrCorrupt ) );

    switch ( aReason )
        {
        case MPSPresetObserver::EPSModified:
            UpdatePresetL();
            break;            
        case MPSPresetObserver::EPSDeleted:
            // Preset deleted from the database, close the session.
            iSession->Close();
            delete iSession;
            iSession = NULL;                            
            break;
        case MPSPresetObserver::EPSCreated: // Falls through.                
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Begins the transaction.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSPresetInterface::BeginTransactionL()
    {
    if( iSession )
        {
        iSession->BeginTransactionL();
        iTransaction = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// Commits the transaction.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSPresetInterface::CommitL()
    {
    if( iSession )
        {
        iSession->SetL( EPSOpSetPresetName, *iName );
        iSession->SetL( EPSOpSetPresetIndex, iIndex );    
    
        CBufFlat* buf = CBufFlat::NewL( KPSExternalizeBufGranularity );
        CleanupStack::PushL( buf );
        RBufWriteStream stream( *buf );
        CleanupClosePushL( stream );
        ExternalizeL( stream );
        stream.CommitL();
        CleanupStack::PopAndDestroy( &stream );
        iSession->SetL( EPSOpSetPresetData, buf->Ptr( 0 ) );
        CleanupStack::PopAndDestroy( buf );

        iSession->CommitL();
    
        iTransaction = EFalse;
        }        
    }
    
// ---------------------------------------------------------------------------
// Reverts all changes done during the transaction.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSPresetInterface::Rollback()
    {
    if( iSession )
        {
        iSession->Rollback();
        iTransaction = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Updates the preset.
// ---------------------------------------------------------------------------
//  
void CPSPresetInterface::UpdatePresetL()
    {
    if( iSession )
        {
        iSession->GetL( EPSOpGetPresetIndex, iIndex );
    
        delete iName;
        iName = NULL;
    
        iSession->GetL( EPSOpGetPresetName, EPSOpGetPresetNameLength, iName );

        HBufC8* data;
        iSession->GetL( EPSOpGetPresetData, EPSOpGetPresetDataLength, data );
        CleanupStack::PushL( data );
    
        if ( data->Length() )
            {
            // There was packed data in the database, implementing plugin can unpack it now.
            RDesReadStream stream( *data );
            CleanupClosePushL( stream );
            InternalizeL( stream );
            CleanupStack::PopAndDestroy( &stream );
            }

        CleanupStack::PopAndDestroy( data );
        }
    }

