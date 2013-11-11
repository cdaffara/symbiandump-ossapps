/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Shutdown timer for the preset server
*
*/


#ifndef C_CPSSHUTDOWN_H
#define C_CPSSHUTDOWN_H

#include <e32base.h>

/**
 * Shutdown timer for the preset server.
 * 
 * Closes the server when it has been unused for a set period of time.
 */
NONSHARABLE_CLASS( CPSShutdown ) : public CTimer
    {

public:

    /**
     * Two-phased constructor.
     *
     * @return  The newly created object.
     */
    static CPSShutdown* NewL();
    
    /**
     * Destructor.
     */    
    ~CPSShutdown();
    
    /**
     * Starts the shutdown timer.
     * @param   aDelay  The delay after PresetServer is shut down.
     */    
    void Start( TTimeIntervalMicroSeconds32 aDelay );


// from base class CActive

    /**
     * @see CActive::RunL()
     */
    void RunL();

    /**
     * @see CActive::DoCancel()
     */
    void DoCancel();

private:

    /**
     * Constructor.
     */    
    CPSShutdown();
    
    /**
     * Second-phase constructor.
     */    
    void ConstructL();

    };

#endif // C_CPSSHUTDOWN_H
