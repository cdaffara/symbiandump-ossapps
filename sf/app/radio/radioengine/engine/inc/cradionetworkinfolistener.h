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

#ifndef C_RADIONETWORKINFOLISTENER_H
#define C_RADIONETWORKINFOLISTENER_H

// System includes
#include <e32base.h>
#include <etel3rdparty.h>

// Forward declarations
class CTelephony;
class MRadioSettingsSetter;

/**
 * An interface for notifying about network id changes
 */
NONSHARABLE_CLASS( MRadioNetworkChangeObserver )
    {
public:

    /**
     * Used for notifying that the network id has changed
     */
    virtual void NetworkIdChanged() = 0;

    };

/**
 * Listens changes in network ID and country code.
 */
NONSHARABLE_CLASS( CRadioNetworkInfoListener ) : public CActive
    {
public:

    static CRadioNetworkInfoListener* NewL( MRadioSettingsSetter& aSetter,
                                            MRadioNetworkChangeObserver* aObserver );

    ~CRadioNetworkInfoListener();

    /**
     * Return current combined network ID and country code as TInt.
     * @return   An integer composed of the current network ID and country code.
     */
    TInt CompoundNetworkId() const;

    /**
     * Returns current country code.
     * @return   The current country code.
     */
    TPtrC CountryCode() const;

    /**
     * SubscriberId
     * @return subscriber id
     */
    TPtrC SubscriberId() const;

private:

    CRadioNetworkInfoListener( MRadioSettingsSetter& aSetter,
                               MRadioNetworkChangeObserver* aObserver );

    void ConstructL();

// from base class CActive

    void DoCancel();
    void RunL();

// New functions

    void SaveNetworkInfoL();

private: // data

    MRadioSettingsSetter&           iSetter;

    MRadioNetworkChangeObserver*    iObserver;

#ifndef __WINS__
    CTelephony*                     iTelephony;
#endif

    CTelephony::TSubscriberIdV1     iSubscriberId;
    CTelephony::TSubscriberIdV1Pckg iSubscriberIdPckg;
    CTelephony::TNetworkInfoV1      iNetworkInfo;
    CTelephony::TNetworkInfoV1Pckg  iNetworkInfoPckg;

    TInt                            iCompoundNetworkId;
    TInt                            iPreviousCompoundNetworkId;

    };

#endif // C_RADIONETWORKINFOLISTENER_H
