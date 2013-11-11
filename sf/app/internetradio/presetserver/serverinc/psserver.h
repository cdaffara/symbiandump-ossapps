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
* Description:  Implementation of the preset server
*
*/


#ifndef C_CPSSERVER_H
#define C_CPSSERVER_H

#include "pscommon.h"
#include "pspresetobserver.h"
#include "psdatabase.h"
#include "pspropertyobserver.h"

class CPSDatabaseCleanup;
class CPSShutdown;

/**
 * Implementation of the preset server.
 */
NONSHARABLE_CLASS( CPSServer ) : public CServer2, public MPSPresetObserver, public MPSPropertyChangeObserver
    {

public:

    /**
     * Two-phased constructor.
     *
     * @return  The newly created object.
     */
    static CPSServer* NewL();

    /**
     * Destructor.
     */
    ~CPSServer();      

    /**
     * Panics the client with a specific reason.
     *
     * @param   aReason     Reason of the panic.
     */
    void PanicClient( TInt aReason ) const;

    /**
     * Returns the current IPC message.
     *
     * @return  The current IPC message.
     */
    const RMessage2& Message() const;

    /**
     * Returns the preset database.
     *
     * @return  The preset database.
     */
    RPSDatabase& Database();
    
    /**
     * Increments the session count. Should be called whenever a new session is created.
     */
    void IncrementSessionCount();

    /**
     * Decrements the session count. Should be called whenever a session is destroyed.
     */
    void DecrementSessionCount();

    /**
     * Called when a session is disconnected.
     *
     * @param	aMessage	Disconnect message.
     */
    void SessionDisconnected( const RMessage2& aMessage );

    /**
     * Appends a preset observer.
     *
     * @param   aMessage    IPC message of the request.
     */
    void AppendPresetObserverL( const RMessage2& aMessage );

    /**
     * Removes a preset change observer.
     *
     * @param   aMessage    IPC message of the request.
     */
    void RemovePresetObserver( const RMessage2& aMessage );

    /**
     * Appends a pending preset.
     *
     * @param   aId     Id of the preset.
     */
    void AppendPendingPresetL( TInt aId );

    /**
     * Commits a pending preset.
     *
     * @param   aId     Id of the preset.
     */
    void CommitPendingPresetL( TInt aId );

    /**
     * Removes a pending preset.
     *
     * @param   aId     Id of the preset.
     */
    void RemovePendingPreset( TInt aId );

    /**
     * Sets the index of a pending preset.
     *
     * @param   aId     Id of the preset.
     * @param   aIndex   Index to set.
     */
    void SetPendingPresetIndexL( TInt aId, TInt aIndex );

    /**
     * Sets the name of a pending preset.
     *
     * @param   aId     Id of the preset.
     * @param   aName   Name to set.
     */
    void SetPendingPresetNameL( TInt aId, const TDesC& aName );

    /**
     * Sets the data of a pending preset.
     *
     * @param   aId     Id of the preset.
     * @param   aData   Data to set.
     */
    void SetPendingPresetDataL( TInt aId, const TDesC8& aData );

// from CServer2

    /**
     * @see CServer2::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
     */
    CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

// from MPSPresetObserver

    /**
     * @see MPSPresetObserver::HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason )
     */
    void HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason );
    
// from MPSPropertyChangeObserver

    /**
     * @see MPSPropertyChangeObserver::HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TInt aValue )
     */
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TInt aValue );
    
    /**
     * @see MPSPropertyChangeObserver::HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TPSTextProperty& aValue )
     */    
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TPSTextProperty& aValue );
    
    /**
     * @see MPSPropertyChangeObserver::HandlePropertyChangeErrorL( const TUid& aCategory, const TUint aKey, TInt aError)
     */
    void HandlePropertyChangeErrorL( const TUid& aCategory, const TUint aKey, TInt aError);    

private:

    /**
     * Constructor.
     */
    CPSServer();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Returns the index of the preset observer matching the supplied IPC message.
     *
     * @param   aMessage    IPC message of the request.
     * @return  Index of the observer, or <code>KErrNotFound</code> if it doesn't exist.
     */ 
    TInt PresetObserverIndex( const RMessage2& aMessage ) const;

    /**
     * Returns the index of a pending preset.
     *
     * @param   aId     Id of the preset.
     * @return  Index of the preset, or <code>KErrNotFound</code> if it doesn't exist.
     */
    TInt PendingPresetIndex( TInt aId ) const;

    /**
     * Notifies the preset observers about preset changes.
     *
     * @param   aId         Id of the preset that was changed.
     * @param   aReason     Reason for the change.
     */
    void NotifyPresetObserversL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason );

    /**
     * Returns the index of the next unhandled notification related to the last handled notification identifier and the identifier of the observed preset.
     *
     * @param   aHandledNotificationId  Identifier of the notification that was most recently handled.
     * @param   aObservedPresetId       Identifier of the observed preset.
     * @return  Index of the notification matching the supplied parameters, or <code>KErrNotFound</code> if no such notification exists.
     */
    TInt NextUnhandledNotificationIndexById( TUint64 aHandledNotificationId, TInt aObservedPresetId );

    /**
     * Cleanup routine for deleting unused cached notifications.
     *
     * @param   aSelf   Pointer to self.
     * @return  Always <code>KErrNone</code>.
     */
    static TInt StaticCleanCachedNotifications( TAny* aSelf );

private:

    /** The preset database. */
    RPSDatabase iDatabase;
    /** Shutdown timer that is activated after the last client disconnects. */
    CPSShutdown* iShutdown;
    /** Cleaner periodic that deletes all unused cached notifications. */
    CPeriodic* iCachedNotificationCleaner;
    /** Shutdown delay */
    TTimeIntervalMicroSeconds32 iShutdownDelay;
    /** The database cleanup handler. */
    CPSDatabaseCleanup* iCleanup;
    /** Presets that are locked, but not yet committed. */
    RPointerArray<CPSPendingPreset> iPendingPresets;
    /** File server session. */
    RFs iFs;
    /** Preset change notification requests. */
    RArray<RMessage2> iPresetObservers;    
    /** Notifications received so far. */
    RArray<TPSNotifyDataPackage> iNotifications;
    /* Pub&Sub observer */
    CPSPropertyObserver* iPropertyObserver;
    /** Number of connected sessions. */
    TInt iSessionCount;
    /** Running unique identifier for cached notifications. */
    TUint64 iCachedNotificationId;
    };

#endif // C_CPSSERVER_H
