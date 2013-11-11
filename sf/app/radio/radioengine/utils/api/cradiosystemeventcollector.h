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

#ifndef CRADIOSYSTEMEVENTCOLLECTOR_H
#define CRADIOSYSTEMEVENTCOLLECTOR_H

#include <e32std.h>

class MRadioSystemEventObserver;
class MRadioAudioRoutingObserver;

NONSHARABLE_CLASS( CRadioSystemEventCollector )
    {
public:

    IMPORT_C static CRadioSystemEventCollector* NewL();

    IMPORT_C virtual ~CRadioSystemEventCollector();

    /**
     * Adds a system event observer
     *
     * @param aSystemObserver The observer to be added
     */
    virtual void AddObserverL( MRadioSystemEventObserver* aSystemObserver ) = 0;

    /**
    * Removes a system event observer
    *
    * @param aSystemObserver The observer to be removed
    */
    virtual void RemoveObserver( MRadioSystemEventObserver* aSystemObserver ) = 0;

    /**
    * Returns data network coverage state ( wlan or mobile ).
    *
    * @return ETrue if there is network coverage
    */
    virtual TBool IsNetworkCoverage() const = 0;

    /**
    * Returns mobile network coverage state.
    *
    * @return ETrue if there is network coverage
    */
    virtual TBool IsMobileNetworkCoverage() const = 0;

    /**
    * Returns current call state.
    *
    * @return ETrue if call is active
    */
    virtual TBool IsCallActive() const = 0;

    /**
    * Returns current audio resources state.
    *
    * @return ETrue if audio resources are available
    */
    virtual TBool IsAudioResourcesAvailable() const = 0;

    /**
     * Returns current headset connection status
     *
     * @return
     */
    virtual TBool IsHeadsetConnectedL() const = 0;
    
   
    /**
     * Returns object as provided observer
     *
     * @param
     */
    virtual void AsObserver( MRadioAudioRoutingObserver*& obj ) const = 0;


    };

#endif // CRADIOSYSTEMEVENTCOLLECTOR_H
