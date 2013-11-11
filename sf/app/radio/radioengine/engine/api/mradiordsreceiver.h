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

#ifndef MRADIORDSRECEIVER_H
#define MRADIORDSRECEIVER_H

// Forward declarations
class CRadioUtility;
class MRadioRdsDataObserver;

NONSHARABLE_CLASS( MRadioRdsReceiver )
    {
public:

    /**
     * Initializes the RDS receiver
     *
     * @param aRadioUtility A reference to the radio utility
     * @param aPubSub       Pointer to Publish&Subscribe object
     */
    virtual void InitL( CRadioUtility& aRadioUtility ) = 0;

    /**
     * Adds an observer wich will be notified of the new RDS data
     * If observer already exists, it is not added
     *
     * @param aObserver pointer of the observer instance.
     */
    virtual void AddObserverL( MRadioRdsDataObserver* aObserver ) = 0;

    /**
     * Removes the rds observer.
     */
    virtual void RemoveObserver( MRadioRdsDataObserver* aObserver ) = 0;

    /**
     * Getter for automatic switching state of alternate frequencies
     *
     * @return ETrue if automatic switching is enabled
     */
    virtual TBool AutomaticSwitchingEnabled() const = 0;

    /**
     * Getter for the programme service name
     *
     * @return Programme service name
     */
    virtual const TDesC& ProgrammeService() const = 0;

    /**
     * Getter for the radio text.
     *
     * @return Radio text.
     */
    virtual const TDesC& RadioText() const = 0;

    /**
     * Getter for the RDS signal availability
     *
     * @return ETrue if RDS signal is available, otherwise EFalse
     */
    virtual TBool SignalAvailable() const = 0;

    /**
     * Setter for automatic switching of alternate frequencies
     * If this method is overridden, the base implementation should be called
     *
     * @param aEnable ETrue, if automatic swithing is to be enabled
     */
    virtual void SetAutomaticSwitchingL( TBool aEnable ) = 0;

    /**
     * Starts receiving the RDS information
     */
    virtual void StartReceiver() = 0;

    /**
     * Stops receiving the RDS information
     */
    virtual void StopReceiver() = 0;

    };

#endif // MRADIORDSRECEIVER_H
