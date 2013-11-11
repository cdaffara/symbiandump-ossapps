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
* Description:  PointSec monitor header.
*
*/

#ifndef _IMS_POINTSEC_MONITOR_H__
#define _IMS_POINTSEC_MONITOR_H__

// INCLUDES
//#include "MSyncEngineMonitor.h"
//#include "MSyncEngineMonitorIf.h"

#include <e32base.h>
#include <e32property.h>

//<cmail>
#include "DebugLogMacros.h"
//</cmail>

class MImsPointsecObserver;

NONSHARABLE_CLASS ( CImsPointsecMonitor ) : public CActive
    {
    public:
        IMPORT_C static CImsPointsecMonitor* NewL( MImsPointsecObserver&  aObserver );
        ~CImsPointsecMonitor();

        // from MSyncEngineMonitor
        //This is made public to allow caller to check the status immediatelly after constructing this object
        IMPORT_C TBool IsServiceAllowed() const;

    private:    // from CActive
        void RunL();
        void DoCancel();

    private:

        enum TPointsecStatus
            {
            EPointsecRecognizerStarted = 1,
            EPointsecInitializing,
            EPointsecLocked,
            EPointsecUnlocked,
            EPointSecUnknown,              //Do not know what 5 means, ignore it for now
            EPointSecEncryptionStarting,   //We received this while PointSec says "Encrypting Files"
                                           //encryption is done, we received EPointSecUnlocked
            EPointsecNotStarted = -1
            };

    private:
	    CImsPointsecMonitor( MImsPointsecObserver& aObserver );
        void ConstructL();
        void HandleEvent( TPointsecStatus aStatus );
        void SetServiceAllowed( TBool aValue );
        void NotifyObserverIfNeeded( TBool aOldServiceAllowes );
        void UpdateStatus();

    private:

        MImsPointsecObserver*  iObserver;
        RProperty iProperty;
        TBool iServiceAllowed;
        
        __LOG_DECLARATION
    };

#endif  // _IMS_POINTSEC_MONITOR_H__

