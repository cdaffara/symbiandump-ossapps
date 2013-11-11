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
* Description:  Server shutdown.
*
*/



#ifndef __SHUTDOWNSERVER_H__
#define __SHUTDOWNSERVER_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "ContainerStore.h"

// ==============================================================================================
// CLASS: CShutdownServer
//
// This class will shutdown the server after the indicated time period if Stop isn't called before
// the timer expires, and if no background operations are in progress when the timer expires.
// ==============================================================================================
class CShutdownServer : private CTimer, public MBackgroundOperationsObserver
{
public:

    // =========
    // CONSTANTS
    // =========

    // Constant value used for server shutdown delay.
    enum { EDefaultShutdownDelay = 0x200000 };  // approx 2s

    // ==============
    // PUBLIC METHODS
    // ==============
    
    //======================================================================
    // METHOD:  NewL
    //
    // PURPOSE:  Constructs a class instance.
    //
    // PARAMETERS:     None
    //
    // RETURN VALUES:  None
    //
    //======================================================================
    static CShutdownServer* NewL();
    
    ~CShutdownServer();

    //======================================================================
    // METHOD:  Start
    //
    // PURPOSE:  Method to start shutdown timer for the server.
    //
    // PARAMETERS:     None
    //
    // RETURN VALUES:  None
    //
    //======================================================================
    void Start( TTimeIntervalMicroSeconds32 aDelay = EDefaultShutdownDelay );
    
    //======================================================================
    // METHOD:  Start
    //
    // PURPOSE:  Method to stop shutdown timer for the server.
    //
    // PARAMETERS:     None
    //
    // RETURN VALUES:  None
    //
    //======================================================================
    void Stop();

    //======================================================================
    // METHOD:  ShutDownNow
    //
    // PURPOSE:  Some bad has happened.  Shut down the server now, regardless
    //           of whether background operations are in progress.
    //
    // PARAMETERS:     None
    //
    // RETURN VALUES:  None
    //
    //======================================================================
    void ShutDownNow();
    
    // inherited from MBackgroundOperationsObserver
    virtual void BackgroundOperationsInProgress();		
    virtual void BackgroundOperationsCompleted();	

private:

    // ===============
    // PRIVATE METHODS
    // ===============
    
    CShutdownServer();

    void ConstructL();

    void RunL();

    // ==================
    // PRIVATE ATTRIBUTES
    // ==================
        
    TBool iShutdown;
    TBool iBackgroundOperationsInProgress;
    
    __LOG_DECLARATION

    }; // end class CShutdownServer
  
#endif //__SHUTDOWNSERVER_H__
