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
* Description:  CSconTimeOut declaration
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

NONSHARABLE_CLASS( CSconTimeOut ) : public CActive
    {
public:
    // Cancel and destroy
    ~CSconTimeOut();
    static CSconTimeOut* NewL( MTimeOutObserver& aTimeOutObserver );
    
public:
    // Function for making the timeout request
    void Start(TTimeIntervalMicroSeconds32 aDelay);
    
private:
    CSconTimeOut( MTimeOutObserver& aTimeOutObserver );
    void ConstructL();
    
private: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
private:
    RTimer              iTimer; // Provides async timing service
    MTimeOutObserver&   iTimeOutObserver;
    };

#endif // CSCONTIMEOUT_H
