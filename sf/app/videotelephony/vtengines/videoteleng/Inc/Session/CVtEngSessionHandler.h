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
* Description:  Video session interface.
*
*/



#ifndef CVTENGSESSIONHANDLER_H
#define CVTENGSESSIONHANDLER_H

//  INCLUDES
#include <e32std.h>
#include "MVtEngSessionInfo.h"
#include "MVtCtlEventObserver.h"
#include "MVtCtlCallControl.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CVtEngStateManager;
class CVtCtlFactory;
class CVtEngOperation;
class CVtEngIncomingCallMonitor;
// CLASS DECLARATION

/**
*  Abstraction for video telephony session with remote end.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngSessionHandler ) : 
        public CActive, 
        public MVtEngSessionInfo,
        public MVtCtlEventObserver
    {
    public: // constructor and destructors

        static CVtEngSessionHandler* NewL( );

        virtual ~CVtEngSessionHandler();

    public: // New functions

        /**
        * Performs on operation in the handler.
        * @param operation to be performed on the handler
        */
        void HandleL( CVtEngOperation& aOperation );

    public: // from MVtEngSessionInfo

        /**
        * Gets session state.
        * @return
        */
        virtual TSessionState State( TBool aForcedRefresh ) const;

        /**
        * Gets session direction.
        * @return
        */
        virtual TInt GetDirection( TDirection& aDirection ) const;

        /**
        * Gets session duration.
        * @param 
        * @param
        */
        virtual TInt GetDuration( 
            TDuration& aDuration,
            TBool& aEnabled ) const;

        /*
        * @see MVtEngSessionInfo::aCLI
        */
        virtual TInt GetCLI( TCLI& aCLI ) const;
        
    public: // From MVtCtlEventObserver
        virtual void HandleVtSessionEventL( 
            TVtCtlEvent aEvent,
            TAny* aParams );
    
    public: // new functions
        /**
        * Initializes session handler.
        */
        void InitializeL();

        /**
        * Frees resources used by session handler.
        */
        void Uninitialize();

        MVtCtlCallControl::TVtCtlState RealState() const;
        
        /**
        * Starts Incoming call PS key monitoring
        */
        void StartIncomingCallMonitorL();

    private: // constructors

        void ConstructL();

        CVtEngSessionHandler( );

    private: // from CActive
        void RunL();

        void DoCancel();

    private: // Member data
        MVtCtlCallControl* iSessionControl;
        CVtCtlFactory*     iFactory;

        RLibrary iLib;
        
        CVtEngIncomingCallMonitor* iIncomingCallMonitor; 
    };


#endif      // CVTENGSESSIONHANDLER_H
            
// End of File
