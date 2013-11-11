/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     CStartUpMonitor declaration file
 *
 */


#ifndef __STARTUPMONITOR_H__
#define __STARTUPMONITOR_H__

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CMsgSimOperation;

/**
 * CStartUpMonitor
 */
NONSHARABLE_CLASS( CStartUpMonitor ) : public CActive
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aWatcher a pointer to CMsgSimOperation
     */
    static CStartUpMonitor* NewL( CMsgSimOperation* aWatcher );
    
    /**
     * Destructor.
     */
    virtual ~CStartUpMonitor();
    
private:
    
    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();
    
    /**
     * Private C++ constructor.
     *
     * @param aWatcher a pointer to CMsgSimOperation
     */
    CStartUpMonitor( CMsgSimOperation* aWatcher );

private:

    /**
     * From CActive
     */
    void DoCancel();
    
    /**
     * From CActive
     */
    void RunL();

private:    // Data
    CMsgSimOperation*   iWatcher;
    RProperty 		    iStartupProperty;
};

#endif  // __STARTUPMONITOR_H__
// End of File
