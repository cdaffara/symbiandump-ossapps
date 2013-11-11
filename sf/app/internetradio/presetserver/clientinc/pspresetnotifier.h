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
* Description:  Header file for preset change notifier
*
*/


#ifndef C_CVRPRESETCHANGENOTIFIER_H
#define C_CVRPRESETCHANGENOTIFIER_H

#include <e32base.h>

class MPSPresetObserver;
class RPSServNotifier;
class RPSServ;

/**
 * Preset change notifier.
 *
 * Notifies the observer whenever any changes are done to the preset database.
 */
NONSHARABLE_CLASS( CPSPresetNotifier ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     *
     * The notifier is automatically set active after creation.
     * 
     * @param   aServ       Preset server session.
     * @param   aObserver   The observer to notify.
     * @return  The newly created object.
     */
    IMPORT_C static CPSPresetNotifier* NewL( RPSServ& aServ, MPSPresetObserver& aObserver );

    /**
     * Destructor.
     */
    IMPORT_C ~CPSPresetNotifier();

public:

    /**
     * Two-phased constructor.
     * Used only internally.
     *
     * The notifier is automatically set active after creation.
     * 
     * @param   aServ       Preset server session.
     * @param   aObserver   The observer to notify.
     * @param   aId         The ID of the preset to observe, or <code>KErrNotFound</code>
     *                      if all changes should result in a notification.
     * @return  The newly created object.
     */
    static CPSPresetNotifier* NewL( RPSServ& aServ, MPSPresetObserver& aObserver, TInt aId );

private:

    /**
     * Constructor.
     *     
     * @param   aServ       Preset server session.
     * @param   aObserver   The observer to notify.
     * @param	aPriority   Priority of the active object.
     */
    CPSPresetNotifier( RPSServ& aServ, MPSPresetObserver& aObserver, TInt aPriority );

    /**
     * Second-phase constructor.
     *
     * @param   aId         The ID of the preset to observe, or <code>KErrNotFound</code>
     *                      if all changes should result in a notification.
     */
    void ConstructL( TInt aId );

    /**
     * Static notify call back that is used after a failed notification.
     *
     * This will request a new notification to be received.
     *
     * @param   aSelf       Pointer to self.
     * @return  <code>KErrNone</code>.
     */
    static TInt StaticDelayedNotifyCallBack( TAny* aSelf );

// from base class CActive

    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

private:
    
    /** Timer that is used to request a notify after one has been failed. */
    CPeriodic* iTimer;
    /** Preset server session. */
    RPSServ& iServ;
    /** The observer to notify when presets change. */
    MPSPresetObserver& iObserver;
    /** Internal notifier implementation. */
    RPSServNotifier* iNotifier;
    /** Extension. */
    TAny* iExtension;

    };

#endif // C_CVRPRESETCHANGENOTIFIER_H
