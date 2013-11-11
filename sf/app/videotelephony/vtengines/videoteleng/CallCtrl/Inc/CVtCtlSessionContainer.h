/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for ETel session and subsessions
*
*/



#ifndef CVTCTLSESSIONCONTAINER_H
#define CVTCTLSESSIONCONTAINER_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h>
#include "MVtCtlLineEventObserver.h"
#include "MVtCtlCallStatusObserver.h"
#include "MVtCtlCallControl.h"

// FORWARD DECLARATIONS
class CVtCtlLineMonitor;
class CVtCtlCallMonitor;
class CVtCtlVideoCall;

// CLASS DECLARATION

/**
*  Container for ETel (sub)session(s).
*
*  @lib CSCallCtrl
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtCtlSessionContainer ) : 
    public CBase, 
    public MVtCtlLineEventObserver,
    public MVtCtlCallStatusObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aObserver call status observer
        */
        static CVtCtlSessionContainer* NewL(
            MVtCtlCallStatusObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CVtCtlSessionContainer();

    public: // MVtCtlLineEventObserver

        /**
        * @see MVtCtlLineEventObserver::HandleLineEventL
        */
        virtual void HandleLineEventL(
            const TLineEvent aEvent, 
            TDesC& aCallName );

        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        virtual void HandleLineEventL( 
            const TLineEvent aEvent, 
            const RMobileCall::TMobileCallStatus aStatus );

    public: // MVtCtlCallStatusObserver
        
        /**
        * @see MVtCtlLineEventObserver::HandleCallStatusChangeL
        */
        virtual void HandleCallStatusChangeL( 
            TVtCtlSessionId aId,
            MVtCtlCallControl::TVtCtlState aStatus,
            MVtCtlCallControl::TVtCtlState aPreviousStatus );

    public: // New functions

        /**
        * Finds call by name.
        * @param aCallName search key
        * @return call or NULL if not found
        */
        CVtCtlVideoCall* FindCall( TDesC& aCallName ) const;
        
        /**
        * Finds call by session id.
        * @param aId search key
        * @return call or NULL if not found
        */
        CVtCtlVideoCall* FindCall( TVtCtlSessionId aId ) const;

        /**
        * Gets sessions.
        * @param aArray sessions are returned in this
        * @return number of sessions in aArray
        */
        TInt GetSessions( RVtCtlSessionIdArray& aArray ) const;        

    private:

        /**
        * Removes call from array and deletes it.
        * @param aId session id
        */
        void DestructCall( TVtCtlSessionId aId );

        /**
        * Checks that call is of correct type (multimedia)
        * @param aName call object
        * @param aName call name
        * @return ETrue if call is multimedia call
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        TBool CheckCallTypeL( 
            RMobileCall& aCall,
            const TName& aName );

        /**
        * Initializes video call object
        * @param aCallName call name        
        * @return ETrue if call was initialized
        */
        TBool InitCallL( const TName& aCallName );

    private:

        /**
        * C++ constructor.
        * @param aObserver call status observer
        */
        CVtCtlSessionContainer( MVtCtlCallStatusObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Class for maintaining session identities
        */
        class TVtCtlSessionIdentities
            {
            public:
                // c++ default constructor
                TVtCtlSessionIdentities();

                /**
                * Create new session id
                * @return session id
                */
                TVtCtlSessionId Create();

                /**
                * Returns current session id
                * @return session id
                */
                TVtCtlSessionId Current() const;

            private:
                // Last session id
                TVtCtlSessionId iCurrentId;
            };

    private:    // Data
        
        // ETel session
        RTelServer             iServer;

        // Phone session
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobilePhone           iPhone;

        // Data line
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        RMobileLine            iLine;

        // Monitors new calls on line.
        CVtCtlLineMonitor*     iLineMonitor;
        
        // Array of ongoing video calls
        RPointerArray<CVtCtlVideoCall> iCalls;

        // Session id management
        TVtCtlSessionIdentities iSessionIds;

        // Call status observer
        MVtCtlCallStatusObserver& iObserver;
    };

#endif      // CVTCTLSESSIONCONTAINER_H   
            
// End of File
