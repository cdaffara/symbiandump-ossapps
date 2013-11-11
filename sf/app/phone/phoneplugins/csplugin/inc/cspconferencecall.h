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
* Description:  Implements CSPConferenceCall for CS Call Plug-in
*
*/



#ifndef CSPCONFERENCECALL_H
#define CSPCONFERENCECALL_H

#include <mccpconferencecall.h>
#include <mccpconferencecallobserver.h>
#include <mccpcallobserver.h>
#include <mccpcscall.h>
#include <etel.h>
#include <etelmm.h>

#include "mcspconferencestatusobserver.h"
#include "cspconsts.h"
#include "cspetelconferencecallrequester.h" // For the requester and TConferenceRequestType


class MCCPForwardProvider;
class MCCPTransferProvider;
class MCCPObserver;
class MCSPCallInformation;
class CSPConferenceCallObserver;
class RMobileConferenceCall;
class CSPEtelConferenceStatusMonitor; 
class CSPEtelConferenceEventMonitor;
class CSPEtelConferenceCapsMonitor;


/**
* Call class for CS Call Plug-in. Implements the API defined by CCP in classes
* MCCPCall and MCCPCSCall
*
*  @lib csplugin.dll
*/
class CSPConferenceCall : public CBase,
                          public MCCPConferenceCall,
                          public MCSPConferenceStatusObserver
    {
    
    public:
        /*
        * Creates a new CSPConferenceCall object.
        * @param aPhone reference to phone object
        * @return pointer to created call object
        * @param aServiceId the service id
        */
        static CSPConferenceCall* NewL( 
                              RMobilePhone& aPhone,
                              MCSPCallInformation& aCallInfo,
                              TUint32 aServiceId );
        
        /*
        * C++ default destructor
        */
        virtual ~CSPConferenceCall( );

        /** 
        * Getter for observer
        * @return observer for CS calls.
        */
        MCCPCallObserver* Observer() const;

        /**
        * Notifies call specific event to observers.
        * @param aEvent conference event that was occurred
        * @param aCallName the name of the related call object
        */        
        void NotifyEvent( 
            MCCPConferenceCallObserver::TCCPConferenceCallEvent aEvent,
            TName& aCallName );
            
        /**
        * Notifies non-'call specific' event to observers.
        * @param aEvent conference event that was occurred,
        */        
        void NotifyEvent( 
            MCCPConferenceCallObserver::TCCPConferenceCallEvent aEvent );

        /**
        * Notifies about conference call request related errors.
        */
        void NotifyConferenceError( TCCPConferenceCallError aErr );

        /**
        * Handles caps change.
        * @param aNewCaps new caps
        */
        void ConferenceCapsChanged( TUint32 aNewCaps );

// from base class MCSPConferenceStatusObserver
        
        /**
        * From MCSPConferenceStatusObserver.
        * Notifies event to observers.
        * @param aStatus new status of the conference call
        */        
        void NotifyStateChange( 
            MCSPConferenceStatusObserver::TCSPConferenceState aStatus );
        
// from base class MCCPConferenceCall

        /**
        * From MCCPConferenceCall.
        * Adds new call to conference call.
        * @param aCall Call to be added to conference
        * @return Leaves in case of an error. 
        * KErrArgument, KErrNoMemory
        */
        void AddCallL( MCCPCall* aCall );
    
        /**
        * From MCCPConferenceCall.
        * Removes call from conference call.
        * @param aCall Call to be removed from conference
        * @return In error case leaves. 
        * KErrArgument, KErrNotFound
        */
        void RemoveCallL( MCCPCall* aCall );
    
        /**
        * From MCCPConferenceCall.
        * Returns number of calls in conference call.
        * @param aCallCount Number of calls in conference.
        * @return KErrNone if succesfull, otherwise system wide error code.
        */
        TInt CallCount( ) const;
    
        /**
        * From MCCPConferenceCall.
        * Switch to a private call with one call from conference call.
        * @param aCall Call to be removed from conference
        * @return In error case leaves. 
        * KErrArgument, KErrNotFound
        */
        void GoOneToOneL( MCCPCall& aCall );
        
        /**
        * Adds current calls (1 hold + connected )
        * to part of conference
        * @leave In case of an error system wide error code
        */
        void CurrentCallsToConferenceL();        
        
        /**
        * From MCCPConferenceCall
        * Returns serviceId
        * @since S60 3.2
        * @return TUint32 ServiceId
        */
        TUint32 ServiceId() const;
        
        /**
        * From MCCPConferencCall
        * Ends an ongoing call.
        * @since S60 3.2
        */
        TInt HangUp();

        /**
        * From MCCPConferenceCall
        * Puts the specified call on hold.
        * @since S60 3.2
        */
        TInt Hold();

        /**
        * From MCCPConferenceCall
        * Resumes the call.
        * @since S60 3.2
        */
        TInt Resume();

        /**
        * From MCCPConferenceCall
        * Swaps a connected call to its opposite state either active or on hold
        * @since S60 3.2
        */
        TInt Swap();

        /**
        * Adds a new observer for the conference call object.
        * @since S60 3.2
        * @param aObserver the conference observer to be added
        */
        void AddObserverL( const MCCPConferenceCallObserver& aObserver );
        
        /**
        * Removes an observer.
        * @since S60 3.2
        * @param aObserver the observer to be removed
        * @return system wide error code
        */
        TInt RemoveObserver( const MCCPConferenceCallObserver& aObserver );
                
        /**
        * Get conference participants.  
        * @since S60 5.0 
        * @param aCallArray array where participating calls are stored 
        * @return system wide error code
        */
        TInt GetCallArray( RPointerArray<MCCPCall>& aCallArray );  
        
    private:    
        
        /*
        * C++ default constructor
        * @param aPhone handle to the ETel Phone
        * @param aCallInfo call information 
        * @param aServiceId service id
        */
        CSPConferenceCall( RMobilePhone& aPhone, 
                           MCSPCallInformation& aCallInfo,
                           TUint32 aServiceId  );
                 
        /*
        * Constructing 2nd phase.
        * @since S60 3.2
        * @param aObserver the CCP call observer,
        */
        void ConstructL( );
                
        /**
        * Get conference state from ETel.  
        * @since S60 5.0 
        * @param aState param for storing conference call state  
        * @return system wide error code
        */
        TInt GetConferenceCallState( 
                MCCPConferenceCallObserver::TCCPConferenceCallState& aState ); 
        
    private:

      /**
      * Call status monitor.
      * Own.
      */
      CSPEtelConferenceStatusMonitor* iCallStatusMonitor;
        
      /**
      * Call event monitor.
      * Own.
      */
      CSPEtelConferenceEventMonitor* iCallEventMonitor;

      /**
      * Call caps monitor.
      * Own.
      */
      CSPEtelConferenceCapsMonitor* iCallCapsMonitor;
        
      /**
      * ETel call object
      */
      RMobileConferenceCall iCall;
      
      /**
      * ETel phone
      */
      RMobilePhone& iPhone;
      
      /**
      * Conference call state.
      */ 
      MCCPConferenceCallObserver::TCCPConferenceCallState iCallState;
                
      /**
      * Makes the asynchronic requests for various purposes.
      */
      CSPEtelConferenceCallRequester* iRequester;
      
      /**
      * Observer array for conference states, events and errors.
      */      
      RPointerArray<MCCPConferenceCallObserver> iObservers;
      
      /**
      * Call count for adding call.
      */
      TInt iCallCountForAddCall;
      
      /**
      * Call Information for resolving call pointers from
      * the call names.
      */
      MCSPCallInformation& iCallInfo;
      
      /**
      * Service ID
      */
      TUint32 iServiceId;
      
      /**
      * Call name of a last call added to this conference..
      */
      TName iAddedCallName;

      /**
       * Allocate CallInfo into heap since it could be
       * too big fit to stack.
       */
      RMobileCall::TMobileCallInfoV3 iEtelCallInfo;
    };

#endif // CSPCONFERENCECALL_H
