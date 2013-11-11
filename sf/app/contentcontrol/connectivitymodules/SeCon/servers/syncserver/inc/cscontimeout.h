/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCSconTimeOut header file
*
*/


#ifndef CSCONTIMEOUT_H
#define CSCONTIMEOUT_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

class MTimeOutObserver
    {
public:
    virtual void TimeOut() = 0;
    };

class CCSconTimeOut : public CActive
    {
public:
    // Cancel and destroy
    ~CCSconTimeOut();

    // Two-phased constructor.
    static CCSconTimeOut* NewL( MTimeOutObserver& aTimeOutObserver );
    
public:
    // New functions
    // Function for making the timeout request
    void Start(TTimeIntervalMicroSeconds32 aDelay);
    
private:
    // C++ constructor
    CCSconTimeOut( MTimeOutObserver& aTimeOutObserver );
    
    // Second-phase constructor
    void ConstructL();
    
private:
    // From CActive
    // Handle completion
    void RunL();
    
    // How to cancel me
    void DoCancel();
    
    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError(TInt aError);
    
private:
    enum TCSconTimeOutState
        {
        EUninitialized, // Uninitialized
        EInitialized, // Initalized
        EError
        // Error condition
        };

private:
    TInt iState; // State of the active object
    RTimer iTimer; // Provides async timing service
    MTimeOutObserver& iTimeOutObserver;
    };

#endif // CSCONTIMEOUT_H
