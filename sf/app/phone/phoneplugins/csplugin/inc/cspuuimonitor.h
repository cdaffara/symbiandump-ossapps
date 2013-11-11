/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Monitor UUI messages.
*
*/


#ifndef CSPUUIMONITOR_H
#define CSPUUIMONITOR_H

#include <e32base.h>
#include <etelmm.h>

class MCSPUUSMessageObserver;

/**
 *  Monitors UUI messages.
 *
 *  @lib PhoneEngine.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CSPUUIMonitor ) : public CActive
    {
public:

    static CSPUUIMonitor* NewL(RMobileCall& aCall,  
        MCSPUUSMessageObserver& aUUIHandler );

    virtual ~CSPUUIMonitor();

    /**
     * Starts monitoring the UUI messages
     *
     * @since S60 v3.2
     */
    void StartMonitor();
    
protected:

// from base class CActive

    /**
     * From CActive
     * Cancels the outstanding request
     *
     * @since S60 v3.2
     */
    void DoCancel();
    
    /**
     * From CActive
     * Handles request completion
     *
     * @since S60 v3.2
     */
    void RunL();

private:

    CSPUUIMonitor(RMobileCall& aCall,  
        MCSPUUSMessageObserver& aUUIHandler );

    void ConstructL();

private: // data
    
    /**
     * Etel call.
     * Not own.
     */
    RMobileCall& iCall;
    
    /**
     * UUI data.
     */
    RMobileCall::TMobileCallUUI iUUI;
    
   
    /**
     * UUI message observer;
     */
    MCSPUUSMessageObserver& iObserver;

    };

#endif // CSPUUIMONITOR_H
