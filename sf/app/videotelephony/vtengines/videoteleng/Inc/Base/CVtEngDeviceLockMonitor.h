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
* Description:  Monitor for device lock (autolock)
*
*/



#ifndef CVTENGDEVICELOCKMONITOR_H
#define CVTENGDEVICELOCKMONITOR_H

//  INCLUDES
#include <e32base.h>
#include "MVtEngSettingPSObserver.h"

// FORWARD DECLARATIONS
class CVtEngPubSubsListener;

// CLASS DECLARATION

/**
*  Monitor for autolock state.
*
*  @lib videoteleng
*  @since 3.1
*/
NONSHARABLE_CLASS( CVtEngDeviceLockMonitor ) : public CBase, public MVtEngSettingPSObserver
    {
    public:  // Constructors and destructor
        
        static CVtEngDeviceLockMonitor* NewL();

        /**
        * Destructor.
        */
        virtual ~CVtEngDeviceLockMonitor();

    public: // New functions
        
        /**
        * Sends event to UI of current autolock state
        */
        void NotifyState() const;

    public: // From MVtEngSettingPSObserver
    
        /**
        * @see MVtEngSettingPSObserver::HandleNotifyPSL
        */
        void HandleNotifyPSL( const TUid aUid, const TInt& aKey, 
            const TRequestStatus& aStatus );
            
    private:
        /**
        * C++ constructor.
        */
        CVtEngDeviceLockMonitor();

        /**
        * 2nd phase constructor
        */
        void ConstructL();
        
    private:
        
        // Listener for autolock state
        CVtEngPubSubsListener* iAutolockListener;
    };

#endif      // CVTENGDEVICELOCKMONITOR_H
            
// End of File
