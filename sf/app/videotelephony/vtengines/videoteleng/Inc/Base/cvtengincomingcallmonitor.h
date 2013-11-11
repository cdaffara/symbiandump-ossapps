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
* Description:  Monitor for incoming call
*
*/



#ifndef C_VTENGINCOMINGCALLMONITOR_H
#define C_VTENGINCOMINGCALLMONITOR_H

//  INCLUDES
#include <e32base.h>
#include "MVtEngSettingPSObserver.h"

// FORWARD DECLARATIONS
class CVtEngPubSubsListener;

// CLASS DECLARATION
/**
*  Monitor for incoming call state.
*
*  @lib videoteleng
*  @since S60 v3.2
*/
NONSHARABLE_CLASS( CVtEngIncomingCallMonitor ) : public CBase, public MVtEngSettingPSObserver
    {
public:  // Constructors and destructor
        
    static CVtEngIncomingCallMonitor* NewL();

    /**
     * Destructor.
     */
    virtual ~CVtEngIncomingCallMonitor();

public: // New functions
        
	/**
	 * Sends event to UI of current incoming call state
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
    CVtEngIncomingCallMonitor();

    /**
     * 2nd phase constructor
     */
    void ConstructL();
        
private:
        
     // Listener for incoming call state
    CVtEngPubSubsListener* iIncomingCallListener;
    };

#endif      // C_VTENGINCOMINGCALLMONITOR_H
            
// End of File
