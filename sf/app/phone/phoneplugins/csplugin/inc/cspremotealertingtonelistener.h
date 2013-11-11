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
* Description:  Listens remote alerting tone status changes
*
*/


#ifndef CSPREMOTEALERTINGTONELISTENER_H
#define CSPREMOTEALERTINGTONELISTENER_H

#include <e32base.h>
#include <rmmcustomapi.h>

class MCSPRemoteAlertingToneObserver;

/**
 *  Listens remote alerting tone changes.
 *
 *  @lib CSPlugin.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CSPRemoteAlertingToneListener ) : public CActive
    {
public:
    
    /**
     * Two-phased constructor.
     * @param aCustomAPI Connected custom API instance.
     * @param aObserver Observer of the remote alerting tone chnages.
     */
    static CSPRemoteAlertingToneListener* NewL( 
        RMmCustomAPI& aCustomAPI,
        MCSPRemoteAlertingToneObserver& aObserver );
    

    /**
    * Destructor.
    */
    virtual ~CSPRemoteAlertingToneListener();

    /**
     * Starts listening remote alerting tone changes. Observer is notified
     * when change occurs. Listening stops when Cancel is called or listening
     * instance is deleted.
     *
     * @since S60 v5.0
     */
    void StartListening();

protected:

// from base class CActive

    void DoCancel();
    
    void RunL();
    
private:
    
    CSPRemoteAlertingToneListener( 
        RMmCustomAPI& aCustomAPI,
        MCSPRemoteAlertingToneObserver& aObserver );

private: // data

    /** 
     * For listening changes in remote alerting tone status.
     * Not own.
     */
    RMmCustomAPI& iCustomAPI;
    
    /**  
     * Status of the remote alerting tone.
     */
    RMmCustomAPI::TRemoteAlertingToneStatus iRATStatus;
    
    /**
     * Change observer.
     */
    MCSPRemoteAlertingToneObserver& iObserver;

    };

#endif // CSPREMOTEALERTINGTONELISTENER_H
