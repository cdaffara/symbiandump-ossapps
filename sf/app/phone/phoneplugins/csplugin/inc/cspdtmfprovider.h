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
* Description:  Header of CSPDTMFProvider which provides DTMF functionality.
*
*/


#ifndef CSPDTMFPROVIDER_H
#define CSPDTMFPROVIDER_H


#include <e32base.h>
#include <mccpdtmfprovider.h>
#include <mccpdtmfobserver.h>


class RMobilePhone;
class CSPEtelDtmfMonitor;
class CSPEtelDtmfStopMonitor;
class RMmCustomAPI;

/**
* Makes asynchronic request to ETel interface according to given request type.
* Provides canceling via CActive::Cancel().
* 
*/
class CSPDTMFProvider: public CActive,
                       public MCCPDTMFProvider        
    {
        
    public:
    
        /**
        * Two phased constructing of the DTMF provider instance.
        * @param aPhone mobile phone handle
        * @param aMmCustom custom API handle for special DTMF event 
        *        monitoring.
        * @return the DTMF provider instance
        */
        static CSPDTMFProvider* NewL( RMobilePhone& aPhone, 
                                      RMmCustomAPI& aMmCustom );
        
        
        /**
        * C++ default destructor
        */
        virtual ~CSPDTMFProvider( );
        
        /**
        * HandleDTMFEvents.
        * @since S60 3.2
        * @param aEvent Event type
        * @param aError Error code
        * @param aTone Character
        */
        void NotifyDTMFEvent( const MCCPDTMFObserver::TCCPDtmfEvent aEvent,
                              const TInt aError,
                              const TChar aTone );

        
// from base class MCCPDTMFProvider
        /**
        * Cancels asynchronous DTMF string sending.
        * @return KErrNone if succesfull, otherwise another system wide error code
        */
        TInt CancelDtmfStringSending();

        /**
        * Starts the transmission of a single DTMF tone across a
        * connected and active call.
        * @param aTone Tone to be played.
        * @return KErrNone if succesfull, otherwise another system wide error code
        */
        TInt StartDtmfTone( const TChar aTone );

        /**
        * Stops playing current DTMF tone.
        * @return KErrNone if succesfull, otherwise another system wide error code
        */
        TInt StopDtmfTone();

        /**
        * Plays DTMF string.
        * @param aString String to be played.
        * @return KErrNone if succesfull, otherwise another system wide error code
        * KErrArgument if the specified string contains illegal DTMF characters
        */
        TInt SendDtmfToneString( const TDesC& aString );

        /**
        * Continue or cancel sending DTMF string which was stopped with 'w'-character
        * in string.
        * @param aContinue ETrue if sending of the DTMF string should continue,
        * EFalse if the rest of the DTMF string is to be discarded.
        * @return KErrNone if succesfull, otherwise another system wide error code
        */
        TInt ContinueDtmfStringSending( const TBool aContinue );

        /**
        * Add an observer for DTMF related events.
        * Plug-in dependent feature if duplicates or more than one observers 
        * are allowed or not. Currently CCE will set only one observer.
        * @since S60 v3.2
        * @param aObserver Observer
        * @leave system error if observer adding fails
        */
        void AddObserverL( const MCCPDTMFObserver& aObserver );

        /**
        * Remove an observer.
        * @since S60 v3.2
        * @param aObserver Observer
        * @return KErrNone if removed succesfully. KErrNotFound if observer was not found.
        * Any other system error depending on the error.
        */
        TInt RemoveObserver( const MCCPDTMFObserver& aObserver );
            
// from base class CActive
    protected:
        /**
        * From CActive
        * RunL
        * @since S60 3.2
        */
        void RunL();
        
        /**
        * From CActive
        * Cancels the monitor
        * @since S60 3.2
        */
        void DoCancel();  

        
    private:
        /**
        * Constructs the requester.
        *
        * @param aPhone handle to ETel phone
        * @param aMmCustom custom API handle
        */
        CSPDTMFProvider( RMobilePhone& aPhone, RMmCustomAPI& aMmCustom );
        
        /**
        * Constructing the provider in the second phase.
        */
        void ConstructL();
    
    private: // data
    
        /**
        * DTMF event observer.
        */
        RPointerArray<MCCPDTMFObserver> iObservers;
        
        /**
        * ETel phone handle for DTMF functionality.
        */
        RMobilePhone& iPhone;
        
        /**
        * Monitor for DTMF events and changes.
        * Own.
        */
        CSPEtelDtmfMonitor* iMonitor;

        /**
        * Monitor for DTMF stopping.
        * Own.
        */
        CSPEtelDtmfStopMonitor* iStopMonitor;
        
        /**
        * Custom API reference.
        */
        RMmCustomAPI& iMmCustom;
        
    };
    
#endif // CSPDTMFPROVIDER