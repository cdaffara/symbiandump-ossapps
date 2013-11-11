/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header of CSPForwardProvider which provides methods and
*                functionality to handle call forwarding
*
*/


#ifndef CSPFORWARDPROVIDER_H
#define CSPFORWARDPROVIDER_H

#include <e32base.h>
#include <mccpforwardprovider.h>
#include <mccpforwardobserver.h>

/**
* Call forward methods.
* MCCPCallObserver is used for errors, statuses, notifications etc
*
*  @since S60 3.2
*/
class CSPForwardProvider : public CBase,
                           public MCCPForwardProvider
    {
    
    public:
        /**
        * Two phased constructing of the forward provider instance.
        * @return the Forward provider instance
        */
        static CSPForwardProvider* NewL();

        /** 
        * Destructs the forward provider.
        * @since S60 3.2
        */
        virtual ~CSPForwardProvider();
        
        /**
        * Updates the call event for observers.
        * @param aEvent the event to be notified
        */
        void NotifyForwardEventOccurred( 
                        MCCPForwardObserver::TCCPForwardEvent aEvent );
        
// from base class MCCPForwardProvider
        
        /**
        * Returns pointer to array containing addresses to forward the call to.
        * Index zero contains the first address. If there are no addresses available
        * returns an empty array.
        * @return Reference to array containing addresses
        * @leave KErrNotReady call is not in forwarding state
        * @leave KErrNotSupported if call is not mobile originated
        * @leave system error code 
        * @pre Call state is MCCPCallObserver::ECCPStateForwarding and call type is MO
        * @pre Call MCCPForwardObserver::ECCPMultipleChoices event is received
        */
        virtual const CDesC8Array& GetForwardAddressChoicesL();
    
        /**
        * Forward call to address at given index.
        * @param aIndex Index of address where the call is to be forwarded. Address is 
        * found in the array received from GetForwardAddressChoicesL. Index starts from zero.
        * @leave KErrArgument Index is not in array
        * @leave KErrNotReady Call is not in forwarding state
        * @leave KErrNotSupported If call is not mobile originated
        * @pre Call state is MCCPCallObserver::ECCPStateForwarding and call type is MO
        * @pre Call MCCPForwardObserver::ECCPMultipleChoices event is received
        */
        virtual void ForwardToAddressL( const TInt aIndex );
        
        /**
        * Add an observer for forward related events.
        * Currently CCE will set only one observer.
        * @since S60 v3.2
        * @param aObserver Observer
        * @leave system error if observer adding fails
        */
        virtual void AddObserverL( const MCCPForwardObserver& aObserver );
    
        /**
        * Remove an observer.
        * @since S60 v3.2
        * @param aObserver Observer
        * @return KErrNone if removed succesfully. 
        * @return KErrNotFound if observer was not found.
        */
        virtual TInt RemoveObserver( const MCCPForwardObserver& aObserver );

    private:
        
        /**
        * Constructs the provider
        *
        */
        CSPForwardProvider( );

        /**
        * Constructing 2nd phase.
        */
        void ConstructL();
        
    private: // data

        /**
        * DTMF event observer.
        */
        RPointerArray<MCCPForwardObserver> iObservers;
        
        CDesC8Array* iEmptyArray;
            

};

#endif CSPFORWARDPROVIDER_H
