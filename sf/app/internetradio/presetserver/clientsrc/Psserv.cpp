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
* Description:  Preset server client implementation
*
*/


#include "pspresetinterface.h"
#include "psserv.h"

// ======== LOCAL FUNCTIONS ========

namespace 
    {

    // ---------------------------------------------------------------------------
    // Panics the application with the supplied reason.
    // ---------------------------------------------------------------------------
    //
    void Panic( TInt aReason )
        {
        _LIT( panic, "PSServ" );
        User::Panic( panic, aReason );
        }

    // ---------------------------------------------------------------------------
    // Checks whether or not the server is running.
    // ---------------------------------------------------------------------------
    //
    TBool IsServerStarted()
        {
        TFindServer server( KPSServerName );
        TFullName serverName;
        return ( server.Next( serverName ) == KErrNone );
        }
    
    // ---------------------------------------------------------------------------
    // Starts the server.
    // ---------------------------------------------------------------------------
    //
    TInt StartServer()
        {
        TInt result = KErrNone;

        if ( !IsServerStarted() )
            {
            RProcess process;
            result = process.Create( KPSServerExe, KNullDesC );

            if ( result == KErrNone || result == KErrAlreadyExists )
                {
                TRequestStatus logonStatus;
                TRequestStatus rendezvousStatus;
                
                process.Logon( logonStatus );
                process.Rendezvous( rendezvousStatus );
                
                // start process
                if ( result == KErrNone )
                    {
                    process.Resume();
                    }
                
                // wait for either of statuses to be signaled
                User::WaitForRequest( logonStatus, rendezvousStatus );

                // if logonStatus was not signaled (it is still KRequestPending)
                // cancel it
                if ( logonStatus == KRequestPending ) // Server succesfully started.
                    {
                    process.LogonCancel( logonStatus );
                    User::WaitForRequest( logonStatus );
                    }
                else
                    {
                    process.RendezvousCancel( rendezvousStatus );
                    User::WaitForRequest( rendezvousStatus );
                    }
                }

            __ASSERT_ALWAYS( IsServerStarted(), Panic( KErrNotFound ) );
            }

        return result;
        }

    // ---------------------------------------------------------------------------
    // Returns the current version number.
    // ---------------------------------------------------------------------------
    //
    TVersion Version()
        {
        return TVersion( KPSVersionMajor, KPSVersionMinor, KPSVersionBuild );
        }

    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C RPSServ::RPSServ()
    : RSessionBase(), iReserved()
    {
    }

// ---------------------------------------------------------------------------
// Connects the session to the server.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RPSServ::Connect()
    {
    StartServer();
    
    return CreateSession( KPSServerName, Version() );
    }

// ---------------------------------------------------------------------------
// Closes the session.
// ---------------------------------------------------------------------------
//
EXPORT_C void RPSServ::Close()
    {
    RHandleBase::Close();
    }

// ---------------------------------------------------------------------------
// Creates a preset.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSPresetInterface* RPSServ::CreatePresetL( TInt aIndex, TUid aDataHandler )
    {
    TPckgBuf<TInt> id;
    User::LeaveIfError( SendReceive( EPSOpCreatePreset, TIpcArgs( aIndex, aDataHandler.iUid, &id ) ) );   
    
    return CPSPresetInterface::CreateL( *this, id(), aDataHandler );
    }

// ---------------------------------------------------------------------------
// Opens a preset.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSPresetInterface* RPSServ::OpenPresetL( TInt aId )
    {
    TPckgBuf<TInt> dataHandler;
    User::LeaveIfError( SendReceive( EPSOpGetPresetDataHandler, TIpcArgs( aId, &dataHandler ) ) );

    return CPSPresetInterface::CreateL( *this, aId, TUid::Uid( dataHandler() ) );
    }

// ---------------------------------------------------------------------------
// Deletes a preset.
// ---------------------------------------------------------------------------
//
EXPORT_C void RPSServ::DeletePresetL( TInt aId )
    {
    User::LeaveIfError( SendReceive( EPSOpDeletePreset, TIpcArgs( aId ) ) );
    }

// ---------------------------------------------------------------------------
// Moves a preset.
// ---------------------------------------------------------------------------
//
EXPORT_C void RPSServ::MovePresetL( TInt aId, TInt aDestinationIndex )
    {
    User::LeaveIfError( SendReceive( EPSOpMovePreset, TIpcArgs( aId, aDestinationIndex) ) );
    }

// ---------------------------------------------------------------------------
// Gets all presets.
// ---------------------------------------------------------------------------
//
EXPORT_C void RPSServ::GetPresetsL( RPointerArray<CPSPresetInterface>& aPresets )
    {
    GetPresetsL<CPSPresetInterface>( aPresets, KNullUid );
    }

// ======== GLOBAL FUNCTIONS ========
