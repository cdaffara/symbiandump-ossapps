/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header of CSPTransferProvider which provides methods and
*                functionality to handle call transfering
*
*/


#ifndef CSPTRANSFERPROVIDER_H
#define CSPTRANSFERPROVIDER_H

#include <e32base.h>
#include <mccptransferprovider.h>
#include <mccptransferobserver.h>

class RMobileCall;
class MCSPCallErrorObserver;
class MCSPCommonInfo;

/**
* Call transfer methods.
* MCCPCallObserver is used for errors, statuses, notifications etc
*
*  @since S60 3.2
*/
class CSPTransferProvider : public CActive,
                            public MCCPTransferProvider
    {
    
    public:
        /**
        * Two phased constructing of the transfer provider instance.
        * @param aCall mobile call handle
        * @return the Transfer provider instance
        */
        static CSPTransferProvider* NewL( RMobileCall& aCall, 
                MCSPCallErrorObserver& aErrorObserver,
                MCSPCommonInfo& aCommonInfo );        

        /** 
        * Destructs the transfer provider.
        * @since S60 3.2
        */
        virtual ~CSPTransferProvider();
        
    public:

    // from base class MCCPTransferProvider
        /**
        * Attended transfer to given call recipient.
        * There exist a call between A-B and A-C. A wants to transfer the call
        * to B-C. A itself will not be inline in case the transfer is made 
        * succesfully.In above case C will be the aTransferTargetCall. B is 
        * the recipient of the current call between A-B - called on A's call
        * object transfer provider. The A-B call is on hold (optional).
        *
        * @param aTransferTargetCall Target receiver of the transferred call. 
        *                            In this case there exist a call.
        * @return In case of an immediate error returns system error code, 
        *         if request has been started returns KErrNone.
        * Actual request result indication comes via observer class
        */
        TInt AttendedTransfer( MCCPCall& aTransferTargetCall );

        /**
        * Attended transfer to given address. There does not need to be 
        * a current call between the given transfer target.
        * @param aTransferTarget Transfer target address
        * @return In case of an immediate error returns system error code,
        *         if request has been started returns KErrNone.
        * Actual request result indication comes via observer class
        */
        TInt AttendedTransfer( const TDesC& aTransferTarget );

        /**
        * Unattended transfer. Call is requested to be transferred to given
        * address. After ECCPRemoteTransferring event current call will be
        * disconnected and no transfer status is checked from the operation. 
        * @param aTransferTarget Address of the target
        * @return In case of an immediate error returns system error code, 
        *         if request has been started returns KErrNone.
        * Actual request result indication comes via observer class
        */
        TInt UnattendedTransfer( const TDesC& aTransferTarget );

        /**
        * Accept incoming call transfer request from the call remote party.
        * @param aAccept ETrue - accept transfer, 
        *                EFalse do not accept transfer request.
        * @return In case of an immediate error returns system error code, 
        *         if request has been started returns KErrNone.
        * Actual request result indication comes via observer class
        */
        TInt AcceptTransfer( const TBool aAccept );
          
        /**
        * When the other end of the call has requested call transfer to
        * third party it is notified to 
        * CCPCallCall::EventOccurred(ECCPRemoteTransferring).
        * The new recipient of the call can be fetched via this method.
        * @return New recipient for the call after transfer
        */
        const TDesC& TransferTarget() const;
        
        /**
        * Add an observer for transfer related events.
        * Plug-in dependent feature if duplicates or more than one observers 
        * are allowed or not. Currently CCE will set only one observer.
        * @since S60 v3.2
        * @param aObserver Observer
        * @leave system error if observer adding fails
        */
        void AddObserverL( const MCCPTransferObserver& aObserver );

        /**
        * Remove an observer.
        * @since S60 v3.2
        * @param aObserver Observer
        * @return KErrNone if removed succesfully
        *         KErrNotFound if observer was not found.
        *         Any other system error depending on the error.
        */
        TInt RemoveObserver( const MCCPTransferObserver& aObserver );
        
        /**
        * A transfer event has occurred concerning a specific call.
        * @since S60 3.2
        * @param aEvent Occurred event.
        */
        void TransferEventOccurred( 
            const MCCPTransferObserver::TCCPTransferEvent aEvent );

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
        * Constructs the provider
        *
        * @param aCall handle mobile call
        */
        CSPTransferProvider( RMobileCall& aCall, 
                             MCSPCallErrorObserver& aErrorObserver,
                             MCSPCommonInfo& aCommonInfo );

        /**
        * Constructing 2nd phase.
        */
        void ConstructL();
        
    private: // data

        /**
        * DTMF event observer.
        */
        RPointerArray<MCCPTransferObserver> iObservers;
            
        /**
        * Etel call object
        */
        RMobileCall& iCall;
        
        /**
        * Error observer;
        */
        MCSPCallErrorObserver& iErrorObserver;
        
        /**
        * Common information for calls.
        */
        MCSPCommonInfo& iCommonInfo;


};

#endif CSPTRANSFERPROVIDER_H
