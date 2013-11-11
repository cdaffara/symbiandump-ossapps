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
* Description:  Implementation of RPSServSession
*
*/


#include "pscommon.h"
#include "psserv.h"
#include "psservsession.h"

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
RPSServSession::RPSServSession( RPSServ& aServ, TInt aId )
    : iPresetServ( aServ ), iId( aId )
    {
    }

// ---------------------------------------------------------------------------
// Closes the session.
// ---------------------------------------------------------------------------
//
void RPSServSession::Close()
    {
    Rollback();
    }

// ---------------------------------------------------------------------------
// Begins a transaction.
// ---------------------------------------------------------------------------
//
void RPSServSession::BeginTransactionL()
    {
    User::LeaveIfError( iPresetServ.SendReceive( EPSOpStartTransaction, TIpcArgs( iId ) ) ); 
    }

// ---------------------------------------------------------------------------
// Commits the transaction.
// ---------------------------------------------------------------------------
//
void RPSServSession::CommitL()
    {
    User::LeaveIfError( iPresetServ.SendReceive( EPSOpCommitTransaction, TIpcArgs( iId ) ) );
    }

// ---------------------------------------------------------------------------
// Reverts all changes done during the transaction.
// ---------------------------------------------------------------------------
//
void RPSServSession::Rollback()
    {
    iPresetServ.Send( EPSOpRollbackTransaction, TIpcArgs( iId ) );
    }

// ---------------------------------------------------------------------------
// Sets an integer value.
// ---------------------------------------------------------------------------
//
void RPSServSession::SetL( TPSOpCode aServerCommand, TInt aValue )
    {
    User::LeaveIfError( iPresetServ.SendReceive( aServerCommand, TIpcArgs( iId, aValue ) ) );
    }

// ---------------------------------------------------------------------------
// Gets an integer value.
// ---------------------------------------------------------------------------
//
void RPSServSession::GetL( TPSOpCode aServerCommand, TInt& aValue )
    {
    TPckgBuf<TInt> value;
    User::LeaveIfError( iPresetServ.SendReceive( aServerCommand, TIpcArgs( iId, &value) ) );
    aValue = value();
    }

// ---------------------------------------------------------------------------
// Sets an 8-bit descriptor value.
// ---------------------------------------------------------------------------
//
void RPSServSession::SetL( TPSOpCode aServerCommand, const TDesC8& aValue )
    {
    User::LeaveIfError( iPresetServ.SendReceive( aServerCommand, TIpcArgs( iId, &aValue) ) );
    }

// ---------------------------------------------------------------------------
// Gets a 8-bit descriptor value.
// ---------------------------------------------------------------------------
//
void RPSServSession::GetL( TPSOpCode aServerCommand, TPSOpCode aLengthCommand, HBufC8*& aValue )
    {
    TPckgBuf<TInt> length;
    User::LeaveIfError( iPresetServ.SendReceive( aLengthCommand, TIpcArgs( iId, &length ) ) );

    HBufC8* buf = HBufC8::NewLC( length() );
    TPtr8 ptr( buf->Des() );

    User::LeaveIfError( iPresetServ.SendReceive( aServerCommand, TIpcArgs( iId, &ptr ) ) );
    
    CleanupStack::Pop( buf );
    
    aValue = buf;
    }
    
// ---------------------------------------------------------------------------
// Sets a 16-bit descriptor value.
// ---------------------------------------------------------------------------
//
void RPSServSession::SetL( TPSOpCode aServerCommand, const TDesC& aValue )
    {
    User::LeaveIfError( iPresetServ.SendReceive( aServerCommand, TIpcArgs( iId, &aValue) ) );
    }

// ---------------------------------------------------------------------------
// Gets a 16-bit descriptor value.
// ---------------------------------------------------------------------------
//
void RPSServSession::GetL( TPSOpCode aServerCommand, TPSOpCode aLengthCommand, HBufC*& aValue )
    {
    TPckgBuf<TInt> length;
    User::LeaveIfError( iPresetServ.SendReceive( aLengthCommand, TIpcArgs( iId, &length ) ) );

    HBufC* buf = HBufC::NewLC( length() );
    TPtr ptr( buf->Des() );

    User::LeaveIfError( iPresetServ.SendReceive( aServerCommand, TIpcArgs( iId, &ptr ) ) );

    CleanupStack::Pop( buf );
    
    aValue = buf;
    }
