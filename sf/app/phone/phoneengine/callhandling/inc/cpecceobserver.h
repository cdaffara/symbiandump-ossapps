/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observes CCE
*
*/


#ifndef CCCEOBSERVER_H
#define CCCEOBSERVER_H

#include <e32base.h>
#include <mcceobserver.h>

class MPECallInitialiser;
class MPEPhoneModelInternal;

/**
 *  Observes CCE
 *
 *  @lib callhandling.lib
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPECCEObserver ): public CBase, public MCCEObserver
    {
public:

    static CPECCEObserver* NewL( 
        MPECallInitialiser& aCallInitialiser,
        MPEPhoneModelInternal& aPhoneModel );
    
    static CPECCEObserver* NewLC( 
        MPECallInitialiser& aCallInitialiser,
        MPEPhoneModelInternal& aPhoneModel );

    /**
    * Destructor.
    */
    virtual ~CPECCEObserver();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    
// Methods from base class MCCEObserver
    
    /**
     * @see MCCEObserver::IncomingCall
     */
    void IncomingCall( MCCECall& aCall );
    
    /**
     * @see MCCEObserver::IncomingCall
     */
    void MOCallCreated( MCCECall& aCall );

    /**
     * @see MCCEObserver::ConferenceCallCreated
     */
    void ConferenceCallCreated( MCCEConferenceCall& aConferenceCall );
    
    /**
     * @see MCCEObserver::IncomingCall
     */
    void DataPortName( TName& aPortName );

private:

    CPECCEObserver( 
        MPECallInitialiser& aCallInitialiser,
        MPEPhoneModelInternal& aPhoneModel );
    
    void NotifyCallInitialiser( MCCECall& aCall );

private: // data
    
    /**
     * Incoming and external call initialiser
     */
    MPECallInitialiser& iCallInitialiser;
    
    /**
     * Phone model
     */
    MPEPhoneModelInternal& iPhoneModel;

    };

#endif // CCCEOBSERVER_H
