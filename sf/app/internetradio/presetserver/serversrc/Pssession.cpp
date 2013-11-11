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
* Description:  Defines a session with the preset server
*
*/

#include <s32mem.h>

#include "pscommon.h"
#include "psdebug.h"
#include "psserver.h"
#include "pssession.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSSession* CPSSession::NewL( CPSServer& aServer )
    {
    CPSSession* self = new ( ELeave ) CPSSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPSSession::CPSSession( CPSServer& aServer )
    : CSession2(), iPresetServer( aServer )
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPSSession::ConstructL()
    {
    iPresetServer.IncrementSessionCount();
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPSSession::~CPSSession()
    {
    iPresetServer.DecrementSessionCount();
    }

// ---------------------------------------------------------------------------
// From class CSession2
// Invoked when a service request is made.
// ---------------------------------------------------------------------------
//
void CPSSession::ServiceL( const RMessage2& aMessage )
    {
    PSDEBUG2( "CPSSession::ServiceL >> OpCode = %d", static_cast<TInt>( aMessage.Function() ) );

    TBool messageHandled = ETrue;

    TRAPD( err, messageHandled = DoServiceL( aMessage ) )
    
    if ( messageHandled && !aMessage.IsNull() )
        {
        PSDEBUG2( "\tMessage was handled >> err = %d", err );
        aMessage.Complete( err );
        }

    PSDEBUG( "CPSSession::ServiceL <<" );
    }

// ---------------------------------------------------------------------------
// Service request implementation.
// ---------------------------------------------------------------------------
//
TBool CPSSession::DoServiceL( const RMessage2& aMessage )
    {
    TPSOpCode opCode = static_cast<TPSOpCode>( aMessage.Function() );
    TBool messageHandled = ETrue;

    switch ( opCode )
        {
        case EPSOpCreatePreset:
            CreatePresetL( aMessage );
        break;
        case EPSOpDeletePreset:
            DeletePresetL( aMessage );
        break;
        case EPSOpStartTransaction:
            StartTransactionL( aMessage );
        break;
        case EPSOpCommitTransaction:
            CommitTransactionL( aMessage );
        break;
        case EPSOpRollbackTransaction:
            RollbackTransaction( aMessage );
        break;
        case EPSOpMovePreset:
            MovePresetL( aMessage );
        break;
        case EPSOpNotify:
            NotifyPresetChangeL( aMessage );
            messageHandled = EFalse;
        break;
        case EPSOpCancelNotify:
            CancelNotifyPresetChange( aMessage );
        break;
        case EPSOpSetPresetName:
            SetPresetNameL( aMessage );
        break;
        case EPSOpSetPresetIndex:
            SetPresetIndexL( aMessage );
        break;
        case EPSOpSetPresetData:
            SetPresetDataL( aMessage );
        break;
        case EPSOpGetPresetName:
            GetPresetNameL( aMessage );
        break;
        case EPSOpGetPresetNameLength:
            GetPresetNameLengthL( aMessage );
        break;
        case EPSOpGetPresetIndex:
            GetPresetIndexL( aMessage );
        break;
        case EPSOpGetPresetDataHandler:
            GetPresetDataHandlerL( aMessage );
        break;
        case EPSOpGetPresetData:
            GetPresetDataL( aMessage );
        break;
        case EPSOpGetPresetDataLength:
            GetPresetDataLengthL( aMessage );
        break;
        case EPSOpGetPresets:
            GetPresetsL( aMessage );
        break;
        case EPSOpGetPresetsLength:
            GetPresetsLengthL( aMessage );
        break;
        default:
            iPresetServer.PanicClient( KErrArgument );
        break;
        }

    return messageHandled;
    }

// ---------------------------------------------------------------------------
// From class CSession2
// Invoked when a session is disconnected.
// ---------------------------------------------------------------------------
//
void CPSSession::Disconnect(const RMessage2 &aMessage)
    {
    iPresetServer.SessionDisconnected( aMessage );
    
    CSession2::Disconnect( aMessage );
    }

// ---------------------------------------------------------------------------
// Handles a create preset message.
// ---------------------------------------------------------------------------
//
void CPSSession::CreatePresetL( const RMessage2& aMessage )
    {
    TInt id = iPresetServer.Database().CreatePresetL( aMessage.Int0(), TUid::Uid( aMessage.Int1() ) );
    TPckgBuf<TInt> idBuf( id );
    aMessage.WriteL( 2, idBuf );
    }

// ---------------------------------------------------------------------------
// Handles a delete preset message.
// ---------------------------------------------------------------------------
//
void CPSSession::DeletePresetL( const RMessage2& aMessage )
    {
    iPresetServer.Database().DeletePresetL( aMessage.Int0() );
    }

// ---------------------------------------------------------------------------
// Handles a start transaction message.
// ---------------------------------------------------------------------------
//
void CPSSession::StartTransactionL( const RMessage2& aMessage )
    {
    iPresetServer.AppendPendingPresetL( aMessage.Int0() );
    }

// ---------------------------------------------------------------------------
// Handles a commit transaction message.
// ---------------------------------------------------------------------------
//
void CPSSession::CommitTransactionL( const RMessage2& aMessage )
    {
    iPresetServer.CommitPendingPresetL( aMessage.Int0() );
    }

// ---------------------------------------------------------------------------
// Handles a rollback transaction message.
// ---------------------------------------------------------------------------
//
void CPSSession::RollbackTransaction( const RMessage2& aMessage )
    {
    iPresetServer.RemovePendingPreset( aMessage.Int0() );
    }

// ---------------------------------------------------------------------------
// Handles a move preset message.
// ---------------------------------------------------------------------------
//
void CPSSession::MovePresetL( const RMessage2& aMessage )
    {
    iPresetServer.Database().MovePresetL( aMessage.Int0(), aMessage.Int1() );
    }

// ---------------------------------------------------------------------------
// Handles a notify preset change message.
// ---------------------------------------------------------------------------
//
void CPSSession::NotifyPresetChangeL( const RMessage2& aMessage )
    {
    iPresetServer.AppendPresetObserverL( aMessage );
    }

// ---------------------------------------------------------------------------
// Handles a cancel notify preset change message.
// ---------------------------------------------------------------------------
//
void CPSSession::CancelNotifyPresetChange( const RMessage2& aMessage )
    {
    iPresetServer.RemovePresetObserver( aMessage );
    }

// ---------------------------------------------------------------------------
// Handles a set preset name message.
// ---------------------------------------------------------------------------
//
void CPSSession::SetPresetNameL( const RMessage2& aMessage )
    {
    HBufC* name = HBufC::NewLC( aMessage.GetDesLengthL( 1 ) );
    TPtr namePtr( name->Des() );
    aMessage.ReadL( 1, namePtr );
    iPresetServer.SetPendingPresetNameL( aMessage.Int0(), namePtr );
    CleanupStack::PopAndDestroy( name );
    }

// ---------------------------------------------------------------------------
// Handles a set preset index message.
// ---------------------------------------------------------------------------
//
void CPSSession::SetPresetIndexL( const RMessage2& aMessage )
    {
    iPresetServer.SetPendingPresetIndexL( aMessage.Int0(), aMessage.Int1() );
    }

// ---------------------------------------------------------------------------
// Handles a set preset data message.
// ---------------------------------------------------------------------------
//
void CPSSession::SetPresetDataL( const RMessage2& aMessage )
    {
    HBufC8* data = HBufC8::NewLC( aMessage.GetDesLengthL( 1 ) );
    TPtr8 dataPtr( data->Des() );
    aMessage.ReadL( 1, dataPtr );
    iPresetServer.SetPendingPresetDataL( aMessage.Int0(), dataPtr );
    CleanupStack::PopAndDestroy( data );
    }

// ---------------------------------------------------------------------------
// Handles a get preset name message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetNameL( const RMessage2& aMessage )
    {
    HBufC* name = iPresetServer.Database().PresetNameL( aMessage.Int0() );
    CleanupStack::PushL( name );
    aMessage.WriteL( 1, *name );
    CleanupStack::PopAndDestroy( name );
    }

// ---------------------------------------------------------------------------
// Handles a get preset name length message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetNameLengthL( const RMessage2& aMessage )
    {
    TInt length = iPresetServer.Database().PresetNameLengthL( aMessage.Int0() );
    TPckgBuf<TInt> pckg( length );
    aMessage.WriteL( 1, pckg );
    }

// ---------------------------------------------------------------------------
// Handles a get preset index message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetIndexL( const RMessage2& aMessage )
    {
    TPckgBuf<TInt> pckg( iPresetServer.Database().PresetIndexL( aMessage.Int0() ) );
    aMessage.WriteL( 1, pckg );
    }

// ---------------------------------------------------------------------------
// Handles a get preset data handler message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetDataHandlerL( const RMessage2& aMessage )
    {
    TPckgBuf<TInt> pckg( iPresetServer.Database().PresetDataHandlerL( aMessage.Int0() ).iUid );
    aMessage.WriteL( 1, pckg );
    }

// ---------------------------------------------------------------------------
// Handles a get preset data message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetDataL( const RMessage2& aMessage )
    {
    HBufC8* data = iPresetServer.Database().PresetDataL( aMessage.Int0() );
    CleanupStack::PushL( data );
    aMessage.WriteL( 1, *data );
    CleanupStack::PopAndDestroy( data );
    }

// ---------------------------------------------------------------------------
// Handles a get preset data length message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetDataLengthL( const RMessage2& aMessage )
    {
    TInt length = iPresetServer.Database().PresetDataLengthL( aMessage.Int0() );
    TPckgBuf<TInt> pckg( length );
    aMessage.WriteL( 1, pckg );
    }

// ---------------------------------------------------------------------------
// Handles a get presets message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetsL( const RMessage2& aMessage )
    {
    RArray<TInt> presets;
    CleanupClosePushL( presets );
    
    iPresetServer.Database().GetPresetListL( presets, TUid::Uid( aMessage.Int0() ) );

    TInt count = presets.Count();

    HBufC8* buf = HBufC8::NewLC( (presets.Count() + 1) * KDefaultRealWidth );
    TPtr8 ptr( buf->Des() );
    
    RDesWriteStream stream( ptr );
    CleanupClosePushL( stream );

    stream << TCardinality( count );
    
    for ( TInt i = 0; i < count; i++ )
        {
        stream.WriteInt32L( presets[i] );
        }
    
    stream.CommitL();

    aMessage.WriteL( 1, *buf );
    
    CleanupStack::PopAndDestroy( 3, &presets );
    }

// ---------------------------------------------------------------------------
// Handles a get presets length message.
// ---------------------------------------------------------------------------
//
void CPSSession::GetPresetsLengthL( const RMessage2& aMessage )
    {
    RArray<TInt> presets;
    CleanupClosePushL( presets );
    
    iPresetServer.Database().GetPresetListL( presets, TUid::Uid( aMessage.Int0() ) );

    aMessage.WriteL( 1, TPckgC<TInt>( (presets.Count() + 1) * KDefaultRealWidth ) );
    
    CleanupStack::PopAndDestroy( &presets );
    }

// ======== GLOBAL FUNCTIONS ========
