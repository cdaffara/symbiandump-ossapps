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
* Description:  Header file for MCSPCallObserver class for 
*                observing errors related to call requests, e.g. dialling.
*
*/



#ifndef MCSPCALLOBSERVER_H
#define MCSPCALLOBSERVER_H

//INCLUDES
#include <etelmm.h>

#include <mccpcallobserver.h>
#include <mccptransferobserver.h>
#include <mccpforwardobserver.h>
#include <mccpconferencecallobserver.h>



// CLASS DECLARATION

/**
*  Interface for monitoring incoming call.
*
*  @since S60 3.2
*/
class MCSPCallObserver
    {
    public:

        /**
        * Informs about occurred event.
        * @since S60 3.2
        * @param aEvent occurred event
        */    
        virtual void NotifyCallEventOccurred( 
                        MCCPCallObserver::TCCPCallEvent aEvent ) = 0;

        /**
        * Updates the state change and forwards the state for observer.
        * @param aState new state (ETel) of the call 
        */
        virtual void NotifyCallStateChangedETel( 
            RMobileCall::TMobileCallStatus aState ) = 0;

        /**
        * Informs about changed state that includes inband tones.
        * @since S60 3.2
        * @param aState the new state 
        */    
        virtual void NotifyCallStateChangedWithInband( 
                        MCCPCallObserver::TCCPCallState aState ) = 0;

        /**
        * Informs about occurred transfer related event.
        * @since S60 3.2
        * @param aEvent occurred transfer event
        */    
        virtual void NotifyTransferCallEventOccurred( 
                        const MCCPTransferObserver::TCCPTransferEvent aEvent ) = 0;

                        
        /**
        * Updates the call event for observers.
        * @param aEvent the event to be notified
        */
        virtual void NotifyForwardEventOccurred( 
                        MCCPForwardObserver::TCCPForwardEvent aEvent ) = 0;
                        
                        
        /**
        * Updates changed call capability information.
        * @param aCapsFlags new caps flags
        */
        virtual void CallCapsChanged( const TUint32 aCapsFlags ) = 0;
        
        /**
        * Notify that remote party info has changed.
        * @param aRemotePartyName new remote party name
        * @param aRemotePartyNumber new remote party number
        * 
        */
        virtual void NotifyRemotePartyInfoChanged( 
            const TDesC& aRemotePartyName,
            const TDesC& aRemotePartyNumber
        ) = 0;

        
    };

#endif // MINCOMINGCALLOBSERVER_H

//End of file
