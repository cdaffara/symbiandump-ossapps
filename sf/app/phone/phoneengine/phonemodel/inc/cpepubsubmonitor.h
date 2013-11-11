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


#ifndef CPEPUBSUBMONITOR_H
#define CPEPUBSUBMONITOR_H

// INCLUDE FILES
#include <e32base.h>
#include <e32property.h>
#include <talogger.h>

// Forward declarations
class CRepository;
class MPEPhoneModelInternal;
class RProperty;

// BASE CLASS

// CLASS DECLARATION

/**
*   CPEPubSubMonitor
*   Active object base class for publish&subscribe monitor code - not instantiable
*   Exported methods are used by variant phone engine or other PE components
*   This class should not be used by components external to the TelephonyEngine
*
*  @lib PhoneEngineBase.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPEPubSubMonitor ) : public CActive
    {
    protected:

        ~CPEPubSubMonitor();
        
        CPEPubSubMonitor( MPEPhoneModelInternal& aModel );

        /**
        * Creates a new property object and submits a notification request
        */
        void BaseConstructL( TUid aCategory, TUint aKey, TUint aType );

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
        * Gets the current integer value of the monitored key
        * @param TInt& aValue the current value of the monitored setting
        * @return TInt Symbian OS error code from central repository
        */
        virtual TInt Get( TInt& aValue );

        /**
        * Gets the current binary value of the monitored key
        * @param TDes8& aValue the current value of the monitored setting
        * @return TInt Symbian OS error code from central repository
        */
        virtual TInt Get( TDes8& aValue );

        /**
        * Gets the current text value of the monitored key
        * @param TDes16& aValue the current value of the monitored setting
        * @return TInt Symbian OS error code from central repository
        */
        virtual TInt Get( TDes16& aValue );
        
    protected: // From CActive

        virtual void DoCancel();
        virtual TInt RunError( TInt aError );
        virtual void RunL();

    protected:         

        // Not owned by this object: reference to phone model object    
        MPEPhoneModelInternal& iModel;   
        
    private:        
        // Owns handle to property object
        RProperty iProperty;

    #ifdef TEF_LOGGING_ENABLED            
        // Category of property
        TUid iCategory;
        
        // Key of property
        TUint iKey;
    #endif
};

#endif // CPEPUBSUBMONITOR_H
