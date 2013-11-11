/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     This class implements notifications from bearers which is needed when
*     transferring data to/from wap gw.
*
*/



#ifndef CMMSBEARERSTATUS_H
#define CMMSBEARERSTATUS_H

//  INCLUDES
#include    <e32base.h>
#include    <nifman.h>

#include    "mmssession.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MMmsTransportObserver;

// CLASS DECLARATION
/**
*  Class for detecting GPRS state and if transfer was cancelled
*/
NONSHARABLE_CLASS( CMmsBearerStatus ): public CActive
    {
    enum TMmsBearerType
        {
        EMmsBearerGprs = 0,
        EMmsBearerWcdma
        };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aRConnection opened RConnection
        */
        static CMmsBearerStatus* NewL( RConnection& aRConnection );

        /**
        * Destructor.
        */
        virtual ~CMmsBearerStatus();

    public: // New functions

        /**
        * Used to query if the bearer is set to GPRS
        * @return True if GPRS, false otherwise
        */
        TBool IsGprs() const;
        
        /**
        * Used to query if the bearer (GPRS only) is in suspended state
        * @return True if suspended, false otherwise
        */
        TBool IsSuspended();

        /**
        * A request function to notify the requester if transfer is cancelled
        * NOTE! NotifySuspendResume and NotifyTransferCancel can not be called 
        * from same instance at the same time !        
        * @param aRequester Pointer to the object making the request 
        */
        void SubscribeNotification( MMmsTransportObserver* aRequester);

        /**
        * Function to set the bearer in use
        * @param aBearerType Type of bearer
        */
        void SetBearer( TMmsBearerType aBearerType );

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * Active object cancellation
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CMmsBearerStatus();

        /**
        * By default Symbian OS constructor is private.
        * @param aRConnection opened RConnection
        */
        void ConstructL( RConnection& aRConnection );

        /**
        * Completion
        */
        void RunL();

        // By default, prohibit copy constructor
        CMmsBearerStatus( const CMmsBearerStatus& );

        // Prohibit assignment operator
        CMmsBearerStatus& operator= ( const CMmsBearerStatus& );

    public:     // Data

    protected:  // Data

    private:    // Data

    TMmsBearerType      iBearerType;
    MMmsTransportObserver*   iRequester;

    RConnection*        iRConnection;
    TNifProgressBuf     iProgressBuffer;

    TBool               iConnected;
    TBool               iSuspended;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

    #include "mmsbearerstatus.inl"

#endif      // CMMSBEARERSTATUS_H

// End of File
