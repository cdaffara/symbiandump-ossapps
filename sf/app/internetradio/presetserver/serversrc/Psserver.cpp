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
* Description:  Preset server implementation
*
*/

#include <sbdefs.h>

#include "pscommon.h"
#include "psdatabasecleanup.h"
#include "psdebug.h"
#include "pspendingpreset.h"
#include "psscheduler.h"
#include "psserver.h"
#include "pssession.h"
#include "psshutdown.h"
#include "psutils.h"

using namespace conn;

const TInt KPSPriority = CActive::EPriorityStandard;    // Priority of the preset server.

const TInt KPSShutdownTime = 500000; // The time in microseconds between the last client disconnecting and the server being closed.
const TInt KPSCachedNotificationCleanerPeriod = 60000000; // The period in microseconds between checking for unused cached notifications.
const TInt KPSCachedNotificationKeepAliveTime = 300000000; // The amount of time in microseconds to keep cached notifications alive after they have been added.

// ======== LOCAL FUNCTIONS ========

namespace
    {

#ifdef _DEBUG

    // ---------------------------------------------------------------------------
    // Panics the process.
    // ---------------------------------------------------------------------------
    //
    void Panic( TInt aReason )
        {
        User::Panic( KPSServerName, aReason );
        }

#endif // _DEBUG

    // ---------------------------------------------------------------------------
    // Sorts the array by priority of the notification observers. Descending order!
    // ---------------------------------------------------------------------------
    //
    TInt SortPresetObserverArrayByPriority( const RMessage2& aMessage1, const RMessage2& aMessage2 )
        {
        if ( aMessage1.Int1() < aMessage2.Int1() )
            {
            return 1;
            }
        else if ( aMessage1.Int1() > aMessage2.Int1() )
            {
            return -1;
            }
        else
            {
            return 0;
            }
        }

    // ---------------------------------------------------------------------------
    // Initializes the server process.
    // ---------------------------------------------------------------------------
    //
    void InitServerL()
        {
        User::RenameThread( KPSServerName );
        
        TFindServer findServer( KPSServerName );
        TFullName fullName;
        
        if ( findServer.Next( fullName ) ) // Server has not yet been started.
            {
            CPSScheduler* scheduler = new ( ELeave ) CPSScheduler;
            CleanupStack::PushL( scheduler );
            CActiveScheduler::Install( scheduler );

            CPSServer* server = CPSServer::NewL();
            CleanupStack::Pop( scheduler );
            scheduler->SetServer( server );

            // Attention! Absolutely no code that can leave may be run after doing the rendezvous!

            RProcess().Rendezvous( KErrNone );        

            CActiveScheduler::Start();
            
            delete server;
            delete scheduler;
            }
        else
            {
            RProcess().Rendezvous( KErrNone );
            }
        }

    }
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPSServer::CPSServer()
    : CServer2( KPSPriority ), iDatabase( *this ), iShutdownDelay( KPSShutdownTime )
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPSServer::ConstructL()
    {
    PSDEBUG( "CPSServer::ConstructL() [enter]" );

    User::LeaveIfError( iFs.Connect() );

    TFileName databaseFullName;
    PSUtils::GetDatabaseFullNameL( databaseFullName );

    iDatabase.OpenL( iFs, databaseFullName );

    iShutdown = CPSShutdown::NewL();
    iCleanup = CPSDatabaseCleanup::NewL( iDatabase );

    iCachedNotificationCleaner = CPeriodic::NewL( CActive::EPriorityStandard );
    iCachedNotificationCleaner->Start( KPSCachedNotificationCleanerPeriod, KPSCachedNotificationCleanerPeriod,
                                       TCallBack( StaticCleanCachedNotifications, this ) ); 
    
    iPropertyObserver = CPSPropertyObserver::NewL( *this, KUidSystemCategory, 
                                                   KUidBackupRestoreKey, RProperty::EInt);
    
    StartL( KPSServerName );

    PSDEBUG( "CPSServer::ConstructL() [exit]" );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSServer* CPSServer::NewL()
    {
    CPSServer* self = new ( ELeave ) CPSServer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPSServer::~CPSServer()
    {
    PSDEBUG( "CPSServer::~CPSServer() [enter]" );

    Cancel();
    
    delete iPropertyObserver;
    delete iCachedNotificationCleaner;
    delete iShutdown;
    delete iCleanup;

    iDatabase.Close();
    iFs.Close();

    while ( iPresetObservers.Count() )
        {
        if ( !iPresetObservers[0].IsNull() )
            {
            iPresetObservers[0].Complete( KErrServerTerminated );
            }
        iPresetObservers.Remove( 0 );
        }
    iPresetObservers.Close();

    iNotifications.Close();
    iPendingPresets.ResetAndDestroy();

    PSDEBUG( "CPSServer::~CPSServer() [exit]" );
    }

// ---------------------------------------------------------------------------
// Returns the current message.
// ---------------------------------------------------------------------------
//
const RMessage2& CPSServer::Message() const
    {
    return CServer2::Message();
    }

// ---------------------------------------------------------------------------
// Returns the database.
// ---------------------------------------------------------------------------
//
RPSDatabase& CPSServer::Database()
    {
    return iDatabase;
    }

// ---------------------------------------------------------------------------
// Panics the client.
// ---------------------------------------------------------------------------
//
void CPSServer::PanicClient( TInt aReason ) const
    {
    Message().Panic( KPSServerName, aReason );
    }

// ---------------------------------------------------------------------------
// Increments the session count.
// ---------------------------------------------------------------------------
//
void CPSServer::IncrementSessionCount()
    {
    iSessionCount++;
    
    iShutdown->Cancel();
    }

// ---------------------------------------------------------------------------
// Decrements the session count.
// ---------------------------------------------------------------------------
//
void CPSServer::DecrementSessionCount()
    {
    iSessionCount--;
    
    if ( iSessionCount == 0 ) // Last session was closed, so we need to start the shutdown timer.
        {
        iShutdown->Start( iShutdownDelay );
        }

    __ASSERT_DEBUG( iSessionCount >= 0, Panic( KErrCorrupt ) );
    }

// ---------------------------------------------------------------------------
// Called when a session is disconnected.
// ---------------------------------------------------------------------------
//
void CPSServer::SessionDisconnected( const RMessage2& aMessage )
    {
    PSDEBUG( "CPSServer::SessionDisconnected( RMessage2& ) [enter]" );

    for ( TInt i = 0; i < iPresetObservers.Count(); i++ )
        {
        const RMessage2& message = iPresetObservers[i];
        
        if ( message.Session() == aMessage.Session() )
            {
            if ( !message.IsNull() )
                {
                message.Complete( KErrDied );
                }
            iPresetObservers.Remove( i );
            i--;
            }
        }

    PSDEBUG( "CPSServer::SessionDisconnected( RMessage2& ) [exit]" );
    }

// ---------------------------------------------------------------------------
// Appends a preset observer.
// ---------------------------------------------------------------------------
//
void CPSServer::AppendPresetObserverL( const RMessage2& aMessage )
    {
    PSDEBUG( "CPSServer::AppendPresetObserverL( RMessage2& ) [enter]" );

    TInt idx = PresetObserverIndex( aMessage );
    
    if ( idx != KErrNotFound )
        {
        PSDEBUG( "CPSServer::AppendPresetObserverL - Preset observer already found" );
        User::Leave( KErrAlreadyExists );
        }

    TPckgBuf<TPSNotifyDataPackage> data;
    aMessage.ReadL( 2, data );
    
    TInt nextUnhandledNotificationIndex = NextUnhandledNotificationIndexById( data().iNotificationId, aMessage.Int1() );
    
    if ( data().iNotificationId > 0 && nextUnhandledNotificationIndex >= 0 )
        {
        PSDEBUG2( "CPSServer::AppendPresetObserverL - Cached notification that was missed was found at index %d", nextUnhandledNotificationIndex );

        aMessage.WriteL( 2, TPckgC<TPSNotifyDataPackage>( iNotifications[nextUnhandledNotificationIndex] ) );
        aMessage.Complete( KErrNone ); 
        }
    else
        {
        PSDEBUG( "CPSServer::AppendPresetObserverL - No cached notifications of interest were found, starting to observe further ones" );
        iPresetObservers.AppendL( RMessage2( aMessage ) );
        }

    PSDEBUG( "CPSServer::AppendPresetObserverL( RMessage2& ) [enter]" );
    }

// ---------------------------------------------------------------------------
// Removes a preset observer and completes it with KErrCancel.
// ---------------------------------------------------------------------------
//
void CPSServer::RemovePresetObserver( const RMessage2& aMessage )
    {
    TInt idx = PresetObserverIndex( aMessage );

    if ( idx >= 0 )
        {
        if ( !iPresetObservers[idx].IsNull() )
            {
            iPresetObservers[idx].Complete( KErrCancel );
            }
        iPresetObservers.Remove( idx );
        }
    }

// ---------------------------------------------------------------------------
// Returns the index of a given preset observer.
// ---------------------------------------------------------------------------
//
TInt CPSServer::PresetObserverIndex( const RMessage2& aMessage ) const
    {
    TInt idx = KErrNotFound;
        
    CSession2* currentSession = aMessage.Session();
        
    for ( TInt i = 0; i < iPresetObservers.Count(); i++ )
        {
        const RMessage2& message = iPresetObservers[i];

        if ( !message.IsNull() && message.Session() == currentSession && message.Int0() == aMessage.Int0() )
            {
            idx = i;
            /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
            break;
            /*lint -restore*/
            }
        }
    
    return idx;
    }

// ---------------------------------------------------------------------------
// Appends a pending preset.
// ---------------------------------------------------------------------------
//
void CPSServer::AppendPendingPresetL( TInt aId )
    {
    TInt idx = PendingPresetIndex( aId );

    if ( idx == KErrNotFound )
        {
        CPSPendingPreset* preset = CPSPendingPreset::NewL( aId );
        CleanupStack::PushL( preset );
        iPendingPresets.AppendL( preset );
        CleanupStack::Pop( preset );
        }
    else
        {
        User::Leave( KErrLocked );
        }
    }

// ---------------------------------------------------------------------------
// Commits a pending preset.
// ---------------------------------------------------------------------------
//
void CPSServer::CommitPendingPresetL( TInt aId )
    {
    TInt idx = PendingPresetIndex( aId );

    if ( idx >= 0 )
        {
        CPSPendingPreset* preset = iPendingPresets[idx];
        iDatabase.CommitPresetL( *preset );
        iPendingPresets.Remove( idx );
        delete preset;
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// Removes a pending preset from the array.
// ---------------------------------------------------------------------------
//
void CPSServer::RemovePendingPreset( TInt aId )
    {
    TInt idx = PendingPresetIndex( aId );
    
    if ( idx >= 0 )
        {
        CPSPendingPreset* preset = iPendingPresets[idx];
        iPendingPresets.Remove( idx );
        delete preset;
        }
    }

// ---------------------------------------------------------------------------
// Sets the index of a pending preset.
// ---------------------------------------------------------------------------
//
void CPSServer::SetPendingPresetIndexL( TInt aId, TInt aIndex )
    {
    TInt idx = PendingPresetIndex( aId );
    
    if ( idx >= 0 )
        {
        iPendingPresets[idx]->SetIndex( aIndex ); 
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// Sets the name of a pending preset.
// ---------------------------------------------------------------------------
//
void CPSServer::SetPendingPresetNameL( TInt aId, const TDesC& aName )
    {
    TInt idx = PendingPresetIndex( aId );
    
    if ( idx >= 0 )
        {
        iPendingPresets[idx]->SetNameL( aName ); 
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// Sets the data of a pending preset.
// ---------------------------------------------------------------------------
//
void CPSServer::SetPendingPresetDataL( TInt aId, const TDesC8& aData )
    {
    TInt idx = PendingPresetIndex( aId );
    
    if ( idx >= 0 )
        {
        iPendingPresets[idx]->SetDataL( aData ); 
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// From class CSession2
// Creates a new session.
// ---------------------------------------------------------------------------
//
CSession2* CPSServer::NewSessionL( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
    {
    TVersion currentVersion( KPSVersionMajor, KPSVersionMinor, KPSVersionBuild );
    
    if ( !User::QueryVersionSupported( currentVersion, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CPSSession::NewL( *const_cast<CPSServer*>( this ) );
    }

// ---------------------------------------------------------------------------
// From class MPSPresetObserver
// Invoked when a preset is changed.
// ---------------------------------------------------------------------------
//
void CPSServer::HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason )
    {
    NotifyPresetObserversL( aId, aDataHandler, aReason );
    
    if (aReason == MPSPresetObserver::EPSDeleted)
    	{
    	iCleanup->RequestCleanupCheck();
    	}
    }

// ---------------------------------------------------------------------------
// Returns the index of a pending preset matching the supplied id.
// ---------------------------------------------------------------------------
//
TInt CPSServer::PendingPresetIndex( TInt aId ) const
    {
    TInt idx = KErrNotFound;

    for ( TInt i = 0; i < iPendingPresets.Count(); i++ )
        {
        if ( iPendingPresets[i]->Id() == aId )
            {
            idx = i;
            /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
            break;
            /*lint -restore*/
            }
        }

    return idx;
    }

// ---------------------------------------------------------------------------
// Notifies the preset observers.
// ---------------------------------------------------------------------------
//
void CPSServer::NotifyPresetObserversL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason )
    {
    PSDEBUG5( "CPSServer::NotifyPresetObserversL( aId = %d, aDataHandler = %d, aReason = %d ) - Notifications cached: %d", aId, aDataHandler.iUid, aReason, iNotifications.Count() );

    TTime current;
    current.UniversalTime();

    iCachedNotificationId++; // Running cached notification identifier is incremented here. Must be done before the data package is constructed.
    
    TPSNotifyDataPackage data = { aId, aDataHandler, aReason, iCachedNotificationId, current };
    iNotifications.AppendL( data );

    // Sorts the preset observers array so, that presets are first, then other observers.
    // This is done to ensure that presets are notified first, just in case.
    iPresetObservers.Sort( TLinearOrder<RMessage2>( ::SortPresetObserverArrayByPriority ) );

    TInt skipped = 0; // Number of observers that will not be notified, i.e. are skipped.

    while ( iPresetObservers.Count() && skipped < iPresetObservers.Count() )
        {
        const RMessage2& message = iPresetObservers[skipped];

        if ( message.IsNull() )
            {
            iPresetObservers.Remove( skipped ); // Dead notifiers are removed from the array.
            }
        else
            {
            TPckgBuf<TPSNotifyDataPackage> msgData;
            message.ReadL( 2, msgData );

            TInt observedPresetId = message.Int1();

            // If the observer is interested in this preset or it is interested in them all (KErrNotFound),
            // notify it about the change.
            if ( aId == observedPresetId || observedPresetId == KErrNotFound )
                {
                message.WriteL( 2, TPckgC<TPSNotifyDataPackage>( data ) );
                message.Complete( KErrNone );
                
                iPresetObservers.Remove( skipped );
                }
            else
                {
                skipped++;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Cleanup routine for deleting unused cached notifications.
// ---------------------------------------------------------------------------
//
TInt CPSServer::NextUnhandledNotificationIndexById( TUint64 aHandledNotificationId, TInt aObservedPresetId )
    {
    TInt index = KErrNotFound;
    
    for ( TInt i = 0; i < iNotifications.Count() && index == KErrNotFound; i++ )
        {
        if ( iNotifications[i].iNotificationId > aHandledNotificationId )
            {
            if ( iNotifications[i].iId == aObservedPresetId || aObservedPresetId == KErrNotFound )
                {
                index = i;
                }
            }
        }
    
    return index;
    }

// ---------------------------------------------------------------------------
// Cleanup routine for deleting unused cached notifications.
// ---------------------------------------------------------------------------
//
TInt CPSServer::StaticCleanCachedNotifications( TAny* aSelf )
    {
    CPSServer* self = static_cast<CPSServer*>( aSelf );
    if ( self )
        {
        PSDEBUG2( "CPSServer::StaticCleanCachedNotifications [enter] - iNotifications.Count() = %d", self->iNotifications.Count() );

        TTime current;
        current.UniversalTime();
        
        for ( TInt i = 0; i < self->iNotifications.Count(); i++ )
            {
            TInt64 diff = current.MicroSecondsFrom( self->iNotifications[i].iNotificationTime ).Int64();

            if ( diff > KPSCachedNotificationKeepAliveTime )
                {
                PSDEBUG2( "CPSServer::StaticCleanCachedNotifications - Removing cached notification entry from index with time difference of %d seconds", diff / 1000000 ); 
                self->iNotifications.Remove( i );
                i--;
                }
            }
        
        self->iNotifications.Compress();

        PSDEBUG2( "CPSServer::StaticCleanCachedNotifications [exit] - iNotifications.Count() = %d", self->iNotifications.Count() );
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// From class MPSPropertyChangeObserver
// Invoked when a TInt type property is changed.
// ---------------------------------------------------------------------------
//
void CPSServer::HandlePropertyChangeL( const TUid& /* aCategory */, const TUint /* aKey */, const TInt aValue )
    {
    PSDEBUG2( "CPSServer::HandlePropertyChangeL( aValue = %d )", aValue);
    TUint backupPart = aValue & KBURPartTypeMask; 
    if ( backupPart != EBURUnset && backupPart != EBURNormal ) // backup or restore -> shutdown when possible.
        {
        iShutdownDelay = 0;        
        if ( iSessionCount == 0 )
            {
            iShutdown->Start( iShutdownDelay );
            }
        }
    else
        {
        PSDEBUG("CPSServer::HandlePropertyChangeL - no backup/restore ongoing" );
        }        
    }

// ---------------------------------------------------------------------------
// From class MPSPropertyChangeObserver
// Invoked when a TBuf8 type property is changed.
// ---------------------------------------------------------------------------
//
void CPSServer::HandlePropertyChangeL( const TUid& /* aCategory */, 
                                       const TUint /*aKey*/, 
                                       const TPSTextProperty& /*aValue */ ){}

// ---------------------------------------------------------------------------
// From class MPSPropertyChangeObserver
// Invoked when an error is encountered in property processing.
// ---------------------------------------------------------------------------
//
void CPSServer::HandlePropertyChangeErrorL( const TUid& /* aCategory */, 
                                            const TUint /* aKey */, 
                                            TInt /*aError */){}
    
    

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Application entry point.
// ---------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK;

    TInt err = KErrNone;

    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( cleanupStack )
        {
        TRAP( err, InitServerL() )

        __ASSERT_DEBUG( !err, Panic( err ) );
        PSDEBUG( "CPSServer - Exiting" );
        delete cleanupStack;        
        }
    else
        {
        err = KErrNoMemory;
        }
        
    __UHEAP_MARKEND;
    
    return err;
    }

