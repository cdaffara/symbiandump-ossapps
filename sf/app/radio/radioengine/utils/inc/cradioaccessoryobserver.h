/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef CRADIOACCESSORYOBSERVER_H
#define CRADIOACCESSORYOBSERVER_H

#include <AccessoryServer.h>
#include <AccessoryConnection.h>
#include <AccPolSubblockNameArray.h>

class MRadioHeadsetEventObserver;

/**
 *  Observer of accessory states.
 *
 *  Instance of this class observes notifications of headset state changes.
 *  In its turn it sends notifications to its observer of those changes.
 *
 */
NONSHARABLE_CLASS( CRadioAccessoryObserver ): public CActive
    {
public:

    /**
     * C++ default constructor.
     */
    CRadioAccessoryObserver();

    /**
     * Static constructor.
     */
    static CRadioAccessoryObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CRadioAccessoryObserver();

    /**
     * Sets observer. The observer will be notified when headset has been
     * pressed or headset is connected/disconnected.
      *
     * @param aObserver Observer
     */
    void SetObserver( MRadioHeadsetEventObserver* aObserver );

    /**
     * Tells if there is a headset connected.
     *
     * @return <code>ETrue </code> if headset accessory is connected,
     * <code>EFalse</code> otherwise
     */
    TBool IsHeadsetConnectedL() const;

// from base class CActive
    /**
     * From CActive
     *
     * @see CActive::RunL()
     */
    void RunL();

    /**
     * From CActive
     *
     * @see CActive::DoCancel()
     */
    void DoCancel();

private:

    /**
     * Finds an accessory. All wired and bluetooth accessories are taken into account.
     *
     * @param aAcceptOnlyHeadset If ETrue, search is succesful only if there is a headset connected.
     * @param aOutputType The output type of the found accessory ( private/public )
     * @return ETrue, if suitable accessory was found, EFalse otherwise
     */
    TBool FindAccessoryL( TBool aAcceptOnlyHeadset, TAccAudioOutPutType& aOutputType ) const;

    /**
     * Tells whether the given accessory is a lineout ( and not a headset ).
     *
     * @param aAccPolGenId The accessory being investigated.
     * @return ETrue if the accessory is a lineout, EFalse otherwise.
     */
    TBool IsLineoutConnectedL( TAccPolGenericID& aAccPolGenId ) const;

private:

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aAccessoryServer Existing session for The Accessory Server.
     */
    void ConstructL();

private:

    /**
     * Updates the set of known accessories.
     * Method also informs the caller about added/removed accessories.
     *
     * @param aNroChangedAccessories On return contains the number of added/removed accessories.
     */
    void UpdateCurrentAccessoriesL( TInt& aNroChangedAccessories );

    /**
     * Counts the removed accessories.
     *
     * @return The number of removed accessories.
     */
    TInt CountRemovedAccessoriesL() const;

    /**
     * Counts the added accessories.
     *
     * @return The number of added accessories.
     */
    TInt CountAddedAccessoriesL() const;

private:

    /**
     * Instance that gets notifications of the headset state changes
     * Not owned.
     */
    MRadioHeadsetEventObserver* iObserver;

    /**
     * Session for The Accessory Server.
     * Owned.
     */
    RAccessoryServer iAccessoryServer;

    /**
     * Accessory Connection.
     * Owned.
     */
    RAccessoryConnection iAccessoryConn;

    /**
     * Known set of accessories. CRadioAccessoryObserver uses this set to deduce
     * which accessory has been removed ( or added ) and whether or not it was an acceptable accessory.
     * Owned.
     */
    RArray<TAccPolGenericID> iSavedAccessories;

    /**
     * Array of connected accessories.
     */
    TAccPolGenericIDArray iAccPolGenIdArr;

    };

#endif // CRADIOACCESSORYOBSERVER_H
