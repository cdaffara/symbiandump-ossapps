/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header of the phone engine base 
*                central repository monitor classes 
*
*/


#ifndef CPECENREPMONITOR_H
#define CPECENREPMONITOR_H

// INCLUDE FILES
#include <e32base.h>

// Forward declarations
class CRepository;

// BASE CLASS

// CLASS DECLARATION

/**
*   CPECenRepMonitor
*   Active object base class for central repository monitor code - not instantiable
*   Exported methods are used by variant phone engine or other PE components
*   This class should not be used by components external to the TelephonyEngine
*
*  @lib PhoneEngineBase.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPECenRepMonitor )
    : public CActive
    {
    protected:

        ~CPECenRepMonitor();
        CPECenRepMonitor( TUint32 aMonitorSetting );

        /**
        * Creates a new repository object and submits a notification request
        */
        void BaseConstructL( TUid aRepositoryUid );

    protected:

        /**
        * Submits a notification request
        */
        void SubmitNotifyRequestL();

        /**
        * Defines the actions that take place when a change indication
        * is received. Each deriving monitor has to define the implementation for
        * this method. Most common approach would be to save the data to internal
        * data store and send a message via phone model if neccessary.
        */
        virtual void UpdateL() = 0;

    public:

        /**
        * Gets the current value of the monitored setting
        * @param aValue the current value of the monitored setting
        * @return TInt Symbian OS error code from central repository
        */
        virtual TInt Get( TInt& aValue );

    protected: // From CActive

        virtual void DoCancel();
        virtual TInt RunError( TInt aError );
        virtual void RunL();

    protected: // Owned by this object

        // Handle to repository object
        CRepository* iRepository;
        // Repository key, i.e. the identification number of the monitored setting
        TUint32 iMonitorSetting;
};

#endif // CPECENREPMONITOR_H
