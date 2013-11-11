/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for MTelDMCallStateObserver class.
*
*/



#ifndef CTELDMCALLSTATELISTENER_H
#define CTELDMCALLSTATELISTENER_H

//  INCLUDES
#include    <e32property.h>
#include    <e32base.h>


// FORWARD DECLARATIONS

// CLASS DECLARATION
class MTelDMCallStateObserver;

/**
*  Call state listener. Listening TelephonyCallState PS key. 
*
*  @since 3.1
*/
 
NONSHARABLE_CLASS( CTelDMCallStateListener ) : public CActive
    {
    public: // Constructors and destructors.

        /**
        * Two-phased constructor.
        */ 
        static CTelDMCallStateListener* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CTelDMCallStateListener();

   
    public:
   
        void AddObserverL( MTelDMCallStateObserver& aCallStateObserver );
        
    private: // from CActive
   
        /**
        * @see CActive::RunL.
        */
        void RunL();

        /**
        * @see CActive::DoCancel.
        */
        void DoCancel();
     
    private:
        
        /**
        * Handle notification request
        */      
        void HandleChange();
        
        /**
        * Start notification.
        */
        void IssueRequest();
        
        /**
        * Constructor.
        */
        CTelDMCallStateListener();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();
   
    private:
        // Owned Publish & Subscribe instance.
        RProperty iProperty;
        
        // Call state
        TInt iCallState;
        
        // Call State Change Observer array.
        RPointerArray< MTelDMCallStateObserver > iObserverArray;
    };
 
#endif      // CTELDMCALLSTATELISTENER_H  

// End of File
